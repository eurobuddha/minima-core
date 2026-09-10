#!/usr/bin/env python3
"""Compare native consensus rules with all freshly compiled pinned Java sources."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve()
    oracle=build/'consensus-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ConsensusOracle.java']
    sourcefile=build/'consensus-java-sources.txt';sourcefile.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(sourcefile)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ConsensusOracle'];cpp=[str(build/'minima_consensus')]
    def run(cmd,cases):
        label='java' if cmd[0]=='java' else 'native'
        inputfile=build/f'consensus-{label}-input.txt';inputfile.write_text('\n'.join(cases)+'\n')
        outputfile=build/f'consensus-{label}-output.txt';errorfile=build/f'consensus-{label}-stderr.txt'
        with inputfile.open() as source,outputfile.open('w') as output,errorfile.open('w') as errors:
            p=subprocess.Popen(cmd,stdin=source,stdout=output,stderr=errors,text=True,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
            try:p.wait(timeout=1800)
            except subprocess.TimeoutExpired:p.kill();p.wait();raise
        out=[s for s in outputfile.read_text().splitlines() if not s.startswith('Minima @ ')]
        if p.returncode:
            (build/'consensus-crash-input.txt').write_text(cases[min(len(out),len(cases)-1)]+'\n')
            raise RuntimeError(f'Process failed {p.returncode}, completed {len(out)}; input saved to consensus-crash-input.txt')
        if len(out)!=len(cases):raise RuntimeError(f'Output count {len(out)} expected {len(cases)}; inspect '+str(outputfile))
        return out
    rng=random.Random(5564);cases=[]
    scripts=['RETURN TRUE','RETURN FALSE','RETURN @BLKNUM EQ 20','RETURN @BLKNUM GTE 20','RETURN @COINAGE GTE 18','RETURN PREVSTATE(0) EQ STATE(0)','RETURN @AMOUNT EQ 10','LET x = 0 WHILE x LT 5 DO LET x = x + 1 ENDWHILE RETURN TRUE','RETURN SIGNEDBY(0x11)','RETURN [hello]','THIS IS INVALID','RETURN TRUE ASSERT FALSE']
    fixtures=[]
    for mode in range(30):
        for script in scripts[:7]+[scripts[-1]]:fixtures.append(f'fixture {mode} 3 1024 0x{script.encode().hex()}')
    for script in scripts:
        for count in [0,1,9,10,11,64]:
            for ops in [-1,0,1,2,8,64]:fixtures.append(f'fixture 0 {count} {ops} 0x{script.encode().hex()}')
    wires=run(java,fixtures)
    for fixture,wire in zip(fixtures,wires):
        if wire=='ERROR':raise RuntimeError('Java fixture failed: '+fixture)
        for maximum,state in [(64,65536),(2,128),(-1,-1)]:cases.append(f'check {wire} {maximum} {state}')
    for index in [0,8,16,24,72,160,168,176,184,200]:
        mmr,txn,_=wires[index].split()
        for count in [0,1,3]:
            for own in [0,1]:
                for reject in [0,4]:cases.append(f'generate {mmr} {txn} {count} {own} {reject} 1000000 0 256')
    for count in [499,500,501,510]:
        mmr,txn,_=wires[0].split();cases.append(f'generate {mmr} {txn} {count} 0 0 1000000 0 256')
    for hash in ['0','-1','1','1000','1000000','18446744073709551615']:
        for time in ['0','99999999999999']:
            for maximum in ['-1','0','1','256']:
                mmr,txn,_=wires[0].split();cases.append(f'generate {mmr} {txn} 3 0 0 {hash} {time} {maximum}')
    for n in ['0','-1','0.5','1','1.9','1000','18446744073709551615','18446744073709551616']+[str(rng.randrange(1,2**64)) for _ in range(100)]:cases.append('difficulty '+n)
    for i in range(150):cases.append(f'divide {rng.randrange(10**80)} {rng.randrange(1,10**80)} {rng.choice([7,16,34])}')
    for n in [1,2,8,9,16,65,128,257,300]:
        for step in [1,1000,50000,1000000,0,-1]:
            for back in [-7,-1,0,1,64,256]:
                rows=[f'{i+1} {1000000+i*step} 0x{((2**256-1)//(1000000+i%7)):x}' for i in range(n)]
                cases.append(f'chain {back} {n} -1 '+' '.join(rows))
    for i in range(120):
        n=rng.randrange(1,100);times=[rng.randrange(1,10**8) for _ in range(n)];rows=[f'{j+1} {times[j]} 0x{((2**256-1)//rng.randrange(1,10000000)):x}' for j in range(n)]
        cases.append(f'chain {rng.choice([1,2,64,128])} {n} {rng.choice([-1,-1,0,1,23])} '+' '.join(rows))
    (build/'consensus-cases.txt').write_text('\n'.join(cases)+'\n')
    expected=run(java,cases);(build/'consensus-expected.txt').write_text('\n'.join(expected)+'\n');actual=run(cpp,cases)
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Basic, signatures, MMR, scripts and monotonic caches, relay, coin IDs, difficulty, median time, superparents block validation and deterministic block generation/mempool selection; timed checking, live database wiring and complete API parity pending'}
    (build/'consensus-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({**report,'failures':[{**f,'case':f['case'][:160]} for f in failures[:8]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
