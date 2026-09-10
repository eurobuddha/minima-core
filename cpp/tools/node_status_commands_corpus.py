"""Status assembly with independent arithmetic, file, clock and heap checks."""
import itertools,math,re
from decimal import Decimal,localcontext,ROUND_HALF_EVEN,ROUND_CEILING
from main_runtime_differential import op
from healthcheck_commands_corpus import seed

FILES={'databases/walletsql/wallet.mv.db':1023,'databases/txpowsql/txpow.mv.db':1024,
       'databases/archivesql/archive.mv.db':2048,'databases/cascade.db':17,
       'databases/chaintree.db':33,'databases/userprefs.db':45,'databases/p2p.db':67,
       'nested/a/b/c/d/payload':3000,'top':11}
LOGS=['Main Settings Done..','Memory Done..','Chain Tree done..','Mempool done..',
      'RamDB done..','txpowdb done..','archivedb done..','Database done..','Network done..']
COUNTS={'SELECT COUNT(*) AS tot FROM txpow':[{'tot':23}],
        'SELECT COUNT(*) as tot FROM syncblock':[{'tot':42}]}

def setup(locked=False,**kw):
    return op('statusSetup',rowsBySQL={'SELECT * FROM seed WHERE id=1':[{'phrase':'fixture only','seed':'0x00' if locked else '0x01'}]},files=FILES,**kw)

def call(params=None,**kw):return op('direct',name='status',params=params or {},rowsBySQL=COUNTS,**kw)

def devices(difficulty,rate,low):
    with localcontext() as c:
        c.prec=7;c.rounding=ROUND_HALF_EVEN
        weight=Decimal(2**256-1)/Decimal(int(difficulty,16))
        c.prec=64
        ratio=weight/max(Decimal(rate),Decimal(10000))
        if low:ratio*=10
        return format(ratio.to_integral_value(rounding=ROUND_CEILING),'f')

