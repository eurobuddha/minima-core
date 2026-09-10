#!/usr/bin/env python3
"""Pinned P2P2 helper and manager behavior, including source no-op branches."""
import argparse,itertools,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,execute,op,client,address,cases as helper_cases
def process(kind,**kw):return op('process',type=kind,**kw)
def cases():
    groups={'connection-policy','invalid-first','queries','invalid-set'}
    result=[(group,request) for group,request in helper_cases() if group in groups]
    def add(group,ops,**kw):result.append((group,dict(ops=ops,**kw)))
    add('independent',[op('independent'),op('localAdd',value='new-only'),op('locals'),op('addInvalid',value='new-only'),op('independent')])
    add('cached-locals',[op('locals'),op('localAdd',value=None),op('localAdd',value='🐼'),op('locals')],locals=['localhost','127.0.0.1'])
    for known,first,fail in itertools.product([[],['one'],[None]], [True,False,'false'],[-1,1,2,3,4]):
        add('initialization',[process('P2P2_INIT')]*2+[process('P2P2_SLOW_LOOP'),process('P2P2_SHUTDOWN'),op('shutdown')],peers=known,data={'first_startup':first},dbFailAt=fail)
    for online,connections,desired,known,missing in itertools.product([False,True],[0,1,3],[-1,0,2,3],[[],['one'],[None]],[False,True]):
        add('fast-loop',[process('P2P2_FAST_LOOP')]*2,offline=not online,clients=[client(f'c{i}',f'8.8.8.{i+1}') for i in range(connections)],desired=desired,peers=known,missingManager=missing,delay=-1)
    for peers in [[],['same'],[None],['same']*4]:add('selection',[op('random')]*4,peers=peers)
    for old in [{},{'peers':None},{'peers':[address('8.8.8.8')]},{'peers':[address('::1')]},{'peers':['bad']}]:add('conversion',[op('convert'),op('choose')]*2,old=old)
    for kind in [None,'P2P2_INIT','P2P2_FAST_LOOP','P2P2_SLOW_LOOP','P2P2_SHUTDOWN','P2P_SHUTDOWN','unknown']:
        add('nulls',[process(kind,nullMessage=True),process(kind),op('shutdown')],offline=True)
    for offline in [False,True]:add('availability',[op('available')]*2,offline=offline)
    # Compare fresh Random choices by membership and distribution, not equality.
    add('random-multiple',[op('random')]*400,peers=['one','two',None,'four'])
    return result
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'p2p2-runtime-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','P2PFunctionsOracle.java','PeersCheckerOracle.java','P2PManagerOracle.java','P2P2RuntimeOracle.java']];listing=build/'p2p2-runtime-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();requests=[r for _,r in corpus];expected=execute(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2P2RuntimeOracle'],requests);actual=execute([str(build/'minima_p2p2_runtime')],requests)
    successes=set();messages=set()
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if group=='random-multiple':
            for name,output in [('java',left),('cpp',right)]:
                counts={peer:sum(row['result']==peer for row in output) for peer in request['peers']};assert all(40<count<160 for count in counts.values()),(name,counts)
                for row in output:assert row['result'] in request['peers'];row['result']='<random-member>'
        if left!=right or not isinstance(left,list) or len(left)!=len(request['ops']):
            (build/'p2p2-runtime-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Case {i} ({group}); see {build}/p2p2-runtime-failure.json')
        for row in left:
            if isinstance(row['result'],(str,bool)):successes.add(str(row['result']))
            messages.update(message['type'] for message in row['posted'])
    assert {'True','False','ERROR','OK'}<=successes and {'NIO_CONNECT','NIO_DISCONNECT'}<=messages,(successes,messages)
    report=dict(passed=True,programs=len(corpus),operations=sum(len(r['ops']) for _,r in corpus),groups={g:sum(group==g for group,_ in corpus) for g in sorted({g for g,_ in corpus})},scope='Actual pinned/native P2P2Functions and P2P2Manager; independent caches/invalid peers, connection logs/queues, availability, DB call order and failures, startup entries, timers, shutdown and original inactive branches. Fresh random multi-peer choice checked by membership/distribution. Existing P2P2DB log bodies excluded; full node lifecycle and mutable static configuration remain pending.')
    (build/'p2p2-runtime-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
