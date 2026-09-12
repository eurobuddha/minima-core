"""Independent values/call-order expectations for source virtual dispatch."""
import itertools

def blob(raw):return '0x'+raw.hex().upper() if raw else ''
def dispatch_cases():
 result=[]
 byte_methods=['getValue','getByteValue','isEqual','isFalse','isTrue','toString']
 actions=byte_methods+['baseEqual','baseFalse','baseTrue','baseString']
 def byte_expected(q):
  calls=[]
  def right_value():
   if q['rightMask']&1:
    calls.append('right.getValue')
    if q['fault']=='right.getValue':raise RuntimeError('right.getValue')
    return q['rightValue']
   return q['rightRaw']%256
  # isFalse compares against the ordinary FALSE object, while its receiver's
  # overrides still apply. Keep that target distinct from the derived right.
  def invoke(method,base=False,false_target=False):
   bit=1<<byte_methods.index(method)
   if not base and q['leftMask']&bit:
    event='left.'+method;calls.append(event)
    if q['fault']==event:raise RuntimeError(event)
    return q['leftValue'] if method=='getValue' else (q['leftValue']+128)%256-128 if method=='getByteValue' else q['text'] if method=='toString' else q['answer']
   if method=='isEqual':
    left=invoke('getValue');right=0 if false_target else right_value()
    return left==right
   if method=='isFalse':return invoke('isEqual',false_target=True)
   if method=='isTrue':return not invoke('isFalse')
   if method=='toString':return str(invoke('getValue'))
   return q['leftRaw']%256 if method=='getValue' else (q['leftRaw']+128)%256-128
  expected=dict(wire=blob(bytes([q['leftRaw']%256])),calls=calls)
  action=q['action'];base=action.startswith('base');method={'baseEqual':'isEqual','baseFalse':'isFalse','baseTrue':'isTrue','baseString':'toString'}.get(action,action)
  try:expected['value']=invoke(method,base)
  except RuntimeError as error:expected['error']='java.lang.IllegalStateException: '+str(error)
  return expected
 for mask,action,answer in itertools.product(range(64),actions,[False,True]):
  # Deliberately separate stored bytes from overridden int getters, including
  # negative and out-of-byte-range values; getter results must not be narrowed.
  q=dict(op='dispatchByte',action=action,leftMask=mask,rightMask=1,leftRaw=128 if answer else 0,rightRaw=1,leftValue=-17 if answer else 1024,rightValue=-17 if answer else 1024,answer=answer,text='derived 🐼',fault='')
  result.append((q,byte_expected(q)))
 for fault,action in itertools.product(['left.'+m for m in byte_methods]+['right.getValue'],actions):
  # All overrides active covers direct throws; getter-only exposes nested calls.
  for mask in [1,4,8,63]:
   q=dict(op='dispatchByte',action=action,leftMask=mask,rightMask=1,leftRaw=255,rightRaw=0,leftValue=0,rightValue=255,answer=False,text='',fault=fault)
   result.append((q,byte_expected(q)))
 def string_expected(q):
  calls=[];expected=dict(wire='0xAA',calls=calls)
  def overridden(bit,name):
   if not q['mask']&bit:return False
   calls.append(name)
   if name==q['fault']:raise RuntimeError(name)
   return True
  def text():return q['text'] if overridden(1,'toString') else q['raw']
  def data():return bytes.fromhex(q['bytes'].removeprefix('0x')) if overridden(2,'getData') else q['raw'].encode()
  try:
   action=q['action']
   if action=='toString':expected['value']=text()
   elif action=='getData':expected['value']=blob(data())
   elif action in ['isEqual','baseEqual']:expected['value']=q['answer'] if action=='isEqual' and overridden(4,'isEqual') else text()==q['text']
   elif action=='copy':
    value=text();expected.update(value=value,data=blob(value.encode()))
   else:
    value=data();expected['wire']=blob(b'\xAA'+len(value).to_bytes(4,'big')+value)
  except RuntimeError as error:expected['error']='java.lang.IllegalStateException: '+str(error)
  return expected
 for mask,action,raw,fault in itertools.product(range(8),['toString','getData','isEqual','baseEqual','copy','write'],['','raw ASCII','é 🐼','a\0b'],['','toString','getData','isEqual']):
  q=dict(op='dispatchString',mask=mask,action=action,raw=raw,text='different 🐼',bytes='0x00FFEDA080',answer=bool(mask&1),fault=fault)
  result.append((q,string_expected(q)))
 return result
