"""History uses actual SQL, source wire, wallet caches and collection adapters."""
import copy,itertools,json,re
from wallet_balance_corpus import preserve_text

def fixture_request(fixtures):
    groups=fixtures['groups'];native,token=groups[:2]
    specs=[{},dict(inputs=native['coins'][:3],outputs=native['coins'][3:]),
           dict(inputs=token['coins'][:3],outputs=token['coins'][3:]),
           dict(inputs=token['missing'][:3],outputs=token['missing'][3:]),
           dict(burnInputs=native['coins'],burnOutputs=token['coins'],state='0x01'),
           dict(inputs=[c for g in groups[:22] for c in g['coins'][:1]],outputs=[c for g in reversed(groups[:22]) for c in g['coins'][1:2]])]
    specs += [dict(inputs=g['coins'][:3],outputs=g['coins'][3:]) for g in groups]
    specs += [dict(inputs=native['coins'][:1]),dict(outputs=token['coins'][:1]),
              dict(inputs=native['coins'][:1]*3,outputs=native['coins'][3:4]*2),
              dict(inputs=[dict(wire=native['coins'][0],amount='-2')],outputs=[dict(wire=native['coins'][0],amount='0')]),
              dict(inputs=native['coins'][:1],outputs=token['coins'][:1])]
    specs += [dict(block=str(10+i),inputs=native['coins'][:1] if i==1 else [],burnInputs=native['coins'][:1] if i==2 else [],transactions=['0x01']*i) for i in range(3)]
    return dict(kind='historyFixtures',cases=specs)

