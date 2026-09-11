#!/usr/bin/env python3
"""Archive actions with real H2 files and cross-runtime raw/H2 exports."""
import argparse,copy,gzip,hashlib,json,re,shutil,subprocess,tempfile,time
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def project(value,max_millis):
    value=copy.deepcopy(value)
    if 'exception' in value:
        for kind in ['java.lang.NullPointerException','java.lang.ClassCastException','java.io.EOFException']:
            if value['exception'].startswith(kind):value['exception']=kind
    if isinstance(value.get('result'),dict) and 'time' in value['result'].get('response',{}):
        # This action reports wall time, not validation success. Require bounded format.
        duration=value['result']['response']['time'];match=re.fullmatch(r'0 Years 0 Months 0 Weeks 0 Days 0 Hours 0 Minutes (\d+) Seconds',duration);assert match and 0<=int(match[1])<=max_millis//1000+1,duration
        value['result']['response']['time']='<elapsed>'
    return value

def snapshot(folder):
    out={}
    for path in sorted(folder.rglob('*')):
        relative=str(path.relative_to(folder))
        if relative.startswith('input.'):continue
        if path.is_dir():out[relative]='directory'
        elif '.mv.db' not in path.name and '.trace.db' not in path.name:
            data=path.read_bytes();out[relative]=dict(size=len(data),sha256=hashlib.sha256(data).hexdigest())
    return out

