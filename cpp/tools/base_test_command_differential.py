#!/usr/bin/env python3
"""Actual pinned test command, real DB refresh internals, and full source demo."""
import argparse,hashlib,itertools,json,shutil,subprocess,tempfile,time
from pathlib import Path
from minima_db_differential import ROOT,JAVA,op,project
from peers_checker_differential import Runner
from fors_differential import checked_demo

def corpus():
    cases=[]
    def command(**kw):return op('command',name='test',params={},**kw)
    def add(*ops,**kw):cases.append(dict(ops=list(ops),**kw))
    add(command(mode='meta'))
    for loaded in [False,True]:
        prefix=[op('load')] if loaded else []
        add(*prefix,command(),command(),expectSuccess=loaded)
        for key,value in itertools.product(['show','action','unused'],[None,False,True,0,{},[],'','reopen','anything']):
            add(*prefix,op('command',name='test',params={key:value}),expectSuccess=loaded)
        for caller,text in itertools.product(['0x00','0x1234'],['test','test show:anything action:anything','test invalid:true','test;test']):
            add(*prefix,command(mode='runner',caller=caller,text=text))
        for fail,runtime in itertools.product(range(1,51),[False,True]):
            add(*prefix,command(fail=fail,runtimeFailure=runtime),op('info'))
    add(op('load'),op('hold'),op('create'),op('useHeld'),command())
    return cases

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();start=time.monotonic()
    oracle=build/'base-test-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['SqlDBOracle.java','NIOClientOracle.java','MinimaDBOracle.java','ArchiveManagerOracle.java']]
    listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];programs=operations=checks=0;transcript=hashlib.sha256();cases=corpus()
    with tempfile.TemporaryDirectory(prefix='base-test-command-',dir=build) as directory:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MinimaDBOracle',directory]),Runner([str(build/'minima_db'),directory])]
        try:
            for index,request in enumerate(cases):
                values=[]
                for runner in runners:
                    for child in Path(directory).iterdir():
                        if child.is_dir():shutil.rmtree(child)
                        else:child.unlink()
                    out=project(runner.ask(request),request)
                    if request.get('expectSuccess'):
                        row=out['ops'][-1];value=row['value'];assert value['status'] is True and 'response' not in value
                        assert row['logs'][0]=='About to close and reopen DBs' and row['logs'][-1]=='DBs reopened..';checks+=2
                    values.append(out)
                programs+=1;operations+=len(request['ops'])
                # Normalize only the private temporary directory for stable evidence.
                canonical=json.loads(json.dumps([request,*values]).replace(directory,'<TEST_ROOT>'))
                transcript.update(json.dumps(canonical,sort_keys=True,separators=(',',':')).encode())
                if values[0]!=values[1]:failures.append(dict(case=index,request=request,java=values[0],cpp=values[1]));break
            assert not runners[1].errors(),'Native DB fixture stderr: '+runners[1].errors()
        except Exception as error:failures.append(dict(phase='commands',programs=programs,error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    demos=[]
    try:
        for command in [['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.system.commands.base.test'],[str(build/'minima_base_test_demo')]]:
            demo=checked_demo(command,timeout=300);demos.append(dict(stdout=demo.stdout,stderr=demo.stderr,returncode=demo.returncode))
            # For a 32-byte input the envelope occupies exactly39bytes and its
            # base32 form has61digits plus'Mx'. The loop must report no exceptions.
            assert demo.stdout=='Start test..\nFinish test..\n',demo.stdout;checks+=1
    except Exception as error:failures.append(dict(phase='100000-address-demo',error=repr(error)))
    (build/'base-test-command-demo-results.json').write_text(json.dumps(demos,indent=2)+'\n')
    report=dict(passed=not failures,programs=programs,planned_programs=len(cases),operations=operations,independent_assertions=checks,source_demo_iterations_each=100000,source_demos_completed=len(demos),seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual registered test command using existing MinimaDB/ArchiveManager/TxPoWSqlDB and recording SQL; loaded/unloaded state, repeated calls, ignored parameters, caller permissions, 50 SQL failure points for checked/runtime failures, logs, identities, locks and state. Actual source mains each execute100000random-address iterations. JVM-specific diagnostic prose and temporary path normalized. Durable H2 command integration and the unused private classloader resource helper remain unverified.')
    (build/'base-test-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
