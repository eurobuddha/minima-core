#!/usr/bin/env python3
"""Actual pinned NIOManager state transitions, no socket or node startup."""
import argparse,itertools,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def client(label='c0',map_='pending',**kw):
    return dict(label=label,map=map_,host='127.0.0.1',port=9001,**kw)
def message(type_,**kw):return dict(op='message',type=type_,**kw)
def config(**kw):return dict(op='config',**kw)
def cases():
    result=[]
    def add(group,clients,ops):result.append((group,dict(clients=clients,ops=ops)))
    for p2p,slave,attempts,peers,invalid,failure in itertools.product([False,True],[False,True],[-1,1,2,3,4,2147483647],['alone','pending','connected'],[False,True],['','p2p','timer']):
        clients=[client(attempts=attempts)]
        if peers!='alone':clients.append(client('c1',peers,channel=peers=='connected'))
        add('reconnect',clients,[config(p2p=p2p,slave=slave,invalid=['127.0.0.1:9001'] if invalid else [],failure=failure),message('NIO_RECONNECT',client='c0'),config(failure=''),message('NIO_RECONNECT',client='c0')])
    for p2p,slave,valid,incoming,reconnect,invalid,other,failure in itertools.product([False,True],[False,True],[False,True],[False,True],[None,False,True],[False,True],[False,True],['','p2p','timer']):
        clients=[client(map_='none',channel=True,valid=valid,incoming=incoming)]
        if other:clients.append(client('c1','connected',channel=True))
        request=message('NIO_DISCONNECTED',client='c0')
        if reconnect is not None:request['reconnect']=reconnect
        add('disconnected',clients,[config(p2p=p2p,slave=slave,invalid=['127.0.0.1:9001'] if invalid else [],failure=failure),request])
    for host,port,invalid in itertools.product(['127.0.0.1','',None],[-1,0,9001,65536],[False,True]):
        full=('null' if host is None else host)+':'+str(port)
        add('connect',[],[config(invalid=[full] if invalid else []),message('NIO_CONNECT',host=host,port=port)])
    for connectList,failure in itertools.product(['',',,,','bad','127.0.0.1:9001','a:1,,b:-1, :65536',', a:１２３ ,\t\n, b:0 ,garbage,','a:1,a:1,b:2147483648'],['','p2p','timer']):
        add('startup',[],[config(connectList=connectList,failure=failure),message('NIO_SERVERSTARTED')])
    for incoming,sent,invalid,failure in itertools.product([False,True],[False,True],[False,True],['','lock','unlock']):
        add('new-connection',[client(map_='connected',channel=True,incoming=incoming,sent=sent)],[config(invalid=['127.0.0.1:9001'] if invalid else [],failure=failure),message('NIO_NEWCONNECT',client='c0'),config(failure=''),message('NIO_NEWCONNECT',client='c0')])
    for age,incoming,valid,failure in itertools.product([-1000000,0,500000,700000,1200000],[False,True],[False,True],['','timer']):
        add('last-read',[client(map_='connected',channel=True,incoming=incoming,valid=valid)],[config(failure=failure),message('NIO_CHECKLASTMSG',age=age)])
    for p2p,remove,failure,minimaport in itertools.product([False,True],[False,True],['','p2p'],[-1,9002]):
        add('remove-peer',[client(map_='connected',channel=True,minimaport=minimaport)],[config(p2p=p2p,failure=failure),dict(op='disconnect',client='c0',remove=remove)])
    for uid in ['c0','missing',None]:
        add('disconnect',[client(),client('c1','connected',channel=True)],[message('NIO_DISCONNECT',uid=uid)])
    add('disconnect-all',[client(),client('c1','connected',channel=True)],[message('NIO_DISCONNECTALL')])
    add('cancel-attempt',[client(map_='none')],[message('NIO_CONNECTATTEMPT',client='c0')])
    for type_ in ['NIO_RECONNECT','NIO_DISCONNECTED','NIO_NEWCONNECT','NIO_CONNECTATTEMPT']:
        add('null-client',[],[message(type_,client=None)])
    for shutdown,restore,type_ in itertools.product([False,True],[False,True],['UNKNOWN','NIO_HEALTHCHECK','NIO_SERVERSTARTED','NIO_CONNECT','NIO_RECONNECT','NIO_NEWCONNECT','NIO_CHECKLASTMSG','NIO_SHUTDOWN',None]):
        add('guard',[client()],[config(shutdown=shutdown,restore=restore),message(type_,client='c0',host='127.0.0.1',port=9001)])
    add('hard-shutdown',[],[dict(op='hardShutdown'),dict(op='hardShutdown')])
    for limit,attempts in itertools.product([-1,0,3,2147483647],[-2147483648,-1,2147483647]):
        add('reconnect-limits',[client(attempts=attempts),client('c1','pending')],[config(limit=limit),message('NIO_RECONNECT',client='c0')])
    for exists,id_,uid,failure in itertools.product([False,True],['0x00','0xABCD','invalid',None],['c0','missing','',None],['','exists']):
        add('txpow-request',[client(map_='connected',channel=True)],[config(exists=exists,failure=failure),message('NIO_REQTXPOW',txpowid=id_,client=uid,reason=None)])
    roots=[None,dict(height=1,time=0),dict(height=2,time=0),dict(height=2,time=9223372036854775807)]
    for root,last,archive,disappear,failure in itertools.product(roots,roots,[False,True],[False,True],['','root','archive']):
        add('archive-sync',[client(map_='connected',channel=True)],[config(root=root,last=last,archive=archive,disappear=disappear,failure=failure),message('NIO_SYNCTXBLOCK',client='c0')])
    for populated in [False,True]:
        clients=[client(),client('c1','connected',channel=True,valid=True,maxima='key')] if populated else []
        ops=[]
        for method,value,only in itertools.product(['check','host','maxima','uid'],['127.0.0.1:9001','c1','key','missing',None],[False,True]):ops.append(dict(op='query',method=method,value=value,only=only))
        if populated:ops+=[dict(op='set',client='c1',maxima=None),dict(op='query',method='maxima',value='key')]
        add('queries',clients,ops)
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'nio-manager-state-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NIOManagerStateOracle.java']
    listing=build/'nio-manager-state-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();text=''.join(json.dumps(req)+'\n' for _,req in corpus)
    def execute(command):
        run=subprocess.run(command,input=text,capture_output=True,text=True,timeout=100,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if run.returncode:raise RuntimeError(f'{command}: {run.returncode}\n{run.stderr[-8000:]}')
        rows=[json.loads(line) for line in run.stdout.splitlines() if line.startswith('[') or line.startswith('"')]
        if len(rows)!=len(corpus):raise AssertionError(f'Response count {len(rows)} != {len(corpus)}\n{run.stderr[-8000:]}')
        return rows
    expected=execute(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOManagerStateOracle']);actual=execute([str(build/'minima_nio_manager_state')])
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if left!=right or not isinstance(left,list):
            detail={'case':i,'group':group,'request':request,'java':left,'cpp':right};(build/'nio-manager-state-failure.json').write_text(json.dumps(detail,indent=2)+'\n');raise AssertionError(f'Case {i}, {group}; see {build}/nio-manager-state-failure.json')
        if len(left)!=len(request['ops']):raise AssertionError(f'Incomplete program {i}')
        for operation,response in zip(request['ops'],left):
            if operation['op']=='config' and response['result']!='OK':raise AssertionError(f'Fixture setup failed in program {i}')
    # Guard against matching empty/error-only fixture implementations. These
    # are observable protocol outcomes, independent of the translated branches.
    packet_types=set();timer_types=set();peer_types=set();results=set()
    for program in expected:
        for response in program:
            state=response['state'];results.add(str(response['result']))
            for client_ in state['clients']:
                for packet in client_['packets'] or []:
                    if packet and len(packet)>=4:packet_types.add(int(packet[2:4],16))
            for timer in state['timers']:timer_types.add((timer['type'],timer['delay']))
            for peer in state['p2p']:peer_types.add(peer['type'])
    assert {0,3,13}<=packet_types,packet_types
    assert {('NIO_CONNECT',10000),('NIO_CONNECT',5000),('NIO_CONNECTATTEMPT',30000),('NIO_CHECKLASTMSG',120000)}<=timer_types,timer_types
    assert {'P2P_INIT','P2P_NOCONNECT','P2P_DISCONNECTED','P2P_REMOVE_PEER'}<=peer_types,peer_types
    assert {'OK','ERROR'}<=results,results
    groups={name:sum(group==name for group,_ in corpus) for name in sorted({group for group,_ in corpus})}
    report={'passed':True,'programs':len(corpus),'operations':sum(len(req['ops']) for _,req in corpus),'groups':groups,'scope':'Actual pinned NIOManager processMessage, queries and disconnect methods. Initial singleton/database/service receivers and timer/post collection replace full startup; real clients/maps and packet queues. Random UIDs projected by object identity; configured clients use deterministic UIDs before insertion. Generic exceptions normalized. Stale-read tests avoid wall-clock boundary ambiguity. No real connector, NIOMessage dispatch or full node in this fixture.'}
    (build/'nio-manager-state-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
