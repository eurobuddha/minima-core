#!/usr/bin/env python3
import argparse,gzip,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'raw-archive-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ArchiveCacheOracle.java',ROOT/'support/RawArchiveOracle.java'];listing=build/'raw-archive-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','RawArchiveOracle'];cpp=[str(build/'minima_raw_archive')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=240,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'raw-archive-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    makes=[f'make {n} {c} {declared}' for n in [0,1,2,255,256,257,513] for c in [-1,0,3] for declared in [n,n+1,-1]];fixtures=run(java,makes);cases=[];folder=build/'raw-archive-fixtures';folder.mkdir(exist_ok=True)
    def add(data,actions='c n n n n s s n'):
        path=folder/f'{len(cases)}.gz';path.write_bytes(data);cases.append(f'read {path} {actions}')
    for spec,raw in zip(makes,fixtures):
        if raw=='ERROR':raise RuntimeError('Invalid fixture '+spec)
        data=bytes.fromhex(raw[2:]);zipped=gzip.compress(data,mtime=0);add(zipped);add(zipped,'n s c s n c n n')
        if len(data)<10000:
            for p in sorted({0,1,2,9,10,len(zipped)-1,len(zipped)-8,len(zipped)//2}):add(zipped[:p])
            add(zipped[:-8]+bytes(8));add(zipped+b'junk');split=len(data)//2;add(gzip.compress(data[:split],mtime=0)+gzip.compress(data[split:],mtime=0));add(gzip.compress(data[:-1],mtime=0))
            for p in [0,1,2,3,4,len(data)//2]:add(gzip.compress(data[:p],mtime=0))
    cases.append(f'read {folder}/missing.gz n s c n s')
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'RawArchiveInput: streaming gzip, cascade, 256-block boundaries, declared counts, truncated gzip/objects, CRC, concatenation, trailing data, stop/reconnect and before-connect calls'}
    (build/'raw-archive-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:4]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
