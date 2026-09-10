"""Real quit/rescue commands and real Main owner; no worker dispatch or live rescue."""
import itertools

def cases():
 def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[],**kw)
 result=[]
 def add(*ops):result.append(dict(ops=list(ops)))
 for name in ['quit','rescue']:add(op('meta',name=name))
 for name,key,values in [('quit','compact',[None,False,True,0,1,{},[],'',' ','false','False','true','TRUE',' true\t','other']),('rescue','host',[None,False,True,0,1,{},[],'',' ','fixture:9001',' π 🐼:9001\t','127.0.0.1:0','a\0b'])]:
  for value,exists in itertools.product(values,[False,True]):
   add(op('publish' if exists else 'clear'),op('direct',name=name,params={key:value}),op('direct',name=name,params={key:value}),op('drain'))
  for exists,shutdown in itertools.product([False,True],[False,True]):
   add(op('publish' if exists else 'clear'),op('fields',values={'shutdown':shutdown}),op('direct',name=name,params={}),op('drain'))
 for fallback,explicit in itertools.product(['','fixture:9001','  fixture:9001  ','π 🐼:9001'],[None,'',' override:9002\t']):
  add(op('rescueNode',host=fallback),op('direct',name='rescue',params={} if explicit is None else {'host':explicit}),op('drain'))
 texts=['quit','quit compact:true','quit compact:false','quit compact:{}','quit invalid:true','quit compact:true;quit compact:false','rescue','rescue host:fixture:9001','rescue host:"π 🐼:9001"','rescue host:{}','rescue invalid:true','rescue host:fixture:9001;rescue host:other:9002']
 for caller,text in itertools.product(['0x00','0x1234'],texts):add(op('rescueNode',host=''),op('run',caller=caller,text=text),op('drain'))
 return result

def validate(request,output):
 # A command's successful rescue reply must correspond to the actual queued
 # host/type. This independent invariant also checks repeated commands and order.
 pending=[]
 for step,row in zip(request['ops'],output['ops']):
  value=row.get('value')
  replies=value if step['cmd']=='run' and isinstance(value,list) else [value]
  for reply in replies:
   if isinstance(reply,dict) and reply.get('command')=='rescue' and reply.get('status') is True:
    pending.append({'type':'MAIN_DO_RESCUE','host':reply['response']['host']})
  if step['cmd']=='drain':assert value==pending,(step,value,pending);pending=[]
