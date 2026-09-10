#!/usr/bin/env python3
"""Actual pinned startup configuration: args/env/files, settings and HTTP effects."""
import argparse,itertools,json,os,random,re,shutil,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from rpc_client_differential import Server

def op(name,value=None,**kw):return dict(op=name,value=value,**kw)
def cases(keys,folder,url):
    result=[]
    def add(*ops,**kw):result.append(dict(ops=list(ops),**kw))
    add(op('args',[]),op('env',[]),op('conf',[]),op('configure'))
    for value in [None,[None],['-port',None],['word',None],['-unknown'],['-unknown\x00suffix'],['-port','-missing'],['--port'],['-'],['-PORT'],['port','9001'],['-port'],['-port',' 8888 '],['-port','8888','-port','7777']]:add(op('args',value),op('configure'))
    pathkeys={'data','basefolder','mdsinit'}
    for info in keys:
        key=info['key']
        if key in pathkeys or key=='help':continue
        for value in ['true','false','', ' TRUE ', 'π😄']:
            add(op('args',['-'+key,value]),op('configure'),op('configure'))
    for key in ['port','mysqldbdelay','hashtest','txpowdbstore','testchainlength']:
        for value in ['0','-1','+1','2147483647','2147483648','-2147483648','-2147483649','9223372036854775807','9223372036854775808','-9223372036854775808','-9223372036854775809','١٢٣','１２３','𝟙','3.5','1e2',' 24 ','+','\x003\x1f']:
            # Leading '-' is interpreted as an unknown option by program args;
            # environment input reaches the signed numeric consumer directly.
            add(op('env',[[f'MINIMA_{key.upper()}',value]]),op('configure'))
    for key in ['data','basefolder','mdsinit']:
        for suffix in ['/nested/deep','//double///path/','/dot/../inside','/file/child','/nul\x00suffix']:
            add(op('args',['-'+key,str(folder/('effects' if not suffix.startswith('/file/') else ''))+suffix]),op('configure'))
    for key in ['data','basefolder','mdsinit']:
        for suffix in ['link/../inside','link/missing/../inside']:
            add(op('args',['-'+key,str(folder/'effects'/suffix)]),op('configure'),symlinks=True)
    for value in [None,[[None,'x']],[['OTHER','x']],[['MINIMA_PORT',None]],[['MINIMA_PORT','9002'],['minima_port','9003']],[['MINIMA_UNKNOWN','x'],['minima_unknown','y']],[['MINIMA_HOST','MixedCase😄']],[['MINIMA_PORT','9002'],['MINIMA_RPCENABLE','true']]]:
        for lang in ['en','tr']:
            add(op('env',value),op('configure'),language=lang)
    for args in [[],['-test'],['-solo'],['-testchainlength','64','-test'],['-test','-testchainlength','64'],['-desktop','-server','-mobile','-isclient'],['-shownetcalls','-shownetcallsnopoll'],['-p2p-log-level-info','-p2p-log-level-debug']]:add(op('args',args),op('configure'))
    add(op('args',['-daemon','-noconnect','-showparams']),op('configure'),op('args',['-daemon','false','-noconnect','false','-showparams','false']),op('configure'),op('reset'),op('configure'))
    for field,value in [('MINIMA_VERSION','changed'),('MINIMA_BLOCK_SPEED','0.333'),('MINIMA_CASCADE_START_DEPTH','71'),('MINIMA_BLOCKS_SPEED_CALC','19'),('MINIMA_CASCADE_LEVEL_NODES',-1),('MEDIAN_BLOCK_CALC',17)]:
        add(op('setTest',value,field=field),op('applyTest'),op('setGlobal','other',field='MINIMA_VERSION'),op('applyTest'))
    for value in [None,'','word','-port 9999','-port\t9999','  -rpcenable  true  ','-port -2','-txpowdbstore 7','-testchainlength 17','-noshutdownhook false','-invalid','-seed word extra']:
        add(op('checkText',value))
    for value in [None,[],[None],['-port','3'],['-port','x'],['-test','true']]:add(op('checkArgs',value))
    filecases=[b'',b'port=9999\nhost=Mixed\r\nrpcenable=true\rdaemon=true',b'port=1=2\nunknown=x',b'port\n',b'port=1\nport=2',b' host=x\n#port=2\nport =4',b'=',b'==',b'\n\n=',b'host=\n',b'unknown=x\nunknown=y',b'\xef\xbb\xbfport=42',b'host=\xff',b'host=ok\nport=1\nport=2\nhost=\xff',b'host=\xed\xa0\x80',b'host=\xe2\x82',b'host=\xf0\x90\x80',b'host=\xf4\x90\x80\x80',b'host=\xef\xbf\xbd']
    for padding in [8177,8178,8179,8180,8181,8182,8183,8184,8191,8192,8193,16383]:
        for tail in [b'\xff',b'\xe2\x82',b'\xf0\x9f\x98\x84',b'\xed\xa0\x80']:
            filecases.append(b'port=1\nport=2\n'+b'#'*padding+tail)
            filecases.append(b'#'*padding+tail+b'\nport=1\nport=2')
    for i,body in enumerate(filecases):
        path=folder/f'config-{i}.conf';path.write_bytes(body)
        add(op('args',['-port','7777']),op('conf',['-conf',str(path)]),op('configure'))
    add(op('conf',['-conf',str(folder)]),op('configure'))
    add(op('conf',['-conf',str(folder/'nul\x00path')]),op('configure'))
    add(op('conf',None),op('configure'))
    add(op('conf',['-conf']),op('configure'))
    add(op('conf',['-conf',None]),op('configure'))
    valid=folder/'config-1.conf'
    add(op('conf',['-conf',str(valid)]),op('env',[['MINIMA_PORT','8888']]),op('args',['-port','7777']),op('configure'))
    for path in ['/status/200','/status/404','/drop']:
        add(op('args',['-p2pnodes',url+path]),op('configure'))
    rng=random.Random(90210);safe=[k['key'] for k in keys if k['key'] not in pathkeys|{'help','p2pnodes'}]
    for _ in range(100):
        values=rng.sample(safe,rng.randrange(1,len(safe)+1))
        env=[['MINIMA_'+k.upper(),rng.choice(['true','false','7',''])] for k in values]
        add(op('env',env),op('configure'))
    return result

