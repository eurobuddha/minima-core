#!/usr/bin/env python3
"""RamDB behavior and JDK ConcurrentHashMap iteration, including collision bins."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--hashmap',action='store_true');ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();name='hashmap' if args.hashmap else 'ram';oracle=build/(name+'-oracle');oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/('support/HashMapOracle.java' if args.hashmap else 'support/RamOracle.java')];listing=build/(name+'-java-sources.txt');listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HashMapOracle' if args.hashmap else 'RamOracle'];cpp=[str(build/('minima_'+name))]
    rng=random.Random(5564);cases=[]
    def hx(s):return '0x'+s.encode().hex()
    def case(ops):cases.append('map '+str(len(ops))+' '+' '.join(ops))
    keys=[''.join('Aa' if (i>>bit)&1 else 'BB' for bit in range(7)) for i in range(128)]
    for count in [1,7,8,9,10,16,32,64,128]:
        for repeat in range(5):
            order=keys[:count];rng.shuffle(order);ops=[f'put {hx(k)} {i}' for i,k in enumerate(order)]
            ops += [f'put {hx(k)} {-i}' for i,k in enumerate(order)]
            rng.shuffle(order)
            for i,k in enumerate(order):
                ops += [f'remove {hx(k)}',f'get {hx(k)}']
                if i%3==0:ops.append(f'put {hx(k+"x")} {i}')
            ops += [f'put {hx(k)} {i}' for i,k in enumerate(order)];case(ops)
    for repeat in range(90):
        selected=keys[:rng.randrange(1,50)]+[str(rng.randrange(1000000))+'🐼é' for _ in range(rng.randrange(1,70))]+['','é','𐀀','a\x00b']
        ops=[]
        for i in range(rng.randrange(30,180)):
            k=rng.choice(selected);action=rng.choices(['put','remove','get','clear'],[55,25,19,1])[0]
            ops.append('clear' if action=='clear' else f'{action} {hx(k)}'+(f' {rng.randrange(-100000,100000)}' if action=='put' else ''))
        case(ops)
    for count in ([] if args.hashmap else [0,1,2,9,16,33,65,128,513]):
        for maximum in [-1,0,1,7,500]:
            for mode in [0,1,2,3]:cases.append(f'ram {count} {maximum} {mode}')
    def run(cmd):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=False,timeout=600,env={**os.environ,'TZ':'UTC'})
        out=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(out)!=len(cases):
            (build/(name+'-crash-input.txt')).write_text(cases[min(len(out),len(cases)-1)]+'\n');(build/(name+'-crash-stderr.txt')).write_text(p.stderr)
            raise RuntimeError(f'Process {p.returncode}, output {len(out)} expected {len(cases)}; crash input saved')
        return out
    expected=run(java);actual=run(cpp);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':('HashMap ordering, collision trees, resize, lookup and deletion' if args.hashmap else 'Quiescent map ordering, collision trees, resize, lookup, deletion, RAM flags, duplicate insertion, burn cleanup and zero-age cleanup; concurrent enumeration and timing parity unverified')}
    (build/(name+'-differential-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{k:v[:200] for k,v in f.items()} for f in failures[:5]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
