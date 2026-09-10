#!/usr/bin/env python3
"""JDK executor worker exit precedes default-handler dispatch, including drain."""
import argparse,itertools,json,os,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'executor-uncaught-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ExecutorUncaughtOracle.java'];listing=build/'executor-uncaught-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ExecutorUncaughtOracle'],[str(build/'executor_uncaught')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'};failures=[];cases=list(itertools.product(['runtime','error','oom','io','defaultthrows'],['false','true']))
    for kind,queued in cases:
        rows=[]
        for command in commands:
            p=subprocess.run(command+[kind,queued],capture_output=True,text=True,timeout=15,env=env);assert p.returncode==0,(command,kind,queued,p.returncode,p.stdout,p.stderr);rows.append(json.loads(p.stdout))
        if rows[0]!=rows[1]:failures.append(dict(kind=kind,queued=queued,java=rows[0],cpp=rows[1]))
    report=dict(passed=not failures,cases=len(cases),failures=failures,scope='Actual JDK Executors.newFixedThreadPool(1) and native FixedTaskExecutor. Runtime/Error/OOM/IO/default-handler failures, late global-handler replacement, successor thread starts before handler dispatch, queue drain and awaitTermination may complete while uncaught handler remains blocked. Exact first worker name and event order. Native resource exhaustion, JDK thread groups/priority/interruptions/custom ThreadFactory and all ExecutorService APIs remain outside this comparison.')
    (build/'executor-uncaught-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
