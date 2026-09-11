#!/usr/bin/env python3
"""Actual Java/native peer checker using controlled real TCP ping replies."""
import argparse,itertools,json,os,select,socket,struct,subprocess,tempfile,threading
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,op,address
class Runner:
    def __init__(self,command,*,response_timeout=30):
        self.response_timeout=response_timeout
        self.err=tempfile.TemporaryFile(mode='w+t');self.child=subprocess.Popen(command,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=self.err,bufsize=0,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});self.buffer=b''
    def ask(self,request):
        self.child.stdin.write((json.dumps(request)+'\n').encode());self.child.stdin.flush()
        while True:
            while b'\n' not in self.buffer:
                if not select.select([self.child.stdout],[],[],self.response_timeout)[0]:raise TimeoutError('Checker oracle did not respond')
                chunk=os.read(self.child.stdout.fileno(),65536)
                if not chunk:raise RuntimeError('Checker oracle exited: '+self.errors())
                self.buffer+=chunk
            raw,self.buffer=self.buffer.split(b'\n',1);line=raw.decode()
            if line.startswith(('[','{','"','true','false','null')):
                try:return json.loads(line)
                except json.JSONDecodeError:pass
    def errors(self):self.err.seek(0);return self.err.read()[-6000:]
    def close(self,timeout=5):
        self.child.stdin.close()
        try:code=self.child.wait(timeout=timeout)
        except subprocess.TimeoutExpired:self.child.kill();self.child.wait();raise
        if code:raise RuntimeError(self.errors())
        self.err.close()
class Responder:
    def __init__(self):
        self.socket=socket.socket();self.socket.bind(('127.0.0.1',0));self.socket.listen();self.socket.settimeout(.1);self.port=self.socket.getsockname()[1];self.stop=False;self.lock=threading.Lock();self.replies=[];self.requests=[];self.thread=threading.Thread(target=self.run,daemon=True);self.thread.start()
    def configure(self,replies):
        with self.lock:self.replies=list(replies);self.requests=[]
    def trace(self):
        # A missing runtime receiver can fail after writing the request but before
        # reading its reply. Drain prior accepted connections before taking the
        # snapshot or changing the next program's replies. This control frame is
        # fixture-only and never enters either Minima implementation.
        with socket.create_connection(('127.0.0.1',self.port),timeout=5) as barrier:
            barrier.sendall(struct.pack('>i',-1))
            if self.read(barrier,1)!=b'!':raise RuntimeError('Responder barrier failed')
        with self.lock:return list(self.requests)
    @staticmethod
    def read(conn,n):
        result=b''
        while len(result)<n:
            value=conn.recv(n-len(result))
            if not value:break
            result+=value
        return result
    def run(self):
        while not self.stop:
            try:conn,_=self.socket.accept()
            except socket.timeout:continue
            except OSError:return
            with conn:
                conn.settimeout(3)
                try:
                    header=self.read(conn,4)
                    if len(header)!=4:continue
                    size=struct.unpack('>i',header)[0]
                    if size==-1:conn.sendall(b'!');continue
                    if size<0 or size>1024:raise AssertionError('Invalid ping frame')
                    payload=self.read(conn,size)
                    with self.lock:
                        self.requests.append((header+payload).hex());reply=self.replies.pop(0) if self.replies else None
                    if reply is not None:conn.sendall(bytes.fromhex(reply.removeprefix('0x')))
                except (ConnectionError,socket.timeout):pass
    def close(self):self.stop=True;self.socket.close();self.thread.join(timeout=5);assert not self.thread.is_alive()
