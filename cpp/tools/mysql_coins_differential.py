#!/usr/bin/env python3
"""Actual mysqlcoins and MySQLConnect against the shared recording SQL fixture."""
import argparse,copy,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

RANGE='SELECT syncdata FROM syncblock WHERE block>=? ORDER BY block ASC LIMIT 250'
FIRST='SELECT block FROM syncblock ORDER BY block ASC LIMIT 1'
LATEST='SELECT MAX(blockcreated) as maxblock from coins'
LOGIN=dict(host='fixture:3306',database='fixture',user='student',password='synthetic')
def request(params=None,**kw):
    raw=kw.pop('raw_params',None)
    return dict(op='run',params=raw if raw is not None else LOGIN|(params or {}),prefs={},mobile=False,sql=dict(rows=[],columns=[]))|kw

def corpus(blocks):
    cases=[]
    def add(params=None,**kw):cases.append(request(params,**kw))
    add(op='meta');add(params={'ignored':'old'},op='meta')
    for mobile in [True,False]:
        for params in [{},{'action':None},{'action':True},{'action':'bogus'}]:add(mobile=mobile,raw_params=params)
    for stop in range(5):add(raw_params=dict(list(LOGIN.items())[:stop]))
    for field,value in itertools.product(['action','host','database','user','password','logs','readonly','maxblocks','maxcoins'],[None,True,False,0,{},[], '', '  x  ']):
        add({field:value})
    prefs=dict(mysql_autologindetails=True,mysql_host='saved:3306',mysql_db='saved-db',mysql_user='saved-user',mysql_password='saved-secret',mysqlcoins_autobackup=True)
    for action in ['info','wipe','autobackup','search','update','bogus']:
        for read in ['true','false']:
            p=dict(action=action,readonly=read,enable='true',where='coinid=1',maxblocks='0');add(p);add(p,prefs=prefs)
    for enable in ['true','false','TRUE','',None,True]:add(dict(action='autobackup',enable=enable),prefs=prefs)
    for action in ['info','wipe','search','update']:
        for caller in ['0x00','0x1234']:
            text='mysqlcoins '+' '.join(k+':'+v for k,v in LOGIN.items())+' action:'+action+' where:1=1 maxblocks:0'
            add(op='runner',caller=caller,text=text);add(op='runner',caller=caller,text=text+' bad:true')
    for row in [dict(tot=5,maxblock=123),dict(tot=-1,maxblock=-1),dict(tot=0,maxblock=None)]:add(sql=dict(rows=[row],columns=[]))
    columns=[dict(name='address',**{'class':'java.lang.String'}),dict(name='token',**{'class':'java.lang.String'})]
    searchsql=dict(rows=[['0x01','large token'],[None,None]],columns=columns,results=True)
    for params in [dict(where='amountdouble > 3'),dict(address='0x001122'),dict(address='0x01',spent='true',limit='2'),dict(address='0x01',spent='false',limit='-2'),dict(query='SELECT * FROM coins'),dict(where='x',address='bad',query='bad',spent='bad',limit='bad'),dict(address='0x01',query='ignored'),dict(query='DELETE FROM coins'),dict()]:
        for hide in ['true','false']:add(dict(action='search',hidetoken=hide)|params,sql=searchsql)
    for field,value in itertools.product(['where','address','query','spent','limit','hidetoken'],[None,True,0,{},[], '', 'bad', '4294967296']):
        params=dict(action='search',address='0x01',query='SELECT fixture')
        if field in ['where','query']:params.pop('address')
        params[field]=value;add(params)
    tables={FIRST:[dict(block=10)],LATEST:[dict(maxblock=-1)],RANGE:[dict(syncdata=b) for b in blocks]}
    for maxblocks,maxcoins,logs in itertools.product(['-1','0','1','2','3','4294967296'],['-1','0','2','3','6','100'],['true','false']):
        add(dict(action='update',maxblocks=maxblocks,maxcoins=maxcoins,logs=logs),sql=dict(rows=[],columns=[],rowsBySQL=tables))
    for last in [-1,0,10,9223372036854775807]:
        t=copy.deepcopy(tables);t[LATEST]=[dict(maxblock=last)];add(dict(action='update',maxblocks='2'),sql=dict(rows=[],columns=[],rowsBySQL=t))
    for rows in [[],[dict(syncdata='0x')],[dict(syncdata=None)]]:
        t=copy.deepcopy(tables);t[RANGE]=rows;add(dict(action='update',maxblocks='1'),sql=dict(rows=[],columns=[],rowsBySQL=t))
    for action in ['info','wipe','autobackup','search','update','bogus']:
        p=dict(action=action,enable='false',where='1=1',maxblocks='1')
        for flag in ['nullConnection','nullStatement','nullPrepared','nullRows']:add(p,sql=dict(rows=[],columns=[],**{flag:True}))
    return cases,tables,searchsql

