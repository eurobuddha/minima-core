#!/usr/bin/env python3
"""Pinned Maths and Stack comparisons, with raw floating-point results recorded."""
import argparse,itertools,json,os,random,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'small-utilities-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SmallUtilitiesOracle.java'];listing=build/'small-utilities-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    versions=['','.', '..','1','1.0','1.0.0','0','-1','+1','01','1.','1..0','.1','0.1','2147483647','2147483648','-2147483648','-2147483649','١.２','１２.３','1.𝟙',' 1','1 ','1\n','1.0x1','1.e2','1.-0','1.+0','0.0.0.0','2.1','bad','2.bad']
    for a,b in itertools.product(versions,repeat=2):cases.append({'op':'versions','a':a,'b':b})
    for _ in range(300):
        versions=['.'.join(str(rng.randrange(-50,50)) for _ in range(rng.randrange(1,10))) for _ in range(2)];cases.append({'op':'versions','a':versions[0],'b':versions[1]})
    numbers=set(range(-40,40))
    for bits in [8,31,32,52,53,54,63,64,65,255,256,257,1023,1024,2048]:
        for offset in [-2,-1,0,1,2]:
            for sign in [-1,1]:numbers.add(sign*((1<<bits)+offset))
    for _ in range(600):numbers.add(rng.randrange(-(1<<rng.randrange(1,2049)),1<<rng.randrange(1,2049)))
    for number in sorted(numbers):cases.append({'op':'logbig','a':str(number)})
    bits=[0,1,(1<<63),(1<<63)+1,0x7ff0000000000000,0xfff0000000000000,0x7ff8000000000000]
    bits += [int.from_bytes(struct.pack('>d',x),'big') for x in [-10,-1,0.5,1,2,3,4,10,1e-300,1e300]]
    bits += [rng.getrandbits(64) for _ in range(600)]
    for value in bits:cases.append({'op':'log','a':str(value)})
    for levels in [-2147483648,-1,0,1,2,32,256,2147483647]:
        for a,b in itertools.product(['0x','0x00','0x01','0x02','0xFF','0x'+('FF'*32)],repeat=2):cases.append({'op':'super','a':a,'b':b,'levels':levels})
    for _ in range(250):cases.append({'op':'super','a':'0x'+rng.randbytes(rng.randrange(1,100)).hex(),'b':'0x'+rng.randbytes(rng.randrange(1,100)).hex(),'levels':rng.choice([1,32,256])})
    for _ in range(250):
        ops=[{'op':'peek'},{'op':'pop'},{'op':'push','index':0},{'op':'peek'},{'op':'pop'}]
        for i in range(100):ops.append({'op':rng.choice(['push','push','peek','pop','mutate']),'index':rng.randrange(-1,8),'text':str(i)})
        cases.append({'op':'stack','values':[None,True,3,'text',{'text':'original'},False,{'text':'second'}],'ops':ops})
    for _ in range(250):
        ops=[{'op':'send','id':None},{'op':'send','id':'0x01'},{'op':'send','id':'0x01'},{'op':'add','id':'0x01','time':'-9223372036854775808'},{'op':'send','id':'0x01'},{'op':'send','id':'0x0001'}]
        for i in range(75):ops.append({'op':rng.choice(['send','send','add','change','clear','reset','nulllist','nullunit']),'id':rng.choice([None,'0x','0x00','0x01','0x0001','bad','']), 'index':rng.randrange(-1,6),'time':rng.choice(['-9223372036854775808','9223372036854775807','0'])})
        cases.append({'op':'timed','ops':ops})
    def run(command):
        process=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=process.stdout.splitlines()
        if process.returncode or len(lines)!=len(cases):
            (build/'small-utilities-crash.txt').write_text(process.stderr);raise RuntimeError(f'Process exit {process.returncode}, lines {len(lines)}, expected {len(cases)}')
        return [json.loads(line) for line in lines]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SmallUtilitiesOracle']);actual=run([str(build/'minima_small_utilities')]);failures=[]
    for case,j,n in zip(cases,expected,actual):
        if j!=n:failures.append({'case':case,'java':j,'cpp':n})
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Maths versions, raw double logarithm bits, signed BigInteger bitLength/testBit, superlevel boundaries; Stack LIFO, null payloads, empty operations and mutable object identity; TimedRequest ID/list mutation, no automatic recording/expiry, constructor clock bounds'}
    (build/'small-utilities-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
