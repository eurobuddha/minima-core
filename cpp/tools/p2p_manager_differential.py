#!/usr/bin/env python3
"""Compare actual pinned manager methods and their ordered partial effects."""
import argparse,itertools,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,execute,op,client,address
from swap_links_differential import canonical as secrets_canonical

def process(kind,**kw):return op('process',type=kind,**kw)
def cases():
    rows=[]
    def add(group,ops,**kw):rows.append((group,dict(ops=ops,**kw)))
    def links(n):return [dict(uid=f'c{i}',address=address(f'8.8.8.{i+1}')) for i in range(n)]
    status=[op('status'),op('status',full=True),op('clients'),op('have'),op('copy')]
    for noconnect,accept,hostset,first,saved in itertools.product([False,True],repeat=5):
        add('initialization',[process('P2P_INIT')]*2+status,noconnect=noconnect,accept=accept,hostSet=hostset,first=first,db={'peers':[address('8.8.8.8'),address('8.8.4.4',9002)]} if saved else {})
    for root in ['8.8.8.8:9001','8.8.8.8:0','8.8.8.8:65536',':9001','::9001','8.8.8.8:',':','bad','8.8.8.8:9001:ignored','8.8.8.8:+9001','8.8.8.8: 9001','8.8.8.8:９００１']:
        add('root-parsing',[process('P2P_INIT')]+status,root=root)
    for nodes in ['8.8.8.8:9001',' ,,, ','bad,8.8.8.8:9001,,8.8.4.4:9002',' 8.8.8.8:9001 ,8.8.8.8:9001',':9001,8.8.8.8:-1,8.8.8.8:65536']:
        for noconnect in [False,True]:add('addnodes',[process('P2P_INIT'),process('P2P_SAVE_DATA')]+status,addnodes=nodes,noconnect=noconnect)
    for initial,cap,known in itertools.product([-5,0,1,2,3,4,10],[-1,0,1,2],[[],[address('8.8.8.8')],[address('8.8.8.8'),address('8.8.4.4')]]):
        add('admission-persistence',[op('have'),process('P2P_ADD_PEER',address=address('8.8.8.8')),process('P2P_ADD_PEER',address=address('8.8.4.4')),op('copy'),process('P2P_SAVE_DATA'),process('P2P_REMOVE_PEER',address=address('8.8.8.8')),op('update'),op('shutdown'),op('have')],initial=initial,cap=cap,known=known,db={'peers':[address('1.1.1.1')]})
    for field in ['defaults','verified','unverified','known']:
        add('peer-precedence',[op('failures',db=True),op('have'),op('failures'),op('have')],**{field:[address('8.8.8.8')]})
    add('empty-peers',[op('have'),op('copy'),op('update'),op('shutdown'),op('have')])
    for db in [{'peers':None},{'peers':17},{'peers':['bad']},{'version':17},{'peers':[]}]:
        add('database-errors',[process('P2P_INIT'),process('P2P_SAVE_DATA'),op('shutdown'),op('have')],db=db)
    for db,save in itertools.product([False,True],repeat=2):
        add('receiver-failures',[op('failures',db=db,save=save),process('P2P_INIT'),process('P2P_SAVE_DATA'),op('shutdown'),op('failures'),process('P2P_SAVE_DATA'),op('shutdown')],known=[address('8.8.8.8')])
    for size in [0,49,50,51,52]:
        add('queue-overload',[op('post',type='discard',count=size),op('post',type='P2P_LOOP'),op('post',type='P2P_SAVE_DATA'),op('post',type='P2P_ASSESS_CONNECTIVITY'),process('P2P_HEALTH_CHECK')]*2,healthDelay=-1)
    for count,maximum,noconnect,accept,startup in itertools.product([0,1,2,5],[-1,0,2,5],[False,True],[False,True],[False,True]):
        add('loop-routing',[process('P2P_LOOP'),process('P2P_ASSESS_CONNECTIVITY'),process('P2P_RESET'),process('P2P_RANDOM_CONNECT')]+status,max=maximum,stateNoConnect=noconnect,noconnect=noconnect,stateAccept=accept,accept=accept,startup=startup,known=[address('8.8.4.4')],maps={'out':links(count),'in':links(count),'all':links(count)},clients=[client(f'c{i}',f'8.8.8.{i+1}',valid=True) for i in range(count)])
    for variability,delay in itertools.product([-1,0,1,2147483647],[-2147483648,0,2147483647]):
        add('loop-numbers',[process('P2P_LOOP')]*3,variability=variability,loopDelay=delay,max=0,stateNoConnect=True)
    for noconnect,startup in itertools.product([False,True],repeat=2):
        add('empty-loop',[process('P2P_LOOP')]*2+[process('P2P_INIT'),process('P2P_LOOP')],stateNoConnect=noconnect,noconnect=noconnect,startup=startup,db={'peers':[address('8.8.8.8')]})
    for count,minimum in itertools.product([0,1,3],[-2,0,2,3]):
        add('client-fractions',status,min=minimum,maps={'none':links(count)},my=None if count==3 else address('::1'))
    for name,raw in itertools.product(['in','out','none','not','all'],[None,[dict(uid='c0',address=None)],[dict(uid=None,address=address('8.8.8.8'))]]):
        add('map-errors',status+[process('P2P_ASSESS_CONNECTIVITY'),process('P2P_NOCONNECT',client=0),process('P2P_DISCONNECTED',uid='c0'),process('P2P_LOOP')],maps={name:raw},clients=[client()],known=[address('8.8.4.4')])
    for incoming,connected,valid in itertools.product([False,True],repeat=3):
        add('connection-lifecycle',[process('P2P_CONNECTED',uid='c0',client=0),process('P2P_CONNECTED',uid='c0',client=0),process('P2P_NOCONNECT',client=0),process('P2P_DISCONNECTED',uid='c0')]+status,clients=[client(incoming=incoming,connected=connected,valid=valid)],known=[address('8.8.8.8')])
    for payload,uid in itertools.product([None,17,{},dict(swap_links_p2p=None),dict(swap_links_p2p=17),dict(swap_links_p2p={})],['c0','missing',None]):
        add('json-casts',[process('P2P_MESSAGE',uid=uid,message=payload)],clients=[client(valid=True)])
    bodies=[{'req_ip':'0x01'},{'res_ip':'8.8.4.4','secret':'0x'},{'res_ip':'8.8.4.4','secret':'0x01'},{'notAcceptingMsg':False},{'do_swap':{'swap_target_host':'8.8.4.4','swap_target_port':9001}}, {'do_swap':{}},{'greeting':None},{'greeting':17}]
    for accept,noconnect in itertools.product([False,True],repeat=2):
        bodies.append({'greeting':{'myMinimaPort':9002,'isAcceptingInLinks':accept,'knownPeers':[address('1.1.1.1'),address('8.8.4.4')]},'notAcceptingMsg':noconnect})
    for body in bodies:
        add('json-routing',[process('P2P_MESSAGE',uid='c0',message={'swap_links_p2p':body})]*2+status,clients=[client(valid=True)],maps={name:[dict(uid='c0',address=address('8.8.8.8'))] for name in ['all','in','out','none','not']},stateNoConnect=True)
    for incoming,accept,noconnect in itertools.product([False,True],repeat=3):
        greeting={'myMinimaPort':9002,'isAcceptingInLinks':accept,'knownPeers':[address('8.8.4.4')]}
        add('greetings',[process('P2P_MESSAGE',uid='c0',message={'swap_links_p2p':{'greeting':greeting}})]*2+status,clients=[client(valid=True,incoming=incoming)],stateNoConnect=noconnect)
    for returning,walkin,clientwalk,path in itertools.product([False,True],[False,True],[False,True],[[],[address()],[address('8.8.8.8'),address()]]):
        walk=dict(secret='0x01',walkInLinks=walkin,isReturning=returning,isClientWalk=clientwalk,availableClientSlots=2,pathTaken=path)
        add('walk-routing',[process('P2P_MESSAGE',uid='c0',message={'swap_links_p2p':{'walk_links':walk}})],my=address(),clients=[client(valid=True)],maps={'in':[dict(uid='c0',address=address('8.8.8.8'))],'out':[dict(uid='c0',address=address('8.8.8.8'))]})
    for kind in ['P2P_INIT','P2P_CONNECTED','P2P_NOCONNECT','P2P_MESSAGE','P2P_ADD_PEER','P2P_REMOVE_PEER','unknown']:
        add('missing-payloads',[process(kind),process(kind,client=-1),process(kind,nullMessage=True)])
    return rows

