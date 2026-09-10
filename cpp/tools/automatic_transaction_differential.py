#!/usr/bin/env python3
"""Pinned txnauto command and static helper with real transaction components."""
import argparse,copy,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
from wallet_balance_corpus import preserve_text

def corpus(fixtures,key,tokens):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def command(params=None,**kw):return op(name='txnauto',params=dict(id='test',amount='1',address='0x02')|(params or {}),replyText=True,**kw)
    def setup(nodes=None):return [op('load'),op('chain',history='8',nodes=fixtures['nodes'] if nodes is None else nodes),op('mining')]
    def add(*steps,**kw):cases.append(dict(funding=True,ops=list(steps),**kw))
    add(op('meta',name='txnauto'))
    for amount,sign,mmr,burn in itertools.product(['0','1','4','5','8','9','-1','0.000000000000000000000000000000000000000000001'],['true','false'],['true','false'],['0','0.5']):add(*setup(),command(dict(amount=amount,sign=sign,mmrscript=mmr,burn=burn)))
    for field,value in itertools.product(['id','amount','address','tokenid','sign','burn','mmrscript'],[None,True,False,1,{},[],'','bad']):add(*setup(),command({field:value}))
    for field in ['id','amount','address']:
        params=dict(id='test',amount='1',address='0x02');del params[field];add(*setup(),op(name='txnauto',params=params,replyText=True))
    for id in ['test','two words','test;txnlist','test"','[id]','π🐼',' x ', '']:
        add(*setup(),command(dict(id=id,mmrscript='false')))
    add(*setup(),command(dict(amount='4')),command(dict(amount='bad')),command(dict(amount='4')),expectRows=1)
    add(*setup(),command(dict(amount='99')),expectRows=0,expectError=True)
    add(*setup(),command(dict(amount='4'),rows=[]),expectRows=1,expectError=True)
    add(*setup(),command(dict(amount='4',mmrscript='false'),rows=[]),expectRows=1,expectSuccess=True)
    add(*setup(),command(dict(amount='4',sign='true')),op('verifyStored'),expectVerified=[True])
    add(*setup(),command(dict(amount='4',sign='false')),op('verifyStored'),expectVerified=[])
    for mmr,sign in itertools.product(['true','false'],repeat=2):add(op('load'),command(dict(mmrscript=mmr,sign=sign)))
    for token,burn in itertools.product(['0x00','0x0000','Mx200104JMFSAS','0x01'],['0','1']):add(*setup(),command(dict(tokenid=token,burn=burn)))
    for fixture in tokens:
        for amount in ['1','10','101']:add(*setup(fixture['nodes']),command(dict(tokenid=fixture['tokenid'],amount=amount,mmrscript='false')))
    for rows in [[],[row|{'script':None}],[row|{'privatekey':None}],[row|{'publickey':None}],[row|{'address':None}],[row|{'uses':4}],[row|{'simple':0}]]:
        for mmr in ['true','false']:add(*setup(),command(dict(amount='4',sign='true',mmrscript=mmr),rows=rows))
    for fail,runtime in itertools.product(range(1,35),[False,True]):add(*setup(),command(dict(sign='true'),fail=fail,runtimeFailure=runtime))
    for coins in [fixtures['inputcoins'][:1],fixtures['inputcoins'][2:],fixtures['inputcoins']]:add(*setup(),op('miner',coins=coins),command(dict(amount='5')))
    for proofs in [fixtures['proofs'][:1],fixtures['proofs'][2:],fixtures['proofs']]:add(*setup(),op('pool',proofs=proofs),command(dict(amount='5')))
    add(*setup(),op(name='txncoinlock',params={'action':'lock'}),command(dict(amount='4')),command(dict(id='next',amount='4')),op(name='txncoinlock',params={}))
    for caller,text in itertools.product(['0x00','0x1234'],['txnauto id:test amount:4 address:0x02','txnauto id:test amount:99 address:0x02','txnauto id:test amount:4 address:0x02 sign:true','txnauto invalid:true','txnauto id:test amount:1 address:0x02 mmrscript:false;txnlist']):add(*setup(),op('run',caller=caller,text=text,replyText=True))
    # The static helper consumes amount but ignores address and creates only change.
    def helper(amount='1',address='0x02',**kw):return op('automaticHelper',amount=amount,address=address,**kw)
    for amount,address in itertools.product(['0','1','3','4','5','8','9','-1','0.000000000000000000000000000000000000000000001',None],[None,'','bad','0x02','0xFF']):add(*setup(),helper(amount,address),helperCheck=True)
    for amount in ['-1','1',None]:
        add(op('load'),helper(amount));add(op('load'),op('mining',nullMiner=True),helper(amount));add(*setup([]),helper(amount))
    for rows in [[],[row|{'script':None}],[row|{'publickey':None}],[row|{'address':None}],[row|{'privatekey':None}],[row|{'uses':4}]]:add(*setup(),helper(rows=rows))
    for fail,runtime in itertools.product(range(1,45),[False,True]):add(*setup(),helper(fail=fail,runtimeFailure=runtime))
    for locked in [fixtures['inputcoins'][:1],fixtures['inputcoins'][2:],fixtures['inputcoins']]:add(*setup(),op('miner',coins=locked),helper())
    for proofs in [fixtures['proofs'][:1],fixtures['proofs'][2:],fixtures['proofs']]:add(*setup(),op('pool',proofs=proofs),helper())
    return cases

