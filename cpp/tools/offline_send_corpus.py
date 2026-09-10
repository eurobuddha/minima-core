"""Actual complete-TxPoW file commands; preserve bytes, signing effects and errors."""
import copy,itertools,re

def corpus(fixtures,key,root):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=[],caller='0x00')|kw
    def command(name,params=None,**kw):return op(name=name,params={'file':'input.txn'} if params is None else params,**kw)
    def put(data,file='input.txn'):return op('filePut',file=file,data=data)
    def add(*ops,**kw):cases.append(dict(offline=True,ops=list(ops),**kw))
    for null in [False,True]:add(op('signDemo',nullArgs=null),expectDemo=True)
    for name in ['sendview','sendpost','sendsign']:
        add(op('meta',name=name));add(op('run',text=name+' invalid:true'))
        for params in [{},{'file':None},{'file':True},{'file':1},{'file':{}},{'file':[]},{'file':''},{'file':' '},{'file':'missing.txn'},{'file':'input.txn\0ignored'},{'file':str(root)+'/offline/nested/missing.txn'}]:add(command(name,params))
        add(op('fileDirectory',file='directory'),command(name,{'file':'directory'}))
    for fixture in fixtures:
        wire=fixture['wire']
        add(put(wire),command('sendview'),expectView=fixture['json'])
        add(put(wire),op('mining'),command('sendpost'),expectOfflinePosted=True,expectPostSnapshot=fixture['json'])
        add(op('load'),put(wire),command('sendsign'),op('verifyFile'),command('sendview',{'file':'$SIGNED'}),expectSigned=[fixture['main'],fixture['burn']])
        add(op('load'),put(wire),op('mining'),command('sendsign'),command('sendpost',{'file':'$SIGNED'}),expectOfflinePosted=True)
    valid=fixtures[3]['wire']
    for name in ['sendview','sendpost','sendsign']:
        for data in ['0x','0x00',valid[:10],valid[:100],valid[:-2],valid+'FF']:
            add(op('load'),put(data),op('mining'),command(name))
        for caller in ['0x00','0x1234']:
            add(op('load'),put(valid),op('mining'),op('run',text=name+' file:input.txn',caller=caller))
    for rows in [[],[row|{'script':None}],[row|{'publickey':None}],[row|{'privatekey':None}],[row|{'script':'RETURN FALSE'}],[row|{'uses':4}]]:
        add(op('load'),put(valid),command('sendsign',rows=rows))
    for fail,runtime in itertools.product(range(1,31),[False,True]):add(op('load'),put(valid),command('sendsign',fail=fail,runtimeFailure=runtime))
    for value in [None,True,1,{},[],'','wrong']:
        add(op('load'),put(valid),command('sendsign',{'file':'input.txn','password':value}))
    for data,password in itertools.product([valid,'0x'],['fixture pass','wrong','']):
        locked=row|{'phrase':'','seed':'0x00'}
        add(op('load'),put(data),op('vaultLock',password='fixture pass',rows=[locked]),command('sendsign',{'file':'input.txn','password':password}),vault=True)
    add(put(valid),command('sendpost'))
    add(put(valid),op('mining',nullMiner=True),command('sendpost'))
    add(op('load'),put(valid),op('fileBase',folder=str(root)+'/offline/input.txn'),command('sendsign',{'file':str(root)+'/offline/input.txn'}),expectWriteAfterSigning=True)
    for path in [str(root)+'/offline//input.txn/',str(root)+'/offline/missing/../input.txn',str(root)+'/offline/missing/../absent.txn']:
        add(put(valid),command('sendview',{'file':path}))
    folder=str(root)+'/offline/'
    for append in [False,True]:
        def write(name):return op('fileWrite',file=folder+name,data='0x010203',append=append)
        add(write('nested/new.bin'),expectFiles={'nested/new.bin':'0x010203'})
        add(put('0xAABB'),write('input.txn'),expectFiles={'input.txn':'0xAABB010203' if append else '0x010203'})
        add(op('fileDirectory',file='directory'),write('directory'))
        add(put('0xAA','directory/child'),write('directory'))
        add(put('0xAA'),write('input.txn/child'))
        add(put('0xAA'),write('input.txn\0ignored'))
    # Snapshot is constructed before the miner overwrites the body hash.
    return cases

def normalize(value):
    value=copy.deepcopy(value)
    for entry in value.get('ops',[]):
        names=[name for name in entry.get('files',{}) if name.startswith('signedtransaction-')]
        for name in names:
            assert re.fullmatch(r'signedtransaction-[0-9]+\.txn',name),name
        def replace(item):
            if isinstance(item,str):
                for name in names:item=item.replace(name,'signedtransaction-<TIME>.txn')
                return item
            if isinstance(item,list):return [replace(i) for i in item]
            if isinstance(item,dict):return {replace(k):replace(v) for k,v in item.items()}
            return item
        entry.update(replace(entry))
        # Replacing an entry by update leaves its old key dictionary only inside
        # the outer container, whose keys are stable observation field names.
    return value

def validate(request,output):
    final=output['ops'][-1]
    if request.get('expectDemo'):
        expected=''.join('0x'+value.to_bytes(max(1,(value.bit_length()+7)//8),'big').hex().upper()+'\n' for value in range(512))
        assert final['value']==expected,final
    if 'expectFiles' in request:assert final['files']==request['expectFiles'],final
    if 'expectPostSnapshot' in request:assert final['value']['response']['txpow']==request['expectPostSnapshot'],final
    if request.get('expectWriteAfterSigning'):
        assert final['exception']=='java.io.IOException: Not a directory',final
        assert len([event for event in final['events'] if event[:3]==['prepared.setInt',4,1]])==1,final
        assert list(final['files'])==['input.txn'],final
    if 'expectView' in request:assert final['value']['response']['txpow']==request['expectView'],final
    if request.get('expectOfflinePosted'):
        assert len(final['mining']['calls'])==1 and len(final['mining']['pending'])==1,final
        assert final['value']['status'] is True,final
        mining=final['mining'];assert mining['held']==[mining['pending'][0]['wire']],mining
        assert mining['held'][0]!=mining['calls'][0]['wire'],mining
    if 'expectSigned' in request:
        main,burn=request['expectSigned'];verified=output['ops'][-2]['value'];assert verified=={'main':[True]*int(main>0),'burn':[True]*int(burn>0)},output
        body=final['value']['response']['txpow']['body'];assert len(body['witness']['signatures'])==int(main>0) and len(body['burnwitness']['signatures'])==int(burn>0),body
        updates=[event for event in output['ops'][2]['events'] if event[:3]==['prepared.setInt',4,1]]
        assert len(updates)==main+burn,updates  # Signing still runs once per input.
