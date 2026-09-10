"""Stored-transaction mining/posting using actual existing consensus/wallet fixtures."""
import itertools

def corpus(fixtures,wallet):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='run',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def direct(name,params=None,**kw):return op('direct',name=name,params=params or {},**kw)
    def fixture(f,**kw):return op('validationFixture',mmr=f['mmr'],pow=f['pow'],block=f['block'],hashrate='1',proofCoins=True,**kw)
    def add(*ops,**kw):cases.append(dict(ops=list(ops),**kw))
    valid=fixtures[0]
    for name in ['txnpost','txnmine','txnminepost']:
        add(op('meta',name=name));add(op(text=name+' invalid:true'))
        for params in [{},{'id':'missing'},{'data':'bad'},{'id':None},{'id':1},{'data':None},{'data':1}]:add(direct(name,params))
    for f in fixtures[::3]:
        add(fixture(f),op('mining'),direct('txnpost',{'id':'test'}),op('mineState'))
    for name,params in [('txnpost',{'id':'test'}),('txnpost',{'id':'test','mine':'true'}),('txnmine',{'id':'test'}),('txnmine',{'data':valid['row']})]:
        add(fixture(valid),direct(name,params))
        add(fixture(valid),op('mining',nullMiner=True),direct(name,params))
    for name,params,kind,post,shared in [('txnpost',{'id':'test'},'async',True,True),('txnpost',{'id':'test','mine':'true'},'sync',True,True),('txnmine',{'id':'test'},'sync',False,True),('txnmine',{'data':valid['row']},'sync',False,False)]:
        add(fixture(valid),op('mining'),direct(name,params),expectMining=dict(op=2,kind=kind,post=post,shared=shared))
    add(fixture(valid),op('mining'),direct('txnpost',{'id':'test','txndelete':'true'}),op('mineNext'),expectMining=dict(op=2,kind='async',post=True,deleted=True))
    add(fixture(valid),op('mining'),direct('txnmine',{'id':'test'}),op('postHeld'),expectMining=dict(op=2,kind='sync',post=False))
    for text in ['txnpost id:test','txnpost id:test auto:true','txnmine id:test']:
        add(fixture(valid),op('mining'),op(text=text,caller='0x1234'))
    for field,values in [('auto',[None,True,1,'','true','false','TRUE','bad']),('mine',[None,True,1,'','true','false','TRUE','bad']),('txndelete',[None,True,1,'','true','false','TRUE','bad']),('burn',[None,True,1,'','-1','0','1','0.5','bad'])]:
        for value in values:add(fixture(valid),op('mining'),direct('txnpost',{'id':'test',field:value}),op('mineState'))
    for value in [None,True,1,'','missing','test']:
        add(fixture(valid),op('mining'),direct('txnmine',{'id':value,'data':'bad'}))
    for mode,post in itertools.product(['false','throw'],['txnpost','txnmine']):
        add(fixture(valid),op('mining'),direct(post,{'id':'test','mine':'true','txndelete':'true'},mineResult=mode),op('mineState'))
    add(fixture(valid),op('mining'),direct('txnpost',{'id':'test','mine':'true','txndelete':'true'},failPost=True),op('mineState'))
    for size in ['-1','0','10','512.9','1000','65536']:
        add(fixture(valid),op('mining'),op('limitSize',size=size),direct('txnpost',{'id':'test'}))
    for name,data in [('txnmine',valid['row']),('txnminepost',valid['pow'])]:
        for length in [0,1,2,3,4,8,16,32,64,(len(data)-2)//2-1,(len(data)-2)//2,(len(data)-2)//2+1]:
            payload=(data+'FF')[:2+length*2]
            for receiver in [[],[op('mining')]]:add(fixture(valid),*receiver,direct(name,{'data':payload}))
    add(op('mining'),direct('txnminepost',{'data':valid['bodyless']}))
    for f in [fixtures[0],fixtures[1],next(f for f in fixtures if f['mode']==29)]:
        add(op('mining'),direct('txnminepost',{'data':f['pow']}))
    add(op('mining'),direct('txnminepost',{'data':valid['pow']},failPost=True))
    # Reuse the input/burn corpus's source-generated chain and actual wallet rows.
    for amount,automatic,fail in itertools.product(['0','1','4','9'],['false','true'],[0,1,12]):
        add(op('load'),op('chain',history='8',nodes=wallet['nodes']),op('mining'),
            op('txn',id='test',coins=[],proofs=[],scripts=[]),
            direct('txnpost',{'id':'test','burn':amount,'auto':automatic},fail=fail),op('mineState'),expectBurnPost=(fail==0 and amount in ['1','4']))
    for amount in ['0','1','4','9']:
        add(op('load'),op('chain',history='8',nodes=wallet['nodes']),op('mining'),
            op('txn',id='test',coins=[dict(data=wallet['inputcoins'][0])],proofs=[],scripts=[]),
            direct('txnpost',{'id':'test','burn':amount,'auto':'true'}),op('mineState'))
    for text in ['txnstate id:test port:5 value:23','txnclear id:test','txndelete id:test']:
        add(fixture(valid),op('mining'),direct('txnpost',{'id':'test'}),op(text=text),op('mineState'))
    return cases
