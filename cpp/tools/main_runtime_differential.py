#!/usr/bin/env python3
"""Pinned Main bodies with unstarted workers and actual native database objects."""
import argparse,itertools,json,shutil,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[],**kw)
def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    add(op('info'),op('low'),op('normal'),op('sync',value=True,logs=True),op('sync',value=False),op('error',value=True,message=None),op('error',value=False,message='fixture'),op('keysSize'))
    for field,value in itertools.product(['restoring','shutdown','keys','complete','importing'],[False,True]):add(op('fields',values={field:value}),op('info'))
    for time in [-2**63,-1,0,1,2**63-1]:add(op('fields',values={'uptime':time}),op('uptime'))
    for enabled,fail,error,name in itertools.product([False,True],[False,True],[False,True],[None,'','EVENT','π 🐼']):
        add(op('listener',enabled=enabled),op('notify1',event=name,listenerFail=fail,listenerError=error),op('notify2',event=name,listenerFail=fail,listenerError=error),op('mutate',value='later'),op('notify2',event=name,**{'null':True}),op('info'))
    for to in [None,'','*','0x01']:add(op('listener',enabled=True),op('notifyTo',event='EVENT',to=to),op('info'))
    for fail,error in itertools.product([False,True],[False,True]):add(op('listener',enabled=True),op('shutdownNotice',listenerFail=fail,listenerError=error),op('publish'),op('shutdownNotice'),op('info'))
    for text in [None,'',' ','\t hash data:0xAB\r','hash data:0xAA','hash bad:x','hash data:0xAA; hash data:0xBB']:
        add(op('command',text=text))
    for cmd,flag in itertools.product(['shutdown','restore','final'],[False,True]):add(op(cmd,mds=flag,compact=flag),op(cmd,mds=flag,compact=flag),op('info'))
    names=['MAIN_CALLCHECKER','MAIN_SYSTEMCLEAN','MAIN_CLEANDB_RAM','MAIN_CLEANDB_SQL','MAIN_AUTOBACKUP_MYSQL','MAIN_AUTOBACKUP_TXPOW','MAIN_DO_RESCUE','MAIN_PULSE','MAIN_NEWBLOCK','MAIN_BALANCE','MAIN_MINING','MAIN_NETRESTART','MAIN_NETRESET','MAIN_SHUTDOWN','MAIN_INIT_KEYS','MAIN_CHECKER','MAIN_P2PNETMDS_CHECKER','MAIN_LOAD_ALL_KEYS',None,'unknown']
    for name,shutdown,restoring in itertools.product(names,[False,True],[False,True]):
        add(op('fields',values={'shutdown':shutdown,'restoring':restoring,'keys':True}),op('message',type=name,slave=True),op('info'))
    for name in names+['MAIN_AUTOBACKUP']:
        add(op('fields',values={'keys':True}),op('message',type=name,timerFail=True,pow=1,fields={'starting':True,'keyuses':5}),op('info'))
    for field,value in itertools.product(['starting','timer','keyuses','host'],[None,False,True,0,1,'string']):
        name={'starting':'MAIN_MINING','timer':'MAIN_CALLCHECKER','keyuses':'MAIN_LOAD_ALL_KEYS','host':'MAIN_DO_RESCUE'}[field]
        if field=='host' and value=='string':continue # Runtime.halt path needs a dedicated subprocess test.
        add(op('fields',values={'keys':True}),op('message',type=name,pow=1,fields={field:value}),op('info'))
    for count in [0,1,3]:add(op('chain',count=count),op('message',type='MAIN_CHECKER'),op('message',type='MAIN_CHECKER'))
    for prefs in [{'mysql_autobackup':True},{'mysqlcoins_autobackup':True},{'mysql_autobackup':True,'mysqlcoins_autobackup':True},{'mysql_autobackup':'wrong'}]:
        add(op('prefs',values=prefs),op('fields',values={'importing':True}),op('message',type='MAIN_AUTOBACKUP_MYSQL'))
    for text in [None,'',':','@','u:p@h/db','u:p:x@h/db/sub','π:🐼@host/db','u@host/db','u:p@host']:
        for coins in [False,True]:add(op('mysqlConfig',text=text,coins=coins))
    for fail,runtime in itertools.product(range(1,18),[False,True]):add(op('mysqlConfig',text='fixture:synthetic@localhost:3306/fixture',fail=fail,runtimeFailure=runtime))
    for delay in [-1,0,1]:add(op('mysqlConfig',text='fixture:synthetic@localhost:3306/fixture',delay=delay))
    prefs={'mysql_autobackup':True,'mysql_host':'fixture-host','mysql_db':'fixture-db','mysql_user':'fixture-user','mysql_password':'synthetic'}
    for field,value in itertools.product(['mysql_host','mysql_db','mysql_user','mysql_password'],[None,False,0,'','π 🐼']):
        add(op('prefs',values=prefs|{field:value}),op('message',type='MAIN_AUTOBACKUP_TXPOW',storeTxPoW=True,pow=1))
    for fail,runtime in itertools.product(range(0,30),[False,True]):
        add(op('prefs',values=prefs),op('message',type='MAIN_AUTOBACKUP_TXPOW',storeTxPoW=True,pow=1,fail=fail,runtimeFailure=runtime))
    add(op('prefs',values=prefs),op('message',type='MAIN_AUTOBACKUP_TXPOW',storeTxPoW=True))
    return result