def corpus(exports,blocks):
    cases=[]
    def add(params=None,**kw):cases.append(dict(params=dict(action='inspect') if params is None else params,**kw))
    add(op='meta')
    for baseline in ['empty','normal','gap','bad','coins','many']:
        add(dict(action='integrity'),baseline=baseline,expected='integrity')
        add(baseline=baseline,expected='inspect')
        for maximum in [None,'0','-1','1','2','256','1.9','4294967296']:
            p=dict(action='exportraw',file='output.dat')
            if maximum is not None:p['maxexport']=maximum
            add(p,baseline=baseline,expected='raw' if baseline!='empty' else None)
        add(dict(action='export',file='output.gzip'),baseline=baseline,expected='h2')
    for writer,files in exports.items():
        for suffix,data in files.items():
            for action in ['inspect','inspectraw','import','importold']:
                add(dict(action=action,file='input.'+suffix),data=data,suffix=suffix,recordSync=True,writer=writer)
            for cut in sorted({0,1,9,len(data)//2,len(data)-1}):
                for action in ['inspectraw' if suffix=='dat' else 'inspect','import']:
                    add(dict(action=action,file='input.'+suffix),data=data[:cut],suffix=suffix,recordSync=True)
            if suffix=='dat':
                add(dict(action='import',file='input.dat',phrase='fixture phrase',keys='2',keyuses='123',anyphrase='true'),data=data,recordSync=True,expected='forward')
                for name in ['input.DAT','input.dat.gzip']:
                    add(dict(action='import',file=name),data=data,suffix=name[6:],recordSync=True)
    for action in ['integrity','export','exportraw','inspect','inspectraw','import','importold','importraw','addresscheck','resync','unknown']:
        base=dict(action=action)
        for key in ['action','file','logs','maxexport']:
            for value in [None,False,1,{},[],'','  ']:add({**base,key:value})
        add(base,keys=False)
    for file in ['missing','missing.dat','directory','nul\x00file']:
        for action in ['inspect','import','importraw']:add(dict(action=action,file=file),obstruct=True)
    for address in ['0x01','0x02','MxINVALID',None]:
        for state in ['', '0xAABB','0xFFFF','MxBAD','MxBAD@label']:
            add(dict(action='addresscheck',address=address,statecheck=state),baseline='coins',treeBlock=blocks['coins'][-1])
    for message in [None,'','hello','π😄']:
        add(op='notify',message=message);add(op='notify',message=message,nullListener=True);add(op='notify',message=message,listenerThrow=True)
    for host in ['bad','archiverestore','localhost:no','localhost:-1']:
        add(dict(action='resync',host=host),keys=False)
        if host!='localhost:-1':add(dict(action='resync',host=host))
    for command in ['archive action:inspect','archive action:integrity','archive action:exportraw file:output.dat','archive action:inspect bad:1','archive action:import file:missing']:
        add(op='runner',command=command);add(op='runner',command=command,caller='0x01')
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--start-case',type=int,default=0);parser.add_argument('--end-case',type=int);args=parser.parse_args();build=args.build.resolve();oracle=build/'archive-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['ArchiveCommandOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']];listing=build/'archive-command-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ArchiveCommandOracle']),Runner([str(build/'archive_command_runtime')])];failures=[];completed=raw=exported=0;observations=[]
    try:
        with tempfile.TemporaryDirectory(prefix='archive-command-',dir=build) as scratch:
            root=Path(scratch);baselines={};blocks={}
            specs={'empty':dict(heights=[]),'normal':dict(heights=[1,2,3]),'gap':dict(heights=[1,3,4]),'bad':dict(heights=[1,2,3],badParent=True),'coins':dict(heights=[1,2,3],coins=True),'many':dict(heights=list(range(1,260)))}
            for name,spec in specs.items():
                path=root/name;path.mkdir();reply=runners[0].ask(dict(op='seed',folder=str(path),**spec));assert reply.get('seeded'),reply;baselines[name]=path;blocks[name]=reply['blocks']
            # Verify the native fixture constructors against the source too.
            check=root/'native-seed';check.mkdir();reply=runners[1].ask(dict(op='seed',folder=str(check),**specs['coins']));assert reply.get('seeded') and reply['blocks']==blocks['coins'],reply
            exports={}
            for name,runner in zip(['java','cpp'],runners):
                folder=root/('exports-'+name);shutil.copytree(baselines['normal'],folder);exports[name]={}
                for action,suffix in [('export','gzip'),('exportraw','dat')]:
                    reply=runner.ask(dict(folder=str(folder),params=dict(action=action,file='output.'+suffix)));assert reply.get('result',{}).get('status'),reply;exports[name][suffix]=(folder/('output.'+suffix)).read_bytes()
            assert exports['java']==exports['cpp'],'Archive export bytes differ'
            for i,case in enumerate(corpus(exports,blocks)):
                if i<args.start_case or (args.end_case is not None and i>=args.end_case):continue
                values=[];folder=root/'node'
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    shutil.copytree(baselines[case.get('baseline','normal')],folder)
                    if case.get('obstruct'):(folder/'archiverestore').mkdir();(folder/'archiverestore/old').write_text('old');(folder/'directory').mkdir()
                    if 'data' in case:(folder/('input.'+case.get('suffix','dat'))).write_bytes(case['data'])
                    request={k:v for k,v in case.items() if k not in ['data','suffix','baseline','expected','writer','obstruct']};request['folder']=str(folder)
                    before=time.monotonic();wall_before=int(time.time()*1000);raw_value=runner.ask(request);wall_after=int(time.time()*1000);value=project(raw_value,wall_after-wall_before);elapsed=time.monotonic()-before;assert elapsed<30,value;assert 'fixtureError' not in value,value;files=snapshot(folder)
                    result=value.get('result');response=result.get('response',{}) if isinstance(result,dict) else {}
                    output=response.get('path',response.get('file',''))
                    match=re.fullmatch(r'archivebackup-(\d+)\.(gzip|raw.dat)',Path(output).name)
                    if match and not case['params'].get('file'):
                        assert wall_before<=int(match[1])<=wall_after,(wall_before,match[1],wall_after)
                        old=Path(output).name;new='archivebackup-TIMESTAMP.'+match[2]
                        for key in ['file','path']:
                            if key in response:response[key]=response[key].replace(old,new)
                        if old in files:files[new]=files.pop(old)
                    if not case['params'].get('file'):
                        for name in list(files):
                            match=re.fullmatch(r'archivebackup-(\d+)\.(gzip|raw.dat)',name)
                            if match:
                                assert wall_before<=int(match[1])<=wall_after,(wall_before,match[1],wall_after)
                                files['archivebackup-TIMESTAMP.'+match[2]]=files.pop(name)
                    values.append(dict(reply=value,files=files))
                if values[0]!=values[1]:failures.append(dict(case=i,request={k:v for k,v in case.items() if k!='data'},java=values[0],cpp=values[1]));break
                value=values[0]['reply'];expected=case.get('expected');response=value.get('result',{}).get('response',{}) if isinstance(value.get('result'),dict) else {}
                if expected:
                    assert value.get('result',{}).get('status'),value
                    if expected=='integrity':
                        name=case['baseline'];assert response['valid'] is True,value
                        assert response['errors']==(1 if name=='gap' else 2 if name=='bad' else 0),value
                    if expected=='raw':
                        data=gzip.decompress((folder/'output.dat').read_bytes());assert data[0]==0 and len(data)>3,data[:10];raw+=1
                        if case['params'].get('maxexport') in ['0','-1','4294967296']:assert response['total']==1,value
                    if expected=='h2':assert b'CREATE' in gzip.decompress((folder/'output.gzip').read_bytes());exported+=1
                    if expected=='forward':assert value['events'][0]['params']==dict(action='resync',host='archiverestore',phrase='fixture phrase',keys='2',keyuses='123',anyphrase='true'),value
                observations.append(dict(case=i,action=case['params'].get('action'),exception='exception' in value,static=value['static']))
                completed+=1
    except Exception as error:failures.append(dict(case=completed,error=repr(error)))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,start_case=args.start_case,end_case=args.end_case,raw_exports=raw,h2_exports=exported,observations=observations,failures=failures,scope='Real H2 archive integrity/inspection/export and exact cross-runtime H2/raw bytes; malformed imports and scoped recursive resync forwarding, address events, listener and parser gates. Resync body/actual processor/network helpers require separate tests. Direct EOF/NPE/class-cast prose and wall-clock duration normalized. Full archive/node parity remains unverified.')
    (build/'archive-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
