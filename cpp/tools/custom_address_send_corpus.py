"""Source sendfrom/createtokenfrom bodies with observable stored output replacement."""
import itertools
BASE={
    'sendfrom':dict(fromaddress='0x01',address='0x02',amount='1',script='RETURN TRUE',privatekey='0x12',keyuses='0'),
    'createtokenfrom':dict(fromaddress='0x01',amount='123.9',name='Student token',script='RETURN TRUE',privatekey='0x12',keyuses='0')}
OUTPUTS=[dict(address='0x03',amount='1'),dict(address='0x04',amount='2')]
def corpus():
    cases=[]
    def add(name,params=None,**kw):
        spec=dict(name=name,params=BASE[name]|(params or {}),materialize=True,fundedOutputs=OUTPUTS)
        spec.update(kw);cases.append(spec)
    for name,base in BASE.items():
        add(name,meta=True);add(name,expectSequence=True)
        for helper in ['hash data:0x12','txncreate id:helper;txnlist','not_a_command','']:add(name,helper=helper)
        add(name,runner=name+' invalid:true')
        for field in list(base)+(['tokenid','burn','state','split','mine'] if name=='sendfrom' else ['decimals','mine']):
            for value in [None,True,1,{},[],'','bad']:add(name,{field:value})
        for field in base:
            params=base.copy();del params[field];add(name,params={});cases[-1]['params']=params
        for index in range(1,9):
            add(name,throwAt=index,expectThrown=index<=7)
            for result in [dict(status=False,error='child rejected'),dict(status=False),dict(status=False,error=1),dict(status=False,error='before\0after'),dict(status=True),dict(status=True,response=None),dict(status=True,response='wrong'),{},dict(status=None),dict(status='true'),dict(status=1)]:
                add(name,resultAt=index,result=result)
        for script in ['RETURN [hello]','RETURN "broken','RETURN TRUE;txnlist','','é\n\t']:add(name,dict(script=script))
        for mine in ['true','TRUE','false','bad',' true ']:add(name,dict(mine=mine),expectSequence=True)
        for keyuses in ['-1','0','1.9','262144']:add(name,dict(keyuses=keyuses),expectSequence=True)
        for error in [None,'','before\0after','é\n\t']:
            for index in [2,6]:
                runner=name+' '+(' '.join(k+':"'+v+'"' for k,v in base.items()))
                add(name,runner=runner,recordRunner=True,resultAt=index,result=dict(status=False,error=error),expectRunnerError=error)
    for token,burn in itertools.product(['0x00','0x0000','Mx200104JMFSAS','0x01'],['-1','0','0.1']):add('sendfrom',dict(tokenid=token,burn=burn))
    for split in ['-1','0','1.5','2','256']:add('sendfrom',dict(split=split),expectSequence=True)
    for state in [None,True,1,[], 'bad',{}, {'1':'TRUE','2':'falſe','3':'[note]'}, {'01':1,'1':2}, {'256':'TRUE'}, {'bad':None}, {'0':None}, {'0':1.1}, {'0':{}}, {'0':[]}, {'٠':'TRUE'}, {'0':'0xABC'}, {'0':''}]:add('sendfrom',dict(state=state))
    for decimals,amount in itertools.product(['-1000','-45','-44','-1','0','1','8','16'],['1','123.9','1000000000000.99']):add('createtokenfrom',dict(decimals=decimals,amount=amount),expectToken=True)
    for decimals in [' 8','8 ','8.0','+8','٠٨','１７','17','2147483647','2147483648','-2147483649','-1000000000','-2147483648']:add('createtokenfrom',dict(decimals=decimals))
    for amount in ['-1','0','0.999','1.001','1000000000001','1000000000000.999']:add('createtokenfrom',dict(amount=amount))
    for name in [{'name':None},{'name':True},{'name':123},{'name':'course','description':'A university exercise','url':'https://example.invalid'}, {'name':{'nested':1}}, {'ticker':'NO NAME'}, 'é "token"\n', '[Student]']:
        add('createtokenfrom',dict(name=name))
    add('createtokenfrom',fundedOutputs=[],expectEmpty=True)
    add('createtokenfrom',materialize=False,expectMissing=True)
    add('createtokenfrom',nullDB=True,expectMissing=True)
    for outputs in [OUTPUTS[:1],OUTPUTS,OUTPUTS+[dict(address='0x05',amount='3')]]:add('createtokenfrom',fundedOutputs=outputs,expectToken=True)
    # With the source's space after fromaddress:, real children leave an empty row.
    # sendfrom still executes later ignored-error children; token creation fails
    # replacing output zero. Use a controlled signing reply to keep this case fast.
    add('createtokenfrom',real=True,materialize=False,expectEmpty=True)
    add('sendfrom',real=True,materialize=False,resultAt=5,result=dict(status=True),expectRealSend=True)
    # Exercise ordering of competing errors and state suppression.
    add('sendfrom',dict(tokenid='0x01',burn='1',privatekey=None,state={'bad':1}))
    add('sendfrom',dict(mine=True,split='bad'))
    add('sendfrom',dict(state={'bad':None}),expectStateFailure=True)
    add('createtokenfrom',dict(name={},decimals='17',amount='0'))
    add('createtokenfrom',dict(decimals='17',amount='0'))
    return cases