def project(raw,request):
    assert 'ops' in raw,raw
    value=normalize(raw);uptime=0
    delays={'MAIN_PULSE':None,'MAIN_SYSTEMCLEAN':300000,'MAIN_CLEANDB_RAM':1800000,'MAIN_CLEANDB_SQL':43200000,'MAIN_AUTOBACKUP_MYSQL':7200000,'MAIN_AUTOBACKUP':86400000,'MAIN_CHECKER':180000,'MAIN_P2PNETMDS_CHECKER':1800000,'MAIN_INIT_KEYS':10000,'MAIN_NETRESET':86400000}
    for row,step in zip(value['ops'],request['ops']):
        before=row.pop('before');after=row.pop('after')
        if step['cmd']=='fields':uptime=step['values'].get('uptime',uptime)
        if step['cmd']=='uptime':
            reconstructed=(row['value']+uptime)%(2**64);assert before<=reconstructed<=after
            row['value']='<validated uptime>'
        for timer in row['timers']:
            delay=delays[timer[0]]
            if delay is None:delay=600000
            assert before+delay<=timer[1]<=after+delay,(timer,before,after,delay)
            timer[1]=delay
    return value

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--group',choices=['main','commands','network','status','diagnostics','health','logs','node-status'],default='main');args=ap.parse_args();build=args.build.resolve();oracle=build/('main-runtime-oracle-'+args.group);oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['MainRuntimeOracle.java','MainNetworkOracle.java','SqlDBOracle.java','NIOClientOracle.java','ArchiveManagerOracle.java']];listing=build/('main-runtime-java-sources-'+args.group+'.txt');listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=0;corpus=cases();projector=project;validate=lambda request,output:None
    if args.group=='commands':
        from main_lifecycle_commands_corpus import cases as command_cases,validate
        corpus=command_cases()
    if args.group=='network':
        from network_peer_commands_corpus import cases as network_cases,validate
        corpus=network_cases()
    if args.group=='status':
        from network_status_commands_corpus import cases as status_cases,validate,project as projector
        corpus=status_cases()
    if args.group=='diagnostics':
        from trace_clock_commands_corpus import cases as diagnostic_cases,validate,project as projector
        corpus=diagnostic_cases()
    if args.group=='health':
        from healthcheck_commands_corpus import cases as health_cases,validate
        corpus=health_cases()
    if args.group=='node-status':
        from node_status_commands_corpus import cases as node_status_cases,validate,project as projector
        corpus=node_status_cases()
    if args.group=='logs':
        from logs_commands_corpus import cases as logs_cases,validate
        corpus=logs_cases()
    with tempfile.TemporaryDirectory(prefix='main-runtime-',dir=build) as root:
        runners=[Runner(['java']+(['-Duser.timezone=UTC'] if args.group in ['status','diagnostics','node-status'] else [])+['-cp',f'{oracle}:{JAVA}/jar/minima.jar','MainRuntimeOracle',root]),Runner((['env','TZ=UTC'] if args.group in ['status','diagnostics','node-status'] else [])+[str(build/'minima_main_runtime'),root])]
        try:
            for index,request in enumerate(corpus):
                outputs=[]
                for runner in runners:
                    for p in Path(root).iterdir():
                        if p.is_dir():shutil.rmtree(p)
                        else:p.unlink()
                    output=projector(runner.ask(request),request);validate(request,output);outputs.append(output)
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,total_programs=len(corpus),operations=operations,failures=failures,scope='Actual Main lifecycle/message/listener bodies with unstarted workers, real MinimaDB child objects, controlled SQL driver and timers; source MySQL configuration helper. Full startup, worker/network scheduling, durable SQL, enabled backup commands and complete MegaMMR/runtime integration remain unverified.')
    if args.group=='commands':report['scope']='Actual quit/rescue commands, real unstarted Main lifecycle and queue, metadata, typed parameters and runner permissions. No successful full-node shutdown, live rescue, durable SQL or exhaustive concurrent singleton replacement is certified.'
    if args.group=='network':report['scope']='Actual disconnect/message/p2pstate commands through Main, real NIO/P2P components and unstarted fixtures; exact queues, packet bytes, traffic, status and typed failures. No real network delivery or full startup is certified.'
    if args.group=='status':report['scope']='Actual network command and Main/NIO/NetworkManager/traffic/trace composition. Direct hidden UID filter versus runner rejection, typed failures, partial logging effects, real local host enumeration, queue-only restart and validated reset clocks. Initial fixture clocks fixed; reset date prose excluded. No live restart or full startup.'
    if args.group=='diagnostics':report['scope']='Actual trace and timemilli commands, Main/runner dispatch, shared MessageProcessor/NIO trace state and matcher. Raw versus trimmed parameters, partial state after errors, integer offset wrap, minutes precedence, independently bounded clocks and exact UTC date rendering. Sampled clocks normalized after validation; enhanced NPE/CCE diagnostic prose excluded. No full-node scheduling or exhaustive locale parity.'
    if args.group=='health':report['scope']='Actual healthcheck command through registered Main/runner dispatch, real MinimaDB tree/cascade and arithmetic. Synthetic linear/branched/gapped heights, absent tip/cascade, repeated state replacement, exact response types and independent expected values. No full-node startup, cryptographic validity certificate or exhaustive concurrent owner replacement.'
    if args.group=='logs':report['scope']='Actual logs command and all nine native/Java global flags, ordered partial mutations, omitted values, hidden SQL transaction flag, exact metadata, runner gates and Main.setSyncIBD consumer. Seven GeneralParams fields use atomic native storage. No transactional snapshot, exhaustive concurrent Java scheduling or full-node logging certification.'
    if args.group=='node-status':report['scope']='Actual status command, database child objects, synthetic chain/cascade, controlled SQL and files, device arithmetic and debug order. JVM heap validated for shape/bounds; native heap and collection hint checked independently. Uptime bounded before normalization. No durable SQL, full-node startup, actual JVM/native heap equality or exhaustive concurrent scheduling.'
    (build/('node-status-command-results.json' if args.group=='node-status' else 'logs-command-results.json' if args.group=='logs' else 'healthcheck-command-results.json' if args.group=='health' else 'trace-clock-command-results.json' if args.group=='diagnostics' else 'network-status-command-results.json' if args.group=='status' else 'main-runtime-results.json' if args.group=='main' else 'main-lifecycle-commands-results.json' if args.group=='commands' else 'network-peer-commands-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
