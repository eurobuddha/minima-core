#!/usr/bin/env python3
"""Pinned Magic field identity, failure order, arithmetic, and full demonstration."""
import argparse,hashlib,itertools,json,os,random,subprocess,time
from pathlib import Path
from decimal import Decimal,localcontext,ROUND_DOWN
from peers_checker_differential import Runner
from horst_differential import number,data,hx
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
NPE='java.lang.NullPointerException';EOF='java.io.EOFException'
DEFAULT=['65536','1024','256',hx(((2**256-1)//10000).to_bytes(31,'big'))]*2
BASE=['70000','1100','300','0x001234','80000','1400','400','0x005678']
TARGET=['90000','1200','350','0xFEDC','85000','1300','375','']
def parts(values):return [data(bytes.fromhex(v[2:])) if i in [3,7] else number(int(v)) for i,v in enumerate(values)]
def next_values(values):
    result=DEFAULT.copy()
    with localcontext() as context:
        context.prec=64;context.rounding=ROUND_DOWN
        for i,floor in enumerate([65536,1024,256]):
            current=Decimal(values[i]);desired=Decimal(values[i+4]);low=current/2;high=current*2
            desired=low if desired<low else high if desired>high else desired
            desired=max(desired,Decimal(floor));value=(current*16383+desired)/16384
            result[i]=format(value,'f').rstrip('0').rstrip('.') if '.' in format(value,'f') else format(value,'f')
            if result[i]=='-0':result[i]='0'
    current=int(values[3][2:] or '0',16);desired=int(values[7][2:] or '0',16)
    desired=current//2 if desired<current//2 else current*2 if desired>current*2 else desired
    value=(current*16383+min(desired,(2**256-1)//10000))//16384
    result[3]=hx(value.to_bytes(max(1,(value.bit_length()+7)//8),'big'))
    return result

def op(name,**kw):return dict(op=name,**kw)
def cases():
    result=[]
    def add(values,ops,checks=()):result.append((dict(**({} if values is None else dict(values=values)),ops=ops),list(checks)))
    # Independent checks are (observation index, key, expected value).
    add(None,[op('defaults'),op('write')],[(0,'value',[True]*8),(0,'fresh',DEFAULT),(1,'wire',hx(b'\xaa\x55'+b''.join(parts(DEFAULT))))])
    old=parts(BASE);new=parts(TARGET);wire=b''.join(new)
    for offset in [0,2]:
        for cut in range(len(wire)+1):
            end=0;state=[];same=[]
            for before,after,raw in zip(BASE,TARGET,new):end+=len(raw);state.append(after if end<=cut else before);same.append(end>cut)
            checks=[(0,'state',state),(0,'alias',state),(0,'containing',state),(0,'held',BASE),(0,'same',same),(0,'offset',offset+cut),(1,'wire',hx(b'\xaa\x55'+b''.join(parts(state))))]
            if cut<len(wire):checks.append((0,'error',EOF))
            add(BASE,[op('read',wire=hx(b'\xcc'*offset+wire[:cut]),offset=offset),op('write')],checks)
        add(BASE,[op('read',wire=hx(b'\xcc'*offset+wire+b'\xff\xff'),offset=offset)],[(0,'offset',offset+len(wire)),(0,'state',TARGET),(0,'same',[False]*8)])
    # Reject each malformed child only after earlier complete replacements.
    for i in range(8):
        malformed=[(b'\xff\xff\xff\xff','java.lang.NegativeArraySizeException'),(b'\x20\x00\x00\x01','java.io.IOException')] if i in [3,7] else [(b'\x00\x00','java.lang.NumberFormatException'),(b'\x00\xff','java.io.IOException'),(b'\x00\x21','java.io.IOException')]
        for bad,error in malformed:
            prefix=b''.join(new[:i]);add(BASE,[op('read',wire=hx(prefix+bad),offset=0)],[(0,'state',TARGET[:i]+BASE[i:]),(0,'same',[False]*i+[True]*(8-i)),(0,'held',BASE),(0,'offset',len(prefix+bad)),(0,'error',error)])
    for i in range(8):
        null=BASE.copy();null[i]=None
        add(null,[op('write'),op('json'),op('next')],[(0,'wire',hx(b'\xaa\x55'+b''.join(old[:i]))),(0,'error',NPE),(1,'error',NPE),(2,'error',NPE)])
        add(BASE,[op('set',index=i,value=None),op('write')],[(0,'held',BASE),(1,'error',NPE),(1,'wire',hx(b'\xaa\x55'+b''.join(old[:i])))])
        for replace in [False,True]:
            add(BASE,[op('fault',index=i,**(dict(replaceField=True) if replace else {})),op('write')],[(1,'wire',hx(b'\xaa\x55'+b''.join(old[:i])+b'\xab')),(1,'error','java.io.IOException')])
        replacement='0x99' if i in [3,7] else '99';changed=BASE.copy();changed[i]=replacement
        encoded=data(b'\x99') if i in [3,7] else number(99)
        add(BASE,[op('set',index=i,value=replacement),op('mutate',index=i,wire=hx(encoded)),op('write')],[(0,'held',BASE),(0,'state',changed),(1,'held',changed),(1,'same',[j!=i for j in range(8)])])
        for release in ['replace','release']:
            add(BASE,[op(release),op('mutate',index=i,wire=hx(encoded))],[(1,'alias',changed),(1,'containing',changed),(1,'held',changed),(1,'same',[False]*8)])
    for i,j in itertools.product(range(4),range(4)):
        # Earlier inequality must not hide a later null in either operand.
        left=BASE.copy();right=BASE.copy();right[i]='0x99' if i==3 else '99'
        for side in ['left','right']:
            l=left.copy();r=right.copy();(l if side=='left' else r)[j]=None
            add(l,[op('same',values=r)],[(0,'error',NPE)])
    add(BASE,[op('same',values=BASE),op('same',nullOther=True)],[(0,'value',True),(1,'error',NPE)])
    desired_null=BASE[:4]+[None]*4;add(BASE,[op('same',values=desired_null)],[(0,'value',True)])
    leading=BASE.copy();leading[3]='0x1234';add(BASE,[op('same',values=leading)],[(0,'value',False)])
    for i in range(4):
        encoded=data(b'\x99') if i==3 else number(99)
        state=BASE.copy();state[i+4]=state[i]
        changed=state.copy();changed[i]=changed[i+4]='0x99' if i==3 else '99'
        add(BASE,[op('setHeld',index=i+4,**{'from':i}),op('mutate',index=i,wire=hx(encoded))],[(0,'state',state),(1,'state',changed),(1,'alias',changed),(1,'containing',changed)])
    for cached,which in itertools.product([False,True],[3,7]):
        steps=([op('cache',index=which)] if cached else [])+[op('mutate',index=which,wire=hx(data(b'\x99'))),op('next')]
        add(BASE,steps)
    rng=random.Random(5564)
    for _ in range(120):
        values=[str(rng.randrange(-100000,500000)) for _ in range(8)]
        for i in [3,7]:values[i]=hx(rng.randbytes(rng.choice([0,1,2,31,32,33,65])))
        add(values,[op('next'),op('same',values=values)],[(0,'value',next_values(values)),(1,'value',True)])
    for i in [3,7]:
        large=BASE.copy();large[i]='0x'+'AB'*65;add(large,[op('write'),op('read',wire=hx(b''.join(parts(large))),offset=0)],[(0,'wire',hx(b'\xaa\x55'+b''.join(parts(large)))),(1,'state',large),(1,'offset',len(b''.join(parts(large))))])
    boundary=DEFAULT.copy();boundary[5]='2048';expected=DEFAULT.copy();expected[1]='1024.0625'
    add(boundary,[op('next')],[(0,'value',expected),(0,'state',boundary)])
    below=DEFAULT.copy();below[5]='1';add(below,[op('next')],[(0,'value',DEFAULT)])
    # Constants are restored through their actual Streamable objects after each mutation.
    for i in range(4):
        encoded=data(b'\x99') if i==3 else number(99)
        state=DEFAULT.copy();state[i]=state[i+4]='0x99' if i==3 else '99'
        add(None,[op('cache',index=3),op('mutate',index=i,wire=hx(encoded)),op('defaults'),op('mutate',index=i,wire=hx(parts(DEFAULT)[i])),op('defaults')],[(2,'value',[True]*8),(2,'fresh',state),(4,'fresh',DEFAULT)])
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--skip-demo',action='store_true');args=ap.parse_args();build=args.build.resolve();started=time.monotonic()
    oracle=build/'magic-oracle';oracle.mkdir(exist_ok=True);sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/MagicOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();runners=[];failures=[];executed=assertions=0;transcript=hashlib.sha256();demo={}
    try:
        runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MagicOracle']))
        runners.append(Runner([str(build/'minima_magic')]))
        for index,(request,checks) in enumerate(corpus):
            outputs=[runner.ask(request) for runner in runners];executed+=1
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]))
            for runtime,value in enumerate(outputs):
                for row,key,expected in checks:
                    assertions+=1
                    if value[row].get(key)!=expected:failures.append(dict(case=index,runtime=runtime,row=row,key=key,expected=expected,actual=value[row].get(key)))
            transcript.update(json.dumps([request,*outputs],sort_keys=True,separators=(',',':')).encode())
    except Exception as error:failures.append(dict(phase='execution',case=executed,error=repr(error)))
    finally:
        for runtime,runner in enumerate(runners):
            with os.fdopen(os.dup(runner.err.fileno()),'r') as errors:
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
                errors.seek(0);stderr=errors.read()
                if stderr:failures.append(dict(phase='stderr',runtime=runtime,stderr=stderr))
    if executed!=len(corpus):failures.append(dict(phase='accounting',executed=executed,planned=len(corpus)))
    if not args.skip_demo:
        try:
            observations=[]
            for runtime,command in [('java',['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.objects.Magic']),('cpp',[str(build/'minima_magic'),'--demo'])]:
                now=time.monotonic();p=subprocess.run(command,capture_output=True,timeout=1200,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
                (build/f'magic-demo-{runtime}.txt').write_bytes(p.stdout);observations.append(p.stdout)
                demo[runtime]=dict(exit=p.returncode,seconds=round(time.monotonic()-now,3),sha256=hashlib.sha256(p.stdout).hexdigest(),lines=len(p.stdout.splitlines()),stderr=p.stderr.decode())
                assertions+=1
                if p.returncode or p.stderr or len(p.stdout.splitlines())!=1729 or not p.stdout.startswith(b'Start:1024 Desired:3000\n1) 1024.0625\n'):failures.append(dict(phase='demo',runtime=runtime,details=demo[runtime]))
            assertions+=1
            if observations[0]!=observations[1]:failures.append(dict(phase='demo equality',details=demo))
        except Exception as error:failures.append(dict(phase='demo',error=repr(error)))
    report=dict(passed=not failures,cases=executed,planned_cases=len(corpus),independent_assertions=assertions,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),demo=demo,demo_skipped=args.skip_demo,failures=failures,scope='Actual pinned Magic: eight nullable shared fields, retained getters/setters and TxPoW setter aliases, complete-field assignment on in-place read, all read prefixes and malformed children, prefix-preserving polymorphic writes with reentrant replacement, eager current comparisons, weighted updates, defaults shared across instances, mutable defaults and work cache. Full original 86400-step demonstration compared unless explicitly skipped. Diagnostic prose, arbitrary streams, non-stream scalar subclass overrides, allocation/concurrency and full containing node parity remain unverified.')
    (build/('magic-quick-results.json' if args.skip_demo else 'magic-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:4]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
