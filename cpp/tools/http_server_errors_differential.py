#!/usr/bin/env python3
"""Actual pinned HTTPServer failures, source catches and native thread dispatch."""
import argparse,itertools,json,os,re,socket,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def normalize(row):
    row['events']=[re.sub(r'Thread-\d+','Thread-<id>',line) for line in row['events']]
    return row

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'http-errors-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/HTTPServerErrorsOracle.java'];listing=build/'http-errors-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HTTPServerErrorsOracle'],[str(build/'http_server_errors')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'};failures=[]
    with socket.socket(socket.AF_INET6,socket.SOCK_STREAM) as occupied:
        occupied.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0);occupied.bind(('::',0));occupied.listen();port=occupied.getsockname()[1]
        cases=[(stage,kind,0,0) for stage,kind in itertools.product(['factory','stopped','handler'],['runtime','error','oom','io','socket','bind'])]
        cases += [(stage,'runtime',value,0) for stage,value in itertools.product(['invalid','direct'],[-1,65536])]
        cases += [('startup','runtime',0,1),('collision','runtime',port,1),('handler','defaultthrows',0,0)]
        cases += [(stage,kind,0,2) for stage,kind in itertools.product(['factory','stopped'],['io','bind'])]
        cases += [('factory','socket',0,2)]
        for i,case in enumerate(cases):
            rows=[]
            for command in commands:
                p=subprocess.run(command+list(map(str,case)),capture_output=True,text=True,timeout=15,env=env)
                assert p.returncode==0,(command,case,p.returncode,p.stdout,p.stderr)
                rows.append(normalize(json.loads(p.stdout)))
            if rows[0]!=rows[1]:failures.append(dict(case=case,java=rows[0],cpp=rows[1]))
    report=dict(passed=not failures,cases=len(cases),failures=failures,scope='Unmodified pinned HTTPServer, actual listener and daemon request threads, loopback sockets; runtime/Error/OOM/IO/socket/bind failures in factories and handlers, stopped-server catches, logger failures, invalid ports and direct-run propagation. Explicit test default handler prevents fatal OOM halt. Native/JVM automatic stack lines and generated Thread counters excluded; source log text/order, handler thread name, listener completion/running/bound state compared.')
    (build/'http-errors-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
