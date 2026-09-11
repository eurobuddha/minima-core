#!/usr/bin/env python3
"""Pinned MySQL JDBC and its native build against an isolated real MySQL server."""
import argparse,hashlib,json,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA
from native_h2_differential import op,query

def corpus(state):
    url='jdbc:mysql://'+state['address']+'/'+state['database']+'?autoReconnect=true'
    opening=op('open',save='c',url=url,user=state['user'],password=state['password'])
    prefix=[opening,op('statement',id='c',save='s'),op('execute',id='s',sql='DROP TABLE IF EXISTS NATIVE_TYPES')]
    cases=[]
    def add(steps,**marks):cases.append(dict(ops=steps,**marks))
    ddl='CREATE TABLE NATIVE_TYPES(ID BIGINT PRIMARY KEY, TXT LONGTEXT, BIN LONGBLOB, NUM DOUBLE)'
    add(prefix+[op('execute',id='s',sql=ddl),op('prepare',id='c',save='p',sql='INSERT INTO NATIVE_TYPES VALUES(?,?,?,?)'),
        op('bind',id='p',index=1,kind='long',value='9223372036854775807'),op('bind',id='p',index=2,kind='string',value='hello\x00π😄'),op('bind',id='p',index=3,kind='bytes',value='00FF1234'),op('bind',id='p',index=4,kind='double',value='3FF4000000000000'),op('update',id='p'),
        *query('SELECT * FROM NATIVE_TYPES'),op('long',id='r',column='ID'),op('string',id='r',column='TXT'),op('bytes',id='r',column='BIN'),op('object',id='r',index=4,save='d'),op('double',id='d'),op('text',id='d'),op('meta',id='r',save='m'),op('columns',id='m'),*[op(n,id='m',index=i) for i in range(1,5) for n in ['columnName','columnClass']],op('next',id='r'),op('closeStatement',id='s'),op('closed',id='c'),op('closeConnection',id='c'),op('closed',id='c')],marker='typed')
    for value in [None,'','a','π😄中文','a\x00b',"quote'\\backslash",'x'*8193]:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS S'),op('bind',id='p',index=1,kind='string',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('string',id='r',column='S')],marker='last',expected=None if value is None else value.encode('utf-16-be').hex().upper())
    for value in [None,'','00','00FF','AB'*8193]:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS B'),op('bind',id='p',index=1,kind='bytes',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('bytes',id='r',column='B')],marker='last',expected=value)
    for bits in ['0000000000000000','8000000000000000','0000000000000001','7FEFFFFFFFFFFFFF','7FF0000000000000','FFF0000000000000','7FF8000000000000','3FF0000000000001']:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS D'),op('bind',id='p',index=1,kind='double',value=bits),op('executePrepared',id='p'),op('result',id='p',save='r')])
    for value in [-2147483648,-1,0,1,2147483647]:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS N'),op('bind',id='p',index=1,kind='int',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('int',id='r',column='N'),op('long',id='r',column='N')],marker='last',expected=str(value))
    for value in ['-9223372036854775808','9223372036854775807']:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS N'),op('bind',id='p',index=1,kind='long',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('long',id='r',column='N')],marker='last',expected=value)
    for sql in ['SELECT NULL AS N','SELECT 9223372036854775807 AS N','SELECT -9223372036854775808 AS N',"SELECT '1.5' AS N","SELECT 'foo' AS N",'SELECT TRUE AS N']:
        add(prefix+query(sql)+[op('int',id='r',column='N'),op('long',id='r',column='N'),op('string',id='r',column='N'),op('string',id='r',column='missing')])
    for index in [-1,0,1,2,2147483647]:
        add(prefix+[op('prepare',id='c',save='p',sql='SELECT ? AS N'),op('bind',id='p',index=index,kind='string',value='4'),op('clear',id='p'),op('queryPrepared',id='p',save='r'),*query('SELECT 1 AS N'),op('object',id='r',index=index,save='o'),op('meta',id='r',save='m'),op('columnName',id='m',index=index),op('columnClass',id='m',index=index)])
    for sql in [None,'','SELECT','SELECT 1 AS N','CREATE TABLE NATIVE_TYPES(ID INT)','SELECT * FROM NATIVE_MISSING']:
        add(prefix+[op('execute',id='s',sql=sql),op('result',id='s',save='r'),op('query',id='s',save='r',sql=sql),op('closeStatement',id='s'),op('execute',id='s',sql=sql)])
    for ending in ['COMMIT','ROLLBACK']:
        add(prefix+[op('execute',id='s',sql='CREATE TABLE NATIVE_TYPES(ID INT)'),op('autocommit',id='c',value=False),op('execute',id='s',sql='INSERT INTO NATIVE_TYPES VALUES(1)'),op('execute',id='s',sql=ending),*query('SELECT COUNT(*) AS N FROM NATIVE_TYPES'),op('int',id='r',column='N')],marker='last',expected=1 if ending=='COMMIT' else 0)
    for user,password in [(state['user'],'wrong'),(None,None),('', ''),(state['user'],None)]:add([opening|dict(user=user,password=password)],marker='auth')
    add(prefix+[op('prepare',id='c',save='p',sql='SELECT 1'),op('execute',id='p',sql='SELECT 2'),op('query',id='p',save='r',sql='SELECT 2'),op('update',id='p')])
    return cases,opening

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');p.add_argument('--fixture',type=Path,required=True);p.add_argument('--profile',type=Path);p.add_argument('--graalvm',type=Path);args=p.parse_args();build=args.build.resolve();state=json.loads(args.fixture.read_text());start=time.monotonic()
    if args.profile and not args.graalvm:p.error('--profile requires --graalvm')
    assert state['address'].startswith('127.0.0.1:') and state['database']=='minima_cpp' and state['name'].startswith('minima-cpp-mysql-')
    oracle=build/'native-mysql-oracle';oracle.mkdir(exist_ok=True);sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NativeH2Oracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');dependencies=f'{JAVA}/lib/mysql-connector-java-8.0.24.jar:{JAVA}/jar/minima.jar';subprocess.run([str(args.graalvm/'bin/javac') if args.profile else 'javac','-cp',dependencies,'-d',str(oracle),'@'+str(listing)],check=True)
    command=['java','-cp',f'{oracle}:{dependencies}','NativeH2Oracle']
    if args.profile:
        assert args.graalvm;args.profile.mkdir(parents=True,exist_ok=True)
        filters=build/'mysql-profile-filter.json';filters.write_text(json.dumps(dict(rules=[dict(excludeClasses='org.minima.**'),dict(excludeClasses='NativeH2Oracle'),dict(excludeClasses='NativeH2Oracle$DB')])))
        command=[str(args.graalvm/'bin/java'),'-agentlib:native-image-agent=config-output-dir='+str(args.profile.resolve())+',access-filter-file='+str(filters),*command[1:]]
    runners=[Runner(command)];failures=[];programs=operations=checks=0;digest=hashlib.sha256()
    if not args.profile:runners.append(Runner([str(build/'native_h2_runtime')]))
    def check(ok,label):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(label)
    def validate(request,values):
        check(len(values)==len(request['ops']),'operation count')
        if request.get('marker')=='last':check(values[-1]==dict(value=request['expected']),'expected final value')
        if request.get('marker')=='typed':
            check(all('exception' not in v for v in values),'typed row all operations succeed')
            for expected in ['9223372036854775807','00FF1234','3FF4000000000000']:check(dict(value=expected) in values,'typed exact observation '+expected)
        if request.get('marker')=='auth':check(values[0]==dict(exception='java.sql.SQLNonTransientConnectionException: Could not create connection to database server. Attempted reconnect 3 times. Giving up.'),'bad credentials rejected with source reconnect wrapper')
    try:
        cases,opening=corpus(state)
        for index,request in enumerate(cases):
            values=[r.ask(request) for r in runners];programs+=1;operations+=len(request['ops'])
            for value in values:validate(request,value)
            if len(values)==2 and values[0]!=values[1]:raise AssertionError(dict(case=index,request=request,java=values[0],cpp=values[1]))
            digest.update(json.dumps([request,*values],sort_keys=True,separators=(',',':')).encode())
        for writer in range(len(runners)):
            setup=dict(ops=[opening,op('statement',id='c',save='s'),op('execute',id='s',sql='DROP TABLE IF EXISTS NATIVE_CROSS'),op('execute',id='s',sql='CREATE TABLE NATIVE_CROSS(ID BIGINT, TXT TEXT, BIN BLOB)'),op('execute',id='s',sql="INSERT INTO NATIVE_CROSS VALUES(9223372036854775807,'cross π😄',X'00FF42')")])
            check(all('exception' not in x for x in runners[writer].ask(setup)),'cross-writer setup')
            read=dict(ops=[opening,op('statement',id='c',save='s'),*query('SELECT * FROM NATIVE_CROSS'),op('long',id='r',column='ID'),op('string',id='r',column='TXT'),op('bytes',id='r',column='BIN')])
            for reader in runners:
                rows=reader.ask(read);check(rows[-3:]==[dict(value='9223372036854775807'),dict(value='cross π😄'.encode('utf-16-be').hex().upper()),dict(value='00FF42')],'cross-runtime durable row')
        if len(runners)>1:check(runners[1].errors()=='','native stderr empty')
    except Exception as error:failures.append(dict(programs=programs,error=repr(error)))
    finally:
        for r in runners:
            try:r.close()
            except Exception as error:failures.append(dict(cleanup=repr(error)))
    report=dict(passed=not failures,profile_only=bool(args.profile),programs=programs,operations=operations,independent_assertions=checks,seconds=round(time.monotonic()-start,3),transcript_sha256=digest.hexdigest(),failures=failures,mysql_image=state['image'],driver_sha256=hashlib.sha256((JAVA/'lib/mysql-connector-java-8.0.24.jar').read_bytes()).hexdigest(),scope='Real isolated MySQL server and pinned Connector/J8.0.24. Reuses unchanged NativeH2Oracle and native SQL fixture; exact values/errors and cross-writer durable rows, prepared bindings, null/empty/binary/Unicode/limits, metadata, transactions and authentication failures. Native handle registry must empty per request. Profile-only mode executes Java solely to collect native-image metadata. No complete node/MySQL command/TLS-platform/reflection-extension parity claim.')
    (build/('native-mysql-profile-results.json' if args.profile else 'native-mysql-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