def corpus(fixtures,wires):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    rows=[row|dict(address='0x0'+str(i)) for i in range(1,4)];cases=[];groups=fixtures['groups'];token=groups[1]['id']
    def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=['0x01','0x02','0x03'],caller='0x00')|kw
    def cmd(params=None,**kw):return op(name='history',params=params or {},replyText=True,**kw)
    def add(*ops,load=True,sql=True,walletRows=None,label='',**expected):
        cases.append(dict(history=True,label=label,ops=([op('load',rows=rows if walletRows is None else walletRows)] if load else [])+([op('historyLoad')] if sql else [])+list(ops),**expected))
    def details(wire,**kw):return op('historyDetails',data=wire,replyText=True,**kw)
    def totals(tid,inp,out):return dict(inputs={tid:str(inp)},outputs={tid:str(out)},difference={tid:str(out-inp)})
    empty=dict(inputs={},outputs={},difference={})
    add(op('meta',name='history'))
    add(cmd(),expectedResponse=dict(relevant=True,txpows=[],details=[],size=0))
    add(details(wires[0]),expectedDetails=empty)
    add(details(wires[1]),expectedDetails=totals('0x00',6,15))
    add(details(wires[2]),expectedDetails=totals(token,600,1500))
    add(details(wires[3]),expectedDetails=totals(token,6,15),label='missing token metadata falls back')
    add(details(wires[4]),expectedDetails=empty,label='ignore burn and state')
    add(details(wires[1]),walletRows=[rows[0]],expectedDetails=totals('0x00',1,4))
    add(details(wires[1]),walletRows=[rows[0]|dict(simple=0)],expectedDetails=totals('0x00',1,4))
    add(details(wires[1]),walletRows=[r|dict(track=0) for r in rows],expectedDetails=empty)
    add(details(wires[1]),load=False,expectedDetails=empty)
    for wire,wanted in zip(wires[-8:-3],[
        dict(inputs={'0x00':'1'},outputs={},difference={'0x00':'-1'}),
        dict(inputs={},outputs={token:'100'},difference={token:'100'}),
        totals('0x00',3,8),totals('0x00',-2,0),
        dict(inputs={'0x00':'1'},outputs={token:'100'},difference={'0x00':'-1',token:'100'})]):
        add(details(wire),expectedDetails=wanted,label='one-sided, repeated and signed totals')
    many=groups[:22]
    add(details(wires[5]),expectedDetails=dict(inputs={g['id']:str(1 if i==0 else 100) for i,g in enumerate(many)},outputs={g['id']:str(2 if i==0 else 200) for i,g in enumerate(many)},difference={g['id']:str(1 if i==0 else 100) for i,g in enumerate(many)}),label='independent totals across Hashtable resize')
    for wire in wires:
        add(details(wire),label='helper wire')
        add(cmd(rows=[dict(txpowdata=wire)]),label='list wire')
    for kw in [dict(nullTxPoW=True),dict(bodyless=True),dict(nullTransaction=True)]:add(details(wires[1],**kw),expectException='java.lang.NullPointerException')
    for rel in ['true','false','TRUE','bad']:
        add(cmd(dict(relevant=rel),rows=[dict(txpowdata=wires[1]),dict(txpowdata=wires[2])]),expectedSize=2)
        for value in [-1,0,4,2147483648]:add(cmd(dict(action='size',relevant=rel,startmilli='123'),rows=[dict(tot=value)]))
    for action in ['list','size','customsize','transactions','nope','',None,True,False,1,{},[]]:
        add(cmd(dict(action=action,where='isblock=1',depth='0',startmilli='123')))
    for field in ['max','offset','depth','startmilli','relevant','where']:
        action='transactions' if field=='depth' else 'size' if field=='startmilli' else 'customsize' if field=='where' else 'list'
        for value in [None,True,False,0,{},[],'','bad','1.5','-1','2147483648','4294967296']:
            add(cmd(dict(action=action,relevant='false')|{field:value}),label='parameter '+field)
    for limit,offset in itertools.product(['-1','0','1.9','2147483648','4294967296'],repeat=2):
        add(cmd(dict(max=limit,offset=offset),rows=[dict(txpowdata=wires[1])]))
    for limit in ['-1','0','2.9','2147483648','4294967296']:
        add(op('historyLimit',limit=limit),cmd(),expectedLimit=int(float(limit))%(1<<32))
    for start in ['-9223372036854775808','9223372036854775807','18446744073709551615','18446744073709551616','-1.9','0']:
        add(cmd(dict(action='size',relevant='false',startmilli=start),rows=[dict(tot=7)]))
    for where in ["isblock=1 AND timemilli>1728037509020",';UPDATEDELETEinsert;',"txpowid='MiXeD';",'upupdatedate','upd;ate','İΣ𐐀😄 UPDATE','\u0000delete']:
        add(cmd(dict(action='customsize',where=where),rows=[dict(tot=7)]),expectedSize=7)
    actions=[dict(action='list'),dict(action='list',relevant='false'),dict(action='size'),dict(action='size',relevant='false'),dict(action='customsize',where='isblock=1')]
    for params in actions:
        add(cmd(params),sql=False)
        for resultRows in [[],[{}],[dict(tot=None)],[dict(txpowdata=wires[1],tot=4),dict(txpowdata=wires[2],tot=3)]]:
            add(cmd(params,rows=resultRows))
        for fail,runtime in itertools.product(range(1,16),[False,True]):
            add(cmd(params,rows=[dict(txpowdata=wires[1],tot=4),dict(txpowdata=wires[2],tot=3)],fail=fail,runtimeFailure=runtime))
        for flag in ['nullRows','nullPrepared','nullStatement']:
            add(op('historyLoad',**{flag:True}),cmd(params,rows=[dict(txpowdata=wires[1],tot=4)],**{flag:True}),sql=False)
    for fail in [1,2,5,8,12,16,19]:
        add(op('historyLoad',fail=fail),*(cmd(params,rows=[dict(txpowdata=wires[1],tot=4)]) for params in actions),sql=False)
    for wire in ['0x','0x0000',wires[1][:42],wires[1][:-2],wires[1]+'FEED']:
        add(cmd(rows=[dict(txpowdata=wires[0]),dict(txpowdata=wire),dict(txpowdata=wires[2])]))
    for custom in [dict(detailResults=[None,None]),dict(detailResults=[dict(teacher='first'),dict(teacher='second')]),dict(detailResults=[[],{}]),dict(detailThrow=1),dict(detailThrow=2)]:
        add(op('historyOverride',params={},rows=[dict(txpowdata=wires[1]),dict(txpowdata=wires[2])],replyText=True,**custom),label='public helper override')
    add(op('historyOverride',params={},rows=[dict(txpowdata=wires[1])],replyText=True,detailBlock='99'),expectedOriginalBlock='20',label='mutable helper runs after serialization')
    # Source nodes/MMRs, replacing only the TxPoW used by this counting command.
    nodes=copy.deepcopy(fixtures['nodes'][:3])
    assert len(nodes)==3
    for node,wire in zip(nodes,wires[-3:]):node['pow']=wire;node['coins']=[];node['relevant']=[]
    for depth,count,total in [('0',0,0),('-1',0,0),('1',1,3),('2',2,5),('3',3,5),('100',3,5),('1.9',1,3),('2147483648',0,0),('4294967296',0,0)]:
        add(op('chain',history='8',nodes=nodes),cmd(dict(action='transactions',depth=depth)),expectedResponse=dict(startblock=12,depth=count,transactions=total))
    add(op('chain',history='8',nodes=nodes),cmd(dict(action='transactions')),expectedResponse=dict(startblock=12,depth=3,transactions=5))
    for depth in ['0','1','-1']:add(cmd(dict(action='transactions',depth=depth)),expectException='java.lang.NullPointerException')
    for text in ['history','history max:2','history action:size','history action:customsize where:isblock=1','history invalid:true']:
        for caller in ['0x00','0x1234']:add(op('run',text=text,caller=caller,replyText=True))
    return cases

