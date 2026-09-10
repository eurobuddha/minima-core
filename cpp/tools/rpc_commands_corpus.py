"""RPC management and ordered failures; live enable integration is a later gate."""
import itertools
from main_runtime_differential import op

USER={'username':'alice','password':'synthetic','mode':'read'}
def config(**kw):return op('rpcConfig',**kw)
def call(params=None,**kw):return op('direct',name='rpc',params=params or {},**kw)
def response(users=0,enabled=False,authenticate=True,port=9005):
    return dict(enabled=enabled,port=port,authenticate=authenticate,username='minima',password='***',rpcusers=users)
def cases():
    result=[]
    def add(*steps):result.append(dict(rpcFixture=True,ops=list(steps)))
    add(op('meta',name='rpc'),call(expect=response(authenticate=False)))
    for port in [-1,0,1,9005,65535]:add(config(port=port),op('meta',name='rpc'),call(expect=response(authenticate=False,port=port)))
    for password,auth in itertools.product([None,'','null','synthetic'],[False,True]):
        add(config(password=password,authenticate=auth),call(expect=response(authenticate=auth)),op('rpcState',passwordExpected=password))
    for auth in [False,True]:
        for action in ['',' ','unknown','listusers','LISTUSERS']:
            add(config(authenticate=auth,users=[USER]),call({'action':action},**({'error':'org.minima.system.commands.CommandException: BLANK param not allowed : action'} if not action.strip() else {'expect':response([USER] if action=='listusers' else 1,authenticate=auth)})))
    # Returned arrays retain identity after later additions and replacement.
    add(config(authenticate=True,users=[],hold=True),call({'action':'listusers'},expect=response([USER,USER])),
        call({'action':'adduser',**USER},expect=response(1)),call({'action':'adduser',**USER},expect=response(2)),
        op('rpcState',same=True,held=[USER,USER]),call({'action':'removeuser','username':'alice'},expect=response(0)),
        op('rpcState',same=False,held=[USER,USER]),call({'action':'listusers'},expect=response([])))
    for auth,username,mode in itertools.product([False,True],['alice','minima','Minima',' minima ','','π 🐼'],['read','write','READ','WRITE',' Read ','invalid','']):
        params={'action':'adduser','username':username,'password':' synthetic ','mode':mode}
        okay=auth and bool(username.strip()) and username.strip()!='minima' and mode.strip().lower() in ['read','write']
        extra={'expect':response(1),'users':[dict(username=username.strip(),password='synthetic',mode=mode.strip().lower())]} if okay else {}
        add(config(authenticate=auth,users=[]),call(params,**extra))
    for locale,mode in itertools.product(['en','tr','az'],['READ','WRITE']):
        expected={'expect':response(1)} if mode=='READ' or locale=='en' else {'error':'org.minima.system.commands.CommandException: RPC User mode MUST be either read or write'}
        add(config(authenticate=True,locale=locale),call({'action':'adduser',**USER,'mode':mode},**expected))
    values=[None,False,True,0,1,{},[],'',' ','true','false','invalid']
    for key,value in itertools.product(['action','username','password','mode'],values):
        params={'action':'adduser',**USER};params[key]=value
        add(config(authenticate=True,users=[]),call(params))
    for missing in ['username','password','mode']:
        params={'action':'adduser',**USER};del params[missing];add(config(authenticate=True),call(params))
    for users in [None,False,0,'wrong',{},[],[None],[False],[0],['wrong'],[{}],[{'username':None}],[{'username':False}],[USER,{'username':None}],[USER,USER]]:
        for action in ['listusers','removeuser','adduser']:
            add(config(authenticate=True,users=users),call({'action':action,**USER}))
    for username in ['alice',' alice ','ALICE','missing','',None,False]:
        add(config(authenticate=True,users=[USER,USER,dict(USER,username='bob')],hold=True),call({'action':'removeuser','username':username}),op('rpcState'))
    for key,value in itertools.product(['ssl','enable'],values):
        # A missing NetworkManager rejects true/false enable before any sleep.
        add(config(authenticate=True,password='old'),call({key:value}),op('rpcState'))
    for params,interrupt in [({'enable':'false'},False),({'enable':'false','password':' new '},False),({'enable':'true','password':None},False),({'enable':'true','password':False},False),({'enable':'false','action':'adduser'},False),({'enable':'true','password':'new'},True),({'enable':'false','password':'new'},True),({'enable':'wrong'},True)]:
        add(op('network',statusFixture=True,clients=[],hostset=True,host='fixture'),config(authenticate=True,password='old'),call(params,interruptThread=interrupt,pause=params['enable']!='wrong' and not interrupt),op('rpcState'))
    for caller,text in itertools.product(['0x00','0x1234'],['rpc','rpc action:listusers','rpc action:adduser username:bob password:synthetic mode:read','rpc action:removeuser username:alice','rpc ssl:{}','rpc extra:true','rpc enable:{}','rpc action:listusers;rpc']):
        add(config(authenticate=True,users=[USER]),op('run',caller=caller,text=text),op('rpcState'))
    return result

def project(raw,request):
    from main_runtime_differential import project as base_project
    output=base_project(raw,request)
    for source,row,step in zip(raw['ops'],output['ops'],request['ops']):
        if step.get('pause'):assert source['after']-source['before']>=990,(step,source)
        if step.get('interruptThread') and step.get('params',{}).get('enable') in ['true','false']:
            assert row.get('exception')=='java.lang.InterruptedException: sleep interrupted' and not row['interrupted'],row
    return output

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd'] in ['rpcConfig','network']:assert 'exception' not in row,(step,row)
        if step['cmd']=='meta':assert row['value']['name']=='rpc' and row['value']['fresh'] and row['value']['valid']==['enable','ssl','password','action','username','password','mode'],row
        if 'error' in step:assert row.get('exception')==step['error'],row
        if 'expect' in step:assert 'exception' not in row and row['value']['response']==step['expect'],(step,row)
        if 'users' in step and step['cmd']=='direct':assert row['user']['rpcusers_allusers']==step['users'],row
        if 'same' in step:assert row['value']['sameArray']==step['same'] and row['value']['heldUsers']==step['held'],row
        if 'passwordExpected' in step:assert row['value']['password']==step['passwordExpected'],row
        assert row['timers']==[] and row['listeners']==[] and row['state']['queue']==0,row
