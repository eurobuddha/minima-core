"""Coins command through actual source search, reservations and selector."""
import copy,itertools

def corpus(fixtures,witness):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    rows=[row|dict(address='0x0'+str(i)) for i in range(1,4)]
    groups=fixtures['groups'];native,token=groups[:2];cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01','0x02','0x03'],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='coins',params=params or {},**kw)
    def chain(coins=None,relevant=None,older=None,empty=False,mega=False,megaCoins=None,original=False):
        nodes=copy.deepcopy(witness['nodes'] if original else fixtures['nodes'])
        if not original:
            for node in nodes:node['coins']=[];node['relevant']=[]
            if coins is None:coins=native['coins']
            nodes[-1]['coins']=coins;nodes[-1]['relevant']=coins if relevant is None else relevant
            if older is not None:nodes[-2]['coins']=older;nodes[-2]['relevant']=older
        return op('chain',history='8',nodes=[] if empty else nodes,mega=mega,megaCoins=megaCoins or [])
    def add(*ops,setup=None,load=True,miner=True,walletRows=None,label='',**expected):
        cases.append(dict(label=label,ops=([op('load',rows=rows if walletRows is None else walletRows)] if load else [])+[setup or chain()]+([op('miner',coins=[])] if miner else [])+list(ops),**expected))
    add(op('meta',name='coins'),miner=False)
    baseIDs=['0x80','0x81','0x82','0x84','0x85']
    add(cmd(),expectedIDs=baseIDs,expectedAges=['3','2','0','11','1'])
    add(cmd(dict(order='asc')),expectedIDs=['0x84','0x80','0x81','0x85','0x82'])
    for order in ['desc','DESC','ASC','other']:add(cmd(dict(order=order)),expectedIDs=baseIDs)
    for rel in [None,True,False,0,1,{},[],'','true','false','bad']:
        add(cmd(dict(relevant=rel,tokenid='0x00')),setup=chain(relevant=native['coins'][::2]),expectedIDs=['0x80','0x82','0x84'],label='presence not Boolean value')
    add(cmd(),setup=chain(relevant=[]),expectedIDs=[])
    add(cmd(dict(tokenid='0x00')),setup=chain(relevant=[]),expectedIDs=baseIDs)
    add(cmd(dict(amount='1')),setup=chain(relevant=[]),expectedIDs=[])
    for age,ids in [('-1',['0x80','0x81','0x82','0x83','0x84','0x85']),('0',baseIDs),('1',['0x80','0x81','0x84','0x85']),('2',['0x80','0x81','0x84']),('3',['0x80','0x84']),('3.0001',['0x84']),('11',['0x84']),('12',[])]:
        add(cmd(dict(coinage=age)),expectedIDs=ids,label='age threshold')
    for depth in ['-2','-1','0','1','2','10','11','12','2147483647','2147483648','4294967296','1.9']:
        add(cmd(dict(depth=depth)),setup=chain(coins=native['coins'][:1],older=native['coins'][1:]),label='depth wrapping')
    add(cmd(dict(depth='0')),setup=chain(coins=native['coins'][:1],older=native['coins'][1:]),expectedIDs=['0x80'])
    add(cmd(dict(depth='1')),setup=chain(coins=native['coins'][:1],older=native['coins'][1:]),expectedIDs=baseIDs)
    add(cmd(),setup=chain(coins=native['coins']*2),expectedIDs=baseIDs)
    add(cmd(),setup=chain(coins=native['spent'][:1],older=native['coins']),expectedIDs=['0x81','0x82','0x84','0x85'])
    # Same creation heights retain search order, including after the asc sort.
    add(cmd(dict(order='asc')),setup=chain(coins=native['coins']+token['coins']),expectedIDs=['0x84','0x94','0x80','0x90','0x81','0x91','0x85','0x95','0x82','0x92'])
    for address in ['0x01','0x1',fixtures['mx'],'0x0001','0X01','01','0xFF','0xzz','MxINVALID','']:
        add(cmd(dict(address=address)),label='address getter')
    for address in ['0x01',fixtures['mx']]:add(cmd(dict(address=address)),expectedIDs=['0x80'])
    for tid in ['0x00','0x0','0X00','0x0000',token['id'],token['id'].lower(),'0xFF','bad','']:
        add(cmd(dict(tokenid=tid)),setup=chain(coins=native['coins']+token['coins']),label='raw token parsing')
    for cid in ['0x80','0x080','0X80','80','0xFF','MxINVALID','bad','']:
        add(cmd(dict(coinid=cid)),label='raw coin parsing')
    for amount in ['0','1','2','3','6','100','-1','1.00','1E0']:
        add(cmd(dict(amount=amount,tokenid='0x00')),label='raw amount filter')
    for simple in ['true','false','TRUE']:
        add(cmd(dict(sendable=simple)),walletRows=[r|dict(simple=0) if r['address']=='0x03' else r for r in rows],expectedIDs=['0x80','0x81','0x84'] if simple=='true' else baseIDs)
    add(cmd(dict(sendable='true')),load=False,expectedIDs=[])
    for simple in ['true','false']:
        add(cmd(dict(sendable=simple)),setup=chain(coins=groups[2]['coins']),expectedIDs=['0xA0','0xA1','0xA2','0xA4','0xA5'],label='sendable does not evaluate token script')
    for field in ['simplestate','sendable','coinid','amount','address','tokenid','state','coinage','depth','megammr','checkmempool','order','totalamount']:
        for value in [None,True,False,1,{},[],'','bad']:
            add(cmd({field:value}),label='typed parameter '+field)
    # Missing tip skips only the later getters; errors in early getters remain.
    for field in ['simplestate','sendable','coinid','amount','address','tokenid','state','coinage','depth','megammr','checkmempool','order','totalamount']:
        add(cmd({field:True}),setup=chain(empty=True),miner=False,label='no tip '+field)
    add(cmd(dict(megammr=True,checkmempool=True,order=True,totalamount=True)),setup=chain(empty=True),miner=False,expectedIDs=[])
    for enabled,requested in itertools.product([False,True],['false','true']):
        add(cmd(dict(tokenid='0x00',megammr=requested)),setup=chain(coins=[],mega=enabled,megaCoins=native['coins']),expectedSet=baseIDs if enabled and requested=='true' else [])
    add(cmd(dict(tokenid='0x00',megammr='true')),setup=chain(coins=native['spent'][:1],mega=True,megaCoins=native['coins']),expectedSet=['0x81','0x82','0x84','0x85'])
    # Source-proven state fixtures exercise both JSON formats and substring search.
    for state,simplestate in itertools.product(['state','state0','[state1]','0x01','missing',''],['false','true']):
        add(cmd(dict(state=state,simplestate=simplestate)),setup=chain(coins=witness['tokencoins'][:8]),label='state matching',**({'stateShape':simplestate=='true','expectedIDs':{'state':['0x0'+str(i) for i in range(1,9)],'state0':['0x01','0x04','0x07'],'[state1]':['0x02','0x05','0x08'],'0x01':['0x01','0x04','0x07'],'missing':[]}[state]} if state else {'expectException':'BLANK param not allowed : state'}))
    # Main lookup and miner dereference occur at distinct points.
    for empty in [False,True]:
        setup=chain(coins=[] if empty else None)
        add(cmd(dict(checkmempool='true')),setup=setup,miner=False,expectException='java.lang.NullPointerException')
        add(op('miner',coins=[],null=True),cmd(dict(checkmempool='true')),setup=setup,miner=False,**({'expectedIDs':[]} if empty else {'expectException':'java.lang.NullPointerException'}))
        add(cmd(dict(totalamount='1')),setup=setup,miner=False,expectException='java.lang.NullPointerException')
        add(op('miner',coins=[],null=True),cmd(dict(totalamount='1')),setup=setup,miner=False,**({'expectedIDs':[]} if empty else {'expectException':'java.lang.NullPointerException'}))
    for reserved in [[],native['coins'][:1],native['coins'][::2],native['coins']]:
        ids={f'0x{128+i:02X}' for i,coin in enumerate(native['coins']) if coin in reserved}
        add(op('miner',coins=reserved),cmd(dict(checkmempool='true')),expectedIDs=[i for i in baseIDs if i not in ids])
    for cascade,onChain,burn in itertools.product([False,True],repeat=3):
        add(op('pool',proofs=witness['proofs'][:1],cascade=cascade,onChain=onChain,burnWitness=burn),cmd(dict(checkmempool='true')),setup=chain(original=True),expectedIDs=['0x68','0x67','0x65'] if cascade else ['0x68','0x67'])
    add(op('miner',coins=witness['inputcoins'][2:3]),op('pool',proofs=witness['proofs'][:1]),cmd(dict(checkmempool='true')),setup=chain(original=True),expectedIDs=['0x68'])
    for total,ids in [('0',['0x80']),('-1',['0x80']),('1',['0x80']),('2',['0x81']),('3',['0x84']),('4',['0x85']),('5',['0x85']),('6',['0x85']),('7',['0x85','0x82','0x84','0x81','0x80'])]:
        for order in ['asc','desc']:add(cmd(dict(totalamount=total,order=order)),expectedIDs=ids,label='reuse grouped selector')
    for total in ['0','1','100','300','400','600','700']:
        add(cmd(dict(totalamount=total,tokenid=token['id'])),setup=chain(coins=token['coins']),label='scaled token selection')
    add(cmd(dict(totalamount='400',tokenid=token['id'])),setup=chain(coins=token['coins']),expectedIDs=['0x95'])
    add(cmd(dict(totalamount='400',tokenid=token['id'])),setup=chain(coins=token['missing']),expectException='java.lang.NullPointerException')
    # Reservation skipping in selectCoins is not identical to a pre-filtered list.
    for check in ['false','true']:
        add(op('miner',coins=native['coins']),cmd(dict(totalamount='7',checkmempool=check)),expectedIDs=[] if check=='true' else ['0x85','0x82','0x84','0x81','0x80'])
    for text in ['coins','coins relevant:false','coins order:asc','coins totalamount:4','coins invalid:true','coins depth:-1;coins amount:1']:
        for caller in ['0x00','0x1234']:add(op('run',text=text,caller=caller),label='public '+text)
    return cases

def validate(request,output):
    entry=output['ops'][-1]
    if 'expectException' in request:assert request['expectException'] in entry['exception'],entry
    if not any(k in request for k in ['expectedIDs','expectedSet','expectedAges','stateShape']):return
    assert 'value' in entry,entry
    response=entry['value']['response']
    ids=[c['coinid'] for c in response]
    if 'expectedIDs' in request:assert ids==request['expectedIDs'],(ids,request['expectedIDs'])
    if 'expectedSet' in request:assert sorted(ids)==sorted(request['expectedSet']),(ids,request['expectedSet'])
    if 'expectedAges' in request:assert [c['age'] for c in response]==request['expectedAges'],response
    if 'stateShape' in request:assert all(isinstance(c['state'],dict if request['stateShape'] else list) for c in response),response