def cases(peer,other,wires,ids,replies):
    result=[]
    def add(group,ops,reply='valid',**kw):result.append((group,dict(ops=ops,**kw),[replies.get(reply,reply)]*max(1,len(ops))))
    for host,allow in itertools.product([None,'','127.0.0.1','localhost','10.1.1.1','100.1.1.1','169.1.1.1','172.1.1.1','192.1.1.1','198.1.1.1','8.8.8.8','::1'],[False,True]):
        target=None if host is None else address(host,9001);add('admission',[op('have'),op('add',address=target)]*2+[op('identity')],allow=allow)
    for cap,existing,target in itertools.product([-1,0,1,2,250],[[],[peer],[None]],[None,peer,other]):
        # Avoid nondeterministic admission here; covered separately below.
        if existing and cap>0 and len(existing)>=cap and target not in existing:continue
        add('caps',[op('admit',address=target)]*2+[op('identity')],max=cap,verified=existing)
    for reply,connected,force,verified,logging in itertools.product(list(replies),[False,True],[False,True],[False,True],[False,True]):
        add('checks',[op('check',address=peer,force=force,forcelog=logging)]*2,reply=reply,connected=connected,verified=[peer] if verified else [],unverified=[] if verified else [peer],chain=wires,logs=logging)
    for reply,chain,test in itertools.product(['valid','future','past','wrong','missing','malformed','test'],[[],wires[:1],wires],[False,True]):
        add('chain',[op('check',address=peer,force=True,forcelog=True)],reply=reply,chain=chain,test=test,verified=[peer])
    for setting,reply in itertools.product(['missingParent','missingManager','noMain','tipFailure','timerFailure'],['valid','drop','wrong']):
        add('receivers',[op('check',address=peer,force=True,forcelog=True)]*2,reply=reply,chain=wires,verified=[peer],**{setting:True})
    for cap,existing in itertools.product([-1,0,1,250],[[],[other],[None],[peer]]):
        add('replacement',[op('check',address=peer,force=True)],max=cap,verified=existing,chain=wires)
    for offline,timerfail,known in itertools.product([False,True],[False,True],[[],[peer],[peer,other],[None]]):
        add('loops',[op('loop'),op('force')]*2,offline=offline,timerFailure=timerfail,verified=known)
    for value,key in itertools.product([None,0,1,'true',False,True],['force','forcelog']):
        add('flags',[op('check',address=peer,**{key:value})],chain=wires)
    for kind,connected,force in itertools.product(['check','raw','init'],[False,True],[False,True]):
        add('nulls',[op(kind,address=None,force=force),op(kind,address=None,nullMessage=True)],connected=connected)
    add('set-alias',[op('verify',address=peer),op('have'),op('identity'),op('clear'),op('have'),op('unverify',address=other),op('identity')])
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'peers-checker-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/P2PFunctionsOracle.java',ROOT/'support/PeersCheckerOracle.java'];listing=build/'peers-checker-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','PeersCheckerOracle']);native=Runner([str(build/'minima_peers_checker')]);server=Responder()
    try:
        fixture=[java.ask(dict(pow=True,height=n)) for n in [1,2]];wires=[p['wire'] for p in fixture];ids=[p['id'] for p in fixture]
        specifications={'valid':('1',ids[0]),'future':('10','0xAB'),'past':('0','0xAB'),'wrong':('1','0xAB'),'missing':('',ids[0]),'malformed':('abc',ids[0]),'test':('1',ids[0])}
        replies={name:java.ask(dict(encode=True,version='1.1.2.8-TEST' if name=='test' else '1.1.2.8',extra={'50block':block,'50hash':h})) for name,(block,h) in specifications.items()};replies['drop']=None;replies['truncated']='0x000000030C00';replies['unknown-type']=java.ask(dict(encode=True,type=99,version='1.1.2.8',extra={'50block':'1','50hash':ids[0]}))
        peer=address('127.0.0.1',server.port);other=address('127.0.0.2',9001);corpus=cases(peer,other,wires,ids,replies);types=set();delays=set();outcomes=set();pings=0
        for i,(group,request,reply) in enumerate(corpus):
            server.configure(reply);left=java.ask(request);java_trace=server.trace();server.configure(reply);right=native.ask(request);native_trace=server.trace()
            if left!=right or java_trace!=native_trace or not isinstance(left,list) or len(left)!=len(request['ops']):
                (build/'peers-checker-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right,java_trace=java_trace,cpp_trace=native_trace),indent=2)+'\n');raise AssertionError(f'Case {i} ({group}); see {build}/peers-checker-failure.json')
            pings+=len(java_trace)
            for row in left:
                outcomes.add(str(row['result']));types.update(message['type'] for message in row['parent']);delays.update(timer['delay'] for timer in row['timers'])
        assert {'P2P_ADD_PEER','P2P_REMOVE_PEER'}<=types and {60000,1800000,21600000}<=delays and {'True','False','OK','ERROR'}<=outcomes and pings>0,(types,delays,outcomes,pings)
        # Java constructs a fresh Random for each admission; compare invariants,
        # not coincidental choices. Each admitted candidate is distinct.
        stochastic={}
        for name,runner in [('java',java),('cpp',native)]:
            operations=[]
            for i in range(500):operations.extend([op('admit',address=address('1.1.1.1',20000+i)),op('clearPending')])
            request=dict(max=1,verified=[address('8.8.8.8',9001)],ops=operations)
            output=runner.ask(request);assert isinstance(output,list) and len(output)==1000
            accepted=sum(len(row['unverified']) for row in output[::2]);assert 15<=accepted<=100,(name,accepted)
            assert len(output[-1]['posted'])==accepted and len(output[-1]['verified'])==1
            stochastic[name]=accepted
        report=dict(passed=True,programs=len(corpus),operations=sum(len(r['ops']) for _,r,_ in corpus),tcp_pings=pings,stochastic_admissions=stochastic,groups={g:sum(group==g for group,_,_ in corpus) for g in sorted({g for g,_,_ in corpus})},scope='Actual pinned/native P2PPeersChecker and NIOManager ping over real loopback; exact request bytes, traffic, peer sets/order, parent messages, timer delays, log bodies and chain lookup effects. Source tree identities from Java fixtures. Fresh Random admission checked statistically with state invariants; nondeterministic multi-peer eviction, full runtime logger/timer lifecycle, concurrent mutation and generic exception identity remain open.')
        (build/'peers-checker-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
    finally:server.close();java.close();native.close()
if __name__=='__main__':main()