def project(out):
    out=normalize(out);before=out.pop('before');after=out.pop('after')
    def duration(value):
        if isinstance(value,dict):
            if 'duration' in value and value.get('message')=='MySQL CoinsDB Updated..':
                seconds=int(value['duration'].removesuffix(' seconds'));assert 0<=seconds<=(after-before)//1000
                value['duration']='<elapsed seconds>'
            for child in value.values():duration(child)
        elif isinstance(value,list):
            for child in value:duration(child)
    duration(out.get('value'));return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();start=time.monotonic();os.environ['TZ']='UTC'
    oracle=build/'mysql-coins-oracle';oracle.mkdir(exist_ok=True)
    names=['SqlDBOracle.java','MySQLCoinsOracle.java','ArchiveManagerOracle.java','NIOClientOracle.java','ObjectOracle.java']
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in names];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MySQLCoinsOracle']),Runner([str(build/'minima_mysql_coins')])];failures=[];cases=checks=effects=0;digest=hashlib.sha256()
    def check(ok,text):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(text)
    def compare(q):
        nonlocal cases,effects
        values=[project(r.ask(q)) for r in runners];cases+=1;effects+=len(values[0]['events']);digest.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:raise AssertionError(dict(case=cases,request=q,java=values[0],cpp=values[1]))
        for value in values:
            if q['mobile'] and q['op']=='run':check(value.get('exception')=='org.minima.system.commands.CommandException: Sorry - MySQL does not work on Android..' and value['events']==[],'mobile rejected before connection')
            reply=value.get('value');params=q['params'];action=params.get('action','info')
            if q['op']=='run' and isinstance(reply,dict):
                if action=='wipe':check(reply.get('respnse')=='CoinsDB wiped' and 'response' not in reply,'pinned misspelling preserved')
                if action=='update':
                    response=reply['response'];insertions=[e for e in value['events'] if e[0]=='prepared.setString' and e[1]==10 and e[2]==1]
                    if not q['sql'].get('fail'):check(response['coinsadded']==len(insertions),'all coin insert attempts counted')
                    check(response['blocks']>=0,'nonnegative processed block count')
                if action=='autobackup':check('mysql_autologindetails' not in value['prefs'] or value['prefs']['mysql_autologindetails']==q['prefs'].get('mysql_autologindetails'),'auto-login flag unchanged')
                check(any(e[0]=='connection.close' for e in value['events']) or bool(value['logs']),'normal return reaches shutdown or its logged SQL failure')
        return values[0]
    try:
        initial=compare(request(op='bootstrap'));corpus_cases,tables,searchsql=corpus(initial['blocks'])
        for q in corpus_cases:compare(q)
        # Inject at every SQL event observed in each complete action path.
        for action in ['info','wipe','autobackup','search','update']:
            q=request(dict(action=action,enable='true',where='1=1',maxblocks='1'),sql=searchsql if action=='search' else dict(rows=[],columns=[],rowsBySQL=tables))
            baseline=compare(q)
            for fail,runtime in itertools.product(range(1,len(baseline['events'])+1),[False,True]):
                trial=copy.deepcopy(q);trial['sql'].update(fail=fail,runtimeFailure=runtime);compare(trial)
        check(runners[1].errors()=='','native stderr empty')
    except Exception as error:failures.append(dict(cases=cases,error=repr(error)))
    finally:
        for name,r in zip(['java','cpp'],runners):
            try:r.close()
            except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,cases=cases,independent_assertions=checks,sql_events=effects,seconds=round(time.monotonic()-start,3),transcript_sha256=digest.hexdigest(),failures=failures,scope='Actual pinned mysqlcoins and native counterpart with real MySQLConnect bodies/codecs and shared recording SQL: all action paths, metadata, parsing and error order, preferences, exact SQL/bindings, per-event SQL/runtime failures, bounded synthetic block ranges. Duration checked against operation window before normalization; NPE/class-cast prose normalized. No live MySQL driver, durable paging, concurrent archive updates, >5s progress-log timing or full node parity certification.')
    (build/'mysql-coins-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:1]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
