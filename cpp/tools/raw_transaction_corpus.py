"""Raw transaction wrapper ordering and actual command/codec integration."""
import itertools

def corpus():
    cases=[]
    def base(name):return dict(inputs=[dict(coinid='0x01',script='RETURN TRUE'),dict(coinid='0x02',script='RETURN FALSE')] if name=='rawfrom' else ['0x01','0x02'],outputs=[dict(address='0x03',amount='1')]) | (dict(scripts=['RETURN TRUE','RETURN FALSE']) if name=='rawtxnfrom' else {})
    def add(name,params=None,**kw):cases.append(dict(name=name,params=base(name)|(params or {}),**kw))
    for name in ['rawfrom','rawtxnfrom']:
        add(name,meta=True);add(name,expectOrder=True)
        for helper in ['', 'hash data:0x12','txncreate id:helper;txnlist']:add(name,helper=helper)
        add(name,runner=name+' invalid:true')
        for field in list(base(name))+['state']:
            for value in [None,True,1,{},[],'','bad']:add(name,{field:value})
        for field in base(name):
            params=base(name);del params[field];cases.append(dict(name=name,params=params))
        for field in ['inputs','outputs']+(['scripts'] if name=='rawtxnfrom' else []):
            for item in [None,True,1,{},[], '','0x01']:add(name,{field:[item]})
        for field in ['address','amount','tokenid','storestate']:
            for value in [None,False,True,0,1,{},[],'','true','false','0x00']:add(name,dict(outputs=[dict(address='0x03',amount='1')|{field:value}]))
            add(name,dict(outputs=[{k:v for k,v in dict(address='0x03',amount='1').items() if k!=field}]))
        if name=='rawfrom':
            for field in ['coinid','script']:
                for value in [None,True,1,{},[],'']:add(name,dict(inputs=[dict(coinid='0x01',script='RETURN TRUE')|{field:value}]))
                add(name,dict(inputs=[{k:v for k,v in dict(coinid='0x01',script='RETURN TRUE').items() if k!=field}]))
        for port in ['-1','0','01','1','255','256','2147483648','١','𝟙','bad',' 1']:
            for value in [None,True,False,0,1.25,1e-7,-0.0,{},[],'','[hello]','0xabc','Mx200104JMFSAS']:
                add(name,dict(state={port:value}))
        add(name,dict(state={'2':'[first]','01':'[old]','1':'[new]','0':123}),expectStateOrder=True)
        for index in range(1,11):
            add(name,throwAt=index)
            for result in [{},dict(status=False),dict(status=False,error='reject'),dict(status=True,response=None),dict(status=True,response=7)]:add(name,resultAt=index,result=result)
        for field in ['inputs','outputs']+(['scripts'] if name=='rawtxnfrom' else []):add(name,{field:None},real=True,expectRetained=True)
        add(name,dict(inputs=[],outputs=[dict(address='0x03',amount='1',storestate=False),dict(address='0x04',amount='2')],state={'0':123,'1':'[hello]'},**(dict(scripts=['RETURN TRUE']) if name=='rawtxnfrom' else {})),real=True,expectRawExport=True)
        add(name,dict(inputs=[],outputs=[],**(dict(scripts=[]) if name=='rawtxnfrom' else {})),real=True,expectEmptyRaw=True)
        add(name,real=True,expectIgnoredInputs=True)
        add(name,dict(state={'0':'[valid]','256':'[invalid]'}),real=True,expectRetained=True)
        for text in ['true','TrUe','false','FALSE','falſe','FALſE']:
            add(name,dict(state={'0':text}),expectBoolean='TRUE' if text.lower()=='true' else 'FALSE')
            add(name,dict(inputs=[],outputs=[],state={'0':text},**(dict(scripts=[]) if name=='rawtxnfrom' else {})),real=True,expectBoolean='TRUE' if text.lower()=='true' else 'FALSE')
    return cases

def validate(case,out):
    names=[entry['text'].split()[0] for entry in out['calls']]
    if 'expectBoolean' in case:
        state=[entry['text'].split(' value:')[1] for entry in out['calls'] if entry['text'].startswith('txnstate ')]
        assert state==['\"'+case['expectBoolean']+'\"'],out
        if case.get('real'):assert out['value']['response']['txn']['transaction']['state']==[dict(port=0,type=8,data=case['expectBoolean'])],out
    if case.get('expectOrder'):
        expected=['txncreate','txninput','txnscript','txninput','txnscript','txnoutput','txnexport','txndelete'] if case['name']=='rawfrom' else ['txncreate','txninput','txninput','txnoutput','txnscript','txnscript','txnexport','txndelete']
        assert names==expected,out
    if case.get('expectStateOrder'):
        states=[entry['text'].split(' port:')[1] for entry in out['calls'] if entry['text'].startswith('txnstate ')]
        assert states==['2 value:"[first]"','1 value:"[old]"','1 value:"[new]"','0 value:"123"'],out
    if case.get('expectRawExport'):
        assert out['rows']==[] and out['export']['signatures']==[],out
        txn=out['value']['response']['txn']['transaction'];assert len(txn['outputs'])==2 and txn['inputs']==[],out
        assert [(v['port'],v['data']) for v in txn['state']]==[(0,'123'),(1,'[hello]')],out
        assert [v['storestate'] for v in txn['outputs']]==[False,True],out
    if case.get('expectEmptyRaw'):
        assert out['rows']==[] and names==['txncreate','txnexport','txndelete'],out
    if case.get('expectIgnoredInputs'):
        assert out['rows']==[] and out['value']['status'] is True and out['value']['response']['txn']['transaction']['inputs']==[],out
