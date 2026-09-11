#!/usr/bin/env python3
"""TxHeader ownership, RLE aliases, partial reads and writes against pinned Java."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import number,data,hx
from streamable_differential import magic
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
EOF='java.io.EOFException';NPE='java.lang.NullPointerException';AIOOB='java.lang.ArrayIndexOutOfBoundsException';IO='java.io.IOException'
def fields(n):return [number(n),data(bytes([n+1])),number(n+2),number(n+3),data(bytes([n+4])),data(bytes([n+5])),number(n+6),magic(n),data(bytes([n+8])),data(bytes([n+9]))]
def encoded(n,runs):
 f=fields(n);return b''.join(f[:5])+b''.join(bytes([count])+data(bytes([v])) for count,v in runs)+b''.join(f[5:])
INITIAL=encoded(1,[(2,21),(2,22)])
def read_model(q,new,runs,cut,prefix):
 old=fields(1);same=[True]*10;length=q.get('arrayLength',4);parents=None if length<0 else ['0x33']*length if 'arrayLength' in q else ['0x15']*2+['0x16']*2
 ids=None if parents is None else list(range(length)) if 'arrayLength' in q else [0,0,1,1]
 error=None;cursor=0;completed=True
 def consume(chunk):
  nonlocal cursor,error
  cursor+=len(chunk)
  if cursor>cut:error=EOF;cursor=cut;return False
  return True
 for i,part in enumerate(fields(new)[:5]):
  if not consume(part):completed=False;break
  old[i]=part;same[i]=False
 if completed:
  for ri,(count,value) in enumerate(runs):
   if not consume(bytes([count])) or not consume(data(bytes([value]))):completed=False;break
   for _ in range(count):
    index=sum(c for c,v in runs[:ri])+_
    if parents is None:error=NPE;completed=False;break
    if index>=length:error=AIOOB;completed=False;break
    parents[index]=hx(bytes([value]));ids[index]=100+ri
   if not completed:break
 if completed:
  for i,part in enumerate(fields(new)[5:],5):
   if not consume(part):break
   old[i]=part;same[i]=False
 state=dict(fields=[hx(p) for p in old],parents=parents,parentAliases=[] if ids is None else [a==b for a in ids for b in ids])
 result=dict(offset=prefix+cursor,state=state,alias=state,sameFields=same,sameArray=True,oldParents=parents)
 if error:result['error']=error
 return result

def write_model(q):
 f=fields(1);out=b'\xaa\x55';fault=q.get('fault',-1);null=q.get('nullField',-1);error=None
 def write(i):
  nonlocal out,error
  if i==null:error=NPE;return False
  if i==fault:out+=bytes([0xBC if i in [5,8,9] else 0xAB]);error=IO;return False
  out+=f[i];return True
 for i in range(5):
  if not write(i):break
 else:
  size=q.get('arrayLength',4);p=None if size<0 else [b'\x33']*size if 'arrayLength' in q else [b'\x15']*2+[b'\x16']*2
  if 'nullParent' in q:p[q['nullParent']]=None
  if fault>=10:p[fault-10]=b''
  prev=None;prev_index=-1;count=0
  def run_write():
   nonlocal out,error
   out+=bytes([count%256])
   if prev is None:error=NPE;return False
   if prev_index==fault-10:out+=b'\xbc';error=IO;return False
   out+=data(prev);return True
  for i in range(q.get('runLevels',4)):
   if p is None:error=NPE;break
   if i>=len(p):error=AIOOB;break
   curr=p[i]
   if prev is None:prev=curr;prev_index=i;count+=1
   elif curr is None:error=NPE;break
   elif prev==curr:count+=1
   else:
    if not run_write():break
    prev=curr;prev_index=i;count=1
   if i==q.get('runLevels',4)-1 and not run_write():break
  if error is None:
   for i in range(5,10):
    if not write(i):break
 result={'wire':hx(out)}
 if error:result['error']=error
 return result

def cases():
 result=[]
 for n in [-1,0,1,4]:result.append((dict(op='construct',levels=n),dict(error='java.lang.NegativeArraySizeException') if n<0 else dict(length=n)))
 result.append((dict(op='defaults'),dict(checks=[True]*12,values=['0x66','0x77','0x55','44','0x33'])))
 result.append((dict(op='alias'),dict(checks=[True]*4,oldBody='0x0A')))
 for runs in [[(2,31),(2,32)],[(0,30),(3,31),(1,32)],[(6,31)]]:
  raw=encoded(2,runs)
  for length,prefix in itertools.product([-1,0,1,4,6,8],[0,3]):
   for cut in range(len(raw)+1):
    q=dict(op='read',arrayLength=length,wire=hx(b'\x66'*prefix+raw[:cut]),offset=prefix);result.append((q,read_model(q,2,runs,cut,prefix)))
 for level,length in itertools.product([-1,0,1,4,6],[-1,0,1,4,6,9]):
  runs=[] if level<=0 else [(level+2,41)];raw=encoded(2,runs);q=dict(op='read',runLevels=level,arrayLength=length,wire=hx(raw));result.append((q,read_model(q,2,runs,len(raw),0)))
 for null in range(10):
  q=dict(op='write',nullField=null);result.append((q,write_model(q)))
 for fault,replace in itertools.product(range(14),[False,True]):
  q=dict(op='write',fault=fault,replace=replace);result.append((q,write_model(q)))
 for size,level in itertools.product([-1,0,1,4,6],[-1,0,1,4,6]):
  q=dict(op='write',arrayLength=size,runLevels=level);result.append((q,write_model(q)))
 for slot in range(4):
  q=dict(op='write',nullParent=slot);result.append((q,write_model(q)))
  result.append((dict(op='json',nullParent=slot),dict(error=NPE) if slot else {}))
 for null in range(10):result.append((dict(op='json',nullField=null),dict(error=NPE)))
 result.extend([
  (dict(op='convert',wire=hx(INITIAL)),dict(converted=True,wire=hx(INITIAL),logs=[])),
  (dict(op='convert',wire=hx(INITIAL+b'\xff')),dict(converted=True,wire=hx(INITIAL),logs=[])),
  (dict(op='convert',wire=hx(INITIAL[:-1])),dict(converted=False,logs=[EOF])),
  (dict(op='convert',wire=hx(encoded(2,[(6,31)]))),dict(error=AIOOB,logs=[])),
  (dict(op='convert',wire=hx(INITIAL),nullInput=True),dict(error=NPE,logs=[])),
  (dict(op='convert',wire=hx(INITIAL),levels=-1),dict(error='java.lang.NegativeArraySizeException',logs=[])),
 ])
 for q,expected in result:
  q.setdefault('initial',hx(INITIAL))
  if q['op']=='convert':q.setdefault('nullInput',False)
 return result

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'txheader-oracle';oracle.mkdir(exist_ok=True)
 sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/TxHeaderOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
 corpus=cases();runners=[];failures=[];count=checks=0;transcript=hashlib.sha256()
 try:
  runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxHeaderOracle']));runners.append(Runner([str(build/'txheader')]))
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
 report=dict(passed=not failures,cases=count,planned_cases=len(corpus),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual TxHeader shared fields/defaults, retained wrappers/arrays/body getter, complete-field factory assignment, every byte prefix of represented RLE streams, zero/oversized runs versus actual array length, per-run alias matrix, nullable fields/arrays/slots, direct polymorphic write prefixes with self replacement, JSON errors. Existing TxPoW convenience return references, arbitrary streams, non-stream scalar overrides, allocation/concurrency and full node parity remain open.')
 (build/'txheader-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
