#!/usr/bin/env python3
"""Pinned tokencreate through real wallet/proofs/signatures/generator/miner."""
import argparse,copy,itertools,json,subprocess,tempfile
from decimal import Decimal,localcontext,ROUND_DOWN,ROUND_FLOOR
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from scan_chain_differential import project as scan_project

def corpus(f,key,tokens):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='tokencreate',params=dict(name='Teaching token',amount='13.9')|(params or {}),replyText=True,echoParams=True,**kw)
    def setup(nodes=None,load=True,mining=True):return ([op('load')] if load else [])+[op('chain',history='8',nodes=f['nodes'] if nodes is None else nodes)]+([op('mining')] if mining else [])
    def add(*steps,**kw):cases.append(dict(ops=list(steps),**kw))
    add(op('meta',name='tokencreate'),metadata=True)
    add(*setup(),cmd(),expectMint=True)
    add(*setup(),cmd(dict(name={'name':'Lecture token'},signtoken=key['publickey'],webvalidate='https://example.invalid/token.txt',state={'0':'[course]','1':'42'})),op('verifyMined'),op('verifyMinted'),expectVerified=True)
    for amount,decimals,burn in itertools.product(['1','13.9','1000000000000.99'],['0','8','16'],['0','0.5','-1']):add(*setup(),cmd(dict(amount=amount,decimals=decimals,burn=burn)),expectMint=True)
    for amount in ['-1','0','0.999','1.0001','1000000000001','1e19','1e20',' bad ',' 1 ','１２','1e-100',None]:
        for limits in ['true','false']:add(*setup(),cmd(dict(amount=amount,uselimits=limits)))
    for decimals in ['-1000','-45','-44','-1','0','16','17','30','36','44','45',' 8','8 ','8.0','+8','٠٨','１７','2147483647','2147483648','-2147483649','-2147483648']:
        for limits in ['true','false']:add(*setup(),cmd(dict(decimals=decimals,uselimits=limits)))
    for field,value in itertools.product(['name','amount','decimals','script','state','signtoken','webvalidate','burn','mine','uselimits'],[None,True,1,{},[],'','bad']):add(*setup(),cmd({field:value}))
    for field in ['name','amount']:
        params=dict(name='Teaching token',amount='1');del params[field];add(*setup(),op(name='tokencreate',params=params,replyText=True))
    for name in [{'name':None},{'name':True},{'name':123},{'name':{'nested':1}},{'ticker':'NO NAME'},'π🐼','java.lang.NullPointerException: ordinary token name']:
        add(*setup(),cmd({'name':name}))
    for state in [{'0':'[hello]','1':'42'},{'0':123},{'0':None},{'256':None},{'256':123},{'bad':None},{'01':'first','1':'second'},{'١':'42'}]:add(*setup(),cmd({'state':state}))
    for script in [None,'','RETURN TRUE','RETURN FALSE','return true','RETURN [hello]','RETURN 0xABC']:
        add(*setup(),cmd(dict(name={'name':'Shared'},webvalidate='https://example.invalid/test',script=script,signtoken=key['publickey'])))
    for extras in [dict(state=None),dict(state={'bad':None}),dict(script=None),dict(signtoken='0xBAD'),dict(decimals='17'),dict(amount=None),dict(burn='bad')]:
        add(*setup(),cmd(dict(name={'name':'Shared'},webvalidate='https://example.invalid/shared')|extras))
    for nodes in [[],f['nodes'][:1],f['nodes'][:3],f['nodes'][-1:]]:add(*setup(nodes),cmd())
    for token in tokens[:2]:add(*setup(token['nodes']),cmd())
    for coins in [f['inputcoins'][:1],f['inputcoins'][2:],f['inputcoins']]:add(*setup(),op('reserveMining',coins=coins),cmd())
    for proofs in [f['proofs'][:1],f['proofs'][2:],f['proofs']]:add(*setup(),op('pool',proofs=proofs),cmd())
    for amount in ['1','9','99']:
        add(*setup(),cmd(dict(burn=amount)))
    # Zero/negative required funding with all candidates reserved can reach an empty input list.
    for sign in [False,True]:add(*setup(),op('reserveMining',coins=f['inputcoins']),cmd(dict(burn='-1',**({'signtoken':key['publickey']} if sign else {}))))
    for rows in [[],[row|{'script':None}],[row|{'address':None}],[row|{'privatekey':None}],[row|{'publickey':None}],[row|{'uses':4}],[row|{'simple':0}]]:
        for sign in [False,True]:add(*setup(),cmd(dict(**({'signtoken':key['publickey']} if sign else {})),rows=rows))
    for fail,runtime in itertools.product(range(1,46),[False,True]):add(*setup(),cmd(dict(name={'name':'Failure'},webvalidate='https://example.invalid/failure',signtoken=key['publickey']),fail=fail,runtimeFailure=runtime))
    add(*setup(mining=False),cmd());add(*setup(),op('mining',nullMiner=True),cmd())
    for mode in ['real','false','throw']:
        add(*setup(),cmd(dict(mine='true'),mineResult=mode),**({'expectMint':True,'synchronous':True} if mode=='real' else {}))
    add(*setup(),cmd(dict(mine='true'),failPost=True))
    add(*setup(),cmd(),op('mineNext'),op('verifyMined'))
    for caller,text in itertools.product(['0x00','0x1234'],['tokencreate name:Demo amount:1','tokencreate name:{"name":"Demo"} amount:1 decimals:0','tokencreate invalid:true','tokencreate name:Demo amount:bad;tokencreate name:Demo amount:1','tokencreate name:Demo amount:1 mine:true']):add(*setup(),op('run',text=text,caller=caller,replyText=True))
    return cases

