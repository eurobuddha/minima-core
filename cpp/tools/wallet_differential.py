#!/usr/bin/env python3
"""Actual pinned Wallet, crypto and row classes; JDBC remains a recording boundary."""
import argparse,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def corpus():
    cases=[]
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    def op(cmd,**kw):
        r=dict(cmd=cmd,rows=[row],script='RETURN TRUE',address='0x01',publickey='0x01',phrase='new fixture',seed='0xBB',privatekey='0x00',modifier='0x01',max=0,uses=3,data='0xFF');r.update(kw);return r
    def add(*ops,**kw):cases.append(dict(ops=list(ops),**kw))
    commands=['base','available','updateSeed','wipeSeed','reset','resetPrivate','checkPrivate','checkSingle','stop','shutdown','defaults','init','default','addScript','remove','script','keyRelevant','addressRelevant','addressSimple','keys','key','addresses','uses','increment','load','create','save','hard','open']
    add(*(op(c) for c in commands[:4]))
    add(op('load'),op('hold'),op('updateSeed'),op('mutate',held=True,phrase=None,seed=None),op('base'))
    for cmd in commands:add(op(cmd))
    for cmd,fail,runtime in itertools.product(commands,range(1,19),[False,True]):
        add(op('load'),op(cmd,fail=fail,runtimeFailure=runtime),op('base'),op('keyRelevant'),op('addressRelevant'))
    for fail,runtime in itertools.product(range(1,56),[False,True]):
        add(op('load',fail=fail,runtimeFailure=runtime),op('base'),op('keys'),op('addresses'),op('available'))
    for flag in ['nullConnection','nullStatement','nullPrepared','nullRows']:
        add(op('load',**{flag:True}),*[op(c,**{flag:True}) for c in ['keys','addresses','base','init','addScript','updateSeed']])
    for phrase,anyseed,fail in itertools.product([' fixture arbitrary seed ','aban abil able',' bad '],[False,True],[0,27,28,29,30,31,32,33,34,35]):
        add(op('load',rows=[],phraseParam=phrase,any=anyseed,fail=fail),op('base'),op('available'))
    for simple,default,track in itertools.product([False,True],repeat=3):
        add(op('load'),op('addScript',simple=simple,default=default,track=track),op('remove'),op('addressRelevant'),op('addressSimple'))
    for seed in [None,'0x00','0x','0x1234']:
        add(op('load'),op('hold'),op('updateSeed',seed=seed,phrase=None),op('available'),op('checkSingle'),op('resetPrivate',seed=seed),op('available'))
    for target,maxnum,stop in itertools.product([-1,0,1,8,64],[-1,0],[False,True]):
        add(op('load'),op('stop',stop=stop),op('init',target=target,max=maxnum,log=True),test=True)
    for cmd,table in itertools.product(['keys','key','addresses','script','defaults','checkPrivate'],[[],[row,row], [{**row,'track':0,'simple':0}], [{**row,'publickey':None,'seed':None}]]):
        add(op('load'),op(cmd,rows=table))
    for closed,cmd in itertools.product([False,True],commands):
        add(op('load'),op('hard') if closed else op('clearConnection'),op(cmd))
    for uses,fail in itertools.product([0,3,4],range(0,20)):
        add(op('load'),op('sign',rows=[{**row,'uses':uses}],fail=fail))
    add(op('load'),op('sign',data=None),op('sign',rows=[]))
    add(op('load'),op('newKey'),op('keys'))
    add(op('load'),op('newSimple',default=True))
    for fail in [1,8,9]:add(op('load'),op('newKey',fail=fail),op('base'),op('keyRelevant'))
    add(op('load'),op('init',target=2,max=1,log=True))
    for testing in [False,True]:add(op('base'),op('construct',test=False),op('construct',test=True),op('construct',test=False),test=testing)
    add(op('load'),op('updateSeed',seed='0x00'),op('newKey'),op('newSimple'),op('sign'))
    return cases

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'wallet-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/WalletOracle.java'];listing=build/'wallet-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=effects=0
    with tempfile.TemporaryDirectory(prefix='minima-wallet-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletOracle',root]),Runner([str(build/'minima_wallet'),root])]
        try:
            for index,request in enumerate(corpus()):
                outputs=[normalize(r.ask(request)) for r in runners]
                if outputs[0]!=outputs[1] or 'ops' not in outputs[0]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);effects+=sum(len(x['events']) for x in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,effects=effects,failures=failures,scope='Actual pinned Wallet/rows/BIP39/Address/TreeKey/Winternitz and native counterparts. SQL and failure ordering, seed/retained-row identity, cache membership, default-key gates, script flags, seed clearing, key derivation/signing and use updates. Fixture JDBC only; no native H2 persistence or live wallet. Private cache sets sorted for membership; NPE expression diagnostics and logger prefixes/stacks excluded. Random default selection exercised only with zero or one candidate; generated seed phrases use a supplied fixture phrase.')
    (build/'wallet-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
