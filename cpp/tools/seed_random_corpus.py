"""Actual seedrandom command, using only synthetic fixture seed rows."""
import hashlib,itertools,json
from wallet_balance_corpus import preserve_text

def corpus():
 row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
 cases=[]
 def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=[],caller='0x00')|kw
 def command(params=None,**kw):return op(name='seedrandom',params=params or {},replyText=True,**kw)
 def add(*ops,load=True,seed='0xAA'):
  cases.append(dict(ops=([op('load',rows=[row|dict(seed=seed)])] if load else [])+list(ops)))
 add(op('meta',name='seedrandom'),load=False)
 add(command(dict(modifier='hello')),load=False)
 modifiers=['hello','Hello you','  hello\t\n','π 🐼','é','e\u0301','\u00a0hi\u00a0','a\0b','a\r\nb','𐐀','RETURN TRUE','java.lang.NullPointerException','?','\ud800','x\udc00y','a'*4097]
 seeds=['0xAA','0x01','0x0001','0x'+'FF'*32,'0x'+'12'*64,'0x'+'AB'*129,'0x','00','0x0000','0X00']
 for seed,modifier in itertools.product(seeds,modifiers):
  normalized=modifier.strip(''.join(chr(i) for i in range(33)))
  add(command(dict(modifier=modifier),expected=dict(seed=seed,modifier=normalized)),command(dict(modifier=modifier),expected=dict(seed=seed,modifier=normalized)),seed=seed)
 for value in [None,True,False,7,{},[],'',' ','\0','\t\n']:
  add(command(dict(modifier=value)));add(command(dict(modifier=value)),seed='0x00');add(command(dict(modifier=value)),load=False)
 add(command());add(command(),seed='0x00')
 for seed in [None,'bad','0xGG',' 0xAA ','0x1','a','0x00']:
  add(command(dict(modifier='hello')),seed=seed);add(command(),seed=seed)
 # updateSeedRow changes the cached SeedRow only after the actual SQL update.
 for fail,runtime in itertools.product(range(0,7),[False,True]):
  add(op('vaultSeed',phrase='synthetic update',seed='0xBB',fail=fail,runtimeFailure=runtime),command(dict(modifier='hello')))
 for fail,runtime in itertools.product(range(1,39),[False,True]):
  add(op('load',fail=fail,runtimeFailure=runtime),command(dict(modifier='hello')),load=False)
 add(command(dict(modifier='hello')),op('locked'),command(dict(modifier='hello')))
 for caller in ['0x00','0x1234']:
  for text in ['seedrandom modifier:hello','seedrandom modifier:"Hello you"','seedrandom','seedrandom modifier:hello invalid:true','seedrandom modifier:hello;seedrandom modifier:hello']:
   add(op('run',text=text,caller=caller,replyText=True))
 return cases

def project(raw,normalized,request):return preserve_text(raw,normalized,request)

def validate(request,output):
 def frame(data):return len(data).to_bytes(4,'big')+data
 for step,result in zip(request['ops'],output['ops']):
  if step['op']=='direct' and 'exception' not in result:
   assert result['events']==[],result['events']
  if 'expected' not in step:continue
  expected=step['expected'];modifier=expected['modifier'];seed=expected['seed']
  seed=seed[2:] if seed.lower().startswith('0x') else seed
  raw=modifier.encode('utf-8',errors='replace')
  first=hashlib.sha3_256(frame(frame(raw))+frame(bytes.fromhex('DEADDEAD'))).digest()
  final='0x'+hashlib.sha3_256(frame(first)+frame(bytes.fromhex(seed))).hexdigest().upper()
  response=json.loads(result['value'])['response']
  # The Java fixture stdout uses UTF-8; isolated surrogates become '?'.
  observed=modifier.encode('utf-8',errors='replace').decode()
  assert response['modifier']==observed,(response['modifier'],observed)
  assert response['seedrandom']==final,(response['seedrandom'],final)
