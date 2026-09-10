#!/usr/bin/env python3
"""Source peers command with actual Main/P2P queues, files and loopback HTTP."""
import argparse,itertools,json,shutil,subprocess,tempfile
from pathlib import Path
from main_runtime_differential import ROOT,JAVA,op,project as main_project
from peers_checker_differential import Runner
from rpc_client_differential import Server,normalize as rpc_normalize

def direct(**params):return op('direct',name='peers',params=params)
def corpus(server,root):
    result=[]
    def add(*steps,known=(),enabled=True,slave=False,peer='real',**kw):
        result.append(dict(ops=[op('peersSetup'),op('network',peersFixture=True,known=list(known),enabled=enabled,peer=peer,verified=kw.pop('verified',False),peerLogs=kw.pop('peerLogs',False)),op('prefs',values={'slavenode_enabled':slave}),*steps,op('networkSnapshot')],**kw))
    add(op('meta',name='peers'))
    for enabled,slave,action in itertools.product([False,True],[False,True],[None,False,0,{},[],'invalid']):add(direct(action=action),enabled=enabled,slave=slave)
    pools=[[],[{'host':'127.0.0.1','port':9001}],[{'host':'127.0.0.'+str(i+1),'port':9001+i} for i in range(30)],[{'host':'::1','port':1}]]
    for known,limit,action in itertools.product(pools,[-2,-1,0,1,19,20,29,1000,2**31-1],['list','forcecheck']):
        add(direct(action=action,max=str(limit)),known=known,verified=True,peerLogs=True,checkList=dict(max=limit,action=action))
    for known,limit in itertools.product(pools,[-1,0,2,1000]):add(op('peersList',max=limit),known=known,helper=True,limit=limit)
    for action,value in itertools.product(['list','forcecheck'],[None,False,True,0,{},[],'','a','1.9','-0.5','2147483648']):add(direct(action=action,max=value))
    for peer,action in itertools.product(['null','wrong'],['list','forcecheck','addpeers','publish','invalid']):add(direct(action=action,peerslist='',file='owner.txt'),peer=peer)
    for known in [[None],[{'host':'fixture.invalid','port':1,'unresolved':True}]]:
        for action in ['list','forcecheck','publish']:add(direct(action=action,file='unresolved.txt'),known=known)
    texts=['',',',',,,','bad',':','127.0.0.1:9001',' 127.0.0.1 : 9001 ','127.0.0.1:1,,bad,,127.0.0.2:2,','127.0.0.1:1,127.0.0.1:1','127.0.0.1:-1','127.0.0.1:65536','127.0.0.1:1,127.0.0.2:65536,127.0.0.3:3','127.0.0.1:0','127.0.0.1:65535','127.0.0.1:+12','127.0.0.1:１２','::1:9001','httpbad','127.0.0.1:2147483648','127.0.0.1:1.0']
    for text in texts+[None,False,True,0,{},[]]:add(direct(action='addpeers',peerslist=text))
    add(direct(action='addpeers'))
    address={'type':'PEERS_CHECKPEERS','host':'127.0.0.1','port':1,'force':True}
    add(direct(action='addpeers',peerslist='127.0.0.1:1,127.0.0.1:1'),expectedQueue=[address,address])
    add(direct(action='addpeers',peerslist='127.0.0.1:1,127.0.0.2:65536'),expectedQueue=[address],expectError=True)
    add(direct(action='forcecheck',max='bad'),expectedQueue=[{'type':'PEERS_FORCEFULLCHECK'}],expectError=True)

    for body,path,action in itertools.product(['','bad',',,,','127.0.0.1:1,bad,127.0.0.2:2','127.0.0.1:\n12','127.0.0.1:1,127.0.0.2:65536'],['/status/200','/status/404','/redirect/302?to=/status/200'],['addpeers','fetch']):
        url=server.url+path;add(direct(action=action,**{'peerslist' if action=='addpeers' else 'url':url}),body=body,http=True)
    for value in [None,False,0,{},[],'','bad URL','file:///unused-peers']:
        add(direct(action='fetch',url=value));add(direct(action='fetch',file=value,url=server.url),noHTTP=True)
    for known,name in itertools.product(pools,['peerslist.txt','nested/file.txt','existing.txt','directory','bad\0path']):
        add(direct(action='publish',file=str(Path(root)/name) if '/' in name else name),known=known,publish=True)
    add(direct(action='publish'),known=pools[1],publish=True)
    for value in [None,False,0,{},[]]:add(direct(action='publish',file=value))
    for caller,text in itertools.product(['0x00','0x1234'],['peers','peers action:forcecheck max:bad','peers action:addpeers peerslist:127.0.0.1:1','peers action:fetch file:local','peers invalid:true','peers action:list;peers action:addpeers peerslist:127.0.0.1:1']):add(op('run',caller=caller,text=text))
    return result

