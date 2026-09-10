"""Input, automatic witness and burn construction using pinned Java fixture bytes."""
import itertools,copy

def corpus(fixtures):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='run',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def direct(name,params=None,**kw):return op('direct',name=name,params=params or {},**kw)
    def txn(coins=(),scripts=(),proofs=()):return op('txn',id='test',coins=[dict(data=c) if isinstance(c,str) else c for c in coins],scripts=list(scripts),proofs=list(proofs))
    def chain(history='8',nodes=None):return op('chain',history=history,nodes=fixtures['nodes'] if nodes is None else nodes)
    def util(**kw):return op('utilities',id='test',**kw)
    def burn(amount='1',**kw):return op('burn',amount=amount,exclude=[],link='0xAABB',sign=False)|kw
    def miner(coins=(),**kw):return op('miner',coins=list(coins),**kw)
    def add(*ops,**kw):cases.append(dict(ops=list(ops),**kw))
    coins=fixtures['inputcoins'];ids=fixtures['ids'];proofs=fixtures['proofs']
    for name in ['txninput','txnbasics']:
        add(op('meta',name=name));add(op(text=name+' unknown:value'))
        for params in [{},{'id':'missing'},{'id':''},{'id':None},{'id':1}]:add(direct(name,params))
    for name in ['txninput','txnbasics']:
        for caller in ['0x00','0x1234']:
            add(chain(),txn(),op(text=name+' id:test;txnlist id:test',caller=caller))
    # Both transaction and coin overloads are observed after success and failure.
    for selected in [[],coins[:1],coins,coins[::-1],[coins[0],coins[0]]]:
        for exit_on_fail in [True,False]:
            add(op('load'),chain(),txn(selected),util(exit=exit_on_fail),util(exit=exit_on_fail))
            add(op('load'),chain(),txn(selected),direct('txnbasics',{'id':'test'}))
    for coin in coins:
        for single in [True,False]:
            for history in ['-1','0','1','2','8','11','12','256','1.5']:
                add(op('load'),chain(history),txn([coin]),util(single=single))
    for nodes in [[],fixtures['nodes'][4:],fixtures['nodes'][:4],fixtures['nodes'][::2]]:
        for single in [True,False]:add(op('load'),chain(nodes=nodes),txn(coins[:1]),util(single=single))
    for fail,runtime,single in itertools.product(range(1,26),[False,True],[False,True]):
        add(op('load'),chain(),txn(coins[:1]),util(single=single,fail=fail,runtimeFailure=runtime))
    for rows in [[],[row,row],[row|{'script':None}],[row|{'script':'RETURN FALSE'}]]:
        for single,exit_on_fail in itertools.product([True,False],[True,False]):
            add(op('load'),chain(),txn(coins[:1],scripts=['RETURN TRUE']),util(single=single,exit=exit_on_fail,rows=rows))
    # Found coin, spent coin, floating coin and wire import precedence.
    for coinid in [*ids,'0x9999','0x','bad',' 0x65 ']:
        for floating,scriptmmr in itertools.product(['true','false'],repeat=2):
            add(op('load'),chain(),txn(),direct('txninput',dict(id='test',coinid=coinid,floating=floating,scriptmmr=scriptmmr)),direct('txnbasics',{'id':'test'}))
    for data in [*coins,coins[0]+'FF','0x','0x00',coins[0][:-2],'bad']:
        for floating,scriptmmr in itertools.product(['true','false'],repeat=2):
            add(op('load'),chain(),txn(),direct('txninput',dict(id='test',coindata=data,floating=floating,scriptmmr=scriptmmr)))
    for address,amount,floating in itertools.product(['0x01','Mx200104JMFSAS','0x02'],['0','1','2','3','-1','1.5'],['true','false']):
        add(op('load'),chain(),txn(),direct('txninput',dict(id='test',address=address,amount=amount,floating=floating)),direct('txnbasics',{'id':'test'}),util())
    for params in [dict(coinid=ids[0],coindata='bad',address='bad'),dict(coinid='0x9999',coindata=coins[0]),dict(coindata=coins[0],address='bad'),dict(address='bad',amount='bad',tokenid='bad'),dict(address='0x01',amount='bad',tokenid='bad'),dict(address='0x01',amount='bad',tokenid='0x01')]:
        add(chain(),txn(),direct('txninput',dict(id='test')|params))
    for field in ['id','coinid','coindata','floating','address','amount','tokenid','scriptmmr']:
        for value in [None,True,False,1,[],{},'',' ','TRUE','bad']:
            params=dict(id='test',address='0x01',amount='1');params[field]=value
            add(op('load'),chain(),txn(),direct('txninput',params))
    # An output's ID changes only after input insertion; failures must keep that mutation.
    for smmr in ['true','false','',None,1]:
        add(chain(),txn(),op(text='txnoutput id:test amount:1 address:0x02'),direct('txninput',dict(id='test',coindata=coins[0],scriptmmr=smmr)),op(text='txnlist id:test'))
    # Burn construction: ordered exclusion, mining membership, amount checks and signing.
    for amount,sign in itertools.product(['0','1','3','4','5','7','8','9','10','-1','0.000000000000000000000000000000000000000000001'],[False,True]):
        add(op('load'),chain(),miner(),burn(amount,sign=sign),expectBurn=dict(amount=amount,sign=sign))
    for excluded,locked in itertools.product([[],ids[:1],ids[2:],ids],[[],coins[:1],coins[2:],coins]):
        add(op('load'),chain(),miner(locked),burn('1',exclude=excluded))
    for rows in [[],[row|{'script':None}],[row|{'publickey':None}],[row|{'address':None}],[row|{'seed':'0x00'}]]:
        add(op('load'),chain(),miner(),burn('1',rows=rows,sign=True))
    for fail,runtime in itertools.product(range(1,65),[False,True]):
        add(op('load'),chain(),miner(),burn('1',sign=True,fail=fail,runtimeFailure=runtime))
    for amount in ['-1','0','1']:
        add(op('load'),chain(),burn(amount))
        add(op('load'),chain(),miner(null=True),burn(amount))
        add(op('load'),chain(nodes=[]),miner(),burn(amount))
    for text in ['bad','TRUE','a'*18,'a'*19,'+'+'a'*18,'-'+'a'*19,'.','+','-','e','e1','1e','1e+','1e-','1e.','1e++1','1e1.2','1..2','0'*19+'x','1e12345678901x','1e0000000000x','1e00000000000001','１２.３','١.٢e٢','12🐼','0'*19+'🐼']:
        add(chain(),txn(),direct('txninput',dict(id='test',address='0x01',amount=text)))
    tokennodes=copy.deepcopy(fixtures['nodes']);tokennodes[-1]['coins']+=fixtures['tokencoins'][1:4]
    for tokenid,amount in itertools.product(fixtures['tokenids'],['1','0.001','10','-1']):
        add(chain(nodes=tokennodes),txn(),direct('txninput',dict(id='test',address='0x01',amount=amount,tokenid=tokenid)))
    for selected,cascade,onchain,burnw in itertools.product([proofs[:1],proofs[2:],proofs],[False,True],[False,True],[False,True]):
        add(op('load'),chain(),miner(),op('pool',proofs=selected,cascade=cascade,onChain=onchain,burnWitness=burnw),burn('1'),expectPool=True)
    return cases
