#!/usr/bin/env python3
"""P2P2DB shared-list, persistence and partial failure parity."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'p2p2-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/P2P2DBOracle.java'];listing=build/'p2p2-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    def op(action,value=None,key=None):return {'op':action,'value':value,'key':key}
    def add(ops):cases.append({'ops':ops})
    def wire(text):
        value=text.encode();return '0x'+(len(value).to_bytes(4,'big')+value).hex()
    values=[None,'','127.0.0.1:9001','INVALID','🐼','a\x00b',True,1,[],{}]
    texts=['{}','null','true','[]','bad','{"all_known_peers":null}','{"all_known_peers":42}','{"all_known_peers":["first",null,"second",7,"last"]}','{"all_known_peers":["x","x",null,null],"first_startup":false}','{"first_startup":"false"}']
    for text in texts:
        for method in ['read','file']:
            add([op('append','original'),op(method,wire(text)),op('load' if method=='file' else 'loadMissing'),op('add','first'),op('save'),op('clear'),op('load')])
    for length in range(30):
        add([op('put',['old'], 'all_known_peers'),op('append','external'),op('file',wire('{"all_known_peers":["peer"]}')[:2+length*2]),op('load'),op('saveFail'),op('loadMissing')])
    add([op('add',v) for v in values for _ in range(2)]+[op('save'),op('append','late'),op('load'),op('clear'),op('loadMissing')])
    for _ in range(160):
        ops=[]
        for i in range(30):
            action=rng.choice(['add','add','append','clear','first','put','read','file','save','saveFail','load','loadMissing']);value=rng.choice(values);key=rng.choice(['all_known_peers','first_startup','other'])
            if action in ['read','file']:value=wire(rng.choice(texts))
            elif action=='put':value=rng.choice(values+[rng.choices(values,k=5)])
            elif action=='first':value=rng.choice([True,False])
            ops.append(op(action,value,key))
        add(ops)
    def run(command):
        process=subprocess.run(command,input='\n'.join(json.dumps(case,ensure_ascii=True) for case in cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=[line for line in process.stdout.splitlines() if not line.startswith('Minima @ ')]
        if process.returncode or len(lines)!=len(cases):
            (build/'p2p2-db-crash.txt').write_text(process.stdout+'\n'+process.stderr);raise RuntimeError(f'Process {process.returncode}, {len(lines)} lines expected {len(cases)}')
        return [json.loads(line) for line in lines]
    java=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2P2DBOracle']);native=run([str(build/'minima_p2p2_db')]);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,java,native) if j!=n]
    report={'cases':len(cases),'operations':sum(len(c['ops']) for c in cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Actual P2P2DB class, shared list identity and mutation, null/duplicate peers, startup flag, JSON and exact wire/disk bytes, virtual load/save through JsonDB, absent/truncated/malformed files and incremental failure mutation. Exception classes/messages and log prefixes normalized; full MinimaDB runtime remains pending.'}
    (build/'p2p2-db-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
