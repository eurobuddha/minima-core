#!/usr/bin/env python3
"""Actual nodecount traversal over isolated, scripted NIO loopback graphs."""
import argparse,copy,itertools,json,selectors,socket,struct,subprocess,tempfile,threading
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from nio_manager_transport_differential import receive,execute
from sql_db_differential import normalize

def local_cases():
    cases=[]
    def add(params=None,**kw):cases.append(dict(mode='direct',params=params or {},defaults=[])|kw)
    add(mode='meta')
    for complete,file in itertools.product([None,'true','false',True,False,1,{},[],'','TRUE',' true '],['','scan.csv']):add(dict(complete=complete,file=file),initial='0x414243')
    for value in [None,True,1,{},[],'','scan.csv','nul\0file','dir','dir/more.csv']:
        # Slash-containing paths are expanded into the temporary observation root.
        add(dict(file=value),directory='dir')
    add(defaults=None);add(defaults=[None]);add(dict(file='scan.csv'),defaults=None)
    for text,caller in itertools.product(['nodecount','nodecount complete:true','nodecount invalid:true','nodecount file:scan.csv complete:{}','nodecount;hash data:0x12'],['0x00','0x1234']):add(mode='run',text=text,caller=caller)
    for text in ['',' ','0','-0','-0.0','+0','1','1.25','.25','1.','1e-300','1e300','1e9999','-1e9999','1e-9999','1d','1F','0x1p0','-0X1.Ap+2','0x.1p-1','0x1','0xp1','0x1.0p0f','NaN','-NaN','+NaN','Infinity','-Infinity','+Infinity','nan','inf','NaNF','InfinityD','1_000','１２','١٢','1,2',' 1.5\t','\u00a01.5','\x001.5\x1f','1.2\x00x','2.2250738585072014e-308','4.9e-324','-4.9e-324','1.7976931348623157e308']:
        cases.append(dict(mode='double',text=text))
    return cases

def graphs():
    cases=[]
    def add(nodes,defaults=[0],params=None,**kw):cases.append(dict(nodes=nodes,defaults=defaults,params=params or {})|kw)
    for count,complete,csv in itertools.product([1,2,3,12,13,24,40],['true','false'],[False,True]):
        nodes=[dict(peers=list(range(count)),clients='99')]+[dict(peers=list(range(count)),clients=str(i)) for i in range(1,count)]
        add(nodes,params=dict(complete=complete,**({'file':'scan.csv'} if csv else {})),basic=count)
    for complete in ['true','false']:
        add([dict(peers=[1,2]),dict(peers=[2,3],clients='1.5'),dict(peers=[0,4],clients='2.5'),dict(peers=[4],clients='3.5'),dict(peers=[],clients='4.5')],params=dict(complete=complete,file='scan.csv'))
        add([dict(peers=[1,1,2,2]),dict(peers=[1,2],clients='2'),dict(peers=[0],clients='3')],defaults=[0,0],params=dict(complete=complete,file='scan.csv'))
    for clients in [None,True,False,1,1.25,{},[], '', 'bad','-1','-0','0.25','NaN','Infinity','-Infinity','1e100','0x1.8p2',' 2f ','1_000','１２']:
        add([dict(peers=[1]),dict(peers=[0],clients=clients)],params=dict(complete='true',file='scan.csv'))
    for extra in [None,{},dict(**{'peers-list':None}),dict(**{'peers-list':True}),dict(**{'peers-list':[]}),dict(**{'peers-list':[None]}),dict(**{'peers-list':[123]}),dict(**{'peers-list':['bad']}),dict(**{'peers-list':[':']}),dict(**{'peers-list':['127.0.0.1:bad']}),dict(**{'peers-list':['127.0.0.1:65536']})]:
        add([dict(extra=extra)],params=dict(file='scan.csv'))
        add([dict(peers=[1]),dict(extra=extra)],params=dict(complete='true',file='scan.csv'))
    for broken in ['empty','truncated','type-only']:
        add([dict(broken=broken)],params=dict(file='scan.csv'))
        add([dict(peers=[1]),dict(broken=broken)],params=dict(complete='true',file='scan.csv'))
    return cases

