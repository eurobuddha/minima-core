"""Actual mempool, cointrack and magic bodies, including retained state on failure."""
import copy,itertools,json,re
from wallet_balance_corpus import fixture_request,preserve_text

def fixtures(runner):
    request=fixture_request();request['groups']=request['groups'][:1];plain=runner.ask(request);request['entries']=True;unique=runner.ask(request)
    return unique,plain

def corpus(data):
    fixture,plain=data;coins=fixture['groups'][0]['coins'];spent=fixture['groups'][0]['spent'];cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00')|kw
    def cmd(name,params=None,**kw):return op(name=name,params=params or {},replyText=True,classOnly=name=='magic',**kw)
    def chain(values=None,entries=None,empty=False,older=None):
        nodes=copy.deepcopy(fixture['nodes']);
        for n in nodes:n['coins']=[];n['relevant']=[]
        nodes[-1]['coins']=coins if values is None else values
        if older is not None:nodes[-2]['coins']=older
        return op('chain',history='8',nodes=[] if empty else nodes)
    def add(*ops,store=True,setup=None,label='',**expected):
        cases.append(dict(nodeState=True,label=label,ops=([op('coinStore')] if store else [])+([setup] if setup else [])+list(ops),**expected))
    def entries(values,recalculate=True,**kw):return op('relevantEntries',entries=[str(v) for v in values],recalculate=recalculate,**kw)
    def row(i=0,**kw):return dict(key='key'+str(i),id=f'0x{i:02X}',block=False,transaction=False,cascade=False,onchain=False)|kw
    def pool(values,**kw):return op('poolEntries',entries=values,clear=True,**kw)
    def track(enable='true',coin='0x80',**kw):return cmd('cointrack',dict(coinid=coin,enable=enable),**kw)
    # Before the first CoinDB creation, its singleton is null in both processes.
    add(cmd('mempool'),store=False,expectException='java.lang.NullPointerException')
    for name in ['mempool','cointrack','magic']:add(op('meta',name=name,classOnly=name=='magic'))
    add(cmd('mempool',rows=[dict(tot=7)]),expectedResponse=dict(txpow=[],total=0,onchain=0,cascade=0,transactions=0,blocks=0,sqlcoindb=7))
    for block,transaction,cascade,onchain in itertools.product([False,True],repeat=4):
        add(pool([row(block=block,transaction=transaction,cascade=cascade,onchain=onchain)]),cmd('mempool',rows=[dict(tot=9)]),poolCounts=[1,int(not cascade),int(cascade),int(transaction and not cascade),int(block and not cascade)],label='pool flags')
    for cascade in [False,True]:
        add(pool([row(nullTxPoW=True,cascade=cascade)]),cmd('mempool'),**({'poolCounts':[1,0,1,0,0]} if cascade else {'expectException':'java.lang.NullPointerException'}))
    keys=[''.join('Aa' if (i>>bit)&1 else 'BB' for bit in range(7)) for i in range(128)]
    for count in [1,7,8,9,16,32,64,128]:
        values=[row(i,key=keys[i],block=i%2==0,transaction=i%3==0,cascade=i%5==0,onchain=i%7==0) for i in range(count)]
        add(pool(values),cmd('mempool'),poolCounts=[count,sum(not v['cascade'] for v in values),sum(v['cascade'] for v in values),sum(v['transaction'] and not v['cascade'] for v in values),sum(v['block'] and not v['cascade'] for v in values)],label='collision traversal')
        add(pool(values),op('poolEntries',entries=[row(250,key=keys[0],block=True,transaction=True)],remove=keys[1:3]),cmd('mempool'),label='replace and remove')
    add(pool([row(1),row(2,key='key1',transaction=True)]),cmd('mempool'),poolCounts=[1,1,0,1,0])
    add(pool([row(1),row(1,key='other')]),cmd('mempool'),poolCounts=[2,2,0,0,0],label='same ID under two distinct map keys')
    for rows in [[],[{}],[dict(tot=None)],[dict(tot=-1)],[dict(tot=2147483648)]]:add(cmd('mempool',rows=rows))
    for fail,runtime in itertools.product(range(1,10),[False,True]):add(pool([row(1,transaction=True)]),cmd('mempool',rows=[dict(tot=4)],fail=fail,runtimeFailure=runtime))
    for fail in range(1,14):add(op('coinStore',fail=fail),cmd('mempool',rows=[dict(tot=3)]))
    for flag in ['nullPrepared','nullRows','nullStatement']:add(op('coinStore',**{flag:True}),cmd('mempool',**{flag:True}))
    defaults=dict(kissvm=1024,txpowsize=65536,txnsperblock=256)
    add(cmd('magic'),setup=chain(),desired=defaults)
    add(cmd('magic',dict(kissvm='1234',txpowsize='70000',txnsperblock='300')),setup=chain(),desired=dict(kissvm=1234,txpowsize=70000,txnsperblock=300),stored=dict(magic_kissvm='1234',magic_txpowsize='70000',magic_txns='300'))
    for field,key in [('kissvm','magic_kissvm'),('txpowsize','magic_txpowsize'),('txnsperblock','magic_txns')]:
        for value in ['0','-1','1.25','1E3','1e-44','1e40','18446744073709551615','18446744073709551616',None,True,False,7,{},[],'','bad']:
            add(cmd('magic',{field:value}),setup=chain(),label='magic typed parameter '+field)
        for value in [None,True,False,7,{},[],'','bad','-1.25']:
            add(op('preferencePatch',values={key:value}),cmd('magic'),setup=chain(),label='stored number type '+key)
    for params,wanted in [(dict(kissvm='2000',txpowsize='bad',txnsperblock='300'),dict(magic_kissvm='2000')),(dict(kissvm='2000',txpowsize='70000',txnsperblock='bad'),dict(magic_kissvm='2000',magic_txpowsize='70000'))]:
        add(cmd('magic',params),setup=chain(),stored=wanted,label='writes survive later parse error')
    add(cmd('magic',dict(kissvm='2000',txpowsize='70000',txnsperblock='300')),stored=dict(magic_kissvm='2000',magic_txpowsize='70000',magic_txns='300'),expectException='java.lang.NullPointerException',label='no tip after writes')
    for value in ['0','-1','1.5','2000','70000','1e-44']:
        add(cmd('magic',dict(kissvm=value,txpowsize=value,txnsperblock=value)),op('generatedMagic'),setup=chain(),generated=value)
    add(cmd('magic',dict(kissvm='3000')),op('preferenceSave'),cmd('magic',dict(kissvm='4000')),op('preferenceLoad'),cmd('magic'),setup=chain(),desired=defaults|dict(kissvm=3000),stored=dict(magic_kissvm='3000'))
    add(entries([]),track(),setup=chain(),expectedResponse='Coin added to track list',entryState=['0'],relevantCoins=coins[:1])
    add(entries([]),track(),track(),setup=chain(),expectedResponse='Coin already tracked',entryState=['0'],relevantCoins=coins[:1])
    add(entries([0,1,0,9]),track('false'),setup=chain(),expectedResponse='Coin removed from track list',entryState=['1','9'],relevantCoins=coins[1:2])
    add(entries([0],recalculate=False),track(),setup=chain(),expectedResponse='Coin already tracked',entryState=['0'],relevantCoins=[])
    add(entries([1],recalculate=False),track('false'),setup=chain(),expectedResponse='Coin removed from track list',entryState=['1'],relevantCoins=coins[1:2])
    add(entries([]),track('false'),setup=chain(),expectedResponse='Coin removed from track list',entryState=[],relevantCoins=[])
    add(entries([]),track(),setup=chain(values=plain['groups'][0]['coins']),entryState=['0'],relevantCoins=plain['groups'][0]['coins'],label='equal MMR entries track multiple coins')
    for enable in ['true','false','TRUE','bad']:
        add(track(enable),setup=chain(values=spent[:1]+coins[1:],older=coins[:1]),expectException='java.lang.NullPointerException',label='spent newer record')
        add(track(enable),setup=chain(values=coins[1:],older=coins[:1]),label='older node')
    for field in ['coinid','enable']:
        for value in [None,True,False,7,{},[],'','bad','0xFF','0X80','80','0x080',fixture['mx']]:add(cmd('cointrack',dict(coinid='0x80',enable='true')|{field:value}),setup=chain(),label='cointrack typed '+field)
    for params in [{},dict(coinid='0xzz',enable=True),dict(coinid='0x80'),dict(coinid='0xFF',enable='true')]:add(cmd('cointrack',params),setup=chain())
    add(track(),setup=chain(empty=True),expectException='Coin not found coinid : 0x80')
    rows=[dict(coindata=wire) for wire in coins]
    for enable in ['true','false']:
        add(entries([1]),op('coinSQL',enabled=True),track(enable,rows=rows),setup=chain())
        for fail,runtime in itertools.product(range(1,32),[False,True]):
            add(entries([1]),op('coinSQL',enabled=True),track(enable,rows=rows,fail=fail,runtimeFailure=runtime),setup=chain(),label='SQL tracked list failure')
        for table in [[],[{}],[dict(coindata='0x')],rows[:1]+[dict(coindata='0x')],rows[:1]+[{}],rows+rows,rows[::-1]]:
            add(entries([0,0,3]),op('coinSQL',enabled=True),track(enable,rows=table),setup=chain(),label='nullable SQL coins')
    add(op('coinSQL',enabled=True),entries([],rows=[dict(coindata='0x')]),setup=chain(),entryState=[],relevantCoins=[],label='empty inner relevance loop')
    for name,text in [('mempool','mempool'),('magic','magic kissvm:2048'),('cointrack','cointrack enable:true coinid:0x80')]:
        for caller in ['0x00','0x1234']:add(op('run',text=text,caller=caller,replyText=True),setup=chain())
        add(op('run',text=name+' invalid:true',replyText=True),setup=chain())
    return cases

