#!/usr/bin/env python3
"""Pinned peer helpers, seeded selection, native interfaces and HTTP loopback."""
import argparse,itertools,json,os,subprocess,threading
from http.server import BaseHTTPRequestHandler,ThreadingHTTPServer
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def op(name,**kwargs):return dict(op=name,**kwargs)
def client(uid='c0',host='8.8.8.8',port=9001,connected=True,**kw):return dict(uid=uid,host=host,port=port,connected=connected,**kw)
def address(host='127.0.0.1',port=9001,**kw):return dict(host=host,port=port,**kw)
def cases():
    result=[]
    def add(group,ops,**config):result.append((group,dict(ops=ops,**config)))
    hosts=[None,'','127.0.0.1','127.example','localhost','localhost.evil','LOCALHOST','10.1.1.1','100.1.1.1','0.1.1.1','169.1.1.1','172.1.1.1','198.1.1.1','192.1.1.1','11.1.1.1','8.8.8.8',':','a:1','::1','[::1]','a:b:c',' 127.0.0.1','１２７.0.0.1','host\u0000name']
    for host,port,allow,mode in itertools.product(hosts,[-1,0,9001,65536],[False,True],['none','pending','connected']):
        clients=[] if mode=='none' else [client('p0',host,port,mode=='connected')]
        add('connection-policy',[op('local',value=host),op('ipv6',value=host),op('check',host=host,port=port),op('connect',host=host,port=port)],clients=clients,allow=allow,level=2)
    for host,allow in itertools.product(hosts,[False,True]):
        full=('null' if host is None else host)+':9001';add('invalid-first',[op('check',host=host,port=9001)],invalid=[full],allow=allow,missingManager=True)
    for missing in [False,True]:
        add('queries',[op('all'),op('connected')]+[op('info',uid=uid) for uid in ['c0','p0','missing',None]]+[op('disconnect',uid=uid) for uid in ['c0',None,'']],clients=[client(valid=True),client('p0',None,-1,False)],missingManager=missing)
    for valid,missing in itertools.product([False,True],repeat=2):
        add('send',[op('send',uid=uid,message=message) for uid,message in itertools.product(['c0','','missing',None],[{},None,{'type':'greeting','peers':['127.0.0.1:9001'],'null':None,'unicode':'🐈'}])],clients=[client(valid=valid)],missingManager=missing)
    for level in [None,0,1,2]:add('logging',[op('log',value=value) for value in ['',None,'test','\n🐈']],level=level)
    add('invalid-set',[op(action,value=value) for value in [None,'','8.8.8.8:9001','😀','\ue000'] for action in ['invalid','addInvalid','invalid','addInvalid']]+[op('clearInvalid')])
    for failure in [None,'open','connect','input','close']:add('availability',[op('available')]*2,failure=failure)
    add('cached-locals',[op('saveLocal'),op('localAdd',value=None),op('locals'),op('localAdd',value='😀'),op('locals'),op('localClear'),op('locals')],locals=['localhost','127.0.0.1','\ue000'])
    add('real-locals',[op('locals'),op('saveLocal'),op('localAdd',value='fixture'),op('locals')])
    target=address();other=address(port=9002)
    for maps in [{},{'in':None},{'in':[dict(uid='c0',address=None)]},{'in':[dict(uid='missing',address=target)],'out':[dict(uid='p0',address=target)]},{'not':[dict(uid='c0',address=target)],'none':[dict(uid='p0',address=target)]},{'in':[dict(uid=None,address=target)]}]:
        add('map-precedence',[op('find',address=addr) for addr in [target,other,None]],maps=maps,clients=[client(host='127.0.0.1'),client('p0','127.0.0.1',9001,False)])
    for uid,addr,missing in itertools.product(['c0','missing',None],[target,None,other],[False,True]):
        add('map-search',[op('search',address=target,map=[dict(uid=uid,address=addr)])],clients=[client(host='127.0.0.1')],missingManager=missing)
    for seed in [0,1,-1,9223372036854775807,-9223372036854775808,5564]:
        operations=[]
        for repeat in range(12):
            for size in [0,1,2,3,5,16,17,31]:operations.append(op('select',list=[None if i==2 else address(port=9001+i) for i in range(size)]))
            operations.extend(op('draw',bound=bound) for bound in [0,-1,1,2,3,17,1073741825,2147483647])
        operations.append(op('select',list=None));add('random-selection',operations,seed=seed)
    return result
