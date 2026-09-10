"""Unsigned creation using actual chain/proofs, files and subsequent commands."""
import copy,itertools,re

def corpus(fixtures,key,tokens,root):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def command(params=None,**kw):return op(name='sendnosign',params=dict(address='0x02',amount='1',file='unsigned.txn')|(params or {}),**kw)
    def direct(name,file):return op(name=name,params={'file':file})
    def setup(nodes=None):return [op('load'),op('chain',history='8',nodes=fixtures['nodes'] if nodes is None else nodes),op('mining')]
    def add(*ops,**kw):cases.append(dict(offline=True,unsigned=True,ops=list(ops),**kw))
    add(op('meta',name='sendnosign'));add(op('run',text='sendnosign invalid:true'))
    add(*setup(),command(),expectUnsigned=True)
    add(*setup(),command(),direct('sendview','unsigned.txn'),direct('sendsign','unsigned.txn'),op('verifyFile'),direct('sendpost','$SIGNED'),expectFlow=True)
    for count in range(0,6):add(*setup(fixtures['nodes'][:count]),command(),expectShortChain=0<count<=3)
    for amount,split,burn in itertools.product(['0','1','4','8','9','-1'],['1','1.9','3','20','20.1','0'],['0','0.5']):add(*setup(),command(dict(amount=amount,split=split,burn=burn)))
    for field in ['multi','address','amount','tokenid','state','burn','split','debug','dryrun','file','coinage']:
        for value in [None,True,1,{},[],'','bad']:add(*setup(),command({field:value}),expectUnsigned=field=='dryrun')
    for multi in [[],['0x02:1'],[':0x02::1:ignored'],['0x02'],['::'],['0x02:1','0x03:2'],[None],[1],['Mx200104JMFSAS:1'],['MxINVALID:1']]:add(*setup(),command({'multi':multi}))
    for state in [{'0':'[hello]','1':'42'},{'0':123},{'0':None},{'256':None},{'256':123},{'bad':None},{'01':'first','1':'second'},{'١':'42'}]:add(*setup(),command({'state':state}))
    for caller in ['0x00','0x1234']:
        for extra in ['', ' coinage:0', ' dryrun:true']:
            add(*setup(),op('run',text='sendnosign address:0x02 amount:1 file:unsigned.txn'+extra,caller=caller))
    add(*setup(),command({'dryrun':'true'}),expectUnsigned=True)
    for coinage in ['-1','0','1','3','8','9','3.5']:add(*setup(),command({'coinage':coinage}))
    for coinset in [fixtures['inputcoins'][:1],fixtures['inputcoins'][2:],fixtures['inputcoins']]:
        add(*setup(),op('miner',coins=coinset),command())
    for proofs in [fixtures['proofs'][:1],fixtures['proofs'][2:],fixtures['proofs']]:add(*setup(),op('pool',proofs=proofs),command())
    for rows in [[],[row|{'script':None}],[row|{'publickey':None}],[row|{'address':None}],[row|{'privatekey':None}]]:add(*setup(),command(rows=rows))
    for fail,runtime in itertools.product(range(1,36),[False,True]):add(*setup(),command(fail=fail,runtimeFailure=runtime))
    for token in tokens:
        for amount,burn in itertools.product(['1','10','100','101','0.000000000000000000000000000000000000000000001'],['0','1']):
            add(*setup(token['nodes']),command(dict(tokenid=token['tokenid'],amount=amount,burn=burn)))
        if token['script']=='RETURN TRUE':
            add(*setup(token['nodes']),command(dict(tokenid=token['tokenid'],amount='10',burn='1')),expectUnsignedBurn=True)
            add(*setup(token['nodes']),command(dict(tokenid=token['tokenid'],amount='10',burn='1')),direct('sendsign','unsigned.txn'),op('verifyFile'),direct('sendpost','$SIGNED'),expectFlow=True,expectFlowBurn=True)
    # Missing filename uses the clock; dryrun remains an ignored parameter.
    add(*setup(),op(name='sendnosign',params=dict(address='0x02',amount='1')),expectDefaultFile=True)
    for file in [str(root)+'/offline/nested/output.txn',str(root)+'/offline//output.txn/',str(root)+'/offline/missing/../output.txn','unsigned.txn\0ignored']:
        add(*setup(),command({'file':file}))
    add(*setup(),op('fileDirectory',file='unsigned.txn'),command())
    add(*setup(),op('filePut',file='unsigned.txn/child',data='0xAA'),command())
    add(*setup(),op('filePut',file='parent',data='0xAA'),command({'file':str(root)+'/offline/parent/child.txn'}))
    add(*setup(),op('vaultSeed',phrase='',seed='0x00'),command(),expectUnsigned=True)
    add(*setup(),op('mining',nullMiner=True),command())
    add(*setup(),command({'amount':'3','split':'1.5'}),expectFraction=True)
    add(*setup(),command({'amount':'0'}),expectZeroTotal=True)
    add(*setup(),command({'multi':['0x02:0','0x03:1']}),expectZeroOutput=True)
    add(*setup(),command({'multi':['0x02:-1','0x03:2']}))
    add(*setup(),command({'state':{'0':123}}),expectStateCast=True)
    return cases

