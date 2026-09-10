"""Wallet consolidation through actual search, wallet and command runner."""
import copy,itertools
def corpus(fixtures):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    rows=[row|dict(address='0x0'+str(i)) for i in range(1,4)]
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01','0x02','0x03'],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='consolidate',params=dict(tokenid='0x00')|(params or {}),recordSend=True,**kw)
    def chain(mode=None):
        nodes=copy.deepcopy(fixtures['nodes'])
        if mode:
            for node in nodes:node['coins']=[];node['relevant']=[]
            nodes[-1]['coins']=fixtures['variants'][mode];nodes[-1]['relevant']=fixtures['variants'][mode]
        return op('chain',history='8',nodes=nodes)
    def add(*ops,mode=None,load=True,**kw):cases.append(dict(consolidation=True,ops=([op('load',rows=rows)] if load else [])+[chain(mode)]+list(ops),**kw))
    add(op('meta',name='consolidate'))
    add(cmd(),expectAmount='8',expectSplit='2')
    add(op('run',text='consolidate invalid:true'))
    for field in ['tokenid','burn','coinage','maxcoins','maxsigs','debug','dryrun','password']:
        for value in [None,True,1,{},[],'','bad']:add(cmd({field:value}))
    add(op(name='consolidate',params={},recordSend=True))
    for age in ['-1','0','2.999','3','8','9','10','11','12']:add(cmd(dict(coinage=age)))
    for coins,sigs in itertools.product(['-1','0','1','2','3','5','20','2147483648','4294967298'],['-1','0','1','2','5','2147483648']):add(cmd(dict(maxcoins=coins,maxsigs=sigs,debug='true')),mode='grouped')
    # Stable amount sorting first, then grouping by the first appearance of address.
    for mode in ['grouped','tied']:
        add(cmd(dict(debug='true')),mode=mode,expectIDs=['0x80','0x83','0x81','0x84','0x82','0x85'])
    add(cmd(dict(maxsigs='1')),mode='grouped',expectAmount='9')
    add(cmd(dict(maxcoins='0')),mode='grouped',expectAmount='6')
    add(cmd(dict(maxsigs='0')),mode='grouped',expectAmount='0')
    add(cmd(),mode='many',expectAmount='285')
    add(cmd(dict(maxcoins='25')),mode='many',expectAmount='325')
    add(cmd(dict(tokenid=fixtures['tokenid'])),mode='token',expectAmount='2100')
    add(cmd(dict(tokenid=fixtures['tokenid'],maxsigs='0')),mode='missingToken')
    add(cmd(),mode='young',expectNoForward=True)
    add(cmd(dict(maxcoins=True)),mode='young',expectNoForward=True)
    for token in ['0x00','0x0000','0X00','Mx200104JMFSAS','bad','0xFF']:add(cmd(dict(tokenid=token)))
    for burn in ['-1','0','1','100']:add(cmd(dict(burn=burn)),expectAmount='8')
    for debug,dryrun in itertools.product(['true','false','bad'],['true','false','bad']):add(cmd(dict(debug=debug,dryrun=dryrun)))
    for password in ['', 'student secret', 'é\n\t', 'quoted"value', 'before\0after']:
        add(cmd(dict(debug='true',password=password)),expectRedacted=True)
    add(cmd(dict(dryrun='true',password='student secret')),expectRedacted=True,expectDebug=True)
    for result in [dict(status=True,response={'accepted':True}),dict(status=True,response=None),dict(status=False,message='preferred',error='secondary'),dict(status=False,message=None,error='fallback'),dict(status=False,error=17),dict(status=False),dict(status=False,message=False),{},dict(status='true'),dict(status=None),dict(status=1)]:
        add(cmd(sendResult=result),expectResult=result)
    add(cmd(sendThrow=True))
    for fail,runtime in itertools.product(range(1,31),[False,True]):add(cmd(fail=fail,runtimeFailure=runtime))
    for supplied in [[],[row|dict(address=None)],[row|dict(address='invalid')]]:add(cmd(rows=supplied))
    add(cmd(nullRows=True));add(cmd(),load=False)
    cases.append(dict(consolidation=True,ops=[op('load',rows=rows),cmd()],expectNoForward=True))
    # The recorded prototype must not survive a completed or failed operation.
    add(cmd(),op('run',text='send invalid:true'),expectRestored=True)
    # Actual source/native send with existing proof fixture and recorded miner.
    add(op('mining'),op(name='consolidate',params=dict(tokenid='0x00',dryrun='true')),expectReal=True)
    add(op('mining'),op(name='consolidate',params=dict(tokenid='0x00',maxsigs='0',dryrun='true')),expectReal=True)
    for result in [{},dict(status=None),dict(status='true'),dict(status=1)]:
        add(op('run',text='send amount:1 address:0x01',recordSend=True,sendResult=result),expectRunnerStatus='java.lang.NullPointerException' if result.get('status') is None else 'java.lang.ClassCastException')
    add(op('run',text='consolidate tokenid:0x00',recordSend=True),expectAmount='8',expectPublicSuccess=True)
    add(op('run',text='consolidate tokenid:0x00',recordSend=True,caller='0x1234'),expectPermission=True)
    add(op('run',text='consolidate tokenid:0x00',recordSend=True,sendResult={}),expectNestedStatus=True)
    return cases