def project(raw,request):
    value=scan_project(raw,request)
    for original,row in zip(raw['ops'],value['ops']):
        if 'parameterState' in original:row['parameterState']=original['parameterState']
    return value

def validate(request,result):
    rows=result['ops'];last=rows[-1]
    if request.get('metadata'):
        v=last['value'];assert v['name']=='tokencreate' and v['valid']==['name','amount','decimals','script','state','signtoken','webvalidate','burn','mine','uselimits'],v
    if request.get('expectVerified'):
        assert rows[-2]['value'] and all(rows[-2]['value']),rows[-2]
        assert last['value']==[True],last
    if not request.get('expectMint'):return
    reply=json.loads(last['value'],parse_float=Decimal);assert reply['status'],reply
    transaction=reply['response']['body']['txn'];inputs=transaction['inputs'];outputs=transaction['outputs'];params=request['ops'][-1]['params']
    assert inputs and len(outputs)==2,transaction
    with localcontext() as ctx:
        ctx.prec=64;ctx.rounding=ROUND_DOWN
        supply=Decimal(params['amount']).to_integral_value(rounding=ROUND_FLOOR);decimals=int(params.get('decimals','8'))
        color=Decimal(10)**(decimals-44)*supply;burn=Decimal(params.get('burn','0'))
        total=sum(Decimal(c['amount']) for c in inputs)
        assert total-sum(Decimal(c['amount']) for c in outputs)==burn,transaction
        assert Decimal(outputs[0]['amount'])==color and Decimal(outputs[1]['amount'])==total-color-burn,transaction
        token=outputs[0]['token'];assert token['decimals']==decimals and Decimal(token['total'])==supply,token
    assert outputs[0]['tokenid']=='0xFF' and outputs[0]['storestate'] is True and outputs[0]['address']=='0x01',outputs[0]
    assert outputs[1]['tokenid']=='0x00' and outputs[1]['storestate'] is False,outputs[1]
    assert token['script']=='RETURN TRUE' and token['name']=={'name':'Teaching token'},token
    mining=last['mining'];assert len(mining['calls'])==1,mining
    sync=request.get('synchronous',False);assert mining['calls'][0]['kind']==('sync' if sync else 'async'),mining
    assert len(mining['pending'])==int(not sync) and len(mining['posted'])==int(sync),mining

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'token-creation-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['WalletCommandsOracle.java','SqlDBOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java']]
    listing=build/'token-creation-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar','-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=effects=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='token-creation-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root]),Runner([str(build/'token_creation_runtime'),root])]
        try:
            cases=corpus(runners[0].ask({'kind':'witnessFixtures'}),runners[0].ask({'kind':'keyFixtures'}),runners[0].ask({'kind':'fundingFixtures'}))
            for i,request in enumerate(cases):
                output=[]
                for name,runner in zip(['java','cpp'],runners):
                    try:
                        raw=runner.ask(request);value=project(raw,request);validate(request,value);output.append(value)
                    except Exception as error:failures.append(dict(case=i,runtime=name,error=repr(error),request=request,stderr=runner.errors(),raw=locals().get('raw')));break
                if failures:break
                if output[0]!=output[1]:failures.append(dict(case=i,request=request,java=output[0],cpp=output[1]));break
                completed+=1;operations+=len(request['ops']);effects+=sum(len(x['events']) for x in output[0]['ops'])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual tokencreate with real wallet, MMR proofs, token/transaction signatures, generator, queued/synchronous miner and recording SQL. Exact replies/wire/effects and shared-name mutation. Independent coloring/conservation/supply/signature checks. Generated PRNG fixed at existing miner entry fixture; selected VM diagnostic prose normalized. No durable SQL, network acceptance, arbitrary inherited/reference/concurrency or full native-node parity certification.')
    (build/'token-creation-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
