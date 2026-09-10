"""Source txpow search, native stores and source-generated TxPoW/TxBlock wire."""
import copy,itertools,json
from wallet_balance_corpus import preserve_text

SQL='SELECT txpowdata FROM txpow WHERE txpowid=?'
ARCHIVE='SELECT syncdata FROM syncblock WHERE txpowid=?'
HEIGHT='SELECT syncdata FROM syncblock WHERE block=?'
RELEVANT='SELECT * FROM txpow WHERE isrelevant=1 ORDER BY timemilli DESC LIMIT ? OFFSET ?'
ONCHAIN='SELECT * FROM onchaintxpow WHERE txpowid=?'

def fixtures(runner):
    from wallet_balance_corpus import fixture_request
    request=fixture_request();request['groups']=request['groups'][:2];coins=runner.ask(request)
    native=coins['groups'][0]['coins']
    wires=runner.ask(dict(kind='historyFixtures',cases=[dict(block='10'),dict(block='11',inputs=native[:2],outputs=native[3:4]),dict(block='12',burnInputs=native[2:4])]))
    leaves=runner.ask(dict(kind='txpowSearchFixtures',wires=wires));ids=[f['id'] for f in leaves]
    wires=runner.ask(dict(kind='historyFixtures',cases=[dict(block='10',inputs=native[:2],transactions=[ids[1],ids[1],'0xFF']),dict(block='11',burnInputs=native[1:2],transactions=[ids[0],ids[2]]),dict(block='12',inputs=native[2:3],transactions=[ids[1],ids[1],ids[2]])]))
    blocks=runner.ask(dict(kind='txpowSearchFixtures',wires=wires))
    return coins,leaves,blocks