def execute(command,requests):
    r=subprocess.run(command,input=''.join(json.dumps(request)+'\n' for request in requests),capture_output=True,text=True,timeout=100,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
    if r.returncode:raise RuntimeError(f'{command}: {r.returncode}\n{r.stderr[-6000:]}')
    rows=[json.loads(line) for line in r.stdout.splitlines() if line.startswith(('[','"','true','false'))]
    if len(rows)!=len(requests):raise AssertionError(f'Response count {len(rows)}/{len(requests)}\n{r.stderr[-6000:]}')
    return rows
class Handler(BaseHTTPRequestHandler):
    protocol_version='HTTP/1.1'
    def log_message(self,*args):pass
    def do_GET(self):
        self.server.paths.append(self.path)
        if self.path=='/drop':self.close_connection=True;return
        if self.path.startswith('http://'):code=200;location=None
        elif self.path in ['/proxy','/proxy-success']:code=305;location=f'http://127.0.0.1:{self.server.proxy_port if self.path=="/proxy-success" else self.server.server_port}'
        elif self.path=='/loop':code=302;location='/loop'
        elif self.path.startswith('/redirect/'):
            _,_,code,end=self.path.split('/');code=int(code);location='/status/'+end
        else:code=int(self.path.split('/')[-1]);location=None
        self.send_response(code)
        if location:self.send_header('Location',location)
        self.send_header('Content-Length','0');self.end_headers()
class ProxyHandler(Handler):
    def do_GET(self):
        self.server.origin.paths.append('proxy:'+self.path)
        self.send_response(200);self.send_header('Content-Length','0');self.end_headers()
def transport(java,native):
    server=ThreadingHTTPServer(('127.0.0.1',0),Handler);server.daemon_threads=True;server.paths=[];worker=threading.Thread(target=server.serve_forever,daemon=True);worker.start()
    proxy=ThreadingHTTPServer(('127.0.0.1',0),ProxyHandler);proxy.daemon_threads=True;proxy.origin=server;server.proxy_port=proxy.server_port;proxy_worker=threading.Thread(target=proxy.serve_forever,daemon=True);proxy_worker.start()
    paths=[f'/status/{code}' for code in [200,201,204,206,300,301,302,303,304,305,306,307,308,400,401,403,404,407,500]]+['/drop','/loop']+[f'/redirect/{code}/{end}' for code,end in itertools.product([300,301,302,303,304,306,307,308],[200,404])]
    paths.extend(['/proxy','/proxy-success'])
    requests=[dict(url=f'http://127.0.0.1:{server.server_port}{path}') for path in paths]
    try:
        expected=execute(java,requests);java_paths=list(server.paths);server.paths.clear();actual=execute(native,requests);native_paths=list(server.paths)
        if expected!=actual or java_paths!=native_paths:raise AssertionError(f'HTTP mismatch\n{list(zip(paths,expected,actual))}\nJava paths {java_paths}\nNative paths {native_paths}')
        assert True in actual and False in actual
    finally:server.shutdown();server.server_close();worker.join();proxy.shutdown();proxy.server_close();proxy_worker.join()
    return len(paths)
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'p2p-functions-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/P2PFunctionsOracle.java'];listing=build/'p2p-functions-java-sources.txt';listing.write_text('\n'.join('"'+str(path)+'"' for path in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2PFunctionsOracle'];native=[str(build/'minima_p2p_functions')];corpus=cases();requests=[req for _,req in corpus];expected=execute(java,requests);actual=execute(native,requests)
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if left!=right or not isinstance(left,list) or len(left)!=len(request['ops']):
            (build/'p2p-functions-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Case {i}, {group}; see {build}/p2p-functions-failure.json')
    messages=set();types=set();results=set()
    for program in expected:
        for row in program:
            if isinstance(row['result'],(str,bool)):results.add(str(row['result']))
            messages.update(message['type'] for message in row['posted']);types.update(int(packet[2:4],16) for queue in row['queues'] for packet in queue)
    assert {'NIO_CONNECT','NIO_DISCONNECT'}<=messages and 7 in types,(messages,types)
    assert {'True','False','ERROR','OK'}<=results,results
    count=transport(java,native)
    report=dict(passed=True,programs=len(corpus),operations=sum(len(req['ops']) for _,req in corpus),http_cases=count,groups={group:sum(name==group for name,_ in corpus) for group in sorted({name for name,_ in corpus})},scope='Actual pinned P2PFunctions and UtilFuncs; real native manager/client maps, message queues and serialization. Log bodies compared without runtime prefix. Java seeded random selection/rejection, local-interface hostname set and cache ownership. Availability call sequence uses an offline URL provider; HTTP transport separately uses real loopback and compares outcomes and redirect paths. Default HTTPS certificate/provider/proxy/resource semantics and full logger/runtime bindings remain under review.')
    (build/'p2p-functions-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