def normalize(output):
    output=copy.deepcopy(output)
    for entry in output.get('ops',[]):
        names=[name for name in entry.get('files',{}) if re.match(r'(un)?signedtransaction-',name)]
        for name in names:assert re.fullmatch(r'(un)?signedtransaction-[0-9]+\.txn',name),name
        for name,observation in entry.get('fileTxPoW',{}).items():
            if 'json' not in observation or not observation['json']['hasbody']:continue
            prng=observation['json']['body']['prng'];position=observation['prngOffset'];wire=entry['files'][name];data=bytes.fromhex(wire[2:])
            assert observation['prngLength']==32 and len(prng)==66,observation
            assert data[position:position+32].hex().upper()==prng[2:],observation
            assert observation['consumed']==len(data),observation
            # Preserve every byte outside the measured random-byte slice.
            entry['files'][name]='0x'+(data[:position]+bytes([0x12])*32+data[position+32:]).hex().upper()
        def normalize_item(value):
            if isinstance(value,str):
                for name in names:value=value.replace(name,re.sub(r'-[0-9]+\.txn$','-<TIME>.txn',name))
                return value
            if isinstance(value,list):return [normalize_item(x) for x in value]
            if isinstance(value,dict):
                result={normalize_item(k):normalize_item(v) for k,v in value.items()}
                if 'prng' in result and 'witness' in result and 'txn' in result:
                    assert len(result['prng'])==66 and result['prng'].startswith('0x'),result
                    result['prng']='<32 random bytes>'
                return result
            return value
        entry.update(normalize_item(entry))
    return output

def validate(request,output):
    final=output['ops'][-1]
    if request.get('expectShortChain'):
        assert final['value']['status'] is False and final['value']['message']=='Insufficient blocks..' and final['files']=={},final
    if request.get('expectStateCast'):assert final['exception'].startswith('java.lang.ClassCastException'),final
    if request.get('expectFraction'):
        outputs=final['fileTxPoW']['unsigned.txn']['json']['body']['txn']['outputs']
        assert len(outputs)==1 and outputs[0]['amount']=='2' and outputs[0]['storestate'] is True,outputs
    if request.get('expectZeroTotal'):
        assert final['exception']=='org.minima.system.commands.CommandException: Invalid Minima amount to send.. 0' and final['files']=={},final
    if request.get('expectZeroOutput'):
        outputs=final['fileTxPoW']['unsigned.txn']['json']['body']['txn']['outputs']
        assert outputs[0]['amount']=='0',outputs
    if request.get('expectUnsigned') or request.get('expectUnsignedBurn') or request.get('expectDefaultFile'):
        assert final['value']['status'] is True and final['mining']['calls']==[],final
        filename=next(iter(final['files']));body=final['fileTxPoW'][filename]['json']['body']
        assert not body['witness']['signatures'] and not body['burnwitness']['signatures'],body
        assert not [e for e in final['events'] if e[:3]==['prepared.setInt',4,1]],final
        if request.get('expectUnsignedBurn'):
            assert body['burntxn']['inputs'] and body['burntxn']['linkhash']==body['txn']['transactionid'],body
        if request.get('expectDefaultFile'):assert filename=='unsignedtransaction-<TIME>.txn',final
    if request.get('expectFlow'):
        verified=output['ops'][-2]['value'];assert verified['main']==[True] and all(verified['burn']),verified
        assert final['value']['status'] is True and len(final['mining']['pending'])==1,final
        assert len(final['mining']['calls'])==1,final
        if request.get('expectFlowBurn'):assert verified['burn']==[True],verified
