"""Source-driven selector/funding/send experiments, with state after each step."""
import copy,itertools,random

def corpus(fixtures,key,tokens):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def command(name,params=None,**kw):return op(name=name,params=params or {},**kw)
    def fund(**params):return command('txnaddamount',dict(id='test',amount='1',address='0x02')|params)
    def send(**params):return command('send',dict(amount='1',address='0x02',dryrun='true')|params)
    def add(*ops,**kw):cases.append(dict(funding=True,ops=list(ops),**kw))
    def setup(nodes=None):return [op('load'),op('chain',history='8',nodes=fixtures['nodes'] if nodes is None else nodes),op('mining'),op('txn',id='test',coins=[],proofs=[],scripts=[])]
    for name in ['send','txnaddamount','txncoinlock']:
        add(op('meta',name=name));add(op('run',text=name+' unknown:value'))
    # Constructor/helper behavior is tested without a wallet search masking it.
    examples=[[],[('01','AA','10'),('02','BB','9'),('03','AA','1')],[('01','AA','5'),('02','BB','5'),('03','AA','5')],[('01','01','0'),('02','02','-1')]]
    rng=random.Random(5564)
    for _ in range(25):examples.append([(f'{i+1:02X}',f'{rng.randrange(1,5):02X}',str(rng.randrange(-1,20))) for i in range(rng.randrange(1,10))])
    for example in examples:
        coins=[dict(id='0x'+i,address='0x'+a,amount=v) for i,a,v in example]
        add(op('selection',coins=coins,order=True,mutate=True))
        for amount in ['0','8','30']:add(op('mining'),op('selection',coins=coins,amount=amount,debug=True,mutate=True))
    special=[dict(id='0x01',address='0xAA',amount='10'),dict(id='0x02',address='0xBB',amount='9'),dict(id='0x03',address='0xAA',amount='1')]
    add(op('mining'),op('selection',coins=special,amount='8'),expectSelection=['0x01'])
    add(op('mining'),op('selection',coins=special+[dict(alias=0)],order=True,mutate=True),expectAliases=True)
    add(op('selection',coins=special,amount='8'))
    add(op('mining',nullMiner=True),op('selection',coins=special,amount='8'))
    add(op('mining',nullMiner=True),op('selection',coins=[],amount='8'),expectSelection=[])
    for action in ['', 'lock','unlock','LOCK','other']:
        add(command('txncoinlock',{'action':action}))
    add(command('txncoinlock'),op('coinLock',action='add',id='0x01'),command('txncoinlock',{'action':'lock'}),op('coinLock',action='add',id='0x01'),op('coinLock',action='add',id='0x01'),op('coinLock',action='contains',id='0x01'),op('coinLock',action='clear'),op('coinLock',action='contains',id='0x01'),command('txncoinlock',{'action':'unlock'}),expectLocksCleared=True)
    for amount,split,burn in itertools.product(['0','1','3','4','5','8','9','-1'],['1','2','3','0','-1','1.9'],['0','0.5']):
        add(*setup(),fund(amount=amount,split=split,burn=burn))
    for only in ['true','false']:
        add(*setup(),fund(amount='4',onlychange=only),expectFunding=dict(inputs=1,outputs=int(only=='false')))
    for field in ['id','amount','address','onlychange','tokenid','fromaddress','burn','storestate','split']:
        for value in [None,True,1,{},[],'','bad']:
            add(*setup(),command('txncoinlock',{'action':'lock'}),fund(**{field:value}),command('txncoinlock'))
    add(*setup(),command('txncoinlock',{'action':'lock'}),fund(amount='4'),fund(amount='3'),fund(amount='1'),fund(amount='1'),command('txncoinlock',{'action':'unlock'}),fund(amount='4'),expectLocksCleared=True)
    add(*setup(),command('txncoinlock',{'action':'lock'}),fund(address='nothex'),expectRetained=True)
    for state in [True,False]:add(*setup(),fund(amount='1',storestate=str(state).lower(),fromaddress='0x01'))
    for rows in [[],[row|{'address':None}],[row|{'publickey':None}]]:add(*setup(),command('txnaddamount',dict(id='test',amount='1',address='0x02'),rows=rows))
    for fail,runtime in itertools.product(range(1,20),[False,True]):add(*setup(),command('txnaddamount',dict(id='test',amount='1',address='0x02'),fail=fail,runtimeFailure=runtime))
    add(op('load'),op('txn',id='test',coins=[],proofs=[],scripts=[]),fund())
    add(fund());add(op('load'),fund())
    for coinset in [fixtures['inputcoins'][:1],fixtures['inputcoins'][2:],fixtures['inputcoins']]:
        for amount in ['1','5','9']:
            add(*setup(),op('miner',coins=coinset),fund(amount=amount))
            add(*setup(),op('miner',coins=coinset),send(amount=amount))
    for proofs in [fixtures['proofs'][:1],fixtures['proofs'][2:],fixtures['proofs']]:
        for amount in ['1','5','9']:
            add(*setup(),op('pool',proofs=proofs),fund(amount=amount))
            add(*setup(),op('pool',proofs=proofs),send(amount=amount))
    # Custom-token funding requires no proofs; source-derived metadata retained.
    nodes=copy.deepcopy(fixtures['nodes']);nodes[-1]['coins']+=fixtures['tokencoins'][1:4];nodes[-1]['relevant']+=fixtures['tokencoins'][1:4]
    for tokenid,amount in itertools.product(fixtures['tokenids'],['1','0.001','10','-1']):
        add(*setup(nodes),fund(tokenid=tokenid,amount=amount))
        add(*setup(nodes),fund(tokenid=tokenid,amount=amount,burn='1'))
    # Send through actual proofs, output creation, key checking and generator.
    for amount,split,burn in itertools.product(['0','1','4','5','8','9','-1'],['1','3'],['0','0.5']):add(*setup(),send(amount=amount,split=split,burn=burn))
    for field in ['multi','amount','address','tokenid','debug','dryrun','storestate','burn','split','state','mine','coinage','signkey','fromaddress']:
        for value in [None,True,1,{},[],'','bad']:add(*setup(),send(**{field:value}))
    for multi in [[],['0x02:1'],[':0x02::1:ignored'],['0x02'],['::'],['0x02:0'],['0x02:1','0x03:2'],[None],[1],['Mx200104JMFSAS:1'],['MxINVALID:1']]:add(*setup(),send(multi=multi))
    for state in [{'0':'[hello]','1':123,'2':True,'3':None},{'0':[1,2]},{'0':{'x':'y'}},{'256':'x'},{'bad':'x'},{'01':'first','1':'second'},{'١':'42'}]:add(*setup(),send(state=state))
    for coinage in ['2','3','4','8','9','10','11','12','3.5']:add(*setup(),send(coinage=coinage))
    for signkey in ['0x',key['publickey'],'0xFF']:
        for dry in ['true','false']:add(*setup(),send(signkey=signkey,dryrun=dry))
    for size in ['-1','0','1000','65536']:add(*setup(),op('limitSize',size=size),send())
    for mine in ['true','false']:
        add(*setup(),send(dryrun='false',mine=mine),op('verifyMined'),expectMined=True)
        for mode in ['false','throw']:add(*setup(),command('send',dict(address='0x02',amount='1',mine=mine),mineResult=mode))
    add(*setup(),send(),expectDryRun=True)
    add(*setup(),send(password='ignored',amount='4'),expectDryRun=True)
    add(*setup(),send(password='invalid',dryrun='false'))
    for fail,runtime in itertools.product(range(1,30),[False,True]):add(*setup(),command('send',dict(address='0x02',amount='1',dryrun='true'),fail=fail,runtimeFailure=runtime))
    for fixture in tokens:
        for amount,burn,dry in itertools.product(['1','10','100','101','0.000000000000000000000000000000000000000000001'],['0','1'],['true','false']):
            add(*setup(fixture['nodes']),send(tokenid=fixture['tokenid'],amount=amount,burn=burn,dryrun=dry))
        if fixture['script']=='RETURN TRUE':
            add(*setup(fixture['nodes']),send(tokenid=fixture['tokenid'],amount='10',burn='1'),expectDryTokenBurn=True)
    for fail,runtime in itertools.product(range(1,30),[False,True]):add(*setup(),command('send',dict(address='0x02',amount='1'),fail=fail,runtimeFailure=runtime))
    for dry,password in itertools.product(['true','false'],['fixture pass','wrong']):
        locked=row|{'phrase':'','seed':'0x00'}
        add(*setup(),op('vaultLock',password='fixture pass',rows=[locked]),send(dryrun=dry,password=password),vault=True)
    # Pool identity is fixed before hashing, so rejected-ID log bodies compare exactly.
    add(*setup(),op('pool',proofs=fixtures['proofs'][:1]*1000),send())
    return cases