def canonical(program):
    # Decode type-7 MiniString envelopes, retaining exact payload byte lengths.
    # Existing transport tests compare serialization; this suite compares routed
    # JSON including random-secret identity and knownPeers shuffle permutations.
    data=json.loads(json.dumps(program))
    if isinstance(data,list):
        for row in data:
            for queue in row['queues']:
                for i,packet in enumerate(queue):
                    raw=bytes.fromhex(packet[2:]);assert raw[0]==7 and int.from_bytes(raw[1:5],'big')==len(raw)-5
                    queue[i]=dict(type=7,length=len(raw),json=json.loads(raw[5:]))
    return secrets_canonical(data)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'p2p-manager-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','P2PFunctionsOracle.java','PeersCheckerOracle.java','P2PManagerOracle.java']];listing=build/'p2p-manager-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();requests=[request for _,request in corpus];expected=execute(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2PManagerOracle'],requests);actual=execute([str(build/'minima_p2p_manager')],requests)
    results=set();messages=set();packets=0;successful_groups=set();greeting_flags=set()
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if canonical(left)!=canonical(right) or not isinstance(left,list) or len(left)!=len(request['ops']):
            (build/'p2p-manager-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Case {i} ({group}); see {build}/p2p-manager-failure.json')
        for row in left:
            if isinstance(row['result'],(str,bool)):results.add(str(row['result']))
            messages.update(m['type'] for m in row['posted']);messages.update(m['type'] for m in row['checker']);greeting_flags.update(row['greetings'])
        if any(row['result']=='OK' for row in left):successful_groups.add(group)
        packets+=sum(len(queue) for queue in left[-1]['queues'])
    assert {'OK','ERROR','True','False'}<=results,results
    assert {'NIO_CONNECT','NIO_DISCONNECT','PEERS_ADDPEERS','PEERS_CHECKPEERS'}<=messages,messages
    assert packets>0 and greeting_flags=={False,True}
    assert {'initialization','greetings','walk-routing','json-routing','connection-lifecycle'}<=successful_groups,successful_groups
    report=dict(passed=True,programs=len(corpus),operations=sum(len(r['ops']) for _,r in corpus),packets=packets,groups={g:sum(group==g for group,_ in corpus) for g in sorted({g for g,_ in corpus})},scope='Actual pinned P2PManager process/getter/persistence methods; native manager, state, checker queue, DB, NIO clients, timers and ordered partial effects. Log bodies, routing payload JSON and wire lengths compared; OS-generated secrets projected by byte length and identity, greeting knownPeers compared as permutations. Unstarted Java/native fixtures; actual discovery TCP/retry and full runtime bindings remain pending. Constructor and actual worker lifecycle are separately native-tested.')
    (build/'p2p-manager-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
