#!/usr/bin/env python3
"""Pinned Minima public wrappers and pre-startup exits; native process hooks."""
import argparse,itertools,json,os,random,select,signal,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def corpus():
    result=[dict(op='singleton')]
    texts=[None,'',';;;',' ','\t\r\n','; ;','unknown','unknown;hash data:hello','hash','HASH data:hello',
           'hash data:hello type:sha2','hash data:0x1234','hash data:hello type:invalid',
           'hash bad:x','hash data:"hello world"','hash data:{"x":[1,true,null]}',
           'hash data:hello;hash data:world','hash data:hello;unknown;hash data:end',
           'hash data:"one;two"','hash data:π😄中文','  hash data:hello  ',
           'automine','automine enable:true','quit bad:value']
    for text in texts:
        result.append(dict(text=text,arity=1))
        for pretty in [False,True]:
            result.append(dict(text=text,arity=2,pretty=pretty))
            for caller in [None,'0x00','0x1234','', 'π😄']:
                result.append(dict(text=text,arity=3,pretty=pretty,caller=caller))
    for text in ['quit','send','vault','rpc']:
        for caller in [None,'0x1234','']:
            for pretty in [False,True]:result.append(dict(text=text,arity=3,pretty=pretty,caller=caller))
    rng=random.Random(5564)
    for _ in range(150):
        data=''.join(rng.choice('abcXYZ0123 π😄中/,.:-_') for _ in range(rng.randrange(40)))
        text='hash data:'+json.dumps(data,ensure_ascii=False)+' type:'+rng.choice(['sha2','sha3'])
        text=rng.choice(['',' ','\t','\x00'])+text+rng.choice(['',' ','\r','\x1f'])
        result.append(dict(text=text,arity=3,pretty=rng.choice([False,True]),caller=rng.choice(['0x00','0x1234',None])))
    return result

def project(output):
    output=dict(output)
    if output.get('exception','').startswith('java.lang.NullPointerException:'):output['exception']='java.lang.NullPointerException'
    return output