def project(raw,normalized,request,tree_sql=('SELECT * FROM coins WHERE txpowtreeid=?',)):
    normalized=preserve_text(raw,normalized,request);prepared={};count=0;treeIDs={}
    for entry,op in zip(normalized['ops'],request['ops']):
        for index,event in enumerate(entry['events'],1):
            if event[0]=='connection.prepareStatement' and index!=op.get('fail') and not op.get('nullPrepared'):
                count+=1;prepared[count]=event[2]
            if event[0]=='prepared.setString' and prepared.get(event[1]) in tree_sql:
                assert event[2]==1 and re.fullmatch(r'0x[0-9A-F]{64}',event[3]),event
                treeIDs.setdefault(event[3],'<tree-id-'+str(len(treeIDs))+'>');event[3]=treeIDs[event[3]]
    return normalized

def preference_json(wire):
    raw=bytes.fromhex(wire[2:]);length=int.from_bytes(raw[:4],'big');assert len(raw)==length+4
    return json.loads(raw[4:].decode())

def validate(request,output):
    entry=output['ops'][-1]
    if 'expectException' in request:assert request['expectException'] in entry['exception'],(request['label'],entry.get('exception'))
    if 'expectedResponse' in request:assert json.loads(entry['value'])['response']==request['expectedResponse'],entry.get('value')
    if 'poolCounts' in request:
        response=json.loads(entry['value'])['response'];actual=[response[k] for k in ['total','onchain','cascade','transactions','blocks']];assert actual==request['poolCounts'],actual
    if 'desired' in request:assert json.loads(entry['value'])['response']['desired']==request['desired'],entry['value']
    if 'stored' in request:
        prefs=preference_json(entry['preferenceWire'])
        for k,v in request['stored'].items():assert prefs.get(k)==v,(prefs,request['stored'])
    if 'generated' in request:
        r=entry['value']
        # Compare numerical strings after source decimal normalization.
        from decimal import Decimal
        for key in ['desiredmaxkissvmops','desiredmaxtxpowsize','desiredmaxtxn']:assert Decimal(r[key])==Decimal(request['generated']),r
    if 'entryState' in request:assert entry['nodeState'][0]['entries']==request['entryState'],entry['nodeState'][0]
    if 'relevantCoins' in request:assert entry['nodeState'][0]['coins']==request['relevantCoins'],entry['nodeState'][0]
