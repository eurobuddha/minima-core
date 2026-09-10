#!/usr/bin/env python3
"""NIOServer's actual Java catch boundaries against the native selector worker."""
import argparse,itertools,json,os,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def normalize(row):
    row['events']=[re.sub(r'NIOClient:[0-9A-V]+', 'NIOClient:<uid>', e) for e in row['events']]
    return row

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'nio-errors-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NIOServerOracle.java',ROOT/'support/NIOServerErrorsOracle.java'];listing=build/'nio-errors-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOServerErrorsOracle'],[str(build/'nio_server_errors')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'};failures=[]
    cases=[(s,k,0,'runtime',False) for s,k in itertools.product(['startup','accept','register','read','disconnect','send','all','cancel'],['runtime','error','oom','io'])]
    cases += [(s,'runtime',1,k,False) for s,k in itertools.product(['startup','accept','register','disconnect','send','all'],['runtime','error','oom'])]
    cases += [('read',k,0,'runtime',True) for k in ['runtime','error','oom','io']]
    cases += [(s,'runtime',1,k,True) for s,k in itertools.product(['accept','register'],['runtime','error','oom'])]
    cases += [('read','runtime',2,k,True) for k in ['runtime','error','oom']]
    for case in cases:
        rows=[]
        for command in commands:
            args=[str(v).lower() if isinstance(v,bool) else str(v) for v in case]
            p=subprocess.run(command+args,capture_output=True,text=True,timeout=15,env=env)
            assert p.returncode==0,(command,case,p.returncode,p.stdout,p.stderr)
            rows.append(normalize(json.loads(p.stdout)))
        if rows[0]!=rows[1]:failures.append(dict(case=case,java=rows[0],cpp=rows[1]))
    report=dict(passed=not failures,cases=len(cases),failures=failures,scope='Unmodified pinned NIOServer/NIOClient, real selector worker and loopback channels. Controlled application receiver fails at startup, accepted/outgoing registration, decoded message and explicit disconnect; selection keys fail in direct/broadcast send and cancellation. Runtime/Error/OOM/IO distinctions, catch logger failures, trace notify behavior, event order, running flags, map and pending-list effects. Explicit handler prevents fatal OOM halt. Generated client IDs and automatic JVM/native stack lines excluded. Full node startup is not exercised.')
    (build/'nio-errors-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
