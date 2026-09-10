"""Consolidation: actual search/filter loop and distinct public helper."""
import itertools
BASE=dict(fromaddress='0x01',tokenid='0x00',script='RETURN TRUE',privatekey='0x12',keyuses='0',burn='0')
COINS=[dict(id='0x01',address='0x01',amount='2',created='10'),dict(id='0x02',address='0x01',amount='5',created='10'),dict(id='0x03',address='0x01',amount='5',created='10')]
def corpus():
    cases=[]
    def add(params=None,**kw):
        case=dict(name='consolidatefrom',params=BASE|(params or {}),materialize=True,chainCoins=COINS)
        case.update(kw);cases.append(case)
    add(meta=True);add(expectInputs=['0x02','0x03','0x01'],expectAmount='1.2',expectOutputs=10)
    for helper in ['', 'hash data:0x12','txncreate id:helper;txnlist','not_a_command']:add(helper=helper)
    add(runner='consolidatefrom address:0x01')
    for field in list(BASE)+['maxcoins','mine']:
        for value in [None,True,1,{},[],'','bad']:add({field:value})
    for field in ['fromaddress','script','privatekey','keyuses']:
        add();del cases[-1]['params'][field]
    for limit,expected in [('-1',1),('0',1),('1',1),('1.9',1),('2',2),('3',3),('50',3),('2147483647',3),('2147483648',1),('4294967296',1),('4294967298',2)]:
        add(dict(maxcoins=limit),expectInputs=['0x02','0x03','0x01'][:expected],expectOutputs=10)
    for age in ['-1','0','2','2.999','3','3.001','20']:
        from decimal import Decimal
        coin=COINS[0]|dict(created=str(Decimal(20)-Decimal(age)))
        add(chainCoins=[coin],expectInputs=['0x01'] if Decimal(age)>=3 else [],expectNoCoins=Decimal(age)<3)
    for pool,cascade,onchain in itertools.product([[],[COINS[1]],[COINS[0],COINS[1],COINS[2]]],[False,True],[False,True]):
        expected=['0x02','0x03','0x01'] if cascade else [x for x in ['0x02','0x03','0x01'] if x not in [c['id'] for c in pool]]
        add(mempool=pool,poolCascade=cascade,poolOnChain=onchain,expectInputs=expected,expectNoCoins=not expected)
    add(chainCoins=[],expectNoCoins=True)
    add(chainCoins=[COINS[0]|dict(spent=True)],expectNoCoins=True)
    add(chainCoins=[COINS[0]|dict(address='0x02')],expectNoCoins=True)
    add(chainCoins=[COINS[0]|dict(token='0x01')],expectNoCoins=True)
    # Real search handles repeated IDs and spent markers in encounter order.
    add(chainCoins=[COINS[0]|dict(spent=True),COINS[0]],expectNoCoins=True)
    add(chainCoins=[COINS[0],COINS[0]|dict(spent=True)],expectInputs=['0x01'])
    add(mega=True,chainCoins=[],megaCoins=[COINS[0],COINS[1],COINS[2]|dict(amount='4')],expectInputs=['0x02','0x03','0x01'])
    add(mega=True,chainCoins=[COINS[1]|dict(spent=True)],megaCoins=COINS,expectInputs=['0x03','0x01'])
    add(mega=False,chainCoins=[],megaCoins=COINS,expectNoCoins=True)
    for burn in ['-1','0','1','11.999','12','13']:add(dict(burn=burn),expectBurnFailure=burn in ['12','13'])
    for token,burn in itertools.product(['0x00','0x0000','0x01'],['-1','0','1']):add(dict(tokenid=token,burn=burn))
    tokenCoins=[c|dict(token='0x99',scale='2') for c in COINS]
    add(dict(tokenid='0x99'),chainCoins=tokenCoins,expectAmount='120',expectOutputs=10)
    for mine in ['true','false','bad','TRUE']:add(dict(mine=mine))
    add(dict(mine=True),expectCreatedError=True)
    add();del cases[-1]['chainCoins'];cases[-1]['expectCreatedError']=True
    for index in range(1,20):
        add(throwAt=index,expectThrow=index)
        for result in [{},dict(status=False,error='reject'),dict(status=False),dict(status=True,response=None),dict(status=True,response='text')]:add(resultAt=index,result=result)
    # Public helper: no selection/filtering and only one output.
    for coins in [None,[],[None],COINS,COINS[::-1],tokenCoins]:add(consolidateHelper=coins)
    add(consolidateHelper=COINS,expectInputs=['0x01','0x02','0x03'],expectOutputs=1,expectAmount='12')
    for field in ['burn','keyuses','fromaddress','tokenid','script','privatekey']:add({field:None},consolidateHelper=COINS)
    for burn in ['-1','0','1','12','13']:add(dict(burn=burn,tokenid='0x99'),consolidateHelper=COINS,expectBurnFailure=burn in ['12','13'])
    for index in range(1,11):
        add(consolidateHelper=COINS,throwAt=index,expectThrow=index)
        for result in [{},dict(status=False,error='reject'),dict(status=True,response=None),dict(status=True,response='text'),dict(status=True,response={})]:add(consolidateHelper=COINS,resultAt=index,result=result)
    # Run real input/output/script commands and inspect their serialized row.
    add(execute=['txninput','txnoutput','txnscript'],expectInputs=['0x02','0x03','0x01'],expectOutputs=10,expectStored=True)
    add(consolidateHelper=COINS,execute=['txninput','txnoutput','txnscript'],expectInputs=['0x01','0x02','0x03'],expectOutputs=1,expectStored=True)
    add(consolidateHelper=[],execute=['txnoutput','txnscript'],expectOutputs=1)
    # Main does not inspect post status; helper casts only post.response.
    add(resultAt=18,result=dict(status=False,error='post failed',response={'accepted':False}),expectIgnoredPost=True)
    add(consolidateHelper=COINS,resultAt=9,result=dict(status=False,response={'accepted':False}),expectIgnoredPost=True)
    add(consolidateHelper=COINS,resultAt=9,result=dict(status=True,response='wrong'),expectCastAfterDelete=True)
    add(consolidateHelper=COINS,resultAt=9,result=dict(status=True),expectNullResponse=True)
    many=[dict(id='0x'+f'{i:02X}',address='0x01',amount=str(i),created='0') for i in range(1,56)]
    descending=[c['id'] for c in reversed(many)]
    add(chainCoins=many,expectInputs=descending[:50],expectOutputs=10)
    add(dict(maxcoins='51'),chainCoins=many,expectInputs=descending[:51],expectOutputs=10)
    add(consolidateHelper=many,expectInputs=[c['id'] for c in many],expectOutputs=1)
    add(chainCoins=[COINS[0]|dict(amount='1E-44')],expectOutputs=10,expectAmount='0')
    return cases

