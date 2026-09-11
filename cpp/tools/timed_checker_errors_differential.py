#!/usr/bin/env python3
"""Pinned timed/block checker catch ordering, retained flags and actual deadline."""
import argparse,itertools,json,os,re,subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def normalize(row):
    def text(value):
        if not isinstance(value,str):return value
        value=re.sub(r'Thread-\d+', 'Thread-<id>', value)
        value=re.sub(r'Block checker time : \d+ms','Block checker time : <ms>',value)
        return re.sub(r'java\.lang\.NullPointerException(?::.*)?','java.lang.NullPointerException:<diagnostic>',value)
    row['events']=[text(e) for e in row['events']];row['returned']=text(row['returned']);return row

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--deadline',action='store_true');args=ap.parse_args();build=args.build.resolve();name='timed-errors-deadline' if args.deadline else 'timed-errors';oracle=build/(name+'-oracle');oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','ConsensusOracle.java','TimedCheckerErrorsOracle.java']];listing=build/(name+'-java-sources.txt');listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TimedCheckerErrorsOracle'],[str(build/'timed_checker_errors')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'}
    p=subprocess.run(commands[0]+['fixture'],capture_output=True,text=True,timeout=20,env=env);assert p.returncode==0,(p.stdout,p.stderr);fixture=json.loads(p.stdout);(build/(name+'-fixture.json')).write_text(json.dumps(fixture)+'\n')
    mutations=['valid','chain','height','parents','time-before','time-after','difficulty','magic','count','root','duplicate','duplicate-null','null-block','null-parent','null-transactions','null-transaction','empty-mmr']
    cases=[(mode,mutation,'none',False) for mode,mutation in itertools.product(['block','timed'],mutations)]
    cases += [('only',m,'none',False) for m in mutations[:8]+['null-parent']]
    patterns=['runtime','error','oom','io','runtime,runtime','runtime,error','runtime,oom','runtime,none,error','runtime,none,oom','runtime,runtime,error','runtime,runtime,runtime']
    cases += [(mode,'chain',pattern,False) for mode,pattern in itertools.product(['block','timed','only'],patterns)]
    cases += [('primed','chain',pattern,False) for pattern in ['error','oom','runtime,error','runtime,oom','runtime,none,error','runtime,none,oom','runtime,runtime,error','runtime,runtime,runtime']]
    cases += [('stalled','chain','none',False),('interrupted','chain','none',False)]
    cases += [('timed','valid',kind,True) for kind in ['none','runtime','error','oom','io']]
    cases += [('timed','null-block','none',True),('fresh','valid','none',False),('fresh','chain','none',False)]
    cases += [(m,'empty-mmr',f,False) for m,f in itertools.product(['block','timed','primed'],['error','none,error','none,oom','runtime,runtime,error'])]
    if args.deadline:cases=[('fresh','chain','error',False)]
    failures=[]
    def run(command,case):
        p=subprocess.run(command+[str(v).lower() if isinstance(v,bool) else str(v) for v in case],input=json.dumps(fixture)+'\n',capture_output=True,text=True,timeout=135 if args.deadline else 15,env=env)
        assert p.returncode==0,(command,case,p.returncode,p.stdout,p.stderr)
        return normalize(json.loads(p.stdout))
    with ThreadPoolExecutor(max_workers=2) as executor:
        for case in cases:
            futures=[executor.submit(run,c,case) for c in commands];rows=[future.result() for future in futures]
            if case[0]=='interrupted':
                for row in rows:row['events']=sorted(row['events'])
            if rows[0]!=rows[1]:failures.append(dict(case=case,java=rows[0],cpp=rows[1]))
            if case[0]=='stalled':
                expected_stalled={'returned':True,'finished':True,'valid':False,'deadline':False}
                for label,row in zip(['java','cpp'],rows):
                    if any(row.get(k)!=v for k,v in expected_stalled.items()):failures.append(dict(case=case,runtime=label,expected=expected_stalled,actual=row))
            if case[0] in ['stalled','interrupted']:
                for label,row in zip(['java','cpp'],rows):
                    expected_event='workerInterrupted:'+str(case[0]=='stalled').lower()
                    if expected_event not in row['events']:failures.append(dict(case=case,runtime=label,expected=expected_event,actual=row))
                    if case[0]=='interrupted' and (row.get('threadInterrupted') is not False or row['returned'] is not False):failures.append(dict(case=case,runtime=label,actual=row))
            if args.deadline:
                expected={'returned':False,'finished':False,'valid':False,'deadline':True}
                for label,row in zip(['java','cpp'],rows):
                    if any(row.get(k)!=v for k,v in expected.items()):failures.append(dict(case=case,runtime=label,expected=expected,actual=row))
    report=dict(passed=not failures,cases=len(cases),failures=failures,scope='Actual pinned TxPoWChecker/TimedChecker and native implementation, source-generated serialized parent/block/duplicate-proof fixtures, direct and reused timed checks with known matching initial/final values, primed-valid escaping failures and a worker held until the checking call returns; header, duplicate traversal, MMR and null failures, runtime/Error/OOM/IO logger failures at nested catch levels, final block log propagation, retained finished/valid flags and default dispatch. Explicit handlers prevent fatal OOM shutdown. Dedicated deadline run verifies actual120000ms polling after worker Error; no shortened timeout. Automatic stack frames, generated thread counters, elapsed log milliseconds and JVM-generated null diagnostics excluded. Actual pending-caller interruption and worker interrupt after polling are checked; concurrent log ordering is excluded only in the interrupted case. Full runtime/thread lifecycle semantics not established.')
    (build/(name+'-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
