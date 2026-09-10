#!/usr/bin/env python3
"""Compare message boxes, casts, shared timer payloads and queue filtering with Java."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'message-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/MessageOracle.java'];listing=build/'message-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    def case(ops):cases.append(json.dumps({'ops':ops},ensure_ascii=True))
    bits=[0,1,2,3,0x7f800000,0xff800000,0x7fc00000,0x80000000,0x80000001,0x7f7fffff,0x807fffff,0x800000,0x3f800000,0x4b000000]+[rng.getrandbits(32) for _ in range(750)]
    for b in bits:case([{'cmd':'new','value':'FLOAT'},{'cmd':'put','id':0,'key':'number','type':'float','value':b}]+[{'cmd':'get','id':0,'key':'number','type':t} for t in ['float','int','bool','string','object']])
    keys=[None,'','Aa','BB','null','number','😀','é','a\x00b']
    for repeat in range(140):
        ops=[{'cmd':'new','value':rng.choice([None,'','A','B','😀x','AΣ'])} for _ in range(4)]+[{'cmd':'timer','id':i,'delay':rng.choice([0,1,-1,2**63-1,-2**63])} for i in range(4)]
        for i in range(75):
            cmd=rng.choices(['put','putraw','get','exists','settype','is','remove','clearcontents','post','pop','clear','except','contains'],[18,5,14,8,4,3,6,1,15,10,1,4,3])[0];op={'cmd':cmd,'id':rng.randrange(8),'key':rng.choice(keys)}
            if cmd in ['put','putraw','get']:
                t=rng.choice(['int','float','bool','string','null'] if cmd!='get' else ['int','float','bool','string','default','object']);op['type']=t;op['value']=rng.choice([True,False]) if t in ['bool','default'] else rng.choice(keys) if t=='string' else rng.getrandbits(32) if t=='float' else rng.randrange(-2**31,2**31) if t=='int' else None
            elif cmd in ['settype','is']:op['value']=rng.choice([None,'','A','B','😀x','AΣ'])
            elif cmd=='except':op['value']=rng.sample([None,'','A','B','😀x','AΣ'],rng.randrange(7))
            elif cmd=='contains':op['value']=rng.choice(['','A','B','😀','Σ'])
            ops.append(op)
        case(ops)
    case([{'cmd':'new','value':'A'},{'cmd':'post','id':0},{'cmd':'post'},{'cmd':'except','value':[]},{'cmd':'contains','value':'A'},{'cmd':'pop'},{'cmd':'pop'}])
    def run(cmd):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',text=True,capture_output=True,timeout=180,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
        lines=p.stdout.splitlines()
        if p.returncode or len(lines)!=len(cases):
            (build/'message-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, {len(lines)} outputs, expected {len(cases)}')
        return lines
    java=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MessageOracle']);cpp=run([str(build/'minima_messages')]);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,java,cpp) if j!=n]
    result={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Primitive boxes and wrong casts, null values/keys/types, message diagnostics, timer content aliasing and signed deadlines, FIFO and filtering; live thread timing and complete Object/HashMap APIs not asserted'}
    (build/'message-differential-results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({**result,'failures':[{k:v[:300] for k,v in f.items()} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
