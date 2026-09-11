#!/usr/bin/env python3
"""Actual TimerProcessor methods under source-observable reentrant callbacks."""
import argparse,itertools,json,os,subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'timer-reentrancy-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','TimerReentrancyOracle.java']]
    listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TimerReentrancyOracle'],[str(build/'timer_reentrancy')]]
    cases=list(itertools.product(['print','run'],['read','append','throw','print','run','nullpost','log_append','nullentry'],[0,1,2]))
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def run(command,case):
        p=subprocess.run(command+list(map(str,case)),capture_output=True,text=True,timeout=10,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if p.returncode:raise AssertionError((case,p.returncode,p.stdout,p.stderr))
        return json.loads(p.stdout)
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            for case in cases:
                futures=[pool.submit(run,command,case) for command in commands];rows=[f.result() for f in futures];report['observations'].append(dict(case=case,java=rows[0],cpp=rows[1]));check(rows[0]==rows[1],report['observations'][-1]);report['comparisons']+=1
                mode,action,count=case;active=count>0;fails=active and action in ['append','log_append','throw'] or action=='nullentry' and mode=='print'
                error='java.lang.NullPointerException' if action=='nullentry' and mode=='print' else 'java.lang.IllegalStateException' if active and action=='throw' else 'java.util.ConcurrentModificationException' if active and action in ['append','log_append'] else 'none'
                original=['A','B'][:count]+([None] if action=='nullentry' else [])+(['C'] if active and action in ['append','log_append'] else [])
                replaced=mode=='run' and not fails or mode=='print' and active and action=='run'
                remaining=['B'] if count==2 else []
                for row in rows:
                    expected=dict(error=error,pending=mode=='run' and fails,running=not replaced,original=original,remaining=remaining if replaced else original,sameList=not replaced)
                    for key,value in expected.items():check(row[key]==value,(case,key,row,expected))
                    if active:check(bool(row['events']),(case,'missing callback'))
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='48 actual pinned/native TimerProcessor print/run programs, reentrant size/append/nested print/nested run/throw/null-post/logger append, internal null entry, empty and one/two-entry queues, retained list identity and ordered logs. Unsafe Java/manual native timer instances avoid unrelated factory thread; real run and original one-second sleep execute with a pending interrupt to bound one iteration. Only stack frame text and nondeterministic logger prefixes are excluded. No full lifecycle, arbitrary subclass API or all concurrent schedules certification.'
        (build/'timer-reentrancy-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
