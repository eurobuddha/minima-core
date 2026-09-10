#!/usr/bin/env python3
"""Pinned HTTPServer listener with real concurrent socket handlers."""
import argparse,json,socket,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner,Responder

def exercise(command,auto,port=0):
    runner=Runner(command);rows=[]
    try:
        ready=runner.ask(dict(op='create',auto=auto,port=port));assert isinstance(ready,dict),ready;rows.append(ready)
        if not auto:ready=runner.ask(dict(op='start'));rows.append(ready)
        if ready['bound']:
            bound=ready['bound']
            with socket.create_connection(('127.0.0.1',bound),timeout=5) as held:
                rows.append(runner.ask(dict(op='status',completed=0,active=1)))
                for i in range(4):
                    payload=bytes([i,0,128,255,42])
                    with socket.create_connection(('127.0.0.1',bound),timeout=5) as client:
                        client.sendall(payload);assert Responder.read(client,5)==payload[::-1]
                rows.append(runner.ask(dict(op='status',completed=4,active=1)))
                held.sendall(b'abcde');assert Responder.read(held,5)==b'edcba'
            rows.append(runner.ask(dict(op='status',completed=5)))
            assert rows[-1]['factories']==rows[-1]['completed']==5 and rows[-1]['concurrent']
        else:assert ready['finished'] and any('already in use' in message for message in ready['logs'])
        rows.append(runner.ask(dict(op='shutdown')));assert rows[-1]['finished']
        for row in rows:
            assert row['port']==port
            row['bound']=bool(row['bound'])
        return rows
    except Exception as error:
        raise RuntimeError(f'{command[0]} auto={auto} port={port}; observations={rows}; stderr={runner.errors()}') from error
    finally:runner.close()

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'http-server-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/HTTPServerOracle.java'];listing=build/'http-server-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HTTPServerOracle'];native=[str(build/'minima_http_server')]
    with socket.socket(socket.AF_INET6,socket.SOCK_STREAM) as occupied:
        occupied.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0)
        occupied.bind(('::',0));occupied.listen();port=occupied.getsockname()[1]
        corpus=[(True,0),(False,0),(True,port)]
        for i,(auto,target) in enumerate(corpus):
            left=exercise(java,auto,target);right=exercise(native,auto,target)
            if left!=right:
                (build/'http-server-failure.json').write_text(json.dumps(dict(case=i,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Listener case {i}; see {build}/http-server-failure.json')
    report=dict(passed=True,programs=3,tcp_handlers=10,scope='Actual pinned Java HTTPServer and native listener, automatic/manual startup, configured versus ephemeral bound ports, concurrent independent handlers with exact binary payloads, bind collision logs and shutdown completion. Kernel-assigned port numbers normalized. RPC CMDHandler parsing and complete NetworkManager ownership remain pending; repeated concurrent starts and generic resource/exception identity require further audit.')
    (build/'http-server-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
