#!/usr/bin/env python3
"""Pinned mysql command vs native: real command/codecs, recording JDBC/SQL."""
from decimal import Decimal
import argparse,copy,gzip,hashlib,itertools,json,os,re,shutil,subprocess,tempfile,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
from mysql_coins_differential import LOGIN,RANGE
FIRST='SELECT block FROM syncblock ORDER BY block ASC LIMIT 1'
LAST='SELECT block FROM syncblock ORDER BY block DESC LIMIT 1'
ARCHFIRST='SELECT * FROM syncblock ORDER BY block ASC LIMIT 1'
ARCHLAST='SELECT * FROM syncblock ORDER BY block DESC LIMIT 1'
ARCHRANGE='SELECT syncdata FROM syncblock WHERE block>? AND block<?'
COUNT='SELECT Count(*) as tot FROM syncblock'
ARCHCOUNT='SELECT COUNT(*) as tot FROM syncblock'
CASCADE='SELECT fulldata FROM cascadedata ORDER BY cascadetip ASC LIMIT 1'
BYNUM='SELECT syncdata FROM syncblock WHERE block=?'
BYID='SELECT syncdata FROM syncblock WHERE txpowid=?'
TXPOW='SELECT * FROM txpow WHERE txpowid=?'
SAVED=dict(mysql_autologindetails=True,mysql_host='saved:3306',mysql_db='saved-db',mysql_user='saved-user',mysql_password='saved-secret')
def request(params=None,**kw):
    raw=kw.pop('raw_params',None)
    return dict(op='run',params=raw if raw is not None else LOGIN|(params or {}),prefs={},mobile=False,storeAll=False,sql=dict(rows=[],columns=[]))|kw

def tables(blocks):
    return {FIRST:[dict(block=1)],LAST:[dict(block=3)],COUNT:[dict(tot=len(blocks))],ARCHCOUNT:[dict(tot=len(blocks))],ARCHFIRST:[dict(syncdata=blocks[0])],ARCHLAST:[dict(syncdata=blocks[-1])]}
