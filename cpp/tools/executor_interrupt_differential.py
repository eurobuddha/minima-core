#!/usr/bin/env python3
"""Actual fixed executor waits and NIO shutdown interruption ordering."""
import argparse,itertools,json,os,subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'executor-interrupt-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','ExecutorInterruptOracle.java']]
    listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','--add-opens','java.base/java.util.concurrent=ALL-UNNAMED','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ExecutorInterruptOracle'],[str(build/'executor_interrupt')]]
    cases=[['matrix',state,str(millis),str(pending).lower()] for state,millis,pending in itertools.product(['open','terminated','held'],[-2**63,-1,0,1],[False,True])]
    cases += [['matrix',state,str(2**63-1),'true'] for state in ['open','terminated','held']]
    cases += [[mode] for mode in ['interrupt','complete','multiple','reuse','signal_first','reinterrupt','nio_hard_held','nio_hard_idle','nio_message_held','nio_message_idle']]
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):
        report['assertions']+=1
        assert value,detail
    def run(command,case):
        p=subprocess.run(command+case,capture_output=True,text=True,timeout=15,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if p.returncode:raise AssertionError((command,case,p.returncode,p.stdout,p.stderr))
        return json.loads(p.stdout)
    def result(value=None,error='none',pending=False):return dict(value=value,error=error,pending=pending)
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            for case in cases:
                futures=[pool.submit(run,command,case) for command in commands];rows=[future.result() for future in futures]
                report['observations'].append(dict(case=case,java=rows[0],cpp=rows[1]));check(rows[0]==rows[1],report['observations'][-1]);report['comparisons']+=1
                mode=case[0]
                if mode=='matrix':
                    _,state,millis,pending=case;pending=pending=='true';throws=state!='terminated' and int(millis)>0 and pending
                    expected=result(None if throws else state=='terminated','java.lang.InterruptedException' if throws else 'none',pending and not throws)
                elif mode.startswith('nio_'):
                    held=mode.endswith('held');expected=dict(error='java.lang.InterruptedException' if held else 'none',pending=not held,poolShutdown=True,serverShutdown=not held,running=held)
                else:
                    expected=dict(blocked=True)
                    if mode in ['complete','signal_first']:expected['results']=[result(True,pending=mode=='signal_first')]
                    else:
                        expected.update(beforeShutdown=True,results=[result(error='java.lang.InterruptedException',pending=mode=='reinterrupt')])
                        if mode=='multiple':expected.update(othersBlocked=True,results=expected['results']+[result(True)]*3)
                for row in rows:
                    for key,value in expected.items():check(row[key]==value,(case,key,row,expected))
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Actual Executors.newFixedThreadPool versus native FixedTaskExecutor; negative/zero/positive/saturated timeouts, preexisting/external interruption, real termination, independent waiters, repeated waits, signal-before-interrupt with controlled pool lock, and actual NIOManager hard/message shutdown with real held executor task and unstarted server. No injected InterruptedException, shortened source wait, user database or mainnet node. One native ordering fixture performs the existing empty-pool shutdown transition under the pool lock. All races, I/O, thread lifecycle and complete executor API remain unverified.'
        (build/'executor-interrupt-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
