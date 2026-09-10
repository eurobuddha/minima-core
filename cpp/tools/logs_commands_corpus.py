"""Ordered logging flag mutations, including the unreported transaction switch."""
import itertools
from main_runtime_differential import op

KEYS=['scripts','mining','maxima','blocks','networking','ibd','peerschecker','txpowdb']
ALL=KEYS+['txpowTransactions']
MISSING=object()

def state(mode):return {key:bool((i+mode)%2) for i,key in enumerate(ALL)}

def effect(initial,params):
    current=dict(initial);error=None
    for key in KEYS:
        if key not in params:continue
        value=params[key]
        if value is None:error='java.lang.NullPointerException';break
        if not isinstance(value,str):error='java.lang.ClassCastException';break
        value=value.strip(''.join(chr(i) for i in range(33)))
        if not value:error='org.minima.system.commands.CommandException: BLANK param not allowed : '+key;break
        current[key]=value=='true'
        if key=='txpowdb':current['txpowTransactions']=current[key]
    return current,error

def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    add(op('logFlags',values=state(0)),op('meta',name='logs'))
    values=[MISSING,None,False,True,0,1,{},[],'',' ','true','false','TRUE',' true ',' false ','π 🐼','\0true\x1f','\u00a0true\u00a0']
    for key,value,prior in itertools.product(KEYS,values,[0,1]):
        # Opposite values before and after the tested key expose failure order.
        params={k:'true' if not v else 'false' for k,v in state(prior).items() if k in KEYS}
        if value is MISSING:params.pop(key)
        else:params[key]=value
        after,error=effect(state(prior),params)
        add(op('logFlags',values=state(prior)),op('direct',name='logs',params=params,expectedFlags=after,expectedError=error),op('logFlags',expect=after))
    for prior in [0,1]:
        after,_=effect(state(prior),{})
        add(op('logFlags',values=state(prior)),op('direct',name='logs',params={},expectedFlags=after,expectedError=None),op('logFlags',expect=after))
        for key in KEYS:
            params={key:'true'};after,error=effect(state(prior),params)
            add(op('logFlags',values=state(prior)),op('direct',name='logs',params=params,expectedFlags=after,expectedError=error),op('logFlags',expect=after))
    for enabled in [False,True]:
        after=state(0);after['ibd']=enabled
        add(op('logFlags',values=state(0)),op('direct',name='logs',params={'ibd':'true' if enabled else 'false'},expectedFlags=after,expectedError=None),op('syncCurrent',value=True,expectLogs=['SYNC IBD LOCK : true'] if enabled else []),op('logFlags',expect=after))
    for caller,text in itertools.product(['0x00','0x1234'],['logs','logs scripts:true','logs txpowdb:true','logs scripts:true txpowdb:{}','logs scripts:true txpowdb:" "','logs unknown:true','logs scripts:TRUE','logs ibd:true;logs mining:true;logs','logs scripts:true;logs scripts:false']):
        add(op('logFlags',values=state(0)),op('run',text=text,caller=caller),op('logFlags'))
    return result

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd']=='logFlags':
            assert 'exception' not in row,row
            if 'expect' in step:assert row['value']==step['expect'],(step,row)
        if step['cmd']=='meta':assert row['value']['name']=='logs' and row['value']['fresh'] and row['value']['valid']==KEYS,row
        if 'expectedFlags' in step:
            error=step['expectedError']
            if error:assert row.get('exception')==error,(step,row)
            else:
                assert 'exception' not in row and row['value']['status'],row
                assert row['value']['response']=={k:v for k,v in step['expectedFlags'].items() if k in KEYS},(step,row)
            assert row['events']==[] and row['timers']==[] and row['listeners']==[] and row['state']['queue']==0,row
        if 'expectLogs' in step:assert row['logs']==step['expectLogs'],(step,row)
