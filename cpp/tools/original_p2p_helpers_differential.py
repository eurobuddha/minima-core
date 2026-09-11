#!/usr/bin/env python3
"""Validate the mapped QuickInetLists and QuickState helpers against pinned originals."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def addr(host,port):
 return dict(host=host,port=port,resolved=True,portValid=True)
def mapping(uid,prefix,count,port):
 return {uid+str(i):addr(prefix+str(i),-1 if port<=0 else port) for i in range(1,max(0,count)+1)}
def cases():
 result=[]
 for op,prefix,count,port in itertools.product(['list','map'],[None,'192.168.0.','','bad/','::'],[-2147483648,-1,0,1,3,20],[-2147483648,-1,0,9001,65535,65536]):
  for uid in ([None,'','uid🐼'] if op=='map' else ['uid']):
   q=dict(op=op,prefix=prefix,uid=uid,count=count,port=port)
   if count>0 and (prefix is None or (op=='map' and uid is None)):expected=dict(error='java.lang.NullPointerException')
   elif count>0 and prefix=='bad/':expected=dict(error='java.net.UnknownHostException')
   elif count>0 and (port>65535 or (op=='list' and port<0)):expected=dict(error='java.lang.IllegalArgumentException')
   else:
    def host(i):return '0:0:0:0:0:0:0:'+str(i) if prefix=='::' else '0.0.0.'+str(i) if prefix=='' else prefix+str(i)
    if op=='list':
     rows=[addr(host(i),port) for i in range(1,max(0,count)+1)];expected=dict(addresses=rows,aliasSize=0,retained=rows[0] if rows else None)
    else:expected=dict(peers={uid+str(i):addr(host(i),-1 if port<=0 else port) for i in range(1,max(0,count)+1)},aliasSize=0)
   result.append((q,expected))
 for hostname,prefix,count,port in itertools.product([None,'','host'],[None,'','192.168.0.'],[-1,0,1],[-1,9001,65536]):
  q=dict(op='hostname',hostname=hostname,prefix=prefix,count=count,port=port)
  expected=dict(error='java.lang.NullPointerException') if hostname is None or prefix is None else dict(error='java.net.UnknownHostException') if count>0 else dict(addresses=[],aliasSize=0,retained=None)
  result.append((q,expected))
 for mode,count,other,port,minimaPort,hasAddress in itertools.product(['no','out','in','known','both','none','full'],[-1,0,1,3],[-1,0,2],[-1,0,9001,65535,65536],[0,1234,65536],[False,True]):
  # Parameters unused by a factory are represented once, keeping this a focused corpus.
  if mode not in ['both','full'] and other!=0:continue
  if mode!='no' and not hasAddress:continue
  if mode in ['no','known'] and (count!=0 or port!=9001):continue
  if mode=='out' and port!=0:continue
  q=dict(op='state',mode=mode,count=count,other=other,port=port,minimaPort=minimaPort,hasAddress=hasAddress)
  addressed=mode!='no' or hasAddress
  badOwn=addressed and minimaPort>65535
  badPeers=port>65535 and ((mode in ['in','none'] and count>0) or (mode in ['both','full'] and (count>0 or other>0)))
  if badOwn or badPeers:expected=dict(error='java.lang.IllegalArgumentException')
  else:
   known='60.0.0.1' if mode in ['in','known'] else '192.168.0.1'
   expected=dict(own=addr('10.0.0.1' if addressed else '0.0.0.0',minimaPort if addressed else 9001),hostSet=addressed,
     **{'in':{},'out':{},'none':{},'notAccepting':{},'all':{},'known':[addr(known,9001)]},
     accepting=True,maxP2P=17,maxNone=29,delay=12345,discovery=False,startup=False,noConnect=False,secret='',sameKnownPeer=mode=='known',copyAfterClear=1,knownAfterClear=0)
   if mode in ['in','both','full']:expected['in']=mapping('inLinkUID','60.0.0.',other if mode=='both' else count,port)
   if mode in ['out','both','full']:expected['out']=mapping('outLinkUID','70.0.0.',count,0 if mode=='out' else port)
   if mode in ['none','full']:expected['none']=mapping('noneP2PUID','10.0.1.',count,port)
   if mode=='full':expected['notAccepting']=mapping('notAcceptingConnUID','50.0.1.',other,port)
   if mode=='known':expected['in']={'knownPeer':addr('60.0.0.1',9001)}
  result.append((q,expected))
 return result

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'original-p2p-helpers-oracle';oracle.mkdir(exist_ok=True)
 sources=sorted((JAVA/'src').rglob('*.java'))+[JAVA/'test/org/minima/system/network/p2p/testingutils/QuickInetLists.java',JAVA/'test/org/minima/system/network/p2p/testingutils/QuickState.java',ROOT/'support/OriginalP2PHelpersOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
 corpus=cases();runners=[];failures=[];count=checks=0;transcript=hashlib.sha256()
 try:
  runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','OriginalP2PHelpersOracle']));runners.append(Runner([str(build/'original_p2p_helper_probe')]))
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
 report=dict(passed=not failures,cases=count,planned_cases=len(corpus),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,mapped_files=['test/org/minima/system/network/p2p/testingutils/QuickInetLists.cpp','test/org/minima/system/network/p2p/testingutils/QuickState.cpp'],scope='All three address/map generators and seven state factories. Numeric IPv4/IPv6 addresses, prefixes, counts, fixed ports, random-port bounds checked before normalization, null/error precedence, map membership, list aliases, state defaults and known-peer identity/copy. Unseeded port sequences are not compared; external DNS variation, allocation failure, scheduling and complete production P2P parity remain outside this corpus.')
 (build/'original-p2p-helpers-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