def corpus(data):
    coins,leaves,blocks=data;cases=[];native=coins['groups'][0]['coins'];ids=[x['id'] for x in leaves];blockids=[x['id'] for x in blocks]
    def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='txpow',params=params or {},replyText=True,**kw)
    nodes=copy.deepcopy(coins['nodes'][:3]);assert len(nodes)==3
    for node,block in zip(nodes,blocks):node['pow']=block['wire'];node['coins']=native;node['relevant']=[]
    def chain(empty=False):return op('chain',history='8',nodes=[] if empty else nodes)
    def add(*ops,stores=True,tree=False,ram=False,label='',**expected):
        cases.append(dict(txpowSearch=True,label=label,ops=([op('txpowStores')] if stores else [])+([chain()] if tree else [])+([op('txpowRAM',data=x['wire']) for x in leaves] if ram else [])+list(ops),**expected))
    def txrows(*indexes):return [dict(txpowdata=leaves[i]['wire']) for i in indexes]
    baseRow=dict(txpowid='0x99',blockid='0x98',block=9,timemilli=0,tot=4)
    add(op('meta',name='txpow'))
    add(cmd(),expectException='Must Specify search params')
    add(cmd(dict(txpowid=ids[1])),ram=True,expectedResponse=json.loads(leaves[1]['json']),noSQL=True,label='RAM before SQL')
    add(cmd(dict(txpowid=ids[1]),rows=txrows(1)),expectedResponse=json.loads(leaves[1]['json']),queries=[SQL])
    add(cmd(dict(txpowid=ids[1]),rowsBySQL={SQL:[],ARCHIVE:[dict(syncdata=leaves[1]['block'])]}),expectedResponse=json.loads(leaves[1]['json']),queries=[SQL,ARCHIVE],label='ordinary miss then archive hit')
    for block in blocks:
        add(cmd(dict(block=json.loads(block['json'])['header']['block'])),tree=True,expectedResponse=json.loads(block['json']),noSQL=True)
    add(cmd(dict(block='500'),rows=[dict(syncdata=leaves[1]['block'])]),expectedResponse=json.loads(leaves[1]['json']),queries=[HEIGHT])
    for i in range(3):
        add(cmd(dict(onchain=blockids[i])),tree=True,expectedResponse=dict(found=True,block=str(10+i),blockid=blockids[i],tip='12',confirmations=str(2-i)),noSQL=True)
        containing=1 if i==0 else 2
        add(cmd(dict(onchain=ids[i])),tree=True,expectedResponse=dict(found=True,block=str(10+containing),blockid=blockids[containing],tip='12',confirmations=str(2-containing)),noSQL=True)
    for height in [-1,0,9,12,13,9223372036854775807,-9223372036854775808]:
        add(cmd(dict(onchain='0x77'),rows=[baseRow|dict(block=height)]),tree=True,expectedResponse=dict(found=True,block=str(height),blockid='0x98',tip='12',confirmations=str(12-height)),queries=[ONCHAIN])
    add(cmd(dict(onchain='0x77')),expectedResponse=dict(found=False),queries=[ONCHAIN])
    add(cmd(dict(onchain='0x77'),rows=[baseRow]),expectException='java.lang.NullPointerException')
    for address,wanted in [('0x01',[ids[1],ids[2],blockids[0]]),('0x02',[ids[1],blockids[1],blockids[0]]),('0x03',[blockids[2],ids[2]]),('0xFF',[])]:
        add(cmd(dict(address=address)),tree=True,ram=True,expectedIDs=wanted,label='address deduplication')
    add(cmd(dict(address='0x01')),expectedIDs=[])
    for value in [None,True,False,0,1,{},[],'','true','false','bad']:
        add(cmd(dict(relevant=value),rows=txrows(1,1,2,1,0,2)),expectedIDs=[ids[1],ids[2],ids[0]],label='presence and stable deduplication')
    for maximum in ['-1','0','1.9','2147483648','4294967296']:
        add(cmd(dict(relevant='false',max=maximum),rows=txrows(1)),expectedIDs=[ids[1]])
        add(op('historyLimit',limit=maximum),cmd(dict(relevant='true')),expectedBound=int(float(maximum))%(1<<32))
    for value in ['0x01','0x1','0X01','01','-1','1.9','2147483648','9223372036854775808','18446744073709551615','18446744073709551616',coins['mx']]:
        add(cmd(dict(inblock=value),rows=[baseRow,baseRow|dict(block=10,txpowid=None)]))
    for days in [-9223372036854775808,-1,0,3,9223372036854775807]:
        route={'SELECT COUNT(*) AS tot FROM txpow':[dict(tot=7)],'SELECT COUNT(*) AS tot FROM onchaintxpow':[dict(tot=13)],'SELECT * FROM onchaintxpow ORDER BY timemilli ASC LIMIT 1':[baseRow],'SELECT * FROM onchaintxpow ORDER BY timemilli DESC LIMIT 1':[baseRow|dict(block=99,timemilli=1700000000123)]}
        add(op('txpowPrefs',days=days),cmd(dict(action='info'),rowsBySQL=route),expectedInfo=[days,7,13,9,99])
    fields=['action','txpowid','relevant','inblock','onchain','block','address']
    choices=dict(action='info',txpowid=ids[1],relevant='true',inblock='0x01',onchain=blockids[2],block='12',address='0x01')
    for i,field in enumerate(fields):
        for lower in fields[i+1:]:
            add(cmd({field:choices[field],lower:True},rows=txrows(1)),tree=True,ram=True,label=field+' before '+lower)
    for field in fields+['max']:
        for value in [None,True,False,1,{},[],'','bad','0x00','MxINVALID','-1.9','18446744073709551616']:
            params={field:value}
            if field=='max':params['relevant']='true'
            add(cmd(params),tree=True,label='typed '+field)
    for value in ['0x01','0X01','0x1',coins['mx'],'0xzz',' 0x01','π😄']:
        for field in ['txpowid','address','onchain']:add(cmd({field:value}),tree=True)
    # Observe every relevant query boundary and partial result construction.
    selections=[dict(action='info'),dict(txpowid='0x77'),dict(relevant='true'),dict(inblock='0x77'),dict(inblock='9'),dict(onchain='0x77'),dict(block='500'),dict(address='0x01')]
    default=baseRow|dict(txpowdata=leaves[1]['wire'],syncdata=leaves[2]['block'])
    for params in selections:
        add(cmd(params),stores=False)
        for rows in [[],[{}],[default],[default,default]]:add(cmd(params,rows=rows),tree=True)
        for fail,runtime in itertools.product(range(1,23),[False,True]):
            add(cmd(params,rows=[default,default],fail=fail,runtimeFailure=runtime),tree=True)
        for flag in ['nullPrepared','nullRows','nullStatement']:
            add(op('txpowStores',**{flag:True}),chain(),cmd(params,rows=[default],**{flag:True}),stores=False)
    for fail in [1,3,8,12,18,24,32,40,50,60,65,70]:
        add(op('txpowStores',fail=fail),*(cmd(params) for params in selections[:7]),stores=False,label='partial store initialization')
    for malformed in [None,'0x','0x0000',leaves[1]['wire'][:50],leaves[1]['wire'][:-2]]:
        add(cmd(dict(relevant='true'),rows=txrows(1)+[dict(txpowdata=malformed)]+txrows(2)))
        add(cmd(dict(txpowid='0x77'),rowsBySQL={SQL:[dict(txpowdata=malformed)],ARCHIVE:[dict(syncdata=leaves[2]['block'])]}))
        add(cmd(dict(block='500'),rows=[dict(syncdata=malformed)]))
    # The real runner is preserved; only this missing native downstream body is recorded.
    for reply in [{},dict(status=True),dict(status=False,error='fixture failure'),dict(status=False,response=dict(found=True,txpow='returned despite nested status')),dict(status=True,response=None),dict(status=True,response=[]),dict(status=True,response=dict(found=None)),dict(status=True,response=dict(found='true')),dict(status=True,response=dict(found=1)),dict(status=True,response=dict(found=False))]+[dict(status=True,response=dict(found=True,txpow=value)) for value in [None,{},[],True,17,'java.lang.NullPointerException: ordinary text']]:
        expected={'expectedResponse':reply['response']['txpow']} if isinstance(reply.get('response'),dict) and reply['response'].get('found') is True else {}
        add(op('txpowPrefs',login=True),cmd(dict(txpowid='0x77'),recordMysql=True,mysqlResult=reply),expectedForward='mysql action:findtxpow txpowid:0x77',**expected)
    add(op('txpowPrefs',login=True),cmd(dict(txpowid='0x77'),recordMysql=True,mysqlThrow=True,mysqlResult={}),expectException='java.lang.NullPointerException')
    for login in [False,True]:
        add(op('txpowPrefs',login=login),cmd(dict(txpowid=ids[1]),recordMysql=True,mysqlResult={},rowsBySQL={SQL:[],ARCHIVE:[dict(syncdata=leaves[1]['block'])]}),expectedResponse=json.loads(leaves[1]['json']),noForward=True)
    for params,expected in [(dict(block='500'),dict(expectException='TxPoW not found @ height 500')),(dict(onchain='0x77'),dict(expectedResponse=dict(found=False)))]:
        add(op('txpowPrefs',login=True),cmd(params,recordMysql=True,mysqlResult=dict(status=True,response=dict(found=True,txpow={}))),noForward=True,**expected)
    for text in ['txpow relevant:false','txpow action:info','txpow onchain:0x77','txpow address:0x01','txpow invalid:true','txpow']:
        for caller in ['0x00','0x1234']:add(op('run',text=text,caller=caller,replyText=True))
    return cases

