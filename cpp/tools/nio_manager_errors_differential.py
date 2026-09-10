#!/usr/bin/env python3
"""Actual connector runnable and native manager's server thread factory."""
import argparse,itertools,json,os,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def normalize(row):
    row['events']=[re.sub(r'127\.0\.0\.1:\d+', '127.0.0.1:<port>', re.sub(r'Thread-\d+', 'Thread-<id>', e)) for e in row['events']]
    return row

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'nio-manager-errors-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','NIOServerOracle.java','NIOServerErrorsOracle.java','NIOConnectorErrorsOracle.java']];listing=build/'nio-manager-errors-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOConnectorErrorsOracle'],[str(build/'nio_manager_errors')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'};failures=[]
    cases=list(itertools.product(['invalid','success','catch'],['runtime','error','oom','io']))
    def run(command,case):
        p=subprocess.run(command+list(case),capture_output=True,text=True,timeout=15,env=env)
        assert p.returncode==0,(command,case,p.returncode,p.stdout,p.stderr)
        return normalize(json.loads(p.stdout))
    for case in cases:
        rows=[run(command,case) for command in commands]
        if rows[0]!=rows[1]:failures.append(dict(case=case,java=rows[0],cpp=rows[1]))
    for kind,cls in [('runtime','java.lang.IllegalStateException'),('error','java.lang.LinkageError'),('oom','java.lang.OutOfMemoryError'),('io','java.io.IOException')]:
        row=run(commands[1],['server',kind]);expected={'events':['log:java.lang.IllegalArgumentException: Port value out of range: -1',f'uncaught:Thread-<id>:{cls}: logger','running:true']}
        if row!=expected:failures.append(dict(case=['native-server-factory',kind],expected=expected,cpp=row))
    report=dict(passed=not failures,connector_cases=len(cases),native_factory_cases=4,failures=failures,scope='Actual pinned private NIOManager.connectAttempt runnable and native implementation, loopback connection and invalid-port failures, success/catch logger failures with runtime/Error/OOM/IO; exception/default dispatch, pending removal, registered channels and reconnect posting. Native manager server factory separately checked with invalid-port catch logger failures. Does not start Java Main/full node; pre-shutdown server supplies a selector for connector registration. Generated thread names and ephemeral ports excluded.')
    (build/'nio-manager-errors-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
