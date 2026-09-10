"""Vault and stored-transaction signing through actual existing dependencies."""
import itertools

def corpus(vault,wallet,consensus):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey=vault['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    locked={**row,'phrase':'','seed':'0x00'}
    cases=[]
    def op(kind='run',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00',keysCreated=True)|kw
    def direct(name,params=None,**kw):return op('direct',name=name,params=params or {},**kw)
    def draft(coins=None):return op('txn',id='test',coins=[dict(data=c) for c in (coins or [])],proofs=[],scripts=[])
    def add(*ops,**kw):cases.append(dict(vault=True,ops=list(ops),**kw))
    def loaded(*ops,**kw):add(op('load'),*ops,**kw)
    def prepareLocked():return [op('vaultSeed',phrase='',seed='0x00'),op('vaultEncrypted',data=vault['encrypted'])]
    for name in ['vault','txnsign']:
        add(op('meta',name=name));add(op(text=name+' invalid:true'))
        for params in [{},{'id':'missing'},{'action':None},{'publickey':None},{'action':1}]:
            add(direct(name,params));loaded(draft(),direct(name,params))
    for action in ['seed','wipekeys','restorekeys','passwordlock','passwordunlock','testphrase','resetkeys','bad','SEED']:
        loaded(op('vaultMain'),direct('vault',dict(action=action,seed='0xAA',phrase='aban abil able',password='fixture pass',numkeys='0'),rows=[locked] if action in ['wipekeys','passwordlock'] else [row]))
    for kind in ['vaultCheck','vaultStop']:
        loaded(op(kind));loaded(op('vaultMain'),op(kind,keysCreated=True));loaded(op('vaultMain'),op(kind,keysCreated=False))
    for keys,fail in itertools.product([False,True],[0,1,3]):
        loaded(op('vaultMain'),direct('vault',{'action':'wipekeys','seed':'0xAA'},keysCreated=keys,fail=fail,rows=[locked]))
    for params in [{'action':'passwordlock','password':'a;b'},{'action':'passwordlock','password':'fixture pass','confirm':'wrong'},{'action':'passwordlock','password':'fixture pass','confirm':None},{'action':'passwordlock','password':None},{'action':'passwordunlock','password':None},{'action':'wipekeys','seed':'0xBB'}]:loaded(op('vaultMain'),direct('vault',params))
    for password in ['fixture pass','','a;b','π 🐼']:
        loaded(op('vaultLock',password=password,observePassword=password,rows=[locked]),op('vaultUnlock',password=password),direct('vault'),expectUnlocked=True)
    loaded(op('vaultLock',password=None))
    for data,password in [(vault['encrypted'],'fixture pass'),(vault['encrypted'],'wrong'),(vault['encrypted'][:-2],'fixture pass'),('0x00','fixture pass'),('0x','fixture pass'),(vault['encrypted'],None)]:
        loaded(op('vaultSeed',phrase='',seed='0x00'),op('vaultEncrypted',data=data),op('vaultUnlock',password=password))
    for fail,runtime in itertools.product([1,2,3,4,5,6,7,8,12,20,32,50],[False,True]):
        loaded(op('vaultLock',password='fixture pass',rows=[locked],fail=fail,runtimeFailure=runtime),direct('vault'))
        loaded(*prepareLocked(),op('vaultUnlock',password='fixture pass',fail=fail,runtimeFailure=runtime),direct('vault'))
    for phrase in ['ABANDON ABILITY ABLE','abandon ability able','aban abil able','  aban   abil able  ','invalidword','ab','abandon\tability','KIND','kind','𐐀']:
        loaded(direct('vault',{'action':'restorekeys','phrase':phrase}),direct('vault'))
    loaded(direct('vault',{'action':'testphrase','phrase':'aban abil able','numkeys':'1'}))
    for num in ['-1','0','1.5','bad',None]:loaded(direct('vault',{'action':'testphrase','phrase':'aban abil able','numkeys':num}))
    for failure in ['', 'restoreReady','setHasShutDown','stopMessageProcessor']:
        loaded(op('vaultMain'),op('vaultStores'),op('vaultTarget',target=1),direct('vault',{'action':'resetkeys','phrase':'aban abil able','keyuses':'101'},vaultFail=failure),expectReset=not failure)
    for fail in [1,4,12,20,36]:loaded(op('vaultMain'),op('vaultStores'),op('vaultTarget',target=0),direct('vault',{'action':'resetkeys','phrase':'aban abil able'},fail=fail))
    # Actual wallet signing with a row public key derived by the source TreeKey.
    for publickey,coins,count in [('auto',[],0),('auto',wallet['inputcoins'][:1],1),('auto',[wallet['inputcoins'][0]]*2,1),(vault['publickey'],[],1)]:
        loaded(draft(coins),direct('txnsign',{'id':'test','publickey':publickey}),op('verifyStored'),expectSignatures=count)
    loaded(draft(wallet['inputcoins'][:1]),direct('txnsign',{'id':'test','publickey':'auto'}),direct('txnsign',{'id':'test','publickey':'auto'}),op('verifyStored'),expectSignatures=1)
    for rows in [[],[{**row,'simple':0}],[{**row,'publickey':None}],[{**row,'address':None}]]:
        loaded(draft(wallet['inputcoins'][:1]),direct('txnsign',{'id':'test','publickey':'auto'},rows=rows))
        loaded(draft(),direct('txnsign',{'id':'test','publickey':vault['publickey']},rows=rows))
    for fail,runtime in itertools.product(range(1,19),[False,True]):
        loaded(draft(wallet['inputcoins'][:1]),direct('txnsign',{'id':'test','publickey':'auto'},fail=fail,runtimeFailure=runtime))
    for publickey in ['auto',vault['publickey'],'custom']:
        params={'id':'test','publickey':publickey,'password':'fixture pass','privatekey':row['privatekey'],'keyuses':'0'}
        loaded(draft(),*prepareLocked(),direct('txnsign',params,rows=[locked]),direct('vault'),expectLocked=True)
    loaded(draft(wallet['inputcoins'][:1]),*prepareLocked(),direct('txnsign',{'id':'test','publickey':'auto','password':'fixture pass'},rows=[locked],fail=12),direct('vault'))
    for params in [{'id':'test','publickey':'custom'},{'id':'test','publickey':'custom','privatekey':'bad','keyuses':'0'},*({'id':'test','publickey':'custom','privatekey':row['privatekey'],'keyuses':uses} for uses in ['-1','0','1','262144','0.5','bad',None])]:
        if params.get('keyuses') in ['0','1','262144','0.5'] and params.get('privatekey')==row['privatekey']:
            loaded(draft(),direct('txnsign',params),op('verifyStored'),expectSignatures=1)
        else:loaded(draft(),direct('txnsign',params))
    for method,index in itertools.product(['child','wots'],[-2147483648,-1,0,1,2,2147483647]):add(op('treeIndex',method=method,index=index))
    # The presence of txnpostauto triggers posting even when its value is false.
    valid=consensus[0]
    def fixture():return op('validationFixture',mmr=valid['mmr'],pow=valid['pow'],block=valid['block'],hashrate='1',proofCoins=True)
    loaded(fixture(),op('vaultMain'),direct('txnsign',{'id':'test','publickey':vault['publickey'],'txnpostauto':'true'}),expectPostError='Not enough blocks in chain to make valid MMR Proofs..')
    for automatic,mine,remove in itertools.product(['true','false'],['true','false'],['true','false']):
        preparation=[fixture()]
        if automatic=='true':preparation += [op('chain',history='8',nodes=wallet['nodes']),draft(wallet['inputcoins'][:1])]
        loaded(*preparation,op('vaultMain'),direct('txnsign',{'id':'test','publickey':vault['publickey'],'txnpostauto':automatic,'txnpostmine':mine,'txndelete':remove}),expectPosted=dict(deleted=remove=='true',sync=mine=='true'))
    for field in ['txnpostauto','txnpostburn','txnpostmine','txndelete']:
        for value in [None,True,'','bad']:
            loaded(fixture(),op('vaultMain'),direct('txnsign',{'id':'test','publickey':vault['publickey'],'txnpostauto':'false',field:value}))
    loaded(draft(),direct('txnsign',{'id':'test','publickey':'auto','txndelete':None}),op('verifyStored'),expectSignatures=0)
    loaded(draft(),op('addNullSignature'),op('signedBy',publickey=None))
    loaded(draft(),direct('txnsign',{'id':'test','publickey':vault['publickey']}),op('signedBy',publickey=None))
    for caller in ['0x00','0x1234']:
        loaded(draft(),op('vaultMain'),op(text='txnsign id:test publickey:auto;vault',caller=caller))
    return cases
