"""Source network command composition: list, counters, host, queue and tracing."""
import itertools
from main_runtime_differential import op

def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    clients=[dict(uid='valid',valid=True,connected=True),dict(uid='invalid',valid=False,connected=True),dict(uid='pending',valid=True,connected=False)]
    add(op('meta',name='network'))
    for action,owner in itertools.product([None,False,True,0,{},[],'',' ','list','reset','recalculateip','restart','loggingon','loggingoff','LIST','unknown'],['main','noMain','noNetwork','noNIO','noPeer']):
        add(op('network',statusFixture=True,clients=clients,nullNetwork=owner=='noNetwork',nullNIO=owner=='noNIO',peer='null' if owner=='noPeer' else 'real',hostset=True,host='fixture-host'),op('clear' if owner=='noMain' else 'info'),op('direct',name='network',params={'action':action}),op('networkSnapshot'),op('drain'))
    for uid in [None,False,0,{},[],'',' ','valid','invalid','pending','missing',' valid ']:
        add(op('network',statusFixture=True,clients=clients),op('direct',name='network',params={'uid':uid}),op('networkSnapshot'))
    for hostset in [False,True]:
        add(op('network',statusFixture=True,clients=clients,hostset=hostset,host='fixture-host'),op('direct',name='network',params={}),op('direct',name='network',params={'action':'recalculateip'}),op('direct',name='network',params={}),op('networkSnapshot'))
    add(op('network',statusFixture=True,clients=clients),op('direct',name='network',params={'action':'reset'}),op('direct',name='network',params={}),op('networkSnapshot'))
    add(op('network',statusFixture=True,clients=clients),op('direct',name='network',params={'action':'loggingon'}),op('networkSnapshot'),op('direct',name='network',params={'action':'loggingoff'}),op('networkSnapshot'))
    for text,caller in itertools.product(['network','network action:list','network uid:valid','network action:reset','network action:recalculateip','network action:restart','network action:loggingon','network action:loggingoff','network action:LIST','network action:{}','network bad:true'],['0x00','0x1234']):
        add(op('network',statusFixture=True,clients=clients,hostset=True,host='fixture-host'),op('run',text=text,caller=caller),op('networkSnapshot'),op('drain'))
    for peer in ['null','wrong']:
        add(op('network',statusFixture=True,clients=clients,peer=peer),op('direct',name='network',params={'action':'loggingon'}),op('networkSnapshot',expectTrace=True),op('direct',name='network',params={'action':'loggingoff'}),op('networkSnapshot',expectTrace=False))
    add(op('network',statusFixture=True,clients=[]),op('direct',name='network',params={},expectUIDs=[]))
    duplicate=[dict(uid='same',valid=False,connected=False),dict(uid='same',valid=True,connected=True)]
    add(op('network',statusFixture=True,clients=duplicate),op('direct',name='network',params={'uid':'same'},expectUIDs=['same'],expectConnected=[False]),op('direct',name='network',params={},expectUIDs=['same'],expectConnected=[False,True]))
    add(op('network',statusFixture=True,clients=clients),op('direct',name='network',params={'action':'restart'}),op('drain',expectQueue=[{'type':'MAIN_NETRESTART','host':None}]))
    return result

def project(raw,request):
    from main_runtime_differential import project as base_project
    value=base_project(raw,request);reset_bounds=None
    for source,row,step in zip(raw['ops'],value['ops'],request['ops']):
        if step['cmd']=='network':reset_bounds=None
        if (step['cmd']=='direct' and step.get('params',{}).get('action')=='reset' and 'exception' not in row) or (step['cmd']=='run' and step.get('text')=='network action:reset' and 'exception' not in row):reset_bounds=(source['before'],source['after'])
        if step['cmd']=='networkSnapshot':
            current=row['value']['start']
            if reset_bounds:
                assert reset_bounds[0]<=current<=reset_bounds[1],(row,reset_bounds)
                assert row['value']['totalRead']==row['value']['totalWrite']==0,row
                assert row['value']['traffic']=={'reads':{},'writes':{}},row
                row['value']['start']='<validated reset clock>'
            else:assert current==0,row
        replies=row.get('value');replies=replies if isinstance(replies,list) else [replies]
        for reply in replies:
            if not isinstance(reply,dict):continue
            response=reply.get('response')
            if not isinstance(response,dict) or 'details' not in response:continue
            traffic=response['details']['traffic']
            if reset_bounds:
                assert isinstance(traffic['from'],str) and traffic['totalread']=='0 bytes' and traffic['totalwrite']=='0 bytes',traffic
                traffic['from']='<reset date>'
    return value

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd']=='networkSnapshot' and 'expectTrace' in step:assert row['value']['trace']==step['expectTrace'] and row['value']['filter']=='',row
        if step['cmd']=='drain' and 'expectQueue' in step:assert row['value']==step['expectQueue'],row
        if step['cmd']=='meta':assert row['value']['name']=='network' and row['value']['fresh'] and row['value']['valid']==['action'],row
        if step['cmd']=='run' and step['text']=='network uid:valid':assert row['value'][0]['error']=='Invalid parameter : uid',row
        if step['cmd']=='direct' and 'exception' not in row:
            params=step['params'];action=params.get('action','list').strip()
            if action=='list':
                connections=row['value']['response']['connections'];uid=params.get('uid','').strip()
                expected=set(step['expectUIDs']) if 'expectUIDs' in step else ({uid} if uid in ['valid','invalid','pending'] else set() if uid else {'valid','invalid','pending'})
                assert {item['uid'] for item in connections}==expected,row
                if 'expectConnected' in step:assert [item['isconnected'] for item in connections]==step['expectConnected'],row
                if uid:assert len(connections)<=1,row
