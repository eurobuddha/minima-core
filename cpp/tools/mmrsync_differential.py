#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'mmrsync-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ArchiveCacheOracle.java',ROOT/'support/MMRSyncOracle.java'];listing=build/'mmrsync-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MMRSyncOracle'];cpp=[str(build/'minima_mmrsync')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=240,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'mmrsync-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[f'make {kind} {n}' for kind in ['backup','ibd','sync'] for n in [-1,0,1,2,5,17]];fixtures=run(java,cases)
    for case,data in list(zip(cases,fixtures)):
        if data=='ERROR':continue
        kind=case.split()[1];n=(len(data)-2)//2
        variants=[data,data+'00','0x002101','0x0001000001ff0001ff',data[:6]+'ff'+data[8:]]
        variants += [data[:2+2*i] for i in sorted(set(range(min(n,100)))|{n-1,n//2}|{rng.randrange(n) for _ in range(80)})]
        for value in variants:
            for initial in [-1,2]:cases.append(f'read {kind} {initial} {value}')
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'MegaMMRBackup, MegaMMRIBD and MegaMMRSyncData: wire bytes, ignored version, negative counts, constructor list aliases, UTF-8 peers, truncated reads, consumed offsets, partial state and reference replacement'}
    (build/'mmrsync-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{k:v[:160] for k,v in f.items()} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
