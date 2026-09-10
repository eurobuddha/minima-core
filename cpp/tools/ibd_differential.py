#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'ibd-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/IBDOracle.java',ROOT/'support/ArchiveCacheOracle.java'];listing=build/'ibd-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','IBDOracle'];cpp=[str(build/'minima_ibd')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(json.dumps(c) for c in cases)+'\n',capture_output=True,text=True,timeout=600,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'ibd-crash-stderr.txt').write_text(p.stderr);(build/'ibd-crash-input.txt').write_text(json.dumps(cases[min(len(lines),len(cases)-1)]));raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[]
    def add(kind,current=None,archive=None,**rest):cases.append({'kind':kind,'current':current or {},'archive':archive or {},**rest})
    configs=[{'start':start,'cascade':cascade,'count':count,'variant':variant,'bad':bad} for start in [1,10] for cascade in [-1,0,3] for count in [0,1,5] for variant in [0,1,2] for bad in [0,1] if not bad or cascade>0]
    for config in configs:
        add('make',config)
        for index in [-1,0,config['count'],config['count']+1]:add('shorten',config,index=index)
        add('complete',config,preload=1)
    for i in range(150):add('compare',rng.choice(configs),other=rng.choice(configs))
    for i in range(150):
        config=rng.choice(configs);archive={'start':1,'cascade':rng.choice([0,3]),'count':10};kind=rng.choice(['sync','archive']);add(kind,config,archive,first=rng.choice([0,1,4,10,15,100]),local=rng.randrange(2),preload=rng.randrange(2),shutdown=int(rng.randrange(5)==0),fail=int(rng.randrange(7)==0))
    for i in range(180):
        start=rng.choice([1,10,35000]);current={'start':start,'cascade':0,'count':rng.choice([0,1,5,12])};archive={'start':1,'count':25};top=rng.choice([-1,1,5,10,20,start,start+6,start+100]);chain=[[n,rng.choice([0,0,1])] for n in range(max(0,top-rng.randrange(0,8)),max(0,top)+1)];chain.reverse();add('greet',current,archive,top=top,chain=chain,preload=rng.randrange(2),fail=int(rng.randrange(10)==0))
    for found in [0,1,19,20,21,40,42]:
        chain=[[25-i,1] for i in range(43)];chain[found]=[2,0];add('greet',{'start':50,'count':3},{'start':1,'count':25},top=25,chain=chain)
    fixtures=run(java,[{'kind':'make','current':c,'archive':{}} for c in configs[::10]])
    for f in fixtures:
        if f=='ERROR':continue
        data=json.loads(f)['wire'];length=(len(data)-2)//2
        for v in [data,data+'00','0x000001ff','0x000000']+[data[:2+2*i] for i in sorted(set([0,1,2,3,length-1,length//2]+[rng.randrange(length) for _ in range(10)]))]:cases.append({'kind':'wire','data':v})
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'IBD wire/partial reads, cascade validity, exact weights, shortening/crossover precedence, complete/greeting/sync/archive construction, archive request parameters and lock release; database and network integration not asserted'}
    (build/'ibd-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{'case':f['case'],'java':f['java'][:200],'cpp':f['cpp'][:200]} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