def project(raw,normalized,request):return preserve_text(raw,normalized,request)

def validate(request,output):
    entry=output['ops'][-1]
    if 'expectException' in request:assert request['expectException'] in entry['exception'],(request,entry)
    if 'expectedResponse' in request:assert json.loads(entry['value'])['response']==request['expectedResponse'],(request,entry)
    if 'expectedIDs' in request:
        actual=[v['txpowid'] for v in json.loads(entry['value'])['response']];assert actual==request['expectedIDs'],(request['ops'][-1].get('params'),actual,request['expectedIDs'])
    if request.get('noSQL'):assert not entry['events'],entry['events']
    if request.get('noForward'):assert not entry['forwarded'],entry['forwarded']
    if 'expectedForward' in request:assert [v['text'] for v in entry['forwarded']]==[request['expectedForward']],entry['forwarded']
    if 'expectedBound' in request:
        values=[e[3] for e in entry['events'] if e[0]=='prepared.setInt' and e[2]==1];assert len(values)==1 and values[0]%(1<<32)==request['expectedBound'],values
    if 'expectedInfo' in request:
        r=json.loads(entry['value'])['response'];assert [r['storedays'],r['txpowdb']['size'],r['onchaindb']['size'],r['onchaindb']['first']['block'],r['onchaindb']['last']['block']]==request['expectedInfo'],r
    if 'queries' in request:
        prepared={};current=0
        for result in output['ops']:
            for event in result['events']:
                if event[0]=='connection.prepareStatement':current+=1;prepared[current]=event[2]
        actual=[prepared[e[1]] for e in entry['events'] if e[0]=='prepared.executeQuery'];assert actual==request['queries'],(actual,request['queries'])