def project(raw,normalized,request):
    normalized=preserve_text(raw,normalized,request)
    for original,entry,op in zip(raw['ops'],normalized['ops'],request['ops']):
        before=entry.pop('before');after=entry.pop('after');assert after>=before
        params=op.get('params',{})
        default=op.get('name')=='history' and params.get('action')=='size' and isinstance(params.get('relevant'),str) and params['relevant']!='true' and 'startmilli' not in params
        if not default:continue
        times=[]
        for event in entry['events']:
            if event[0]=='prepared.setLong' and event[2]==1:
                times.append(event[3]);assert ((event[3]-(before-86400000))&((1<<64)-1))<=after-before,(event,before,after)
                event[3]='<validated-clock-value>'
        if 'value' in entry:
            reply=json.loads(entry['value']);value=reply['response']['startmilli']
            assert ((value-(before-86400000))&((1<<64)-1))<=after-before
            assert not times or times==[value],(times,value)
            # Replace only this numeric field; retain every other serialized byte.
            entry['value'],count=re.subn(r'("startmilli":)'+str(value)+r'(?=[,}])',r'\1"<validated-clock-value>"',entry['value']);assert count==1
    return normalized

def validate(request,output):
    entry=output['ops'][-1]
    if 'expectException' in request:assert request['expectException'] in entry['exception'],entry
    if any(k in request for k in ['expectedDetails','expectedResponse','expectedSize']):
        value=json.loads(entry['value'])
        if 'expectedDetails' in request:assert value==request['expectedDetails'],(value,request)
        if 'expectedResponse' in request:assert value['response']==request['expectedResponse'],(value,request)
        if 'expectedSize' in request:assert value['response']['size']==request['expectedSize'],value
    if 'expectedLimit' in request:
        values=[e[3] for e in entry['events'] if e[0]=='prepared.setInt' and e[2]==1]
        assert len(values)==1 and values[0]%(1<<32)==request['expectedLimit'],values
    if 'expectedOriginalBlock' in request:
        value=json.loads(entry['value']);assert value['response']['txpows'][0]['header']['block']==request['expectedOriginalBlock'],value
        assert len(entry['historyCalls'])==1 and entry['historyCalls'][0]['block']=='99',entry['historyCalls']
