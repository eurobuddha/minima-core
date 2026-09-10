#!/usr/bin/env python3
"""Pinned notification body, listener aliases and webhook effects against C++."""
import argparse,itertools,json,random,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def op(cmd,**kw):return dict(cmd=cmd,**kw)
def cases():
    result=[]
    hooks=['#http://fixture/a','MIN#http://fixture/b','ING#http://fixture/c']
    def add(ops,initial=None,events=None):
        result.append(dict(hooks=hooks if initial is None else initial,notifications=events if events is not None else [dict(event='MINING',data={'a':1}),dict(event='SECOND')],ops=ops))
    for initial in [[],[''],['invalid'],hooks,[None],['x',None],['',None],hooks*2]:
        add([op('get'),op('copy'),op('add',hook=None),op('add',hook=''),op('add',hook=hooks[0]),op('add',hook=hooks[0]),op('remove',hook=None),op('remove',hook=hooks[0]),op('get'),op('clear'),op('get')],initial)
    for event,initial,listener,logs in itertools.product(['MINING','mining','',None,12,True,{},[]],[[],hooks,['invalid']+hooks,hooks[:1]+['invalid']+hooks[1:],hooks+[None]], [False,True],[False,True]):
        add([op('queue'),op('next',listener=listener,logs=logs),op('unknown'),op('next')],initial,[dict(event=event),dict(event='SECOND')])
    for original,mutated in itertools.product(['MINING',None,'',1],['SECOND','MINING','',None,12]):
        add([op('process',listener=True,listenerEvent=mutated,logs=True),op('process',logs=True)],events=[dict(event=original),dict(event='SECOND')])
    for action,where,logs,fail in itertools.product(['clear','add','remove','stop'],['listenerAction','postAction'],[False,True],[False,True]):
        add([op('process',listener=True,hook='#http://fixture/new',logs=logs,postFailure=fail,**{where:action}),op('process',logs=True)])
    for failure,logFailure,replace in itertools.product([False,True],[False,True],[0,1]):
        add([op('queue'),op('next',listener=True,listenerEvent='CHANGED',listenerFailure=failure,logFailure=logFailure,replace=replace),op('process',event=1)])
    for initial,cmd,fail,null in itertools.product([[],hooks,[None]],['add','remove','clear'],[False,True],[False,True]):
        add([op(cmd,hook='#http://fixture/new',failDB=fail,nullDB=null),op('get'),op('add',hook=''),op('get')],initial)
    add([op('queue'),op('mutate',value='CHANGED'),op('queue',event=1),op('next',listener=True,replace=1),op('mutate',event=1,value='LAST'),op('next'),op('stop'),op('queue'),op('next',listener=True)])
    add([op('queue',event=-1),op('next'),op('wrong'),op('process',event=-1),op('process'),op('unknown')])
    add([op('setDB',hooks=['#http://fixture/external']),op('get'),op('add',hook=''),op('rebind',db=1),op('add',hook='NEW#http://fixture/other'),op('rebind',db=0),op('get'),op('clear')])
    for event,filter in itertools.product(['A😄B','A\ud83dB','πΣ\u0000漢字','A\ude04B','A#B'],['😄','\ud83d','\ude04','Σ','\u0000','π','漢字','a']):
        add([op('process',logs=True)],['#http://fixture/a',filter+'#http://fixture/b'],[dict(event=event)])
    add([op('process',logs=True)],['#http://fixture/a#fragment','MIN#http://fixture/b#second'])
    for hook in ['\ud83d#http://fixture/a','\ude04#http://fixture/b','😄#http://fixture/c']:
        add([op('add',hook=hook),op('add',hook=hook),op('process'),op('remove',hook=hook),op('get')],[],[dict(event='A😄B')])
    add([op('meta')])
    for filter in ['\ud83d','\ude04','😄','π','\u0000','\u00a0','  MINING  ']:
        add([op('direct',params=dict(action='add',filter=filter,hook='http://fixture/new')),op('process'),op('direct',params=dict(action='remove',hook=filter.strip()+'#http://fixture/new'))],[],[dict(event='MINING π😄')])
    for field,value,missing in itertools.product(['action','hook','filter','enable'],[None,True,1,{},[],'',' ','\u0000','\u00a0','\ud83d'],[False,True]):
        params=dict(action='errorlogs' if field=='enable' else 'add',hook='http://fixture/new',filter='MINING',enable='true');params[field]=value
        add([op('direct',params=params,nullManager=missing)])
    for params in [{},{'action':'add'},{'action':'remove'},{'action':'errorlogs'},{'action':'clear'},{'action':'errorlogs','enable':'true'}]:
        add([op('direct',params=params,nullManager=True)])
    for caller in ['0x00','0x1234']:
        add([op('command',caller=caller,text=text) for text in [
            'webhooks','WEBHOOKS action:list','webhooks action:add hook:http://fixture/new',
            'webhooks action:add hook:http://fixture/new','webhooks action:remove hook:http://fixture/new',
            'webhooks action:remove hook:#http://fixture/new','webhooks action:add hook:http://fixture/filtered filter:MINING',
            'webhooks action:errorlogs enable:true','webhooks action:errorlogs enable:TRUE',
            'webhooks action:errorlogs enable:false','webhooks action:unknown','webhooks action:clear;webhooks',
            'webhooks action:add','webhooks action:remove','webhooks action:errorlogs',
            'webhooks action:""','webhooks action:add hook:""','webhooks action:add hook:http://fixture/new filter:""',
            'webhooks bad:input;webhooks action:clear',
            'webhooks action:add hook:http://fixture/a;webhooks action:list;webhooks action:clear'
        ]])
    rng=random.Random(5564)
    for _ in range(120):
        commands=[]
        for i in range(12):
            cmd=rng.choice(['add','remove','clear','get','queue','next','process','copy','rebind','setDB'])
            commands.append(op(cmd,hook=rng.choice(hooks+[None,'','malformed']),listener=rng.choice([False,True]),listenerFailure=rng.choice([False,True]),postFailure=rng.choice([False,True]),logs=rng.choice([False,True]),failDB=rng.randrange(8)==0,db=rng.randrange(2),hooks=rng.sample(hooks,rng.randrange(4))))
        add(commands,rng.sample(hooks,rng.randrange(4)))
    return result

