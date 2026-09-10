#!/usr/bin/env python3
"""Pinned swap-link transitions through actual native peer state and manager."""
import argparse,itertools,json,subprocess,re
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,execute,op,client,address

def cases():
    rows=[]
    def add(group,ops,**kw):rows.append((group,dict(swap=True,level=2,ops=ops,**kw)))
    def links(n,prefix='c',port=9001):return [dict(uid=f'{prefix}{i}',address=address(f'8.8.8.{i+1}',port)) for i in range(n)]
    for incoming,hostset,accept,duplicate in itertools.product([False,True],repeat=4):
        add('connected',[op('s_connected',incoming=incoming)]*2+[op('s_echo',host='8.8.4.4')],clients=[client()],hostSet=hostset,accept=accept,maps={'none':[dict(uid='old',address=address('8.8.8.8',0))] if duplicate else []},known=[address('8.8.4.4')])
    for host,port,incoming in itertools.product([None,'','127.0.0.1','::1'],[-1,0,65535,65536],[False,True]):
        add('connected-errors',[op('s_connected',incoming=incoming)],clients=[client(host=host,port=port)])
    for incoming,accept,noconnect,discovery,maximum,mode in itertools.product([False,True],[False,True],[False,True],[False,True],[-1,0,1,5],['normal','noMain','missingManager','pending']):
        cfg={mode:True} if mode in ['noMain','missingManager'] else {}
        c=client(incoming=incoming,connected=mode!='pending');g=dict(myMinimaPort=9002,isAcceptingInLinks=accept)
        add('greetings',[op('s_greeting',info=c,greeting=g,noconnect=noconnect)]*2,clients=[c],discovery=discovery,max=maximum,maps={'none':[dict(uid='c0',address=address('8.8.8.8'))]},**cfg)
    for info,greeting,nullstate in itertools.product([None,dict(uid=None,host='8.8.8.8'),dict(uid='c0',host=None),dict(uid='c0',host='8.8.8.8')],[None,{},dict(myMinimaPort=-1),dict(myMinimaPort=65536)], [False,True]):
        add('greeting-errors',[op('s_greeting',info=info,greeting=greeting,noconnect=True)],nullState=nullstate)
    for name in ['in','out','not','none']:
        for uid in ['c0',None,'missing']:
            maps={k:[dict(uid=uid,address=address())] for k in ['in','out','not','none']};maps[name]=None
            add('disconnect-partial',[op('s_disconnect',uid=uid)],maps=maps)
    add('disconnect',[op('s_disconnect',uid='c0')]*2,maps={k:[dict(uid='c0',address=address())] for k in ['in','out','not','none']},known=[address()])
    for secret,host in itertools.product([None,'','0x00','0x1','0x01','0x0102','0xzz',17],[None,'','127.0.0.1','::1','8.8.4.4']):
        add('ip-request',[op('s_request',host=host,message={'req_ip':secret})])
        add('ip-response',[op('s_response',message={'secret':secret,'res_ip':host})],secret='0x01',known=[address('8.8.4.4')])
    for secret,port in itertools.product([None,'0x','0x01'],[-1,0,9001,65536]):
        add('response-partial',[op('s_response',message={'secret':'0x01','res_ip':'8.8.4.4'})],secret=secret,minimaPort=port,known=[address('8.8.4.4')])
    for count,maximum,over,accept,missing in itertools.product([0,1,2,5,6],[-1,0,1,2,5],[False,True],[False,True],[False,True]):
        cs=[client(f'c{i}',f'8.8.8.{i+1}') for i in range(count)]
        add('walks',[op('s_balance'),op('s_join',target=6),op('s_in',target=6)]*2,clients=cs,maps={'in':links(count),'out':links(count),'not':links(2 if over else 0,'n')},max=maximum,maxNone=1,accept=accept,missingManager=missing,seed=5564)
    for name in ['in','out','not','none']:
        for raw in [None,[dict(uid='c0',address=None)]]:
            maps={'in':links(1),'out':links(1),'not':links(2,'n')};maps[name]=raw
            add('walk-errors',[op('s_balance'),op('s_join'),op('s_in')],clients=[client(host='8.8.8.1')],maps=maps,maxNone=0)
    add('wrap',[op('s_wrap',message=v) for v in [None,{},dict(a=1),dict(greeting={'myMinimaPort':9001})]])
    add('wrap-identity',[op('s_wrap_alias')])
    return rows

def canonical(program):
    # Preserve equality relationships and exact byte length of OS-generated secrets.
    # Fixed request inputs are short and remain byte-exact in this corpus.
    secrets={}
    def visit(value,key=None):
        if isinstance(value,dict):return {k:visit(v,k) for k,v in value.items()}
        if isinstance(value,list):
            values=[visit(v) for v in value]
            return sorted(values,key=lambda v:json.dumps(v,sort_keys=True)) if key=='knownPeers' else values
        if key in ['secret','req_ip'] and isinstance(value,str) and len(value) in [18,26]:
            assert re.fullmatch(r'0x[0-9A-F]+',value),value
            if value not in secrets:secrets[value]=f'random-{(len(value)-2)//2}-{len(secrets)}'
            return secrets[value]
        return value
    return visit(program)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'swap-links-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/P2PFunctionsOracle.java'];listing=build/'swap-links-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();requests=[request for _,request in corpus];expected=execute(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2PFunctionsOracle'],requests);actual=execute([str(build/'minima_p2p_functions')],requests)
    messages=set();flags=set();results=set();random_ip=0;walks=0
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if canonical(left)!=canonical(right) or not isinstance(left,list) or len(left)!=len(request['ops']):
            (build/'swap-links-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Case {i} ({group}); see {build}/swap-links-failure.json')
        for row in left:
            flags.update(row['greetings'])
            if isinstance(row['result'],(str,bool)):results.add(str(row['result']))
            messages.update(msg['type'] for msg in row['posted'])
            if isinstance(row['result'],list):
                for msg in row['result']:
                    messages.add(msg['type']);body=msg.get('json',{})
                    if 'req_ip' in body:
                        assert body['req_ip']==row['state']['secret'] and len(body['req_ip'])==26;random_ip+=1
                    if 'walk_links' in body:
                        assert len(body['walk_links']['secret'])==18;walks+=1
    assert {'P2P_SEND_MSG','P2P_SEND_DISCONNECT','NIO_DISCONNECT'}<=messages,messages
    assert flags=={True,False} and {'OK','ERROR','True','False'}<=results,(flags,results)
    assert random_ip>0 and walks>0,(random_ip,walks)
    report=dict(passed=True,programs=len(corpus),operations=sum(len(r['ops']) for _,r in corpus),random_ip_messages=random_ip,walk_messages=walks,groups={g:sum(group==g for group,_ in corpus) for g in sorted({g for g,_ in corpus})},scope='Pinned SwapLinksFunctions methods, full peer-map snapshots, actual native NIO client greeting flags and manager disconnect queues; exact deterministic results and partial effects. Only OS-generated secret bytes and independently shuffled greeting knownPeers order canonicalized; byte lengths/equality/reuse preserved. Main presence supplied as explicit receiver; full node/logger integration remains outstanding.')
    (build/'swap-links-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
