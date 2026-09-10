"""Network commands composed with real Main/NIO/P2P queues and packet codecs."""
import itertools

def cases():
 def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[],**kw)
 clients=[dict(uid='valid',valid=True,connected=True),dict(uid='invalid',valid=False,connected=True),dict(uid='pending',valid=True,connected=False)]
 result=[]
 def add(*ops):result.append(dict(ops=list(ops)))
 for name in ['disconnect','message','p2pstate']:add(op('meta',name=name))
 for name,key in [('disconnect','uid'),('message','data')]:
  for value,owner in itertools.product([None,False,True,0,1,{},[],'',' ','all','ALL','valid',' invalid ','π 🐼','a\0b'],['main','noMain','noNetwork','noNIO']):
   add(op('network',clients=clients,nullNetwork=owner=='noNetwork',nullNIO=owner=='noNIO'),op('clear' if owner=='noMain' else 'info'),op('direct',name=name,params={key:value}),op('networkSnapshot'))
  add(op('network',clients=clients),op('direct',name=name,params={}),op('networkSnapshot'))
 for data,uid in itertools.product(['',' ','hello','π 🐼','a\0b','a'*8193],[None,'','valid','invalid','pending','unknown',' all ']):
  params={'data':data}
  if uid is not None:params['uid']=uid
  add(op('network',clients=clients),op('direct',name='message',params=params,expectPacket=True),op('networkSnapshot'))
 for uid in [None,False,True,0,{},[]]:add(op('network',clients=clients),op('direct',name='message',params={'data':'hello','uid':uid}),op('networkSnapshot'))
 for peer,owner in itertools.product(['real','null','wrong'],['main','noMain','noNetwork','noNIO']):
  add(op('network',clients=clients,peer=peer,nullNetwork=owner=='noNetwork',nullNIO=owner=='noNIO'),op('clear' if owner=='noMain' else 'info'),op('direct',name='p2pstate',params={}),op('networkSnapshot'))
 for accept,startup,discovery in itertools.product([False,True],repeat=3):add(op('network',clients=clients,state=dict(accept=accept,startup=startup,discovery=discovery)),op('direct',name='p2pstate',params={}),op('networkSnapshot'))
 for caller,text in itertools.product(['0x00','0x1234'],['disconnect','disconnect uid:all','disconnect uid:valid','disconnect uid:" all "','disconnect uid:{}','disconnect invalid:true','message data:"hello π"','message data:hello uid:invalid','message data:hello uid:unknown','message data:{}','message uid:valid','message invalid:true','p2pstate','p2pstate invalid:true']):
  add(op('network',clients=clients),op('run',caller=caller,text=text),op('networkSnapshot'))
 add(op('network',clients=clients),op('direct',name='disconnect',params={'uid':'all'}),op('direct',name='disconnect',params={'uid':' valid '}),op('direct',name='disconnect',params={'uid':''}),op('networkSnapshot'))
 return result

def validate(request,output):
 pending=[];expected=None;owner=True;network=None
 for step,row in zip(request['ops'],output['ops']):
  if step['cmd']=='network':
   assert 'exception' not in row,row
   network=step
  if step['cmd']=='clear':owner=False
  if step['cmd']=='meta':assert row['value']['name']==step['name'] and row['value']['fresh']
  if step['cmd']=='direct' and step['name']=='p2pstate' and owner and network and not network.get('nullNetwork') and not network.get('nullNIO') and network.get('peer','real')=='real':
   assert 'p2p-state' in row['value'] and 'p2p_state' in row['value']['p2p-state'],row
  if step['cmd']=='direct' and step.get('expectPacket'):
   assert 'exception' not in row,row
   params=step['params'];raw=params['data'].encode('utf-8');packet='0x'+(bytes([5])+len(raw).to_bytes(4,'big')+raw).hex().upper()
   uid=params.get('uid','');expected={'valid':[packet] if uid in ('','valid') else [],'invalid':[packet] if uid=='invalid' else [],'pending':[]}
  if step['cmd']=='direct' and step['name']=='disconnect' and 'exception' not in row:
   uid=step['params']['uid'];pending.append({'type':'NIO_DISCONNECTALL'} if uid=='all' else {'type':'NIO_DISCONNECT','uid':uid})
  if step['cmd']=='networkSnapshot':
   if expected is not None:assert {x['uid']:x['packets'] for x in row['value']['clients']}==expected
   if pending:assert row['value']['queued']==pending,(row['value']['queued'],pending)