def normalized(value):
    if isinstance(value,list):return [normalized(x) for x in value]
    if not isinstance(value,dict):return value
    out=dict(value)
    if 'stdout' in out:out['stdout']=re.sub(r'Minima @ [^\n]*? \[[^\n]*?\] : ','',out['stdout'])
    if 'exception' in out:
        kind=out['exception'].split(':',1)[0]
        if kind in ['java.lang.NullPointerException','java.lang.NumberFormatException','java.lang.IllegalArgumentException','java.lang.ArrayIndexOutOfBoundsException','java.lang.IndexOutOfBoundsException']:out['exception']=kind
    return out

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve();oracle=build/'param-configurer-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ParamConfigurerOracle.java',ROOT/'support/ParamConfigurerProcess.java',ROOT/'support/ParamConfigurerOriginalTests.java',JAVA/'test/org/minima/system/params/ParamConfigurerTest.java'];listing=build/'param-configurer-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar','-d',str(oracle),'@'+str(listing)],check=True)
    classpath=f'{oracle}:{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar';subprocess.run(['java','-cp',classpath,'ParamConfigurerOriginalTests'],check=True);java=Runner(['java','-cp',classpath,'ParamConfigurerOracle']);cpp=Runner([str(build/'minima_param_configurer')]);server=Server();failures=[];requests=0;process_cases=0
    try:
        metadata=java.ask(dict(kind='metadata'));keys=metadata['keys'];hashes=[k['hash'] for k in keys]
        with tempfile.TemporaryDirectory(prefix='config-fixtures-',dir=build) as scratch:
            folder=Path(scratch);(folder/'file').write_text('a file');corpus=cases(keys,folder,server.url)
            for index,request in enumerate(corpus):
                def effects(runner,request):
                    path=folder/'effects'
                    if path.exists():shutil.rmtree(path)
                    if request.get('symlinks'):
                        (path/'target/deep').mkdir(parents=True);(path/'link').symlink_to(path/'target/deep',target_is_directory=True)
                    output=normalized(runner.ask(request));trace=server.trace()
                    listing=sorted(str(p.relative_to(path)) for p in path.rglob('*')) if path.exists() else None
                    return output,trace,listing
                j,jtrace,jfiles=effects(java,request);c,ctrace,cfiles=effects(cpp,{**request,'hashes':hashes})
                if j!=c or jtrace!=ctrace or jfiles!=cfiles:failures.append(dict(index=index,request=request,java=j,cpp=c,javaRequests=jtrace,cppRequests=ctrace,javaFiles=jfiles,cppFiles=cfiles));break
                requests+=len(jtrace)
            if not failures:
                commands=[['java','-cp',classpath,'ParamConfigurerProcess'],[str(build/'param_configurer_process')]]
                for args in [['-help'],['-conf',str(folder/'missing')],['__test_version']]:
                    outputs=[]
                    for command in commands:
                        process=subprocess.run(command+args,capture_output=True,text=True,timeout=10);outputs.append((process.returncode,process.stdout))
                    assert outputs[0]==outputs[1],outputs
                    assert outputs[0][0]==(0 if args==['__test_version'] else 1)
                    if args==['__test_version']:assert outputs[0][1]=='before-TEST\nbefore-TEST\n'
                    process_cases+=1
    finally:
        java.close();cpp.close();server.close()
    report=dict(passed=not failures,cases=index+1,handlers=len(keys),requests_per_runtime=requests,process_cases=process_cases,failures=failures,scope='Actual pinned ParamConfigurer/TestParams against native globals, map contents/order and failure-side effects; Java enum identity hashes supplied to native map for identical arbitrary identity ordering. Argument/environment/conf precedence, all67consumers, strictUTF8, numeric bounds, lazy test settings, actual loopback HTTP. NPE and numeric diagnostics normalized. Full node/JVM-global semantics remain incomplete.')
    (build/'param-configurer-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
