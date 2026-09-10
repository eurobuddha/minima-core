#!/usr/bin/env python3
"""Pinned NetworkManager host selection and status through actual NIO maps/traffic."""
import argparse,itertools,json,random,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'network-manager-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NetworkManagerOracle.java'];listing=build/'network-manager-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NetworkManagerOracle']),Runner([str(build/'minima_network_manager')])]
    cases=[]
    for host,hostset in itertools.product(['','127.0.0.1','test-host','π😄'],[False,True]):cases.append(dict(op='host',host=host,hostset=hostset))
    amounts=[-(1<<63),-1024,-1,0,1,1023,1024,1048576,(1<<63)-1]
    # Five-second margins avoid the live Java clock straddling minute boundaries.
    ages=[-185000,-125000,-65000,-55000,-1,0,1,55000,65000,125000,185000,31536005000]
    for age,amount,all_fields in itertools.product(ages,amounts,[False,True]):
        cases.append(dict(op='status',host='fixture',age=age,read=amount,write=amount,all=all_fields,connecting=2,connected=3))
    rng=random.Random(5564)
    for _ in range(300):
        cases.append(dict(op='status',host=rng.choice(['','127.0.0.1','π😄']),hostset=rng.choice([True,False]),rpc=rng.choice([True,False]),p2p2=rng.choice([True,False]),all=rng.choice([True,False]),port=rng.choice([-1,0,9001,65535]),rpcport=rng.choice([-1,0,9005,65535]),connecting=rng.randrange(5),connected=rng.randrange(8),age=rng.choice(ages),read=rng.choice(amounts),write=rng.choice(amounts)))
    for fail,null,nio,all_fields in itertools.product([False,True],repeat=4):cases.append(dict(op='status',host='fixture',failDB=fail,nullDB=null,nullNIO=nio,all=all_fields))
    failures=[]
    try:
        for index,request in enumerate(cases):
            outputs=[runner.ask(request) for runner in runners]
            for result in outputs:
                if isinstance(result,dict) and isinstance(result.get('result'),dict) and 'traffic' in result['result']:
                    # Date rendering has its own exact multi-timezone differential.
                    # This suite fixes elapsed ages while the Java wall clock runs.
                    result['result']['traffic']['from']='<start-date>'
            if outputs[0]!=outputs[1] or not all(isinstance(row,dict) for row in outputs):
                failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,failures=failures,scope='Actual pinned/native NetworkManager.calculateHostIP and getStatus, real local interface host selection, configured-host bypass, actual NIO client maps and traffic accounting, positive/negative elapsed minutes, signed long rate division and byte formatting, basic/full status, P2P2/RPC configuration reporting and unused UserDB receiver/failure order. Only rendered start date normalized; elapsed-age fixtures stay away from live-clock minute boundaries. Enabled P2P nested status has separate manager comparison; constructor and soft/hard shutdown have native integration tests, not yet full Java lifecycle parity.')
    (build/'network-manager-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