def normalize(value,case):
    # consolidate copies only send.response, so its outer reply lacks dryrun.
    value=copy.deepcopy(value)
    for operation,entry in zip(case['ops'],value.get('ops',[])):
        if operation.get('name')=='consolidate' and not operation.get('recordSend') and operation.get('params',{}).get('dryrun')=='true':
            reply=entry.get('value',{})
            if reply.get('status') is True:
                body=reply['response']['body'];prng=body['prng'];assert prng.startswith('0x') and len(prng)==66,body
                body['prng']='<32 random bytes>'
    return value

def validate(case,out):
    last=out['ops'][-1];calls=last['forwarded']
    if 'expectAmount' in case:assert len(calls)==1 and calls[0]['params']['amount']==case['expectAmount'],last
    if 'expectSplit' in case:assert calls[0]['params']['split']==case['expectSplit'],last
    if 'expectIDs' in case:
        ids=[s.split(' coinid:')[1] for s in last['logs'] if s.startswith('Consolidate - add coin ')];assert ids==case['expectIDs'],last
    if case.get('expectNoForward'):assert calls==[] and 'exception' in last,last
    if case.get('expectRedacted'):
        lines=[s for s in last['logs'] if s.startswith('Consolidate command : ')];assert len(lines)==1 and lines[0].endswith(' password:####'),last
    if case.get('expectDebug'):assert calls[0]['params']['debug']=='true' and calls[0]['params']['dryrun']=='true',last
    if 'expectResult' in case:
        result=case['expectResult'];status=result.get('status')
        if type(status) is bool:
            value=last['value'];assert value['status']==status,last
            if status:assert value.get('response')==result.get('response'),last
            else:assert value['message']==next((result[k] for k in ['message','error'] if result.get(k) is not None),result),last
        else:assert 'exception' in last,last
    if case.get('expectRestored'):assert calls==[] and last['value'][0]['status'] is False,last
    if case.get('expectReal'):assert calls==[] and 'value' in last,last
    if 'expectRunnerStatus' in case:assert last['exception'].startswith(case['expectRunnerStatus']) and len(calls)==1,last
    if case.get('expectPublicSuccess'):assert last['value'][0]['status'] is True and last['value'][0]['response']=={'forwarded':True},last
    if case.get('expectPermission'):assert calls==[] and last['value'][0]['error']=='You need ADMIN privileges to run this command',last
    if case.get('expectNestedStatus'):assert len(calls)==1 and last['value'][0]['status'] is False and last['value'][0]['error'].startswith('java.lang.NullPointerException') and len(last['logs'])==1,last