def validate(request,output):
    final=output['ops'][-1]
    if 'expectRows' in request:assert len(final['txnflags'])==request['expectRows'],final
    if request.get('expectError'):assert 'exception' in final,final
    if request.get('expectSuccess'):assert json.loads(final['value'])['status'] is True,final
    if 'expectVerified' in request:assert final['value']==request['expectVerified'],final
    if request.get('helperCheck') and 'value' in final:
        value=final['value'];assert len(final['txnflags'])==0 and value['row']['id']=='temp',final
        assert all(value['verified']),value
        txn=value['row']['transaction'];outputs=txn['outputs'];assert len(outputs)<=1,txn
        for coin in outputs:assert coin['address']=='0x01' and coin['storestate'] is False,coin
        amount=request['ops'][-1]['amount']
        from decimal import Decimal
        total=sum(Decimal(c['amount']) for c in txn['inputs']);change=sum(Decimal(c['amount']) for c in outputs)
        assert total-change==Decimal(amount),(txn,amount)
        assert txn['linkhash']=='0x00',txn

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'automatic-transaction-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['WalletCommandsOracle.java','SqlDBOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java']]
    listing=build/'automatic-transaction-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar','-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=effects=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='automatic-transaction-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root]),Runner([str(build/'automatic_transaction_runtime'),root])]
        try:
            cases=corpus(runners[0].ask({'kind':'witnessFixtures'}),runners[0].ask({'kind':'keyFixtures'}),runners[0].ask({'kind':'fundingFixtures'}))
            for i,request in enumerate(cases):
                outputs=[]
                for name,runner in zip(['java','cpp'],runners):
                    try:
                        raw=runner.ask(request);value=preserve_text(raw,normalize(raw),request);validate(request,value);outputs.append(value)
                    except Exception as error:failures.append(dict(case=i,runtime=name,error=repr(error),request=request,stderr=runner.errors(),raw=locals().get('raw')));break
                if failures:break
                if outputs[0]!=outputs[1]:failures.append(dict(case=i,request=request,java=outputs[0],cpp=outputs[1]));break
                completed+=1;operations+=len(request['ops']);effects+=sum(len(x['events']) for x in outputs[0]['ops'])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual registered txnauto and static helper with existing wallet, chain/MMR, miner, mempool, funding/signing commands and recording SQL. Exact stored wire and serialized replies, selected VM exception prose normalized. Independent cleanup, conservation, change-only output and signature checks. No durable SQL, network admission, arbitrary inherited reference/concurrency or full-node parity certification.')
    (build/'automatic-transaction-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