def cases():
    result=[]
    def add(*steps):result.append(dict(nodeStatus=True,ops=list(steps)))
    add(op('meta',name='status'),op('statusMain'))
    # Setup omissions expose receiver dereferences in source order.
    add(call(error='java.lang.NullPointerException'))
    for skip in ['skipWallet','skipTxPoW','skipArchive']:
        add(setup(**{skip:True}),call({'debug':'true'},error='java.lang.NullPointerException'))
    for count,complete,debug,locked in itertools.product([0,1,2,4],[False,True],[False,True],[False,True]):
        add(setup(locked=locked),seed([str(i) for i in range(count)],['-2','-1']),
            call({'complete':str(complete).lower(),'debug':str(debug).lower()},
                 **({'error':'java.lang.NullPointerException'} if complete and not count else {'success':True,'empty':not count,'locked':locked,'block':count-1,'length':count+2,'speed':1 if count<4 else 1,'coins':'0'})))
    # Hash-rate clamping and DECIMAL32 occur even for non-block test TxPoWs.
    for difficulty,rate,low in itertools.product(['0x'+'FF'*32,'0x'+'01'+'00'*25,'0x123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF'],['0','-1','9999','10000','10000.1','1000000'],[False,True]):
        add(setup(),seed(['2'],difficulty=difficulty),op('prefs',values={'hashrate':rate}),op('low' if low else 'normal'),
            call({'complete':'true'},success=True,devices=devices(difficulty,rate,low),powmode='low' if low else 'normal'))
    for difficulty in ['0x00','0x01']:
        add(setup(),seed(['2'],difficulty=difficulty),call({'complete':'true'},error='java.lang.ArithmeticException: Division by zero' if difficulty=='0x00' else None))
    # Independent median speed cases, including ties, decreasing time and fractions.
    for times,speed in [(['0','1000','2000','3000'],1),(['0','2000','4000','6000'],0.5),(['3000','2000','1000','0'],None),(['0','0','0','0'],None),(['0','0','1000','2000'],2)]:
        add(setup(),seed(['0','1','2','3'],times=times),call({'debug':'true'},**({'success':True,'speed':speed} if speed is not None else {})))
    for blocks in [['100'],['100','100'],['100','150','900'],['-2','-1','0'],[str(i) for i in range(300)]]:
        add(setup(),seed(blocks),call(success=True,**({'speed':-1} if len(blocks)==1 else {})))
    add(setup(),seed(['0','1','2','3'],parents=[-1,0,0,2],coins=3,minima='123.5'),call(success=True,coins='3',minima='123.5'))
    # The clean value is ignored; debug and complete are typed, trimmed strings.
    values=[None,False,True,0,1,{},[],'',' ','true',' true ','false','TRUE','yes','0']
    for key,value in itertools.product(['clean','debug','complete'],values):
        add(setup(),seed(['2']),call({key:value}))
    for bad in [None,False,0,{},[],'invalid']:
        add(op('clear'),call({'clean':False,'debug':bad,'complete':'invalid'}))
    add(setup(),seed(['2']),call({'complete':'true','debug':'true'},success=True),seed(),call(success=True,empty=True),seed(['100']),call(success=True,speed=-1))
    add(setup(),seed(['0','1','2','3'],['-2','-1'],cascadeWeights=True),call(success=True,weight='6',cascadeWeight='2',treeWeight='4'))
    for count in [0,1]:
        add(setup(),seed(['2'] if count else [],nullCascade=True),call({'debug':'true'},error='java.lang.NullPointerException'))
    for rate in [None,False,0,{},[],'broken','1e40']:
        add(setup(),seed(['2']),op('prefs',values={'hashrate':rate}),call({'complete':'true'}))
    # Difficulty rendering pads short values and preserves overlength values.
    for difficulty in ['0x01','0x1234','0x'+'FF'*33]:
        add(setup(),seed(['2'],difficulty=difficulty),call(success=True,difficulty='0x'+difficulty[2:].zfill(64)))
    for total,free in [(0,0),(1023,0),(1024,0),(1048576,2048),(-2**63,1),(2**63-1,-1)]:
        add(setup(),seed(['2']),call(success=True,heapTotal=total,heapFree=free))
    for caller,text in itertools.product(['0x00','0x1234'],['status','status complete:true debug:true','status clean:false','status extra:true','status complete:{}','status;status clean:true']):
        add(setup(),seed(['2']),op('run',caller=caller,text=text,rowsBySQL=COUNTS))
    for null_network in [False,True]:
        add(setup(mega=True),seed(['2']),op('network',statusFixture=True,clients=[dict(uid='valid',valid=True,connected=True)],hostset=True,host='fixture-host',nullNetwork=null_network),call(success=True,network=not null_network,mega=True))
    # SQL failures must retain the source's return defaults and diagnostic order.
    for failure,runtime in itertools.product(range(1,9),[False,True]):
        add(setup(),seed(['2']),call({'debug':'true'},fail=failure,runtimeFailure=runtime))
    return result

def size(n):
    if n<1024:return f'{n} bytes'
    group=(n.bit_length()-1)//10
    return f'{math.floor(float(n)/(1024**group)*10+0.5)/10:.1f} {" KMGTPE"[group]}B'

def uptime(text,before,after):
    labels=['Years','Months','Weeks','Days','Hours','Minutes','Seconds']
    words=text.split();assert len(words)==14 and words[1::2]==labels,text
    values=list(map(int,words[::2]));seconds=sum(a*b for a,b in zip(values,[365*86400,30*86400,7*86400,86400,3600,60,1]))
    assert before//1000<=seconds<=after//1000,(text,before,after)
    assert all(v>=0 for v in values),text
    assert values[1]<=12 and values[2]<5 and values[3]<7 and values[4]<24 and values[5]<60 and values[6]<60,text

