#!/usr/bin/env python3
"""Actual Greeting.createGreeting and connect command before NIOManager binding."""
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'network-startup-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/PeerMessagesOracle.java',ROOT/'support/NetworkStartupOracle.java'];listing=build/'network-startup-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    hosts=[None,{},[],True,0,'','host','host:',':9001','::1:9001','[::1]:9001','http://host:80','🐼:9001','a\x00b:3']
    for host in ['','127.0.0.1','localhost','invalid host','HOST','é']:
        for port in ['-2147483648','2147483647','2147483648','-2147483649','-1','0','65535','65536','+10','-0','1.0','1e3','١٢','１２','𝟙','²',' 9001\t','\u00a09001','\x00+123\x1f',':3','']:
            hosts.append(host+':'+port)
    for host in hosts:
        for fail in [False,True]:cases.append({'kind':'connect','host':host,'fail':fail})
    for command in ['connect','connect host:noport','connect host:x:','connect host:x:9001','connect host::9001','connect host:x:-1','connect host:x:65536','connect host:"🐼:１２"','connect host:x:1 extra:2','connect host:x:1;connect host:y:2','connect host:bad;connect host:x:1','connect host:x:2147483648']:
        for fail in [False,True]:cases.append({'kind':'connect','host':'unused:1','fail':fail,'command':command})
    addresses=[None,{'host':'127.0.0.1','port':9001},{'host':'::1','port':9002},{'host':'invalid host','port':1,'unresolved':True}]
    def config(**changes):return {'welcome':'hello','hostSet':True,'host':'127.0.0.1','port':9001,'p2p':True,'count':3,'peers':addresses[1:3],'fail':'',**changes}
    for stage in ['','lock','unlock','welcome','tip','peers']:
        for count in [0,1,7]:
            for peerlist in [None,[],addresses[1:3],[None],[addresses[1],None],[addresses[3]]]:
                cases.append({'kind':'greeting','ops':[config(count=count,peers=peerlist,fail=stage),config(hostSet=False,p2p=False,count=1),config(welcome=None,count=0)]})
    for _ in range(120):
        ops=[]
        for i in range(12):
            ops.append(config(welcome=rng.choice([None,'','hello','🐼','a\x00b']),hostSet=rng.choice([False,True]),host=rng.choice(['','127.0.0.1','::1','not an address']),port=rng.choice([-2147483648,-1,0,9001,2147483647]),p2p=rng.choice([False,True]),count=rng.randrange(10),peers=rng.choice([None,[],rng.choices(addresses,k=3)]),fail=rng.choice(['','','','lock','unlock','welcome','tip','peers'])))
        cases.append({'kind':'greeting','ops':ops})
    def run(command):
        process=subprocess.run(command,input='\n'.join(json.dumps(case,ensure_ascii=True) for case in cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=[line for line in process.stdout.splitlines() if not line.startswith('Minima @ ')]
        if process.returncode or len(lines)!=len(cases):
            (build/'network-startup-crash.txt').write_text(process.stdout+'\n'+process.stderr);raise RuntimeError(f'Process {process.returncode}, {len(lines)} lines expected {len(cases)}')
        return [json.loads(line) for line in lines]
    java=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NetworkStartupOracle']);native=run([str(build/'minima_network_startup')]);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,java,native) if j!=n]
    report={'cases':len(cases),'greeting_calls':sum(len(c.get('ops',[])) for c in cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Actual pinned Greeting.createGreeting against controlled DB/UserDB/P2P services, actual TxPoW tree nodes, lock order/depth, caught/propagated faults, repeated chain append, retained JSON and wire bytes. connect helper/command/runner parsing, checked error text, Unicode integer bounds, metadata and posted message identity/content. Node startup bypassed only in Java fixtures; native services require runtime binding. Generic direct exceptions normalized; no network connection attempted.'}
    (build/'network-startup-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
