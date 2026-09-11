#!/usr/bin/env python3
"""Validate the mapped TestUtils and QuickClients helpers against pinned originals."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def cases():
 result=[]
 for prefix,uid,count,port,incoming in itertools.product([None,'','10.0.0.','host🐼'],[None,'','uid'],[-1,0,1,3,10],[-1,0,9001,65536],[False,True]):
  q=dict(op='clients',prefix=prefix,uid=uid,count=count,port=port,incoming=incoming)
  if count>0 and (prefix is None or uid is None):expected=dict(error='java.lang.NullPointerException')
  else:
   rows=[dict(welcome='no welcome set..',uid=uid+str(i+1),incoming=incoming,host=prefix+str(i+1),port=port,minimaport=0,isconnected=True,valid=False) for i in range(max(0,count))];expected=dict(clients=rows,aliasSize=0,retained=rows[0] if rows else None)
  result.append((q,expected))
 for id,short,block,weight in itertools.product(['','0x00','0x1234','test-id🐼'],[False,True],[-2147483648,-1,0,1,2147483647],[-2147483648,-1,0,1,2147483647]):
  if short and (block!=0 or weight!=0):continue
  q=dict(op='pow',id=id,short=short,block=block,weight=weight);expected=dict(id=id,block=str(1 if short else block),weight=str(0 if short else weight),isBlock=True,isTransaction=False,parent='0x00',hasBody=False,distinct=True,initialTransactions=0,transactions=1,otherTransactions=0);result.append((q,expected))
 return result

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'original-helpers-oracle';oracle.mkdir(exist_ok=True)
 sources=sorted((JAVA/'src').rglob('*.java'))+[JAVA/'test/org/minima/utils/TestUtils.java',JAVA/'test/org/minima/system/network/p2p/testingutils/QuickClients.java',ROOT/'support/OriginalHelpersOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
 corpus=cases();runners=[];failures=[];count=checks=0;transcript=hashlib.sha256()
 try:
  runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','OriginalHelpersOracle']));runners.append(Runner([str(build/'original_helpers')]))
  for index,(q,expected) in enumerate(corpus):
   values=[runner.ask(q) for runner in runners];count+=1
   if values[0]!=values[1]:failures.append(dict(case=index,request=q,java=values[0],cpp=values[1]))
   for runtime,value in enumerate(values):
    for key,answer in expected.items():
     checks+=1
     if value.get(key)!=answer:failures.append(dict(case=index,runtime=runtime,key=key,expected=answer,actual=value.get(key)))
    if 'error' not in expected:
     checks+=1
     if 'error' in value:failures.append(dict(case=index,runtime=runtime,unexpected_error=value['error']))
   transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
 except Exception as error:failures.append(dict(phase='execution',case=count,error=repr(error)))
 finally:
  for runtime,runner in enumerate(runners):
   with os.fdopen(os.dup(runner.err.fileno()),'r') as errors:
    try:runner.close()
    except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
    errors.seek(0);stderr=errors.read()
    if stderr:failures.append(dict(phase='stderr',runtime=runtime,stderr=stderr))
 if count!=len(corpus):failures.append(dict(phase='accounting',executed=count,planned=len(corpus)))
 report=dict(passed=not failures,cases=count,planned_cases=len(corpus),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,mapped_files=['test/org/minima/utils/TestUtils.cpp','test/org/minima/system/network/p2p/testingutils/QuickClients.cpp'],scope='Both TestUtils overloads and the complete QuickClients generation loop call existing production constructors. Exact output fields/defaults, signed integer boundaries, nonpositive counts, nullable prefixes, ordered identities, retained child after list clear, independent TxPoW test transactions. Native TestUtils string IDs do not yet expose Java null-ID representation; underlying full TxPoW/NIOClientInfo and arbitrary allocation/concurrency parity remain unverified.')
 (build/'original-helpers-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
