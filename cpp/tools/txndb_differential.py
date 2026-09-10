#!/usr/bin/env python3
"""Custom transaction aliases, imports, partial reads, and preference persistence."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'txndb-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/TxnDBOracle.java'];listing=build/'txndb-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxnDBOracle'];cpp=[str(build/'minima_txndb')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',text=True,capture_output=True,timeout=240,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
        lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'txndb-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, {len(lines)} outputs, expected {len(cases)}')
        return lines
    fixtures=json.loads(run(java,[{'kind':'fixtures'}])[0]);cases=[]
    for f in fixtures:
        for row in [True,False]:
            data=f['row' if row else 'db'];length=(len(data)-2)//2
            variations=[data,data+'00',data+'ffffff']+[data[:2+2*i] for i in sorted(set([0,1,2,3,4,5,6,length-1,length//2]+[rng.randrange(length) for _ in range(8)]))]
            for value in variations:
                for convert in [True,False]:cases.append({'kind':'wire','row':row,'convert':convert,'show':rng.choice([True,False]),'data':value})
    for value in ['0x0000','0x000100','0x0001ff','0x000121','0x0021','0x00ff','0xffffffff','0x7fffffff','0x000000000000','0x000000000021']:
        for row in [True,False]:
            for convert in [True,False]:cases.append({'kind':'wire','row':row,'convert':convert,'show':True,'data':value})
    for repeat in range(125):
        ops=[{'cmd':'fixture','id':str(i),'index':rng.randrange(30)} for i in range(4)];rows=4;held=0
        for i in range(50):
            cmd=rng.choice(['create','fixture','rename','add','delete','get','clear','hold','clearheld','save','load','stored','wire','json','clearwitness']);o={'cmd':cmd,'id':rng.choice([None,'','0','1','2','new','😀']), 'index':rng.randrange(rows)}
            if cmd in ['create','fixture']:rows+=1
            if cmd=='fixture':o['index']=rng.randrange(30)
            if cmd=='hold':held+=1
            if cmd=='clearheld':o['index']=rng.randrange(max(held,1))
            if cmd=='stored':o['value']=rng.choice(['0x00','0x0000','0x000100',fixtures[0]['db']])
            if cmd=='json':o['show']=rng.choice([True,False])
            ops.append(o)
        cases.append({'kind':'sequence','ops':ops})
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    result={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Transaction rows/database wire import, corrupt and partial reads, output CoinID JSON side effects, duplicate IDs, row/list aliases and UserDB persistence; complete stream/container APIs remain under review'}
    (build/'txndb-differential-results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({**result,'failures':[{'case':f['case'],'java':f['java'][:200],'cpp':f['cpp'][:200]} for f in failures[:2]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
