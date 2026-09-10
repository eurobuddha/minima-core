"""Actual diagnostic commands with independent state and sampled-clock checks."""
import itertools
from decimal import Decimal
from datetime import datetime,timezone
from main_runtime_differential import op

MISSING=object()
def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    for name in ['trace','timemilli']:add(op('diagnostics',reset=True),op('meta',name=name))
    values=[MISSING,None,False,True,0,1,{},[],'',' ','true','false','TRUE',' true ','MAIN','π 🐼','a\0b']
    for field,value,prior in itertools.product(['enable','filter','network'],values,[False,True]):
        params=dict(enable='true',filter='MAIN',network='true')
        if value is MISSING:params.pop(field)
        else:params[field]=value
        add(op('diagnostics',reset=True),op('direct',name='trace',params=dict(enable='true' if prior else 'false',filter='OLD',network='false' if prior else 'true')),op('direct',name='trace',params=params),op('diagnostics',match='prefix main suffix'))
    add(op('diagnostics',reset=True),op('direct',name='trace',params={'enable':'true','filter':'MaIn','network':'true'}),op('diagnostics',match='MAIN',expect=dict(trace=True,filter='MaIn',client=True,server=True,matches=True)),op('direct',name='trace',params={'enable':'false'}),op('diagnostics',expect=dict(trace=False,filter='',client=False,server=False)))
    add(op('diagnostics',reset=True),op('direct',name='trace',params={'enable':'false','filter':'OLD','network':'true'}),op('direct',name='trace',params={'enable':'true','filter':'NEW','network':None}),op('diagnostics',expect=dict(trace=True,filter='NEW',client=True,server=True)))
    add(op('diagnostics',reset=True),op('network'),op('direct',name='trace',params={'enable':'true','filter':'MAIN','network':'true'}),op('direct',name='network',params={'action':'loggingoff'}),op('diagnostics',expect=dict(trace=False,filter='',client=True,server=True)))
    numbers=[MISSING,None,False,True,0,{},[],'',' ','invalid','0','1','-1','1.5','-1.5','2147483647','2147483648','-2147483649','4294967296','35791','35792','596','597','1e3',' 2 ']
    for key,value in itertools.product(['minutesback','hoursback'],numbers):
        add(op('diagnostics',reset=True),op('direct',name='timemilli',params={} if value is MISSING else {key:value}))
    for minute,hour in itertools.product(['0','-1','35792',None,{},'invalid'],['1','2147483647',None,{},'invalid']):add(op('diagnostics',reset=True),op('direct',name='timemilli',params={'minutesback':minute,'hoursback':hour}))
    for caller,text in itertools.product(['0x00','0x1234'],['trace','trace enable:true','trace enable:" true "','trace enable:true filter:MAIN network:{}','trace bad:true','timemilli','timemilli minutesback:35792','timemilli hoursback:2147483647','timemilli minutesback:0 hoursback:bad','timemilli bad:true','trace enable:true;timemilli hoursback:597;trace enable:false']):add(op('diagnostics',reset=True),op('run',text=text,caller=caller),op('diagnostics'))
    return result

def signed(value,bits):
    value%=1<<bits
    return value-(1<<bits) if value>=1<<(bits-1) else value

def project(raw,request):
    from main_runtime_differential import project as base_project
    value=base_project(raw,request)
    for original,row in zip(raw['ops'],value['ops']):
        replies=row.get('value');replies=replies if isinstance(replies,list) else [replies]
        for reply in replies:
            if not isinstance(reply,dict) or reply.get('command')!='timemilli' or not reply.get('status'):continue
            response=reply['response'];params=reply.get('params',{});offset=0
            key='minutesback' if 'minutesback' in params else 'hoursback' if 'hoursback' in params else None
            if key:
                narrowed=signed(int(Decimal(params[key].strip(''.join(chr(i) for i in range(33))))),32)
                offset=signed(narrowed*(60000 if key=='minutesback' else 3600000),32)
            millis=response['timemilli'];reconstructed=signed(millis+offset,64)
            assert original['before']<=reconstructed<=original['after'],(response,params,offset,original['before'],original['after'])
            expected=datetime.fromtimestamp(millis//1000,timezone.utc).strftime('%a %b %d %H:%M:%S UTC %Y')
            assert response['date']==expected,(response,expected)
            response['timemilli']='<bounded clock>';response['date']='<verified UTC date>'
    return value

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd']=='diagnostics' and 'expect' in step:assert row['value']==step['expect'],row
        if step['cmd']=='meta':
            assert row['value']['name']==step['name'] and row['value']['fresh'],row
            if step['name']=='timemilli':assert row['value']['fullHelp'].startswith('\timemilli\n'),row
