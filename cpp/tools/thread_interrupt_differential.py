#!/usr/bin/env python3
"""Actual pinned workers/txnlock and JVM interrupt semantics; no injected sleeps."""
import argparse,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'thread-interrupt-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','ThreadInterruptOracle.java']]
    listing=build/'thread-interrupt-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    modes=['flags','external','lock_flags','hard_sleep','hard_lock','hard_unlock','soft_sleep','interrupted_waiter','hard_poll','timer_stop','timer_self','timer_null']
    report={'passed':False,'comparisons':0,'assertions':0,'cases':[],'scope':'Real interruptible sleep and pinned MessageProcessor/txnlock paths. No forced thread termination, arbitrary I/O interruption or complete scheduler interleaving parity claimed.'}
    path=build/'thread-interrupt-results.json'
    try:
        for mode in modes:
            rows=[]
            for command in [['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ThreadInterruptOracle'],[str(build/'thread_interrupt')]]:
                p=subprocess.run(command+[mode],capture_output=True,text=True,timeout=10,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
                assert p.returncode==0,(command,mode,p.returncode,p.stdout,p.stderr)
                rows.append(json.loads(p.stdout))
            report['cases'].append({'mode':mode,'java':rows[0],'cpp':rows[1]});assert rows[0]==rows[1],report['cases'][-1];report['comparisons']+=1
            for row in rows:
                if mode=='flags':
                    for item in row['rows']:
                        negative=int(item['delay'])<0
                        expected={'delay':item['delay'],'error':'java.lang.IllegalArgumentException' if negative else 'java.lang.InterruptedException','pending':negative,'consume':negative,'again':False}
                        assert item==expected,item;report['assertions']+=5
                    assert row['zero']==row['positive']=='returned';report['assertions']+=2
                elif mode=='external':
                    assert row=={'mode':mode,'blocked':True,'error':'java.lang.InterruptedException','pending':False};report['assertions']+=3
                elif mode=='lock_flags':
                    assert row=={'mode':mode,'acquired':True,'pending_after_acquire':True,'locked':False,'pending_after_unlock':True};report['assertions']+=4
                elif mode.startswith('timer_'):
                    assert row==dict(mode=mode,calls=1,before=False,after=True,pending=False,running=False,remaining=0),row;report['assertions']+=6
                elif mode=='hard_poll':
                    assert row==dict(mode=mode,remaining=0,timers=1,cleanup=1,running=False,shutdown=True),row;report['assertions']+=5
                else:
                    soft=mode=='soft_sleep'
                    assert row==dict(mode=mode,error='none' if soft else 'java.lang.InterruptedException',pending=False,returned=soft,cleanup=1,locked=True,running=False,shutdown=True,waiter_pending=False),row;report['assertions']+=8
        report['passed']=True
    finally:path.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='cases'},indent=2))
if __name__=='__main__':main()