class Network:
    def __init__(self,nodes,wire):
        self.nodes=nodes;self.wire=wire;self.errors=[];self.received=[];self.done=threading.Event();self.selector=selectors.DefaultSelector();self.listeners=[]
        for i in range(len(nodes)):
            listener=socket.socket();listener.bind(('127.0.0.1',0));listener.listen();self.listeners.append(listener);self.selector.register(listener,selectors.EVENT_READ,i)
        self.ports=[s.getsockname()[1] for s in self.listeners]
        self.worker=threading.Thread(target=self.serve);self.worker.start()
    def reply(self,spec):
        if 'broken' in spec:return {'empty':b'\0\0\0\0','truncated':struct.pack('>i',100)+b'\x0c','type-only':struct.pack('>i',1)+b'\x0c'}[spec['broken']]
        extra=spec.get('extra',dict(**{'peers-list':[f'127.0.0.1:{self.ports[i]}' for i in spec.get('peers',[])]},**({'clients':spec['clients']} if 'clients' in spec else {})))
        payload=json.dumps(extra,ensure_ascii=False,separators=(',',':')).encode();end=4+struct.unpack('>i',self.wire[:4])[0];old=end+4+struct.unpack('>i',self.wire[end:end+4])[0]
        body=b'\x0c'+self.wire[:end]+struct.pack('>i',len(payload))+payload+self.wire[old:];return struct.pack('>i',len(body))+body
    def serve(self):
        try:
            while not self.done.is_set():
                for key,_ in self.selector.select(.05):
                    with key.fileobj.accept()[0] as sock:
                        sock.settimeout(10);size=struct.unpack('>i',receive(sock,4))[0];assert size==6,size;data=receive(sock,size);assert data==b'\x0b\0\0\0\x01\0',data
                        self.received.append(key.data);sock.sendall(self.reply(self.nodes[key.data]))
        except Exception as error:self.errors.append(repr(error))
    def close(self):
        self.done.set();self.worker.join(11)
        for sock in self.listeners:sock.close()
        self.selector.close();assert not self.worker.is_alive() and not self.errors,self.errors

def project(raw):
    value=copy.deepcopy(raw)
    if 'exception' in value:value['exception']=normalize(value['exception'])
    if isinstance(value.get('value'),list):
        for item in value['value']:
            if isinstance(item,dict) and 'error' in item:item['error']=normalize(item['error'])
    return value

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'node-count-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['NIOClientOracle.java','NIOManagerTransportOracle.java','NodeCountOracle.java']]
    listing=build/'node-count-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar'];failures=[];completed=pings=0;requests=local_cases();network_cases=graphs()
    with tempfile.TemporaryDirectory(prefix='node-count-',dir=build) as root:
        runners=[Runner(java+['NodeCountOracle',root]),Runner([str(build/'node_count_runtime'),root])]
        def compare(request,network=None,case=None):
            nonlocal completed,pings
            output=[];traces=[]
            for name,runner in zip(['java','cpp'],runners):
                if network:network.received.clear()
                value=project(runner.ask(request));output.append(value)
                if network:traces.append(network.received.copy())
            if output[0]!=output[1] or (network and traces[0]!=traces[1]):
                failures.append(dict(case=case,request=request,java=output[0],cpp=output[1],traces=traces));return False
            if network:
                pings+=len(traces[0]);assert output[0]['write']==0 and output[0]['traffic']['writes']['sendPingMessage']==str(6*len(traces[0]))+' bytes',output[0]
                if 'basic' in case:
                    count=case['basic'];response=output[0]['value']['response'];assert output[0]['value']['status'],output[0]
                    if request['params']['complete']=='true':assert len(traces[0])==count and response==dict(total_nodes=count+count*(count-1)/2,server_nodes=count,client_nodes=count*(count-1)/2),output[0]
                    if count==1 and request['params'].get('file'):assert output[0]['files']['scan.csv']=='',output[0]
            completed+=1;return True
        try:
            for i,request in enumerate(requests):
                if isinstance(request.get('params',{}).get('file'),str) and '/' in request['params']['file']:request['params']['file']=str(Path(root)/request['params']['file'])
                if not compare(request,case=i):break
            if not failures:
                wire=bytes.fromhex(execute(java+['NIOManagerTransportOracle'],[dict(mode='create',type=12,kind='greeting')])[0]['result'][2:])[1:]
                for case in network_cases:
                    network=Network(case['nodes'],wire)
                    try:
                        request=dict(mode='direct',params=case['params'],defaults=[dict(host='127.0.0.1',port=network.ports[i]) for i in case['defaults']])
                        if not compare(request,network,case):break
                    finally:network.close()
        except Exception as error:failures.append(dict(error=repr(error),case=locals().get('case'),request=locals().get('request')))
        finally:
            for runner in runners:
                try:runner.close()
                except Exception as error:failures.append(dict(cleanup=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(requests)+len(network_cases),ping_requests_per_runtime=pings,failures=failures,scope='Actual nodecount and NIO ping over isolated loopback graphs, exact ping request bytes/order, traffic, replies, logs and CSV files. Installed JDK collection order, float calculations and Double parser cases. Unstarted manager owner; no public network enumeration, full-node startup or exhaustive inherited/reference/concurrency/API parity claim. Native CSV descriptor closed by RAII; source writer lifetime not reproduced.')
    (build/'node-count-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