def normalize(value):
    if isinstance(value,dict):return {k:normalize(v) for k,v in value.items()}
    if isinstance(value,list):return [normalize(v) for v in value]
    if isinstance(value,str):
        # JVM generated dereference/cast descriptions differ from native errors.
        return re.sub(r'java.lang.(NullPointerException|ClassCastException):.*',r'java.lang.\1',value)
    return value

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'notify-manager-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NotifyManagerOracle.java'];listing=build/'notify-manager-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();failures=[];operations=0;runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NotifyManagerOracle']),Runner([str(build/'minima_notify_manager')])]
    try:
        for index,request in enumerate(corpus):
            outputs=[normalize(r.ask(request)) for r in runners]
            assert all('ops' in output and len(output['ops'])==len(request['ops']) for output in outputs),outputs
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
            operations+=len(request['ops'])
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,failures=failures,scope='Actual pinned NotifyManager and Java RPCClient against controlled HttpURLConnection; native required HTTP/listener/log receivers. UserDB actual JSON mutation and array replacement, nullable/duplicate hooks, FIFO retained JSON and message identity, listener edits/replacement/errors, snapshot delivery, stop, UTF-16 substring and first malformed hook; actual webhook command metadata, runner dispatch/admin caller, parameter errors, exact removal and nullable manager order. JVM-generated null/cast descriptions excluded; only valid http://fixture targets used, with delivery exceptions injected at the HTTP boundary. Full Main, RPCClient transport/URL error handling and logger runtime integration pending.')
    (build/'notify-manager-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
