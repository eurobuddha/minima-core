"""Deterministic queue observations and independently bounded systemcheck timers."""
import itertools, re
from main_runtime_differential import op

LABELS = ['Main', 'TxPowProcesssor', 'TxPowMiner', 'NIOManager', 'P2PManager', 'SendPollManager', 'NotifyManager']
NAMES = ['main', 'txpowprocessor', 'txpowminer', 'niomanager', 'p2pmanager', 'senpollmanager', 'notifymanager', 'timerprocessor']


def call(params=None, **kw):
    return op('direct', name='systemcheck', params={} if params is None else params, systemTimer=True, **kw)


def cases():
    rows = []
    def add(*steps): rows.append(dict(systemCheck=True,ops=list(steps)))
    # Run before the first singleton timer is installed in either process.
    add(op('systemSetup', skipTimer=True), call(), op('systemSnapshot'))
    add(op('meta', name='systemcheck'))
    for queued, last, shutdown in itertools.product([False, True], [None, 'LAST', 'π 🐼'], [False, True]):
        counts = [i+1 if queued else 0 for i in range(7)]
        add(op('systemSetup', queued=queued, last=last), op('fields', values={'shutdown':shutdown}),
            call(expectCounts=counts, expectTimer=int(queued)), op('systemSnapshot'),
            call(expectCounts=[counts[0]+1,counts[1]+1,*counts[2:6],counts[6]+3], expectTimer=int(queued)), op('systemSnapshot'))
    for name, queued, nullqueue in itertools.product(NAMES+['sendpollmanager','unknown','MAIN','MAİN','maın','ſenpollmanager'], [False,True], [False,True]):
        add(op('systemSetup', queued=queued, nullQueue=nullqueue), call(dict(action='details', processor=name)), op('systemSnapshot'))
    for key, value in itertools.product(['action','processor'], [None,False,True,0,1,{},[],'',' ','unknown','details','list','LIST']):
        add(op('systemSetup'), call({'action':'details','processor':'main'}|{key:value}), op('systemSnapshot'))
    add(op('systemSetup'), call(dict(action='details')), call(dict(action='unknown')))
    for missing in ['main','processor','miner','poll','notify','network','nio','p2p']:
        setup=op('systemSetup',missing=missing, nullNetwork=missing=='network',nullNIO=missing=='nio',peer='null' if missing=='p2p' else 'real')
        add(setup,call(),op('systemSnapshot'))
        for name in NAMES: add(setup,call(dict(action='details',processor=name)))
    for index, last, details in itertools.product(range(7),[None,'LAST',''],[False,True]):
        add(op('systemSetup',queued=True,last=last),op('systemHelper',index=index,details=details))
    for index,mode in itertools.product(range(7),['peek','post','clear','keep','first']):
        add(op('systemSetup',queued=True),op('systemHelper',index=index,reentry=mode),op('systemSnapshot'))
    for details in [False,True]:add(op('systemSetup'),op('systemHelper',null=True,details=details))
    add(op('systemSetup'),call(timerFail=True),op('systemSnapshot'))
    add(op('systemSetup'),op('systemLock',write=True,enabled=True),op('systemLock',write=True,enabled=True),op('systemLock',write=False,enabled=True),call(expectLocks=[2,1]),op('systemLock',write=False,enabled=False),op('systemLock',write=True,enabled=False),op('systemLock',write=True,enabled=False),call(expectLocks=[0,0]))
    for caller,text in itertools.product(['0x00','0x1234'],['systemcheck','systemcheck action:details processor:main','systemcheck action:other','systemcheck wrong:true','systemcheck;systemcheck','systemcheck action:details processor:{}']):
        add(op('systemSetup'),op('run',caller=caller,text=text,systemTimer=True),op('systemSnapshot'))
    return rows