def validate(case,out):
    calls=out['calls'];names=[c['text'].split()[0] for c in calls]
    if 'expectInputs' in case:
        assert [c['text'].split(' coinid:')[1] for c in calls if c['text'].startswith('txninput ')]==case['expectInputs'],out
    if 'expectOutputs' in case:assert names.count('txnoutput')==case['expectOutputs'],out
    if 'expectAmount' in case:
        assert all(c['text'].split(' amount:')[1].split()[0]==case['expectAmount'] for c in calls if c['text'].startswith('txnoutput ')),out
    if case.get('expectNoCoins'):
        assert len(out['rows'])==1 and names==['txncreate'] and out['exception'].startswith('org.minima.system.commands.CommandException: No valid coins'),out
    if case.get('expectBurnFailure'):
        assert out['rows']==[] and names[-1]=='txndelete' and 'Burn greater than total' in out['exception'],out
    if case.get('expectCreatedError'):assert len(out['rows'])==1 and names==['txncreate'] and 'exception' in out,out
    if 'expectThrow' in case:
        index=case['expectThrow'];assert len(calls)==index and out['exception']=='java.lang.RuntimeException: injected child failure',out
        assert len(out['rows'])==(0 if index==1 else 1),out
    if case.get('expectStored'):
        txn=calls[-1]['rows'][0]['txn'];assert len(txn['inputs'])==3 and len(txn['outputs'])==case['expectOutputs'],out
        assert out['rows']==[],out
    if case.get('expectIgnoredPost'):
        value=out['value'] if 'consolidateHelper' in case else out['value']['response'];assert value=={'accepted':False} and out['rows']==[],out
    if case.get('expectCastAfterDelete'):assert out['rows']==[] and out['exception'].startswith('java.lang.ClassCastException'),out
    if case.get('expectNullResponse'):assert out['value'] is None and out['rows']==[],out
