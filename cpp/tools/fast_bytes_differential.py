#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'fast-bytes-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/FastByteArrayOracle.java'];listing=build/'fast-bytes-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','FastByteArrayOracle'];cpp=[str(build/'minima_fast_bytes')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=240,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});lines=p.stdout.splitlines()
        if p.returncode or len(lines)!=len(cases):
            (build/'fast-bytes-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[]
    for initial in [-1,0,1,2,32,262143,262144,262145,2147483647]:
        for _ in range(20):
            ops=[]
            for i in range(25):
                op=rng.choice(['fast','write','byte','reset','close','text','output','own']);ops.append({'op':op,'value':rng.randint(-1000,1000),'data':rng.choice([None,'0x','0x'+rng.randbytes(16).hex()]),'offset':rng.choice([-5,-1,0,1,8,16,17]),'length':rng.choice([-5,-1,0,1,8,16,17]),'high':rng.choice([0,1,-1,128,255,256,55296])})
            cases.append(json.dumps({'initial':initial,'ops':ops}))
    for initial in [0,1,262144]:
        for op in ['fast','write']:
            ops=[{'op':op,'offset':0,'length':n,'data':'0x'+rng.randbytes(n).hex()} for n in [262143,2,262145,524289]]+[{'op':'reset'},{'op':'byte','value':255},{'op':'output'}];cases.append(json.dumps({'initial':initial,'ops':ops}))
    for initial in [0,1,2,32]:cases.append(json.dumps({'initial':initial,'ops':[{'op':'own','offset':0,'length':initial+1},{'op':'byte','value':42},{'op':'fast','offset':0,'length':2147483647,'data':None}]}))
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'FastByteArrayStream capacity/growth, bytes and inherited writes/reset/close/output/UTF-8/high-byte strings; negative offsets, copy bounds, own-buffer alias and int overflow; OOM/other charsets excluded'}
    (build/'fast-bytes-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{k:v[:500] for k,v in f.items()} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
