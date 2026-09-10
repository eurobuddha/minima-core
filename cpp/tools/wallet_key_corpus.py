"""Actual key records, deterministic key construction and posted Main requests."""
import itertools

def corpus(fixture):
    row=dict(size=2,depth=2,uses=3,maxuses=4,modifier='0x00',privatekey=fixture['privatekey'],publickey=fixture['publickey'],script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=[],caller='0x00')|kw
    def command(params=None,**kw):return op(name='keys',params=params or {},**kw)
    def add(*ops,**kw):cases.append(dict(vault=True,ops=list(ops),**kw))
    def loaded(*ops,**kw):add(op('load'),*ops,**kw)
    add(op('meta',name='keys'));add(op('run',text='keys invalid:true'))
    for action in ['list','new','checkkeys','genkey','createallkeys','bad','LIST']:
        params=dict(action=action,phrase='fixture seed',keyuses='23')
        add(command(params));loaded(command(params))
    for params in [{},{'publickey':fixture['publickey'].lower()},{'modifier':'0x00'},{'publickey':'0xFF','modifier':'0x00'},{'publickey':fixture['publickey'],'modifier':'missing'},{'modifier':'0xFF'}]:
        for rows in [[],[row],[{**row,'uses':-5},row,{**row,'uses':99}],[{**row,'publickey':None,'modifier':None}]]:loaded(command(params,rows=rows))
    for field in ['action','publickey','modifier','phrase','keyuses']:
        for value in [None,True,1,[],{}]:loaded(command({'action':'genkey' if field=='phrase' else 'createallkeys' if field=='keyuses' else 'list',field:value}))
    for params in [{'action':'list','publickey':'bad'},{'action':'createallkeys','keyuses':'bad'}]:loaded(command(params))
    for publickey in [fixture['publickey'],fixture['publickey'].lower(),'0xFF','bad',None]:loaded(op('keysCheck',publickey=publickey))
    loaded(op('keysCheck',publickey=fixture['publickey']),expectKeyCheck=True)
    loaded(op('keysCheck',publickey=fixture['publickey'],rows=[{**row,'modifier':'0x01'}]),expectKeyCheck=False)
    loaded(command({'action':'checkkeys'},rows=[row,{**row,'publickey':'0xFF'}]),expectKeyCounts=[1,1])
    loaded(command({'action':'checkkeys'},rows=[]),expectKeyCounts=[0,0])
    loaded(op('locked'),command({'action':'checkkeys'}))
    for field in ['privatekey','publickey','modifier']:
        for value in [None,'bad','0x00']:
            loaded(op('keysCheck',publickey=fixture['publickey'],rows=[{**row,field:value}]))
            loaded(command({'action':'checkkeys'},rows=[{**row,field:value}]))
    for action,fail,runtime in itertools.product(['list','checkkeys','new'],[1,3,8,16],[False,True]):loaded(command({'action':action},fail=fail,runtimeFailure=runtime))
    for phrase,parsed in [('fixture seed','fixture seed'),('  fixture seed  ','fixture seed'),('',''),('π 🐼','π 🐼')]:
        loaded(command({'action':'genkey','phrase':phrase}),**({'expectGeneratedPhrase':parsed} if parsed else {'expectKeyError':'BLANK param not allowed : phrase'}))
    for uses,expected in [('-1',-1),('0',0),('1.5',1),('2147483648',-2147483648)]:
        loaded(op('vaultMain'),command({'action':'createallkeys','keyuses':uses}),expectKeyMessage=expected)
    loaded(op('vaultMain'),command({'action':'createallkeys','keyuses':'1'},failPost=True))
    loaded(command({'action':'new'}));loaded(op('locked'),command({'action':'new'}))
    for caller in ['0x00','0x1234']:loaded(op('run',text='keys action:list',caller=caller))
    return cases