def project(raw,request,root):
    result=main_project(raw,request)
    known=request['ops'][1]['known'];possible={x['host']+':'+str(x['port']) for x in known if x and not x.get('unresolved')}
    if known and known[0] and known[0]['host']=='::1':possible={'0:0:0:0:0:0:0:1:1'}
    def selected(text,limit):
        fields=[] if not text else text.split(',');assert len(fields)==min(len(known),max(0,limit+1)),(fields,limit,known)
        assert len(set(fields))==len(fields) and set(fields)<=possible,(fields,possible)
        return '<validated subset:'+str(len(fields))+'>'
    for row,step in zip(result['ops'],request['ops']):
        if step['cmd']=='networkSnapshot':row['value']['known']=sorted(row['value']['known'],key=lambda x:x or '')
        if 'exception' in row:row['exception']=rpc_normalize({'exception':row['exception']})['exception']
        if 'checkList' in request and step['cmd']=='direct':
            reply=row['value'];response=reply['response'];limit=request['checkList']['max'];expected=min(len(known),max(0,limit+1))
            assert reply['status'] is True
            if step['params']['action']=='list':assert response['size']==expected;response['peerslist']=selected(response['peerslist'],limit)
            else:assert response['size']==max(1,expected)
        if request.get('helper') and step['cmd']=='peersList':row['value']=selected(row['value'],request['limit'])
        if request.get('publish') and step['cmd']=='direct' and 'value' in row:
            response=row['value']['response'];path=Path(response['file']);data=path.read_text();assert data==response['peers'];response['peers']=selected(data,20)
    files={}
    for p in Path(root).rglob('*'):
        if p.is_file():files[str(p.relative_to(root))]=p.read_bytes().hex()
    if request.get('publish'):
        for row,step in zip(result['ops'],request['ops']):
            if step['cmd']=='direct' and 'value' in row:
                path=Path(row['value']['response']['file']);files[str(path.relative_to(root))]=row['value']['response']['peers']
    if 'expectedQueue' in request:assert result['ops'][-1]['value']['peerQueue']==request['expectedQueue'],result['ops'][-1]
    if request.get('expectError'):assert 'exception' in result['ops'][-2],result['ops'][-2]
    result['files']=files
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'peers-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['MainRuntimeOracle.java','MainNetworkOracle.java','SqlDBOracle.java','NIOClientOracle.java','ArchiveManagerOracle.java']]
    listing=build/'peers-command-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar','-d',str(oracle),'@'+str(listing)],check=True)
    server=Server();failures=[];completed=operations=requests=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='peers-command-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MainRuntimeOracle',root]),Runner([str(build/'peers_command_runtime'),root])]
        try:
            cases=corpus(server,root)
            for i,request in enumerate(cases):
                server.server.body_override=request.get('body','').encode();outputs=[];traces=[]
                for name,runner in zip(['java','cpp'],runners):
                    for p in Path(root).iterdir():
                        if p.is_dir():shutil.rmtree(p)
                        else:p.unlink()
                    (Path(root)/'existing.txt').write_text('old fixture');(Path(root)/'directory').mkdir();(Path(root)/'directory/child').write_text('keep')
                    try:
                        raw=runner.ask(request);output=project(raw,request,root);trace=server.trace()
                        if request.get('noHTTP'):assert not trace,trace
                        for r in trace:assert r['method']=='GET' and not r['body'] and r['headers']['user-agent']=='Minima/1.0',r
                        outputs.append(output);traces.append(trace)
                    except Exception as error:failures.append(dict(case=i,runtime=name,error=repr(error),request=request,stderr=runner.errors(),raw=locals().get('raw')));break
                if failures:break
                if outputs[0]!=outputs[1] or traces[0]!=traces[1]:failures.append(dict(case=i,request=request,java=outputs[0],cpp=outputs[1],java_requests=traces[0],cpp_requests=traces[1]));break
                completed+=1;operations+=len(request['ops']);requests+=len(traces[0])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
            server.close()
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,http_requests_per_runtime=requests,failures=failures,scope='Actual peers command, unstarted Main/P2P/checker queues, real MiniFile bytes and loopback RPCClient requests. Random selections independently checked for membership/cardinality; enhanced exception and selected URL provider diagnostics normalized. No live admission, public DNS, arbitrary concurrent/runtime parity certification.')
    (build/'peers-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
