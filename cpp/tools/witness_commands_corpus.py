"""Source-order witness command experiments, using Java-produced chain/coin bytes."""
import itertools

def corpus(fixtures):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(operation='run',**kw):
        value=dict(op=operation,rows=[row],probe=['0x01'],caller='0x00');value.update(kw);return value
    def direct(name,params=None,**kw):return op('direct',name=name,params=params or {},**kw)
    def txn(coins=(),proofs=(),scripts=()):return op('txn',id='test',coins=[dict(data=c) if isinstance(c,str) else c for c in coins],proofs=list(proofs),scripts=list(scripts))
    def chain(history='8',nodes=None):return op('chain',history=history,nodes=fixtures['nodes'] if nodes is None else nodes)
    def add(*ops,expected=None):
        case=dict(ops=list(ops))
        if expected is not None:case['expectWitness']=expected
        cases.append(case)
    coins=fixtures['inputcoins'];proofs=fixtures['proofs'];scriptcoins=fixtures['scriptcoins'];branches=fixtures['branches']
    for name in ['txnscript','txnmmr']:
        add(op('meta',name=name));add(op(text=name+' bad:value'));add(direct(name,{'id':'missing'}))
    add(txn(),direct('txnscript',{'id':'test','scripts':{'RETURN TRUE':'','RETURN FALSE':''}}),expected=dict(scripts=2,proofs=0))
    for count in range(5):
        for existing in range(6):
            add(chain(),txn(coins[:count],(proofs*2)[:existing]),direct('txnmmr',{'id':'test'}),direct('txnmmr',{'id':'test'}),expected=dict(scripts=0,proofs=max(count,existing)))
    for history,created in itertools.product(['-1','0','1','2','8','11','12','256','1.5'],['-1','0','3','11','12','3.5']):
        add(chain(history),txn([dict(data=coins[0],created=created)]),direct('txnmmr',{'id':'test'}))
    for nodes in [[],fixtures['nodes'][4:],fixtures['nodes'][:4],fixtures['nodes'][::2]]:
        for existing in [0,1,3]:add(chain(nodes=nodes),txn(coins[:2],proofs[:existing]),direct('txnmmr',{'id':'test'}))
    for entry in ['-1','0','1','2','3','4','99','18446744073709551616']:
        add(chain(),txn([dict(data=coins[0],entry=entry)]),direct('txnmmr',{'id':'test'}))
    for sequence in [coins,coins[::-1],[coins[0],coins[0]],[],scriptcoins[:1]]:
        add(chain(),txn(sequence),direct('txnmmr',{'id':'test'}),op(text='txnclear id:test mmr:true'),direct('txnmmr',{'id':'test'}))
    for caller in ['0x00','0x1234']:
        add(chain(),txn(coins[:2]),op(text='txnmmr id:test;txnlist id:test',caller=caller))
    # Preserve LinkedHashMap order and additions preceding a later bad proof.
    for code in ['RETURN TRUE','RETURN FALSE',' return  true ','','  ','RETURN [🐼] EQ [🐼]','RETURN "unterminated']:
        for proof in ['',*branches,'0x','0x00','bad',None,True,1,{},[]]:
            add(txn(),direct('txnscript',{'id':'test','scripts':{'RETURN 7':'',code:proof}}))
    for proof in [branches[0]+'FF',branches[-1][:-2],branches[-1][:12]]:
        add(txn(),direct('txnscript',{'id':'test','scripts':{'RETURN TRUE':proof}}))
    add(txn(),direct('txnscript',{'id':'test','scripts':{'RETURN TRUE':''}}),direct('txnscript',{'id':'test','scripts':{'RETURN TRUE':''}}),expected=dict(scripts=1,proofs=0))
    for values in [scriptcoins[:1],scriptcoins[:2],[scriptcoins[0],scriptcoins[0]],coins[:2],[]]:
        for existing in [[],['RETURN TRUE'],['RETURN FALSE']]:
            for fail,runtime in [(0,False),(1,False),(4,False),(7,False),(10,False),(1,True),(7,True),(11,True)]:
                add(op('load'),txn(values,scripts=existing),direct('txnscript',{'id':'test','auto':'true','scripts':{'ignored':'bad'}},fail=fail,runtimeFailure=runtime))
    for rows in [[],[row,row],[{**row,'script':None}],[{**row,'script':'RETURN FALSE'}]]:
        add(op('load'),txn(scriptcoins[:2]),direct('txnscript',{'id':'test','auto':'true'},rows=rows))
    add(txn(scriptcoins[:1]),direct('txnscript',{'id':'test','auto':'true'}))
    add(op('load'),txn(scriptcoins[:1]),direct('txnscript',{'id':'test','auto':'true'},nullRows=True))
    add(op('load'),txn(scriptcoins[:1]),direct('txnscript',{'id':'test','auto':'true'}),direct('txnscript',{'id':'test','auto':'true'}),expected=dict(scripts=1,proofs=0))
    for value in [None,True,False,1,[],{},'',' ','true','false','TRUE']:
        for field in ['id','scripts','auto']:
            add(txn(),direct('txnscript',{'id':'test',field:value}))
        add(chain(),txn(),direct('txnmmr',{'id':value}))
    for automatic in ['true','false']:
        add(txn(),direct('txnscript',{'id':'test','scripts':None,'auto':automatic}))
    for caller in ['0x00','0x1234']:
        for text in ['txnscript id:test scripts:{"RETURN TRUE":""};txnlist id:test','txnscript id:test auto:true;txnlist id:test','txnscript id:test scripts:{"RETURN TRUE":"0x"};txnlist id:test','txnscript id:test scripts:{"RETURN TRUE":null};txnlist id:test']:
            add(op('load'),txn(scriptcoins[:1]),op(text=text,caller=caller))
    for value,automatic in itertools.product([True,False,1,[],"","bad"],['true','false']):
        add(txn(),direct('txnscript',{'id':'test','scripts':value,'auto':automatic}))
        add(txn(),direct('txnscript',{'id':'missing','scripts':value,'auto':automatic}))
    return cases
