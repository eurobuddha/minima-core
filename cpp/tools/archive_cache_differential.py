#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'archive-cache-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ArchiveCacheOracle.java'];listing=build/'archive-cache-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ArchiveCacheOracle'];cpp=[str(build/'minima_archive_cache')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'archive-cache-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[f'make {n} {mode}' for n in [0,1,2,5,17] for mode in [0,1]];fixtures=run(java,cases)
    for data in fixtures:
        n=(len(data)-2)//2
        variants=[data,data+'00','0xffffffff','0x80000000','0x000000010000']+[data[:2+2*i] for i in sorted(set([0,1,2,3,n-1,n//2]+[rng.randrange(n) for _ in range(20)]))]
        for v in variants:
            for c in ['true','false']:cases.append(f'list {v} {c}')
    for n in [0,1,8,9,10,16,32,64,128,256]:
        for minimum in [-6,0,1,17,32]:
            for mode in range(4):cases.append(f'cache {n} {minimum} {mode}')
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'TxBlockDB identity, duplicates, collision bins, child ordering, threshold cleanup/reindex; TxPoWList mutable input list, wire round trips, truncation and negative counts'}
    (build/'archive-cache-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{k:v[:160] for k,v in f.items()} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
