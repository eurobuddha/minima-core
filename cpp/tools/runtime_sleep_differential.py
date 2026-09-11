#!/usr/bin/env python3
"""Main, RPC, archive retry and P2P discovery using real interruption signals."""
import argparse,json,os,subprocess
from pathlib import Path
from peers_checker_differential import Runner,Responder
from p2p_functions_differential import address,op
from p2p_manager_differential import process,canonical
from archive_server_differential import project
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'runtime-sleep-oracle';oracle.mkdir(exist_ok=True)
    names=['NIOClientOracle.java','MainNetworkOracle.java','RuntimeSleepOracle.java','SqlDBOracle.java','MySQLConnectOracle.java','ArchiveManagerOracle.java','ObjectOracle.java','ArchiveServerOracle.java','P2PFunctionsOracle.java','PeersCheckerOracle.java','P2PManagerOracle.java']
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in names]
    listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar'];report=dict(passed=False,comparisons=0,assertions=0,cases=[])
    def check(condition,reason):
        report['assertions']+=1
        assert condition,reason
    try:
        for mode in ['shutdown_wait','restart_wait','restart_pause','netrestart','rpc']:
            rows=[]
            for command in [java+['RuntimeSleepOracle'],[str(build/'runtime_sleep')]]:
                p=subprocess.run(command+[mode],capture_output=True,text=True,timeout=15,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
                check(p.returncode==0,(command,mode,p.returncode,p.stdout,p.stderr));rows.append(json.loads(p.stdout))
            report['cases'].append(dict(mode=mode,java=rows[0],cpp=rows[1]));check(rows[0]==rows[1],report['cases'][-1]);report['comparisons']+=1
            expected=dict(mode=mode,error='java.lang.InterruptedException' if mode in ['netrestart','rpc'] else 'none',pending=False,replaced=mode.startswith('restart_'),enabled=False,authenticate=False,locked=False)
            for row in rows:
                for key,value in expected.items():check(row[key]==value,(mode,key,row))
        runners=[Runner(java+['ArchiveServerOracle']),Runner([str(build/'archive_server_runtime')])]
        try:
            requests=[dict(op='retry',first=first,rows=[],columns=[],interrupt=True) for first in ['0',None]]
            requests += [dict(op='retry',first='7',rows=[],columns=[],interrupt=True,fail=fail,runtimeFailure=runtime) for fail in range(1,5) for runtime in [False,True]]
            for request in requests:
                rows=[r.ask(request) for r in runners];left,right=map(project,rows)
                report['cases'].append(dict(archive=request,java=left,cpp=right));check(left==right,report['cases'][-1]);report['comparisons']+=1
                # SQLExceptions are caught inside loadBlockRange and do not trigger
                # retry. Runtime failures and null first-block errors do trigger it.
                retry=request['first'] is None or request.get('runtimeFailure',False)
                for row in rows:
                    check(row['threadInterrupted'] is not retry,(request,row))
                    check(any('Connection failed.. reconnecting..' in log for log in row['logs'])==retry,(request,row))
        finally:
            for r in runners:r.close()
        runners=[Runner(java+['P2PManagerOracle']),Runner([str(build/'minima_p2p_manager')])];server=Responder()
        try:
            reply=runners[0].ask(dict(encode=True,version='1.1.2.8',extra={'peers-list':['8.8.8.8:9001']},type=12))
            for kind in ['P2P_INIT','P2P_LOOP']:
                for dropped in [False,True]:
                    request=dict(discovery=True,defaults=[address('127.0.0.1',server.port)],ops=[process(kind,interrupt=True),op('have')]);rows=[];traces=[]
                    for r in runners:
                        server.configure(([None] if dropped else [])+[reply]);rows.append(r.ask(request));traces.append(server.trace())
                    left,right=map(canonical,rows);report['cases'].append(dict(p2p=kind,dropped=dropped,java=left,cpp=right));check(left==right,report['cases'][-1]);check(traces[0]==traces[1],traces);report['comparisons']+=1
                    for row,trace in zip(rows,traces):
                        check(len(trace)==(2 if dropped else 1),trace);check(row[0]['threadInterrupted'] is not dropped,row)
                        check(row[0]['result']=='OK' and row[0]['state']['known'] and row[0]['verified'] and not row[0]['state']['discovery'],row)
        finally:
            server.close()
            for r in runners:r.close()
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Actual Main shutdown/restart with controlled old-network completion, real replacement network on ephemeral port with outbound peers disabled; RPC state ordering; actual archive retry with recording SQL; actual P2P discovery/TCP responder. Real interrupt flags/sleeps, no injected InterruptedException. No wallet or mainnet node touched. Broader runtime/I/O/thread parity remains open.'
        (build/'runtime-sleep-results.json').write_text(json.dumps(report,indent=2)+'\n')
        print(json.dumps({k:v for k,v in report.items() if k!='cases'},indent=2))
if __name__=='__main__':main()
