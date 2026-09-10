"""Actual txncheck and TxBody/generator ownership using existing consensus fixtures."""

def corpus(fixtures):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='run',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00')|kw
    def direct(params=None):return op('direct',name='txncheck',params=params or {})
    def fixture(f,**kw):return op('validationFixture',mmr=f['mmr'],pow=f['pow'],block=f['block'],**kw)
    def add(*ops,**kw):cases.append(dict(ops=list(ops),**kw))
    add(op('meta',name='txncheck'))
    for params in [{},{'id':'missing'},{'id':''},{'id':None},{'id':1},{'id':[]},{'id':{}}]:add(direct(params))
    add(op(text='txncheck invalid:value'))
    for f in fixtures:
        add(fixture(f),direct({'id':'test'}),direct({'id':'test'}))
        for caller in ['0x00','0x1234']:add(fixture(f),op(text='txncheck id:test;txnlist id:test',caller=caller))
    valid=next(f for f in fixtures if f['mode']==0 and f['script']=='RETURN TRUE')
    signed=next(f for f in fixtures if f['mode']==29 and f['script']=='RETURN TRUE')
    add(fixture(valid),direct({'id':'test'}),expectValid=True)
    add(fixture(signed),direct({'id':'test'}),expectBadSignature=True)
    for hashrate in ['0','0.5','-0.5','-1','1','1000000','1.5']:
        add(fixture(valid,hashrate=hashrate),direct({'id':'test'}))
    for commands in [[],['txnstate id:test port:5 value:23'],['txnclear id:test mmr:true scripts:false signatures:false'],['txnclear id:test'],['txndelete id:test']]:
        add(fixture(valid),op('generate'),*(op(text=text) for text in commands),op('held'),expectShared=True)
    # A body reader mutates its pre-existing field objects, even when two fields alias.
    for alias in [False,True]:
        for nulls in [[],['transaction'],['witness'],['burnTransaction'],['burnWitness']]:
            add(fixture(valid),op('body',alias=alias,nulls=nulls),op('bodyread',data=valid['body']),expectBodyIdentity=True)
        for f in fixtures[::3]:
            add(fixture(valid),op('body',alias=alias,nulls=[]),op('bodyread',data=f['body']),op('bodyread',data=valid['body']),expectBodyIdentity=True)
        size=(len(valid['body'])-2)//2
        for length in sorted(set([0,1,2,3,4,5,6,7,8,12,16,32,36,40,48,64,96,128,256,size-1,size,size+1])):
            data=(valid['body']+'FF')[:2+length*2]
            add(fixture(valid),op('body',alias=alias,nulls=[]),op('bodyread',data=data),expectBodyIdentity=True)
    return cases