def process_checks(build,classpath,folder,native_h2=False):
    environment={k:v for k,v in os.environ.items() if not k.lower().startswith('minima_')}
    environment.update(TZ='UTC',UBSAN_OPTIONS='halt_on_error=1')
    commands=[['java','-cp',classpath,'org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    count=0
    for args in [['-help'],['-unknown'],['--help'],['-PORT','1'],['-conf',str(folder/'missing.conf')]]:
        outputs=[]
        for command in commands:
            process=subprocess.run(command+args,capture_output=True,text=True,timeout=15,env=environment,cwd=folder)
            outputs.append((process.returncode,process.stdout,process.stderr))
        assert outputs[0]==outputs[1],(args,outputs)
        assert outputs[0][0]==1,outputs
        count+=1
    # The native executable must expose the missing provider and clean up its
    # started Main/timer workers. This is a limitation check, not Java parity.
    if not native_h2:
        process=subprocess.run(commands[1]+['-data',str(folder/'native-data'),'-noconnect'],capture_output=True,text=True,timeout=15,env=environment,cwd=folder)
        assert process.returncode==1,(process.returncode,process.stdout,process.stderr)
        assert 'MinimaDB requires native SQL and Main runtime services' in process.stdout,(process.stdout,process.stderr)
    starter_commands=[['java','-cp',classpath,'MinimaEntryOracle'],[str(build/'minima_entry_boundary')]]
    for mode in ['starter-null','starter-help','starter-unknown']:
        outputs=[]
        for command in starter_commands:
            process=subprocess.run(command+[mode],capture_output=True,text=True,timeout=15,env=environment,cwd=folder)
            lines=process.stdout.splitlines()
            if mode=='starter-null':assert lines.count('RETURNED')==1,lines
            outputs.append((process.returncode,sorted(line for line in lines if line!='RETURNED'),process.stderr))
        assert outputs[0]==outputs[1],(mode,outputs)
        assert outputs[0][0]==(0 if mode=='starter-null' else 1),outputs
        if mode=='starter-null':assert outputs[0][1]==['uncaught:Thread-0:java.lang.NullPointerException'],outputs
    return count,int(not native_h2),3

def hooks_checks(build,classpath):
    commands=[['java','-cp',classpath,'MinimaShutdownHooksOracle'],[str(build/'minima_shutdown_hooks')]]
    count=0
    for mode,n,sig in [(mode,n,None) for mode,n in itertools.product(['return','exit','halt','throw','late'],[1,4])]+[('signal',n,sig) for n,sig in itertools.product([1,4],[signal.SIGHUP,signal.SIGINT,signal.SIGTERM])]:
        outputs=[]
        for command in commands:
            process=subprocess.Popen(command+[mode,str(n)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,start_new_session=True,bufsize=0)
            try:
                assert select.select([process.stdout],[],[],10)[0],'Hook setup timeout'
                ready=process.stdout.readline();assert ready==b'READY\n',ready
                if sig is not None:process.send_signal(sig)
                out,err=process.communicate(timeout=15)
                outputs.append((process.returncode,sorted(out.decode().splitlines()),err.decode()))
            finally:
                if process.poll() is None:process.kill();process.communicate(timeout=5)
        assert outputs[0]==outputs[1],(mode,n,sig,outputs)
        code,lines,err=outputs[0]
        assert code==(128+sig if sig is not None else 7 if mode=='halt' else 9 if mode=='exit' else 0),(mode,outputs)
        assert len([line for line in lines if line.startswith('hook:')])==(0 if mode=='halt' else n),outputs
        assert not err,outputs
        if mode=='throw':assert any('uncaught:Thread-0:java.lang.IllegalStateException: fixture hook'==line for line in lines),outputs
        if mode=='late':assert 'late:java.lang.IllegalStateException' in lines,outputs
        count+=1
    return count

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--native-h2',type=int,choices=[0,1],default=0);args=parser.parse_args();build=args.build.resolve()
    oracle=build/'minima-entry-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/MinimaEntryOracle.java',ROOT/'support/MinimaShutdownHooksOracle.java']
    listing=build/'minima-entry-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    classpath=f'{oracle}:{JAVA}/jar/minima.jar';cases=corpus();failures=[];completed=processes=hooks=limitations=starters=0
    runners=[Runner(['java','-cp',classpath,'MinimaEntryOracle']),Runner([str(build/'minima_entry_boundary')])]
    try:
        for index,request in enumerate(cases):
            values=[project(runner.ask(request)) for runner in runners]
            if values[0]!=values[1]:failures.append(dict(case=index,request=request,java=values[0],cpp=values[1]));break
            if request.get('op')=='singleton':assert values[0]==dict(same=True,cleared=True),values
            if request.get('text')=='':assert values[0]['result']==('' if request.get('arity')==1 or request.get('pretty') else '[]'),values
            completed+=1
    except Exception as error:failures.append(dict(error=repr(error)))
    finally:
        for runner in runners:runner.close()
    if not failures:
        try:
            with tempfile.TemporaryDirectory(prefix='minima-entry-',dir=build) as scratch:processes,limitations,starters=process_checks(build,classpath,Path(scratch),bool(args.native_h2))
            hooks=hooks_checks(build,classpath)
        except Exception as error:failures.append(dict(process_error=repr(error)))
    report=dict(passed=not failures,wrapper_cases=completed,configuration_processes=processes,hook_process_pairs=hooks,native_limitation_processes=limitations,starter_processes=starters,failures=failures,
        scope='Unmodified pinned Minima direct wrapper overloads and real command registry, nullable caller ordering, exact formatted strings and zero/one/multi-result shapes; controlled singleton identity only. Real help/unknown/missing-config process exits plus three mainStarter null/help/unknown subprocesses. Native hooks compared with JVM Runtime for concurrent hooks, names, return/exit/halt, HUP/INT/TERM, throws and late registration. Only NPE prose normalized in wrappers; starter exception type/name checked, main/worker print order excluded. UTF-16 isolated surrogate command input, successful mainStarter, normal CLI/daemon Main integration, host lifecycle and resource exhaustion remain unverified; a build without native H2 explicitly asserts the missing-provider limitation; H2-enabled node startup is covered separately by native_node_startup.')
    (build/'minima-entry-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
