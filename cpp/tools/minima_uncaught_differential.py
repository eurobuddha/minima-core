#!/usr/bin/env python3
"""Pinned uncaught handler; fatal paths confined to isolated test children."""
import argparse,itertools,json,os,re,subprocess
from pathlib import Path
from minima_logger_differential import ROOT,JAVA,normalize

def cases():
    result=[]
    def call(text='message',id=0,**kw):return dict(op='call',id=id,text=text,**kw)
    def program(ops,default='prior'):
        result.append(dict(ops=[dict(op='default',name=default),dict(op='new')]+ops))
    for text,kind,main in itertools.product(['***','','message','π😄','a\x00b','x\r\ny','\ud800',None],['exception','error'],[False,True]):
        program([call(text,kind=kind,main=main,frames=['first','second']),call(text,kind=kind,main=main),call('next',main=main),call(text,kind=kind,main=main)])
    for flag,value,default in itertools.product(['nullThread','nullError','failDate','failPost','failStack','nullFrames','failDelegate','failStringAt','failPrintAt'],[1,2,3],[None,'prior']):
        kw={flag:value if flag.endswith('At') else True,'main':True}
        program([call(**kw),call(),call()],default)
    for frame in [[],[None],['first',None,'last'],['π','\ud800','\x00','line\nnext']]:
        program([call(frames=frame),call(frames=[]),call(frames=frame)])
    for texts in [['first','second'],['***','next'],[None,'second'],['first',None],['first','***']]:
        program([call(texts=texts),call(texts=texts),call('first'),call(None)])
    for kind in ['oom','store','timeout']:
        # Even fatal classes must return immediately for the initial "***".
        program([call('***',kind=kind,nullThread=True)])
        # Logging failure precedes halt, and prevents last-error assignment.
        for key in ['failDate','failPost','failStack','nullFrames','nullThread']:
            program([call(kind=kind,main=True,**{key:True}),call()])
    program([call(),dict(op='default',name='new-default'),call('second'),dict(op='new'),call('third'),call('fourth',id=1)])
    program([dict(op='default',name=None),dict(op='new'),call(),call(),call('second',id=1)])
    # Reinstalling captures the first instance; after successful logging it
    # suppresses its recursive delegation using the instance's last error.
    program([dict(op='bind',id=0),dict(op='new'),call('one',id=1),call('two'),call('two',id=1)])
    for thread in ['', 'π😄', '\ud800', 'a\x00b', 'x\nnext']:
        program([call(thread=thread),call(thread='different')])
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'uncaught-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','MinimaLoggerOracle.java','MinimaUncaughtOracle.java','UncaughtWorkersOracle.java']];listing=build/'uncaught-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MinimaUncaughtOracle'],[str(build/'minima_uncaught')]];env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'};corpus=cases();outputs=[]
    for command in commands:
        p=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in corpus)+'\n',text=True,capture_output=True,timeout=90,env=env)
        assert p.returncode==0,(command,p.returncode,p.stderr[-4000:]);rows=[normalize(json.loads(line)) for line in p.stdout.splitlines()];assert len(rows)==len(corpus);outputs.append(rows)
    failures=[dict(case=i,request=c,java=j,cpp=n) for i,(c,j,n) in enumerate(zip(corpus,*outputs)) if j!=n]
    processes=[]
    if not failures:
        for kind,boundary in itertools.product(['oom','store','timeout'],[False,True]):
            run=[]
            for command in commands:
                p=subprocess.run(command+[kind]+(['thread'] if boundary else []),capture_output=True,text=True,timeout=15,env=env)
                assert p.returncode==0,(command,kind,boundary,p.returncode,p.stderr,p.stdout)
                assert not any(s in p.stdout for s in ['SHUTDOWN-HOOK','RETURNED','DELEGATED']),p.stdout
                expected={'oom':'MEMORY ERROR','store':'H2 DATABASE MVStoreException','timeout':'Concurrent Timeout Exception'}[kind]
                assert expected+'.. SHUTTING DOWN' in p.stdout,p.stdout
                assert '[!] UNCAUGHT EXCEPTION at THREAD fatal-worker' in p.stdout,p.stdout
                if not boundary:run.append([normalize(line) for line in p.stdout.splitlines()])
            if not boundary:assert run[0]==run[1],run
            processes.append(dict(kind=kind,worker=boundary,passed=True))
    workers=[]
    if not failures:
        for mode in ['normal','exception','error','oom','store','timeout','cleanup','catchlog','logandcleanup','startlog','endlog','handlerthrows','timer']:
            rows=[]
            for command in [commands[0][:-1]+['UncaughtWorkersOracle'],[str(build/'uncaught_workers')]]:
                p=subprocess.run(command+[mode],capture_output=True,text=True,timeout=15,env=env)
                assert p.returncode==0,(command,mode,p.returncode,p.stdout,p.stderr)
                data=json.loads(p.stdout)
                data['events']=[re.sub(r'process_time_milli:-?\d+','process_time_milli:<time>',re.sub(r'Thread-\d+','Thread-<id>',re.sub(r'java.lang.NullPointerException.*','java.lang.NullPointerException',line))) for line in data['events']]
                rows.append(data)
            if rows[0]!=rows[1]:failures.append(dict(worker=mode,java=rows[0],cpp=rows[1]))
            workers.append(dict(mode=mode,passed=rows[0]==rows[1]))
    report=dict(passed=not failures,cases=len(corpus),processes=processes,workers=workers,failures=failures,scope='Unmodified pinned handler/logger/workers; explicit Throwable values/frames, controlled logger/date/Main and default handlers. Exact call order, duplicate/shared-default/null/logging failures. Native _Exit skips exit handlers for all 3 fatal types directly and on a real thread. Actual worker catch/finally/uncaught ordering, shutdown state and orphan timer; native/JVM frames and elapsed trace times differ. No Main/node/H2 backend started.')
    (build/'uncaught-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