def batches(blocks,split=1):return [[dict(syncdata=b) for b in blocks[:split]],[dict(syncdata=b) for b in blocks[split:]],[]]
def corpus(seed):
    cases=[]
    def add(params=None,**kw):cases.append(request(params,**kw))
    add(op='meta');add({'unused':'retained'},op='meta')
    for mobile in [True,False]:
        for p in [{},{'action':None},{'action':True},{'action':'bogus'}]:add(mobile=mobile,raw_params=p)
    for stop in range(5):add(raw_params=dict(list(LOGIN.items())[:stop]))
    for field,value in itertools.product(['action','host','database','user','password','logs','readonly'],[None,True,False,0,{},[],'','  x  ']):add({field:value})
    for action in ['info','size','wipe','setlogin','clearlogin','autobackup','integrity','update','findtxpow','addresscheck','resync','rawimport','fixmissing','h2import','h2export','rawexport','reset','bogus']:
        p=dict(action=action,enable='true',address='0x01',txpowid='0x01',file='missing')
        add(p)
        for key in ['action','file','logs','readonly','enable','keys','keyuses','phrase','address','statecheck','startfix','endfix','block','txpowid','maxexport']:
            for value in [None,True,0,'bad']:add(p|{key:value})
    for action in ['info','size','wipe','setlogin','clearlogin','autobackup','bogus']:
        for fresh in [{},{'host':'override'},{'database':'override'},{'host':'override','database':'override'},{'host':'override','database':'override','user':'new','password':'new'}]:
            add(raw_params=dict(action=action,enable='false')|fresh,prefs=SAVED)
        for read in ['true','false']:add(dict(action=action,enable='true',readonly=read),prefs=SAVED)
    for rows in [[],[dict(block=-1,tot=-1)],[dict(block=9223372036854775807,tot=4294967295)]]:
        add(dict(action='size'),sql=dict(rows=rows,columns=[]))
    for key in [ARCHFIRST,ARCHLAST]:
        add(sql=dict(rows=[],columns=[],rowsBySQL={key:[dict(syncdata=seed['chains']['normal'][0])]}))
    for name,blocks in seed['chains'].items():
        t=tables(blocks)
        for action in ['info','integrity','update','addresscheck','h2export']:
            for logs in ['true','false']:
                for split in [1,3]:
                    add(dict(action=action,logs=logs,address='0x01',file='output.gzip'),sql=dict(rows=[],columns=[],rowsBySQL=t|{LAST:[dict(block=-1)]},rowsBySQLBatches={RANGE:batches(blocks,split),ARCHRANGE:batches(blocks,split)}))
        for maximum in [None,'-1','0','1','2','3','4','1.9','4294967296','bad']:
            p=dict(action='rawexport',file='output.dat')
            if maximum is not None:p['maxexport']=maximum
            add(p,sql=dict(rows=[],columns=[],rowsBySQL=t,rowsBySQLBatches={RANGE:batches(blocks)}),expectRaw=True)
        raw=gzip.compress(bytes.fromhex(seed['raws'][name][2:]),mtime=0).hex()
        for action in ['rawimport','fixmissing']:
            for logs in ['true','false']:
                for start in ['0','1','3','-1','bad']:
                    add(dict(action=action,file='input.dat',logs=logs,startfix=start,endfix='1'),fileBytes=raw)
            for cut in [0,1,9,len(raw)//4,len(raw)//2-1]:add(dict(action=action,file='input.dat'),fileBytes=raw[:cut*2])
            add(dict(action=action,file='input.dat'),fileBytes=raw,sql=dict(rows=[],columns=[],rowsBySQL={BYNUM:[dict(syncdata=blocks[0])]}))
        for state in ['','0xAABB','0xFFFF','MxBAD','MxBAD@label']:
            add(dict(action='addresscheck',address='0x01',statecheck=state),sql=dict(rows=[],columns=[],rowsBySQLBatches={RANGE:batches(blocks)}))
        for p in [dict(txpowid='x'),dict(block='1'),dict(txpowid='x',block='bad')]:
            for t in [{},{BYID:[dict(syncdata=blocks[0])]},{BYNUM:[dict(syncdata=blocks[0])]},{TXPOW:[dict(txpowdata=seed['pows'][name])]}]:add(dict(action='findtxpow')|p,sql=dict(rows=[],columns=[],rowsBySQL=t))
    for action in ['info','size','wipe','setlogin','clearlogin','autobackup','integrity','update','findtxpow','rawimport','rawexport']:
        for flag in ['nullConnection','nullStatement','nullPrepared','nullRows']:add(dict(action=action,enable='true',block='1',file='output.dat'),sql=dict(rows=[],columns=[],**{flag:True}))
        for caller in ['0x00','0x1234']:
            text='mysql '+' '.join(k+':'+v for k,v in LOGIN.items())+' action:'+action+' enable:true block:1 file:missing'
            add(op='runner',caller=caller,text=text);add(op='runner',caller=caller,text=text+' bad:true')
    for text in [None,'','@','u:p@h/d','u:p@h/d/extra','u::p@h/d','π:🔒@localhost:3306/学生','u:p@h','abc@h/d','@h/d','u:p@/','u:p@h/']:
        for delay in [-1,0,1]:
            for coins in [False,True]:add(op='configure',text=text,delay=delay,coins=coins)
    add(op='main');add(op='main',sql=dict(rows=[dict(tot=3)],columns=[]))
    return cases

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();started=time.monotonic();os.environ['TZ']='UTC'
    oracle=build/'mysql-command-oracle';oracle.mkdir(exist_ok=True);names=['SqlDBOracle.java','MySQLCommandOracle.java','ArchiveManagerOracle.java','NIOClientOracle.java','ObjectOracle.java']
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in names];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MySQLCommandOracle']),Runner([str(build/'minima_mysql_command')])];failures=[];cases=checks=effects=0;digest=hashlib.sha256();groups={}
    def check(ok,message):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(message)
    try:
        with tempfile.TemporaryDirectory(prefix='mysql-command-',dir=build) as scratch:
            folder=Path(scratch)/'node'
            def compare(q):
                nonlocal cases,effects
                q=copy.deepcopy(q);data=q.pop('fileBytes',None);expectRaw=q.pop('expectRaw',False);q['folder']=str(folder);values=[]
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    folder.mkdir()
                    if data is not None:(folder/'input.dat').write_bytes(bytes.fromhex(data))
                    value=normalize(runner.ask(q));before=value.pop('before');after=value.pop('after')
                    for event in value['events']:
                        if event[0]=='prepared.setLong' and event[2]==3:
                            check(before<=event[3]<=after,'insert timestamp within command window');event[3]='<current millis>'
                    def walk(v):
                        if isinstance(v,dict):
                            if 'time' in v:
                                match=re.fullmatch(r'0 Years 0 Months 0 Weeks 0 Days 0 Hours 0 Minutes (\d+) Seconds',v['time']);check(bool(match) and int(match[1])<=(after-before)//1000+1,'elapsed format/window');v['time']='<elapsed>'
                            if isinstance(v.get('stdout'),str):
                                plain=[]
                                for line in v['stdout'].splitlines(keepends=True):
                                    if line.startswith('Minima @ ') and '] : ' in line:
                                        message=line.split('] : ',1)[1].rstrip('\n')
                                        if not message.startswith('     '):value['logs'].append(message)
                                    else:plain.append(line)
                                v['stdout']=''.join(plain)
                            if isinstance(v.get('stderr'),str):v['stderr']='\n'.join(line for line in v['stderr'].split('\n') if not line.startswith('\tat '))
                            for child in v.values():walk(child)
                        elif isinstance(v,list):
                            for child in v:walk(child)
                    walk(value.get('value'))
                    files={}
                    for path in sorted(folder.rglob('*')):
                        if path.is_file() and path.name not in ['input.dat'] and not path.name.endswith(('.mv.db','.trace.db')):
                            content=path.read_bytes();files[str(path.relative_to(folder))]=content.hex()
                    value['files']=files
                    if expectRaw and 'output.dat' in files and 'exception' not in value:
                        raw=gzip.decompress(bytes.fromhex(files['output.dat']));check(raw[:1]==b'\x00','no cascade flag');check(len(raw)>3,'archive has record count')
                        width=raw[2];declared=int.from_bytes(raw[3:3+width],'big',signed=True);check(raw[1]==0,'count has zero scale')
                        maximum=q['params'].get('maxexport');limit=3 if maximum is None else ((int(Decimal(maximum))+2**31)%2**32)-2**31;expected=min(3,limit)
                        check(declared==expected,'declared export count matches Java-width cap')
                        blocks=[row['syncdata'] for batch in q['sql']['rowsBySQLBatches'][RANGE] for row in batch]
                        payload=b''.join(bytes.fromhex(b[2:]) for b in blocks[:max(1,expected)])
                        check(raw[3+width:]==payload,'exact exported records, including one record for nonpositive limit')
                    values.append(value)
                cases+=1;action=q['params'].get('action','info') if q['op']=='run' else q['op'];groups[str(action)]=groups.get(str(action),0)+1;effects+=len(values[0]['events']);digest.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).replace(str(folder),'<fixture>').encode())
                if values[0]!=values[1]:raise AssertionError(dict(case=cases,request=q,java=values[0],cpp=values[1]))
                if q['op']=='run' and q['params'].get('action')=='rawimport':check(not values[0]['importing'],'ordinary rawimport outcome clears flag')
                if q['op']=='run' and q['params'].get('action') in ['info','setlogin'] and isinstance(values[0].get('value'),dict):check(values[0]['value']['response']['password']=='***','reply masks password')
                if cases%100==0:print(json.dumps(dict(cases=cases,failures=0)),flush=True)
                return values[0]
            seed=compare(request(op='bootstrap'))
            for q in corpus(seed):compare(q)
            blocks=seed['chains']['normal'];t=tables(blocks)|{LAST:[dict(block=-1)]};raw=gzip.compress(bytes.fromhex(seed['raws']['normal'][2:]),mtime=0).hex()
            for action in ['info','size','wipe','setlogin','clearlogin','autobackup','integrity','update','findtxpow','addresscheck','rawexport','rawimport','fixmissing']:
                q=request(dict(action=action,enable='true',block='1',address='0x01',file='input.dat' if action in ['rawimport','fixmissing'] else 'output.dat'),sql=dict(rows=[],columns=[],rowsBySQL=t,rowsBySQLBatches={RANGE:batches(blocks),ARCHRANGE:batches(blocks)}))
                if action in ['rawimport','fixmissing']:q['fileBytes']=raw
                baseline=compare(q)
                for fail,runtime in itertools.product(range(1,len(baseline['events'])+1),[False,True]):
                    trial=copy.deepcopy(q);trial['sql'].update(fail=fail,runtimeFailure=runtime);compare(trial)
            demo=request(op='main');baseline=compare(demo)
            for fail,runtime in itertools.product(range(1,len(baseline['events'])+1),[False,True]):
                trial=copy.deepcopy(demo);trial['sql'].update(fail=fail,runtimeFailure=runtime);compare(trial)
            check(runners[1].errors()=='','native stderr empty')
    except Exception as error:failures.append(dict(cases=cases,error=repr(error)))
    finally:
        for name,r in zip(['java','cpp'],runners):
            try:r.close()
            except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,cases=cases,groups=groups,independent_assertions=checks,sql_events=effects,seconds=round(time.monotonic()-started,3),transcript_sha256=digest.hexdigest(),failures=failures,scope='Actual pinned mysql/native command and real archive/MySQL codecs using recording SQL; metadata, parsing, login precedence, preferences, finite paging, integrity order, raw files, imported blocks, missing-block repair, TxPoW lookup, SQL failures, static configuration and demo. Timestamps checked then normalized; NPE/classcast prose and demo stack frames normalized; demo logger output collected separately from plain stdout. Does not certify live H2/MySQL persistence, successful resync, GC/interruption timing, platform parity or complete node behavior.')
    (build/'mysql-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
