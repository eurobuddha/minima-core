"""Actual wallet balance: source-generated coins, exact text and independent totals."""
import copy,itertools,json

def fixture_request():
    base=dict(scale='2',name='{"name":"Teaching token","ticker":"EDU"}',script='RETURN TRUE')
    groups=[base|dict(native=True),base]
    groups += [base|dict(name=name) for name in ['plain token','{broken','[]','null','true','12','"quoted"','{}','{"name":null}','{"name":42}','{"name":{"nested":true}}','{"name":"first","name":"last"}','  {"name":"π 😄"}  ','java.lang.NullPointerException: this is a token name','']]
    groups += [base|dict(script=script) for script in ['RETURN FALSE','return true','RETURN  TRUE','RETURN TRUE ','']]
    groups += [base|dict(scale=scale) for scale in ['-1','0','1','44','45','2147483648','-2147483648']]
    return dict(kind='balanceFixtures',groups=groups)

def corpus(fixtures):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    rows=[row|dict(address='0x0'+str(i)) for i in range(1,4)]
    cases=[];groups=fixtures['groups'];native=groups[0];token=groups[1]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01','0x02','0x03'],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='balance',params=params or {},replyText=True,**kw)
    def chain(coins=None,relevant=None,mega=False,megaCoins=None,empty=False,older=None):
        nodes=copy.deepcopy(fixtures['nodes'])
        for node in nodes:node['coins']=[];node['relevant']=[]
        if coins is None:coins=native['coins']
        nodes[-1]['coins']=coins;nodes[-1]['relevant']=coins if relevant is None else relevant
        if older is not None:nodes[-2]['coins']=older;nodes[-2]['relevant']=older
        return op('chain',history='8',nodes=[] if empty else nodes,mega=mega,megaCoins=megaCoins or [])
    def add(*ops,setup=None,load=True,walletRows=None,label='',**kw):
        cases.append(dict(label=label,ops=([op('load',rows=rows if walletRows is None else walletRows)] if load else [])+[setup or chain()]+list(ops),**kw))
    add(op('meta',name='balance'),label='metadata')
    add(cmd(),expected=[dict(tokenid='0x00',confirmed='6',unconfirmed='15',sendable='6',coins='6')])
    add(cmd(dict(simple='true')),expected=[dict(name='Minima',amount='6')])
    add(cmd(dict(coinlist='true',debug='true')),expected=[dict(confirmed='6',coins='6')],coinIDs=['0x80','0x81','0x82','0x83','0x84','0x85'])
    for confirmations,confirmed in [('-1',21),('0',17),('1',14),('2',8),('2.5',6),('3',6),('3.0001',5),('4',5),('11',5),('12',0),('2147483648',0)]:
        add(cmd(dict(confirmations=confirmations)),expected=[dict(confirmed=str(confirmed),unconfirmed=str(21-confirmed),sendable=str(confirmed))],label='confirmation threshold')
    for simple,coinlist,details in itertools.product(['false','true'],repeat=3):
        add(cmd(dict(simple=simple,coinlist=coinlist,tokendetails=details)),setup=chain(coins=native['coins']+token['coins']),label='two token views')
    for g,spec in zip(groups[1:],fixture_request()['groups'][1:]):
        for simple,details,debug in [('false','false','false'),('true','true','false'),('false','true','true')]:
            add(cmd(dict(simple=simple,tokendetails=details,coinlist='true',debug=debug)),setup=chain(coins=g['coins']),label='name/script/scale '+repr(spec))
    add(cmd(dict(tokendetails='true')),setup=chain(coins=token['coins']),expected=[dict(confirmed='0'),dict(tokenid=token['id'],confirmed='600',unconfirmed='1500',sendable='600',coins='6',total='10000',details=dict(decimals=42,script='RETURN TRUE',totalamount='100',scale='2',created='7'))])
    add(cmd(dict(simple='true')),setup=chain(coins=token['coins']),expected=[dict(name='Minima',amount='0'),dict(name='Teaching token',amount='600')])
    add(cmd(),setup=chain(coins=token['coins']),walletRows=[r|dict(simple=0) if r['address']=='0x02' else r for r in rows],expected=[dict(sendable='0'),dict(confirmed='600',sendable='100')])
    for load in [True,False]:
        add(cmd(dict(debug='true')),load=load,walletRows=[r|dict(simple=0,track=0) for r in rows],expected=[dict(confirmed='6',sendable='0')])
    for address in ['0x01','0x1','0x0001',fixtures['mx'],'0X01','01','0xFF','', '0xzz','MxINVALID',' mxINVALID']:
        add(cmd(dict(address=address,coinlist='true')),label='address '+address)
    for address in ['0x01',fixtures['mx']]:
        add(cmd(dict(address=address)),expected=[dict(confirmed='1',unconfirmed='4',sendable='1',coins='2')])
    for tid in ['', '0x00','0x0','0X00','0xFF',token['id'],token['id'].lower(),'bad']:
        add(cmd(dict(tokenid=tid,coinlist='true')),setup=chain(coins=native['coins']+token['coins']),label='raw token filter')
    add(cmd(dict(tokenid='0x0')),expected=[])
    add(cmd(dict(tokenid=token['id'])),setup=chain(coins=token['coins']),expected=[dict(tokenid=token['id'],confirmed='600')])
    for field in ['address','tokenid','confirmations','megammr','simple','coinlist','tokendetails','debug']:
        for value in [None,True,False,1,{},[],'','bad','TRUE']:
            add(cmd({field:value}),label='parameter type '+field)
    # First token metadata is required even when every coin is unconfirmed.
    for coins in [token['missing'],token['coins'][:1]+token['missing'][1:]]:
        for conf in ['3','100']:
            add(cmd(dict(confirmations=conf)),setup=chain(coins=coins),expectException='java.lang.NullPointerException')
        add(cmd(dict(tokenid='0x00')),setup=chain(coins=coins),expected=[dict(confirmed='0',coins='0')])
    # Source search deduplicates by coin ID and a newer spent record masks older data.
    add(cmd(dict(coinlist='true')),setup=chain(coins=native['coins']*2),expected=[dict(confirmed='6',unconfirmed='15',coins='6')])
    add(cmd(),setup=chain(coins=native['spent'][:1],older=native['coins']),expected=[dict(confirmed='5',unconfirmed='15',sendable='5',coins='5')])
    add(cmd(),setup=chain(coins=native['coins'],relevant=[]),expected=[dict(confirmed='0',coins='0')])
    add(cmd(dict(address='0x01')),setup=chain(coins=native['coins'],relevant=[]),expected=[dict(confirmed='1',coins='2')])
    for enabled,requested,address in itertools.product([False,True],['false','true'],['','0x01']):
        allowed=enabled and requested=='true' and address!=''
        add(cmd(dict(megammr=requested)|({'address':address} if address else {})),setup=chain(coins=[],mega=enabled,megaCoins=native['coins']),expected=[dict(confirmed='1' if allowed else '0',unconfirmed='4' if allowed else '0',coins='2' if allowed else '0')])
    add(cmd(dict(address='0x01',megammr='true')),setup=chain(coins=native['spent'][:1],mega=True,megaCoins=native['coins']),expected=[dict(confirmed='0',unconfirmed='4',coins='1')])
    add(cmd(),setup=chain(coins=token['coins']+groups[-1]['coins']+native['coins']),tokenOrder=['0x00',token['id'],groups[-1]['id']])
    for params in [{},{'megammr':True},{'confirmations':'bad'},{'address':'MxINVALID'}]:
        add(cmd(params),setup=chain(empty=True),label='no tip parse order')
    for text in ['balance','balance simple:true','balance debug:true','balance invalid:true','balance address:0x01;balance tokenid:0x00']:
        for caller in ['0x00','0x1234']:add(op('run',text=text,caller=caller,replyText=True),label='public '+text)
    # Cache setup failure does not change which relevant coins are searched.
    for fail,runtime in itertools.product([1,9,30,55,70], [False,True]):
        cases.append(dict(label='partial wallet load',ops=[op('load',rows=rows,fail=fail,runtimeFailure=runtime),chain(),cmd(dict(debug='true'))]))
    for coefficient,scale in [('6000000000000000000',-1),('6',-19),('-6',-19),('1',-20),('1',-63),('1',-64),('1',-80),('123456789012345678901234567890123456789012345678901234567890123456789012345',-1),('1844674407370955161500',2),('1844674407370955161600',2),('0',-100),('1',-18)]:
        for decimal in [False,True]:
            cases.append(dict(label='number constructor representation',ops=[op('numberRange',decimal=decimal,coefficient=coefficient,scale=scale,text=coefficient+'E'+str(-scale))]))
    return cases

def validate(request,output):
    entry=output['ops'][-1]
    if 'expectException' in request:assert request['expectException'] in entry['exception'],entry
    if not any(k in request for k in ['expected','coinIDs','tokenOrder']):return
    assert 'value' in entry,entry
    response=json.loads(entry['value'])['response']
    if 'expected' in request:
        expected=request['expected'];assert len(response)==len(expected),(response,expected)
        for actual,wanted in zip(response,expected):
            for key,value in wanted.items():assert actual[key]==value,(key,actual,wanted)
    if 'coinIDs' in request:assert [c['coinid'] for c in response[0]['coinlist']]==request['coinIDs'],response
    if 'tokenOrder' in request:assert [g['tokenid'] for g in response]==request['tokenOrder'],response

def preserve_text(raw,normalized,request):
    # Do not let the shared exception-prose regex consume a token name that
    # happens to contain an exception class. Replies and coin logs are data.
    for original,entry,operation in zip(raw['ops'],normalized['ops'],request['ops']):
        if operation.get('replyText') and 'value' in original:entry['value']=original['value']
        for i,line in enumerate(original['logs']):
            if line.startswith(('Coin : ','SENDABLE : ','NON-SENDABLE : ')):entry['logs'][i]=line
    return normalized