def project(raw, request):
    from main_runtime_differential import project as base
    value=base(raw,request)
    for step,original,row in zip(request['ops'],raw['ops'],value['ops']):
        if step['cmd']=='systemSnapshot':
            for i,queue in enumerate(row['value']):
                if queue is not None:
                    row['value'][i]=[{k:v for k,v in message.items() if k!='stackFrame'} if message is not None else None for message in queue if message is None or not message.get('stackFrame',False)]
        before,after=original['before'],original['after']
        replies=row.get('value');replies=replies if isinstance(replies,list) else [replies]
        for reply in replies:
            if not isinstance(reply,dict) or reply.get('command')!='systemcheck' or not reply.get('status'):continue
            response=reply['response']
            if 'RWLockInfo' in response:
                text=response['RWLockInfo'];match=re.fullmatch(r'java.util.concurrent.locks.ReentrantReadWriteLock@[0-9a-f]+\[Write locks = (\d+), Read locks = (\d+)\]',text)
                assert match,text
                response['RWLockInfo']=[int(v) for v in match.groups()]
        for i,line in enumerate(row['logs']):
            match=re.fullmatch(r'Timer:(-?\d+) Delay:(-?\d+) milli Msg:(.*)',line)
            if match:
                deadline,delay=int(match[1]),int(match[2]);assert before<=deadline-delay<=after,(line,before,after)
                row['logs'][i]=f'Timer:{deadline} Delay:<bounded> milli Msg:{match[3]}'
    return value


def validate(request, output):
    for index,(step,row) in enumerate(zip(request['ops'],output['ops'])):
        assert row['prefixedLogs']==len(row['logs']),row
        if step['cmd']=='systemSetup': assert 'exception' not in row,row
        if step['cmd']=='meta':
            value=row['value'];assert value['name']=='systemcheck' and value['fresh'] and value['valid']==['processor','action'],value
            assert value['help']==value['fullHelp']=='Check system processors..',value
        if 'expectCounts' in step:
            assert 'exception' not in row,row
            value=row['value'];assert value['status'],value
            response=value['response'];assert [response[k]['stack'] for k in LABELS]==step['expectCounts'],response
            assert response['TimerProcessor']==step['expectTimer'],response
            assert row['timers']==[['MAIN_CALLCHECKER',1000,True]],row
            assert row['logs']==['Posting Checker Call in TxPoWProcessor..','Posting Checker Call in Main..','Posting TIMED Checker Call in Main..'],row
            queued=output['ops'][index+1]['value']
            assert queued[0][-1]==dict(type='MAIN_CALLCHECKER',timer=False),queued
            assert queued[1][-1]==dict(type='TXPOWPROCESSOR_CHECKER_CALL',timer=False),queued
            assert [r['type'] for r in queued[6][-3:]]==['NOTIFY_POST']*3,queued
        if 'expectLocks' in step: assert row['value']['response']['RWLockInfo']==step['expectLocks'],row
        if step.get('reentry'):
            count=step['index']+1;mode=step['reentry'];assert row['reentryBefore']==count,row
            assert row['reentryAfter']==(count+1 if mode=='post' else 0 if mode=='clear' else 1 if mode=='first' else count),row
            fails=mode=='post' or (mode=='keep' and count>1)
            assert row.get('exception')==('java.util.ConcurrentModificationException' if fails else None),row
        if step.get('timerFail'):
            assert row.get('exception')=='java.lang.RuntimeException: fixture timer failure',row
            queued=output['ops'][index+1]['value'];assert len(queued[0])==len(queued[1])==1 and len(queued[6])==3,queued
        if step['cmd']=='direct' and step.get('params',{}).get('action')=='details':
            assert not row['timers'],row
            # Detail output has notification disabled, including queue contents.
            if index+1<len(output['ops']) and request['ops'][index+1]['cmd']=='systemSnapshot':
                before=output['ops'][index-1] if index else None
                if before and request['ops'][index-1]['cmd']=='systemSetup':
                    setup=request['ops'][index-1];expected=7*bool(setup.get('queued'))+bool(setup.get('nullQueue'))
                    queue=output['ops'][index+1]['value'][6]
                    if queue is not None: assert len(queue)==expected,queue
