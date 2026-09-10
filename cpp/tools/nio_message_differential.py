#!/usr/bin/env python3
"""Pinned NIOMessage.run against native dispatch; no node or socket startup."""
import argparse,itertools,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def execute(command,requests):
    data=''.join(json.dumps(request)+'\n' for request in requests)
    run=subprocess.run(command,input=data,capture_output=True,text=True,timeout=100,
                       env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
    if run.returncode:raise RuntimeError(f'{command}: {run.returncode}\n{run.stderr[-8000:]}')
    rows=[json.loads(line) for line in run.stdout.splitlines() if line.startswith(('[','{','"'))]
    if len(rows)!=len(requests):raise AssertionError(f'Response count {len(rows)} != {len(requests)}\n{run.stderr[-8000:]}')
    return rows

def cases(java):
    samples=execute(java,[dict(kind='pow',type=4,height=height,noBody=no_body) for height,no_body in [(1,False),(2,False),(3,True)]])
    root,pow_,nobody=samples
    specifications=[]
    def sample(name,**request):specifications.append((name,request))
    for type_ in [2,3,8,11,18,19]:
        for name,value in [('zero','0x00'),('empty','0x'),('root',root['id']),('pow',pow_['id'])]:sample(f'data-{type_}-{name}',kind='data',type=type_,value=value)
    for type_ in [5,7]:
        for n,value in enumerate(['','hello','{}','null','[]','true','{"type":"greeting","peers":[],"extra":null}','{"a":1,"a":2}','bad json','🐈\u0000\n']):sample(f'string-{type_}-{n}',kind='string',type=type_,value=value)
    for n,extra in enumerate([{},dict(host='relay.example',port='9002'),dict(host=None),dict(port=None),dict(port=9002),dict(port='2147483648'),dict(port='１２３'),dict(host='relay.example',port='bad')]):
        for version in ['1.1.2.8','anything-TEST','']:
            sample(f'greeting-{n}-{version}',kind='greeting',type=0,version=version,extra=extra)
    for top,ids in [(-1,[]),(1,[root['id']]),(2,[pow_['id'],root['id']]),(2,['0xFF'])]:sample(f'chain-greeting-{top}-{len(ids)}',kind='greeting',type=0,top=top,ids=ids)
    for n,ids in enumerate([[],['0x00'],[root['id']],['0xFE','0xFF',root['id']],['0xFE',root['id'],'0xFF'],[pow_['id'],root['id']],['0xFF']*1001]):sample(f'pulse-{n}',kind='pulse',type=6,ids=ids)
    for type_ in [1,14]:
        for cascade,blocks in itertools.product([False,True],[[],[root['wire']],[root['wire'],pow_['wire']],[pow_['wire'],root['wire']]]):sample(f'ibd-{type_}-{cascade}-{len(specifications)}',kind='ibd',type=type_,cascade=cascade,blocks=blocks)
    for number in ['-1','0','1','2','-2','1.5']:sample(f'archive-{number}',kind='number',type=15,value=number)
    sample('block',kind='block',type=20,value=pow_['wire'])
    encoded=execute(java,[request for _,request in specifications]);packets={name:value['packet'] for (name,_),value in zip(specifications,encoded)}
    result=[]
    def add(group,packets_,**config):result.append((group,dict(packets=packets_,**config)))
    for config in [{},{'shutdown':True},{'sync':True},{'slave':True},{'trace':True,'filter':None}]:
        add('type-bytes',[None,'0x']+[f'0x{type_:02X}' for type_ in range(256)],**config)
    for name,packet in packets.items():
        for config in [{},{'missing':True},{'uid':None},{'slave':True,'p2p':True,'allow':True},{'root':root['wire'],'p2p':True,'incoming':True,'valid':True,'db':[pow_['wire']]}]:add('payloads',[packet],**config)
    for name,packet in packets.items():
        if name.startswith(('greeting','chain-greeting')):
            for incoming,sent,test,limit in itertools.product([False,True],repeat=4):add('greeting-repeat',[packet,packet],incoming=incoming,sent=sent,test=test,limitIBD=limit,p2p=True,root=root['wire'])
    for peer_greeting,p2p,missing in itertools.product([False,True],repeat=3):
        add('peer-json',[packet for name,packet in packets.items() if name.startswith('string-7')],peerGreeting=peer_greeting,p2p=p2p,missing=missing)
    for p2p,p2p2,incoming,valid,root_ in itertools.product([False,True],[False,True],[False,True],[False,True],[None,root['wire']]):
        add('single-ping',[packets['data-11-zero']],p2p=p2p,p2p2=p2p2,incoming=incoming,valid=valid,root=root_)
    for exists,slave,sync in itertools.product([False,True],repeat=3):add('id-flags',[packets['data-2-zero'],packets['data-18-zero']],exists=exists,slave=slave,sync=sync)
    for no_root,slave in itertools.product([False,True],repeat=2):
        add('pow-processing',[pow_['packet'],nobody['packet'],pow_['packet']],root=None if no_root else root['wire'],slave=slave,valid=True)
        add('mining',[f'0x15{pow_["wire"][2:]}']*2,root=None if no_root else root['wire'],slave=slave)
        add('mining-recent',[f'0x15{pow_["wire"][2:]}'],slave=slave,recentMine=True)
        add('ibd-request',[f'0x0D{pow_["wire"][2:]}']*2+[f'0x0D{root["wire"][2:]}'],root=None if no_root else root['wire'],slave=slave)
    # Truncate valid serialized envelopes at their header and tail boundaries.
    # Do not generate unbounded positive Java allocation lengths in this fixture.
    for name,packet in packets.items():
        length=(len(packet)-2)//2
        cuts=sorted({0,1,2,3,4,5,6,7,8,length//2,length-1})
        add('truncated',[packet[:2+2*cut] for cut in cuts if 0<=cut<length],p2p=True,allow=True,slave=True)
    fixtures=execute(java,[dict(kind='consensus',mode=mode,script=script) for mode,script in itertools.product(range(30),['RETURN TRUE','RETURN FALSE','RETURN @BLKNUM LT 20','RETURN @BLKNUM GTE 20'])])
    for fixture in fixtures:
        for config in [{},{'mempoolCoin':True},{'mempoolFull':True},{'mempoolFull':True,'burn':-1},{'mmr':None}, None]:
            # Use the actual generated MMR or the deliberately absent MMR. An
            # empty MMR comes from the existing default tree constructor.
            if config is None:
                config={};mmr={}
            else:mmr={'mmr':fixture['mmr']}
            add('consensus', [fixture['packet']],root=fixture['root'],valid=True,**(mmr|config))
    maximum=(1<<256)-1
    blocks=execute(java,[dict(kind='pow',type=4,height=height,difficulty=str(maximum),time=time,ids=['0xF1','0xF2']) for height,time in [(0,0),(1,0),(2,0),(2,9223372036854775807)]])
    for block in blocks:
        for config in [{},{'slave':True},{'uid':'0x00'},{'uid':'0x01'},{'uid':None},{'mempoolFull':True},{'mmr':None}]:
            add('block-relay',[block['packet']]*2,root=root['wire'],valid=True,**config)
    # A DECIMAL32 rounding boundary straddling 0.01, with a received block
    # difficulty of MAX_UINT256 so the payload is always a proof of work.
    thresholds=[maximum//100,maximum*9999999//1000000000,maximum*99999994//10000000000,maximum*99999995//10000000000,maximum*10000001//1000000000]
    roots=execute(java,[dict(kind='pow',type=4,height=1,difficulty=str(value)) for value in thresholds])
    for root_ in roots:add('difficulty-ratio',[blocks[2]['packet']],root=root_['wire'],valid=True)
    chain=execute(java,[dict(kind='chain',count=515,transactions=True)])[0]
    for count in [1,255,256,257]:
        for uid in ['c0','0x00','0x01',None]:
            add('tree-scan',[chain[count]['packet']]*2,chain=[value['wire'] for value in chain[:count]],uid=uid,valid=True)
    for count in [1,511,512,513]:
        add('parent-scan',[chain[count]['packet']],root=root['wire'],db=[value['wire'] for value in chain[:count]],valid=True)
    for count in [1,99,100,101]:add('ping-ancestor',[packets['data-11-zero']],chain=[value['wire'] for value in chain[:count]])
    heavier=execute(java,[dict(kind='ibd',type=1,cascadeBlocks=[chain[0]['wire']],blocks=[value['wire'] for value in chain[1:end]]) for end in [1,2,3,5]])
    for root_,rescue,full in itertools.product([chain[1]['wire'],chain[4]['wire']],[False,True],['127.0.0.1:9001','',None]):
        add('heavier-chain',[value['packet'] for value in heavier],root=root_,cascade=[chain[0]['wire']],rescue=rescue,full=full,p2p=True)
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'nio-message-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NIOMessageOracle.java',ROOT/'support/ConsensusOracle.java']
    listing=build/'nio-message-java-sources.txt';listing.write_text('\n'.join('"'+str(path)+'"' for path in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOMessageOracle'];corpus=cases(java)
    requests=[request for _,request in corpus];expected=execute(java,requests);actual=execute([str(build/'minima_nio_message')],requests)
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if left!=right or not isinstance(left,list) or len(left)!=len(request['packets']):
            detail={'case':i,'group':group,'request':request,'java':left,'cpp':right};(build/'nio-message-failure.json').write_text(json.dumps(detail,indent=2)+'\n');raise AssertionError(f'Case {i}, {group}; see {build}/nio-message-failure.json')
    types=set();peers=set();effects=set();delays=set();valid=False
    for program in expected:
        for row in program:
            valid|=row['valid']
            types.update(int(packet[2:4],16) for packet in row['packets'])
            for event in row['events']:
                if isinstance(event,dict):
                    if 'type' in event:peers.add(event['type'])
                else:effects.add(event.split(':')[0])
            delays.update(timer['delay'] for timer in row['timers'])
    assert {0,1,2,3,4,12,14,16,19,20}<=types,types
    assert {'P2P_CONNECTED','P2P_NOCONNECT','P2P_MESSAGE','P2P_REMOVE_PEER'}<=peers,peers
    assert {'ibd','sync','mine','pow','txblock','exists','get','archive','block'}<=effects,effects
    assert 2000 in delays and valid,(delays,valid)
    assert 'MDS_HEAVIER_CHAIN' in effects,effects
    assert 'MAIN_DO_RESCUE' in peers,peers
    ratio_relay=[any(int(packet[2:4],16)==2 for packet in program[-1]['packets']) for (group,_),program in zip(corpus,expected) if group=='difficulty-ratio']
    assert ratio_relay==[True,False,False,False,True],ratio_relay
    groups={name:sum(group==name for group,_ in corpus) for name in sorted({group for group,_ in corpus})}
    report={'passed':True,'programs':len(corpus),'packets':sum(len(request['packets']) for _,request in corpus),'groups':groups,'scope':'Actual pinned NIOMessage.run and native dispatcher. Real serialized payloads, client queues, traffic, IBD creation and chain helpers. Runtime DB/P2P/processor/miner receivers captured; no full node, database writes, socket or miner startup. Generic caught errors/logs are not compared. Includes script/signature/MMR/mempool decisions, difficulty-ratio rounding, parent/tree scan limits, and heavier-chain/rescue events. Exact live-clock boundaries, successful MegaMMR service binding, full runtime and log/notification parity remain pending.'}
    (build/'nio-message-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
