#!/usr/bin/env python3
"""Pinned Java/C++ peer state and greeting programs with explicit shuffle scope."""
import argparse,copy,ipaddress,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'peer-state-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/PeerMessagesOracle.java',ROOT/'support/PeerStateOracle.java'];listing=build/'peer-state-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    def addr(host='127.0.0.1',port=9001,unresolved=False):return {'host':host,'port':port,'unresolved':unresolved}
    peers=[None,addr(),addr('127.1'),addr('::1'),addr('fe80::1%0'),addr('fe80::1%1'),addr('invalid host'),addr('INVALID HOST'),addr('İ host'),addr('i host'),addr('𐐀 host'),addr('𐐨 host'),addr('0.0.0.0',0)]
    maps=['InLinks','OutLinks','NotAcceptingConnP2PLinks','NoneP2PLinks','AllLinks'];fields=['AcceptingInLinks','MaxNumNoneP2PConnections','MaxNumP2PConnections','DoingDiscoveryConnection','StartupComplete','LoopDelay','NoConnect','HostSet']
    def state(ops,**extra):cases.append({'kind':'state','ops':ops,**extra})
    state([])
    for host in [None,'','127.1','::1','invalid host','host/with/slash']:
        state([{'op':'port','value':port} if j%2==0 else {'op':'host','value':host} for port in [-1,0,65535,65536] for j in [0,1]])
    for _ in range(220):
        ops=[]
        for i in range(45):
            op=rng.choice(['set','address','secret','put','put','remove','clear','replace','external','alias','knownAdd','knownAdd','knownRemove','knownContains','knownCopy','copyAppend','greeting','loopParam','loopReset','mobile'])
            field=rng.choice(maps);key=rng.choice([None,'','key','Aa','BB','𐀀','x\x00y',str(rng.randrange(15))]);value=rng.choice(peers)
            if op=='set':
                field=rng.choice(fields);value=rng.choice([False,True]) if field not in ['MaxNumNoneP2PConnections','MaxNumP2PConnections','LoopDelay'] else rng.choice([-2147483648,-1,0,1,2147483647])
                if field=='LoopDelay':value=rng.choice([-9223372036854775808,-1,0,9223372036854775807])
            elif op=='replace':value=rng.choice([None,[],[{'key':'external','value':rng.choice(peers)}]])
            elif op=='alias':value=rng.choice(maps)
            elif op=='secret':value=rng.choice([None,'','0x00','0x1234','invalid'])
            elif op=='loopParam':value=rng.choice([-2147483648,0,1,777,2147483647])
            elif op=='mobile':value=rng.choice([False,True])
            ops.append({'op':op,'field':field,'key':key,'value':value})
        state(ops)
    # Distinct hashes exercise ordinary HashSet bin ordering and resize exactly.
    for count in [1,7,8,9,16,33,65,128,257]:
        pool=[addr(str(ipaddress.IPv4Address(rng.getrandbits(32))),rng.randrange(65536)) for _ in range(count)]
        ops=[{'op':'knownAdd','value':p} for p in pool]+[{'op':'knownCopy'},{'op':'greeting'}]
        rng.shuffle(pool);ops += [{'op':rng.choice(['knownAdd','knownRemove','knownContains']),'value':p} for p in pool]
        state(ops)
    # Address keys are non-Comparable in the JDK. Tree-bin tie breaks depend on
    # VM identityHashCode, so only these declared collision cases compare sets.
    for count in [9,16,33,65,128]:
        for _ in range(4):
            pool=[addr('10.0.0.'+str(i),10000-i) for i in range(1,count+1)];rng.shuffle(pool)
            ops=[{'op':'knownAdd','value':p} for p in pool];rng.shuffle(pool)
            ops += [{'op':rng.choice(['knownAdd','knownRemove','knownContains']),'value':p} for p in pool]
            ops += [{'op':'knownCopy'},{'op':'knownClear'}];state(ops,unordered_set=True)
    values=[None,True,False,'','1',1,0,-1,2147483647,-2147483648,2147483648,-2147483649,1.0,1.5,[],{}]
    lists=['KnownPeers','InLinks','OutLinks'];json_names={'KnownPeers':'knownPeers','InLinks':'inLinks','OutLinks':'outLinks'}
    for ctor in ['default','short','full']:
        for _ in range(130):
            known=rng.choices(peers,k=3);inlinks=rng.choices(peers,k=2);outlinks=rng.choices(peers,k=2);ops=[]
            for i in range(25):
                action=rng.choice(['set','read','append','external','equal','self','null','roundtrip']);field=rng.choice(lists);value=rng.choice(peers)
                if action=='set':
                    field=rng.choice(['MyMinimaPort','AcceptingInLinks','NumNoneP2PConnections','MaxNumNoneP2PConnections']+lists)
                    value=rng.choice([None,[],known,known+[None]]) if field in lists else rng.choice([False,True]) if field=='AcceptingInLinks' else rng.choice([-2147483648,0,1,9121,2147483647])
                elif action in ['read','equal']:
                    options={'myMinimaPort':values,'isAcceptingInLinks':values,'numNoneP2PConnections':values,'maxNumNoneP2PConnections':values,'knownPeers':[None,[],known,[None],[{}]],'inLinks':[None,[],inlinks,[None],[{}]],'outLinks':[None,[],outlinks,[None],[{}]]}
                    value={k:rng.choice(v) for k,v in options.items() if rng.choice([False,True])}
                ops.append({'op':action,'field':field,'value':value})
            cases.append({'kind':'greeting','ctor':ctor,'known':known,'in':inlinks,'out':outlinks,'ops':ops})
    # equals ignores peer contents and counts/outlinks; identity short circuit
    # must also succeed when its knownPeers field is null.
    cases.append({'kind':'greeting','ctor':'short','known':[addr()],'in':[],'out':[],'ops':[{'op':'equal','value':{'myMinimaPort':9121,'knownPeers':[addr('::1')]}},{'op':'set','field':'KnownPeers','value':None},{'op':'self'},{'op':'null'},{'op':'equal','value':{'myMinimaPort':9121}},{'op':'equal','value':{'myMinimaPort':1}}]})
    cases.append({'kind':'params','value':addr(),'ops':[]})
    for _ in range(150):
        external=rng.choices(peers,k=3);ops=[]
        for i in range(22):
            action=rng.choice(['set','externalSet','externalAppend','get','append','jsonPut','read','roundtrip','version','versionParam']);key=rng.choice(['peers','version']);value=None
            if action=='set':value=rng.choice([None,[],external,[addr(),None]])
            elif action in ['append','externalAppend']:value=rng.choice(peers)
            elif action=='jsonPut':value=rng.choice(values+[external,[addr()],[addr('127.1',2147483648)]])
            elif action=='versionParam':value=rng.choice(['','1.1','1.9','🐼'])
            elif action=='read':
                text=rng.choice(['{}','null','true','[]','invalid','{"peers":null}','{"peers":[]}','{"version":"0.1"}']);payload=text.encode();wire=len(payload).to_bytes(4,'big')+payload
                if rng.randrange(5)==0:wire=wire[:rng.randrange(len(wire)+1)]
                value='0x'+wire.hex()
            ops.append({'op':action,'key':key,'value':value})
        cases.append({'kind':'db','peers':external,'ops':ops})
    # Test-mode settings use the actual pinned setTestParams method. Observe
    # defaults before applying, copy-on-apply scalars, and shared mutable lists.
    cases.append({'kind':'testParams','ops':[{'op':'apply'},{'op':'append','value':addr()},{'op':'clear'},{'op':'element','value':addr('::1'),'active':True},{'op':'apply'}]})
    test_fields=['TGT_NUM_LINKS','TGT_NUM_NONE_P2P_LINKS','MIN_NUM_CONNECTIONS','LOOP_DELAY','HASH_RATE_UPDATE_DELAY','LOOP_DELAY_VARIABILITY','NODE_NOT_ACCEPTING_CHECK_DELAY','SAVE_DATA_DELAY','WALK_LINKS_EXPIRE_TIME','AUTH_KEY_EXPIRY','METRICS_DELAY']
    for field,value in itertools.product(test_fields,[-2147483648,-1,0,1,777,2147483647]):
        cases.append({'kind':'testParams','ops':[{'op':'set','field':field,'value':value},{'op':'apply'},{'op':'set','field':field,'value':123}]})
    for field,values in [('LOG_LEVEL',[None,0,1,2]),('METRICS_URL',[None,'','http://metrics:5000/network','fixture'])]:
        for value in values:cases.append({'kind':'testParams','ops':[{'op':'set','field':field,'value':value},{'op':'apply'}]})
    cases.append({'kind':'testParams','ops':[{'op':'replace','value':[addr()]},{'op':'apply'},{'op':'append','value':addr('::1'),'active':True},{'op':'replace','value':None},{'op':'append','value':None,'active':True},{'op':'apply'},{'op':'clear'},{'op':'replace','value':[]},{'op':'apply'},{'op':'append','value':None},{'op':'clear','active':True}]})
    def run(command):
        process=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',capture_output=True,text=True,timeout=300,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=[line for line in process.stdout.splitlines() if not line.startswith('Minima @ ')]
        if process.returncode or len(lines)!=len(cases):
            (build/'peer-state-crash.txt').write_text(process.stderr);raise RuntimeError(f'Process exit {process.returncode}, lines {len(lines)}, expected {len(cases)}')
        return [json.loads(line) for line in lines]
    def sorted_peers(values):return sorted(values,key=lambda value:json.dumps(value,sort_keys=True)) if isinstance(values,list) else values
    def normalized(case,value):
        value=copy.deepcopy(value)
        if case['kind']=='state' and isinstance(value,list):
            if case.get('unordered_set'):value[0]['known']=sorted_peers(value[0]['known'])
            for operation,step in zip(case['ops'],value[1:]):
                if operation['op']=='greeting' and isinstance(step['result'],dict):
                    greeting=step['result'];greeting['KnownPeers']=sorted_peers(greeting['KnownPeers'])
                    if isinstance(greeting['json'],dict):greeting['json']['greeting']['knownPeers']=sorted_peers(greeting['json']['greeting']['knownPeers'])
                if case.get('unordered_set'):
                    step['state']['known']=sorted_peers(step['state']['known']);step['copy']=sorted_peers(step['copy'])
        return value
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','PeerStateOracle']);actual=run([str(build/'minima_peer_state')]);failures=[]
    for case,j,n in zip(cases,expected,actual):
        if normalized(case,j)!=normalized(case,n):failures.append({'case':case,'java':j,'cpp':n})
    first_settings=next(result for case,result in zip(cases,expected) if case['kind']=='testParams')
    assert first_settings[0]['test']['LOG_LEVEL']=='DEBUG' and first_settings[0]['test']['LOOP_DELAY']==5000
    assert first_settings[0]['test']['nodes'][0]['host']=='minima_one'
    assert first_settings[1]['alias'] and first_settings[2]['result']=='ERROR' and first_settings[3]['result']=='ERROR'
    assert first_settings[4]['test']['nodes']==first_settings[4]['active']['nodes']
    report={'cases':len(cases),'operations':sum(len(c['ops']) for c in cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Peer state defaults/getters/setters, map ordering/aliases/nulls, address-set equality/retention/resizing, snapshots, greeting constructors/JSON/aliases/equality, P2PParams fixed-list checks, P2PTestParams defaults/scalar copy/shared-list application and mutation, P2PDB wire/default/type failures and detached lists. State-constructor shuffle compared as a permutation; declared non-Comparable HashSet tree-bin cases compare membership because JDK identityHashCode order is unspecified. Exceptions normalized; concurrent mutation not compared.'}
    (build/'peer-state-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:1]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