def validate(case,out):
    names=[call['text'].split(' ')[0] for call in out['calls']]
    if case.get('expectSequence'):
        assert names==['txncreate','txnaddamount','txnscript','txnmmr','txnsign','txnpost','txndelete'],out
        assert out['rows']==[] and out['value']['response']['marker']=='child6',out
    if case.get('expectThrown'):
        index=case['throwAt'];assert len(names)==index and out['exception']=='java.lang.RuntimeException: injected child failure',out
        assert len(out['rows'])==(0 if index==1 else 1),out
    if 'expectRunnerError' in case:
        assert out['value'][0]['status'] is False and out['value'][0]['error']==case['expectRunnerError'],out
        assert out['rows']==[] and out['logs']==[],out
    if case.get('expectToken'):
        assert names==['txncreate','txnaddamount','txnscript','txnmmr','txnsign','txnpost','txndelete'],out
        before=out['calls'][2]['rows'][0]['txn']['outputs'];after=out['calls'][6]['rows'][0]['txn']['outputs']
        assert before==after and len(before)==len(case['fundedOutputs']),out
        coin=before[0];assert coin['tokenid']=='0xFF' and coin['address']=='0x01' and coin['storestate'] is True,out
        from decimal import Decimal
        supply=int(Decimal(case['params']['amount']));decimals=int(case['params'].get('decimals','8'))
        token=coin['token'];assert token['decimals']==decimals and token['name']=={'name':'Student token'},out
        assert token['script']=='RETURN TRUE' and Decimal(token['total'])==(supply if decimals>=0 else 0),out
        for actual,expected in zip(before[1:],case['fundedOutputs'][1:]):
            assert actual['address']==expected['address'] and actual['amount']==expected['amount'] and actual['tokenid']=='0x00',out
        assert out['rows']==[],out
    if case.get('expectEmpty'):
        assert out['exception']=='java.lang.IndexOutOfBoundsException: Index 0 out of bounds for length 0',out
        assert names==['txncreate','txnaddamount'] and len(out['rows'])==1,out
    if case.get('expectMissing'):
        assert out['exception'].startswith('java.lang.NullPointerException') and len(names)==2,out
    if case.get('expectRealSend'):
        assert names==['txncreate','txnaddamount','txnscript','txnmmr','txnsign','txnpost','txndelete'],out
        assert out['rows']==[],out
    if case.get('expectStateFailure'):
        assert names==['txncreate','txnaddamount','txnscript'] and len(out['rows'])==1,out