def normalize(value):
    # A dry-run has no miner entry where the established fixture fixes PRNG.
    # All other TxPoW fields remain compared, including cached header hash and size.
    value=copy.deepcopy(value)
    for entry in value.get('ops',[]):
        reply=entry.get('value')
        if isinstance(reply,dict) and reply.get('dryrun') is True:
            body=reply['response']['body'];prng=body['prng']
            assert prng.startswith('0x') and len(prng)==66,body
            body['prng']='<32 random bytes>'
    return value

def validate(request,output):
    final=output['ops'][-1]
    if 'expectSelection' in request:assert [c['coinid'] for c in final['value']['selected']]==request['expectSelection'],final
    if request.get('expectAliases'):assert sum(c['spent'] for c in final['value']['sorted'])==2,final
    if request.get('expectLocksCleared'):assert final['coinLocks']==[] and final['coinLockEnabled'] is False,final
    if request.get('expectFunding'):
        txn=final['value']['response']['transaction'];expected=request['expectFunding'];assert len(txn['inputs'])==expected['inputs'] and len(txn['outputs'])==expected['outputs'],final
    if request.get('expectRetained'):assert 'exception' in final and len(final['coinLocks'])==1,final
    if request.get('expectMined'):assert final['value']==[True],final
    if request.get('expectDryTokenBurn'):
        body=final['value']['response']['body'];assert body['witness']['signatures']==[] and len(body['burnwitness']['signatures'])==1,final
        assert body['burntxn']['linkhash']==body['txn']['transactionid'],final
    if request.get('expectDryRun'):
        reply=final['value'];assert reply['status'] and reply['dryrun'],final
        assert reply['response']['body']['witness']['signatures']==[] and final['mining']['calls']==[],final
        assert reply['bytesize']['txpow']==reply['response']['size'],final