def project(raw,request):
    from main_runtime_differential import project as base_project
    value=base_project(raw,request)
    for source,row,step in zip(raw['ops'],value['ops'],request['ops']):
        native='nativeCollectionHints' in row
        if native:
            hints=row.pop('nativeCollectionHints')
            if step['cmd']=='direct':assert hints==int('clean' in step['params']),row
            elif step['cmd']=='run':
                expected=1 if step['text'] in ['status clean:false','status;status clean:true'] else 0
                assert hints==expected,(hints,step,row)
            else:assert hints==0,row
        else:maximum=row.pop('jvmMaxMemory')
        replies=row.get('value');replies=replies if isinstance(replies,list) else [replies]
        for reply in replies:
            if not isinstance(reply,dict) or 'response' not in reply or not isinstance(reply['response'],dict) or 'uptime' not in reply['response']:continue
            response=reply['response'];uptime(response['uptime'],source['before'],source['after']);response['uptime']='<validated uptime>'
            ram=response['memory']['ram']
            if native:
                total=step.get('heapTotal',1048576);free=step.get('heapFree',2048);used=(total-free+2**63)%2**64-2**63
                assert ram==size(used),(ram,used)
            else:
                match=re.fullmatch(r'(\d+) bytes|(\d+\.\d) ([KMGTPE])B',ram);assert match,ram
                if match[1]:amount=int(match[1]);assert amount<1024,ram
                else:amount=float(match[2])*1024**('KMGTPE'.index(match[3])+1)
                assert 0<=amount<=maximum*1.051,(ram,maximum)
            response['memory']['ram']='<validated runtime heap>'
    return value

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd'] in ['statusSetup','healthChain','network']:assert 'exception' not in row,(step,row)
        if step['cmd']=='statusMain':assert row['value']=='{\n  \n}\n',row
        if step['cmd']=='meta':assert row['value']['name']=='status' and row['value']['fresh'] and row['value']['valid']==['clean','debug','complete'],row
        if step.get('error'):assert row.get('exception')==step['error'],(step,row)
        if step.get('success'):assert 'exception' not in row and row['value']['status'],(step,row)
        if step['cmd']!='direct' or 'exception' in row:continue
        response=row['value']['response'];assert response['version']=='1.1.2.8',row
        assert response['txpow']==dict(mempool=0,ramdb=0,txpowdb=23,archivedb=42) if not step.get('fail') else True,row
        if step.get('empty'):
            assert {k:response[k] for k in ['length','weight','minima','coins']}==dict(length=0,weight='0',minima='0',coins='0'),row
            assert {k:response['chain'][k] for k in ['block','time','hash','length','branches']}==dict(block=0,time='NO BLOCKS YET',hash='0x00',length=0,branches=0),row
            assert not set(['size','difficulty','speed','weight'])&response['chain'].keys(),row
        else:
            for key in ['coins','minima','length','devices','powmode','weight']:
                if key in step:assert response[key]==step[key],(key,step,row)
            for key in ['speed','block','difficulty']:
                if key in step:assert response['chain'][key]==step[key],(key,step,row)
        if 'cascadeWeight' in step:assert response['chain']['cascade']['weight']==step['cascadeWeight'] and response['chain']['weight']==step['treeWeight'],row
        if 'locked' in step:assert response['locked']==step['locked'],row
        if 'mega' in step:assert response['megammr']==step['mega'],row
        if 'network' in step:assert ('network' in response)==step['network'],row
        assert response['memory']['disk']==size(sum(FILES.values())),row
        expected={key:size(FILES[path]) for key,path in zip(['wallet','txpowdb','archivedb','cascade','chaintree','userdb','p2pdb'],list(FILES)[:7])}
        assert {k:v for k,v in response['memory']['files'].items() if k!='allfiles'}==expected,row
        params=step['params'];complete=params.get('complete','false')=='true' or params.get('complete')==' true '
        assert ('allfiles' in response['memory']['files'])==complete,row
        if complete:
            allfiles=response['memory']['files']['allfiles'];assert allfiles['total']==size(sum(FILES.values())),row
            nested=allfiles['dirs']['nested'];assert nested['total']==size(3000),row
            assert nested['dirs']['a']['dirs']['b']=={'total':size(3000)},row
        if params.get('debug') in ['true',' true '] and not step.get('fail'):assert row['logs']==LOGS,row
        assert row['timers']==[] and row['listeners']==[] and row['state']['queue']==0,row
