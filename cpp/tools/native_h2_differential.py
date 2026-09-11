#!/usr/bin/env python3
"""Actual H2 JDBC vs native H2, with durable files shared across runtimes."""
import argparse,itertools,json,random,shutil,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def op(name,**kw):return dict(op=name,**kw)
def open_db(path,password=None):
    return op('open',save='c',url='jdbc:h2:'+str(path)+';MODE=MySQL;DB_CLOSE_ON_EXIT=FALSE'+('' if password is None else ';CIPHER=AES'),user='SA',password='' if password is None else password+' userpasswd')
def query(sql):return [op('query',id='s',save='r',sql=sql),op('next',id='r')]
def corpus(folder):
    result=[]
    def add(*steps,**marks):result.append(dict(ops=list(steps),**marks))
    prefix=[op('version'),open_db(folder/'main'),op('autocommit',id='c',value=True),op('statement',id='c',save='s'),op('preparedType',id='s')]
    ddl='CREATE TABLE T(ID BIGINT PRIMARY KEY, TXT VARCHAR, BIN BLOB, NUM DOUBLE, BODY CLOB)'
    add(*prefix,op('execute',id='s',sql=ddl),op('prepare',id='c',save='p',sql='INSERT INTO T VALUES(?,?,?,?,?)'),op('preparedType',id='p'),
        op('bind',id='p',index=1,kind='long',value='9223372036854775807'),op('bind',id='p',index=2,kind='string',value='hello\x00π😄'),
        op('bind',id='p',index=3,kind='bytes',value='00FF1234'),op('bind',id='p',index=4,kind='double',value='3FF4000000000000'),op('bind',id='p',index=5,kind='string',value='clob😄text'),
        op('update',id='p'),*query('SELECT * FROM T'),op('long',id='r',column='ID'),op('string',id='r',column='TXT'),op('bytes',id='r',column='BIN'),
        op('object',id='r',index=4,save='d'),op('double',id='d'),op('text',id='d'),op('object',id='r',index=5,save='clob'),op('clobLength',id='clob'),op('clobString',id='clob',start='1',length=20),
        op('meta',id='r',save='m'),op('columns',id='m'),*[op(name,id='m',index=i) for i in range(1,6) for name in ['columnName','columnClass']],
        op('next',id='r'),op('closeStatement',id='s'),op('next',id='r'),op('closed',id='c'),op('closeConnection',id='c'),op('closed',id='c'),marker='typed_row')
    for value in [None,'','a','π😄中文','a\x00b','x'*8193]:
        add(*prefix,op('prepare',id='c',save='p',sql='SELECT CAST(? AS VARCHAR) AS S'),op('bind',id='p',index=1,kind='string',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('string',id='r',column='S'),op('object',id='r',index=1,save='o'),*([op('text',id='o')] if value is not None else []),marker='string',expected=value)
    for value in [None,'','00','00FF','0123456789ABCDEF','AB'*8193]:
        add(*prefix,op('prepare',id='c',save='p',sql='SELECT CAST(? AS BINARY VARYING) AS B'),op('bind',id='p',index=1,kind='bytes',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('bytes',id='r',column='B'),marker='bytes',expected=value)
    for bits in ['0000000000000000','8000000000000000','0000000000000001','7FEFFFFFFFFFFFFF','7FF0000000000000','FFF0000000000000','7FF8000000000000','3FF0000000000001','BFEFFFFFFFFFFFFF']:
        add(*prefix,op('prepare',id='c',save='p',sql='SELECT CAST(? AS DOUBLE) AS D'),op('bind',id='p',index=1,kind='double',value=bits),op('executePrepared',id='p'),op('result',id='p',save='r'),op('next',id='r'),op('object',id='r',index=1,save='d'),op('double',id='d'),op('text',id='d'))
    for value in [-2147483648,-1,0,1,2147483647]:
        add(*prefix,op('prepare',id='c',save='p',sql='SELECT CAST(? AS INTEGER) AS N'),op('bind',id='p',index=1,kind='int',value=value),op('queryPrepared',id='p',save='r'),op('next',id='r'),op('int',id='r',column='N'),op('long',id='r',column='N'))
    for sql in ['SELECT NULL AS N','SELECT 9223372036854775807 AS N','SELECT -9223372036854775808 AS N',"SELECT '1.5' AS N","SELECT 'foo' AS N",'SELECT TRUE AS N']:
        add(*prefix,*query(sql),op('int',id='r',column='N'),op('long',id='r',column='N'),op('string',id='r',column='N'),op('string',id='r',column='missing'))
    for index in [-1,0,1,2,2147483647]:
        add(*prefix,op('prepare',id='c',save='p',sql='SELECT ? AS N'),op('bind',id='p',index=index,kind='string',value='4'),op('clear',id='p'),op('queryPrepared',id='p',save='r'),
            *query('SELECT 1 AS N'),op('object',id='r',index=index,save='o'),op('meta',id='r',save='m'),op('columnName',id='m',index=index),op('columnClass',id='m',index=index))
    for sql in [None,'','SELECT','SELECT 1 AS N','CREATE TABLE X(ID INT)','SELECT * FROM MISSING','DROP ALL OBJECTS']:
        add(*prefix,op('execute',id='s',sql=sql),op('result',id='s',save='r'),op('query',id='s',save='r',sql=sql),op('closeStatement',id='s'),op('execute',id='s',sql=sql))
    add(*prefix,op('prepare',id='c',save='p',sql='SELECT 1'),op('execute',id='p',sql='SELECT 2'),op('query',id='p',save='r',sql='SELECT 2'),op('update',id='p'))
    add(*prefix,*query('SELECT 1 AS N UNION ALL SELECT 2'),op('releaseRow',id='r'),op('result',id='s',save='r2'),op('int',id='r2',column='N'),op('next',id='r2'),op('int',id='r2',column='N'),marker='retained_result')
    for start,length in itertools.product(['-1','0','1','2','7','8','9223372036854775807'],[-1,0,1,5,99]):
        add(*prefix,*query("SELECT CAST('abc😄z' AS CLOB) AS C"),op('object',id='r',index=1,save='clob'),op('clobLength',id='clob'),op('clobString',id='clob',start=start,length=length))
    for ending in ['COMMIT','ROLLBACK']:
        add(*prefix,op('execute',id='s',sql='CREATE TABLE T(ID INT)'),op('autocommit',id='c',value=False),op('execute',id='s',sql='INSERT INTO T VALUES(1)'),op('execute',id='s',sql=ending),*query('SELECT COUNT(*) AS N FROM T'),op('int',id='r',column='N'),marker='transaction',expected=1 if ending=='COMMIT' else 0)
    for encrypted,compact,gzip in itertools.product([False,True],repeat=3):
        load=op('dbEncrypted' if encrypted else 'dbLoad',file=str(folder/'db'),password='fixture password')
        backup=str(folder/('backup.sql.gz' if gzip else 'backup.sql'))
        add(load,op('dbOpen'),op('dbSQL',sql='CREATE TABLE T(ID INT PRIMARY KEY, TXT VARCHAR, B VARBINARY)'),op('dbSQL',sql="INSERT INTO T VALUES(7,'π😄',X'00FF')"),
            op('dbBackup',file=backup,gzip=gzip),op('dbSQL',sql='DELETE FROM T'),op('dbRestore',file=backup,gzip=gzip),op('dbSQL',sql='SELECT ID, TXT FROM T'),op('dbSave',compact=compact),op('dbOpen'),op('dbCheck'),op('dbSQL',sql='SELECT ID, TXT FROM T'),op('dbClose'),marker='backup',encrypted=encrypted)
    rng=random.Random(5564)
    for _ in range(30):
        values=[rng.randrange(-2147483648,2147483648) for _ in range(rng.randrange(1,20))]
        steps=prefix+[op('execute',id='s',sql='CREATE TABLE T(ID INT)'),op('prepare',id='c',save='p',sql='INSERT INTO T VALUES(?)')]
        for value in values:steps += [op('bind',id='p',index=1,kind='int',value=value),op('update',id='p')]
        add(*steps,*query('SELECT SUM(CAST(ID AS BIGINT)) AS N FROM T'),op('long',id='r',column='N'),marker='sum',expected=str(sum(values)))
    return result

def validate(request,rows):
    assert len(rows)==len(request['ops'])
    marker=request.get('marker')
    if marker in ['sum','transaction','bytes']:assert rows[-1]==dict(value=request['expected']),rows[-1]
    if marker=='retained_result':assert rows[-3]==dict(value=1) and rows[-1]==dict(value=2),rows
    if marker=='string':
        value=request['expected'];expected=None if value is None else value.encode('utf-16-be').hex().upper()
        assert rows[9]==dict(value=expected),rows
    if marker=='typed_row':
        for i in [9,10,11,12,13,14]:assert 'exception' not in rows[i],rows[i]
        assert any(row==dict(value='9223372036854775807') for row in rows),rows
        assert any(row==dict(value='00FF1234') for row in rows),rows
        assert any(row==dict(value='3FF4000000000000') for row in rows),rows
    if marker=='backup':
        for i,row in enumerate(rows):assert 'exception' not in row,(i,row)
        assert rows[7]['value']['rows']==[dict(ID='7',TXT='π😄')],rows
        assert rows[9]==dict(value=False) and rows[10]==dict(value=True),rows
        assert rows[11]['value']['rows']==rows[7]['value']['rows'],rows

def cross_files(runners,folder):
    checks=0
    for encrypted,writer in itertools.product([False,True],[0,1]):
        if folder.exists():shutil.rmtree(folder)
        folder.mkdir();path=folder/'cross';password='fixture password' if encrypted else None
        setup=dict(ops=[open_db(path,password),op('statement',id='c',save='s'),op('execute',id='s',sql='CREATE TABLE T(ID BIGINT, TXT VARCHAR, B BLOB)'),op('execute',id='s',sql="INSERT INTO T VALUES(9223372036854775807,'cross π😄',X'00FF42')")])
        rows=runners[writer].ask(setup);assert all('exception' not in row for row in rows),rows
        assert (folder/'cross.mv.db').stat().st_size>0
        read=dict(ops=[open_db(path,password),op('statement',id='c',save='s'),*query('SELECT * FROM T'),op('long',id='r',column='ID'),op('string',id='r',column='TXT'),op('bytes',id='r',column='B')])
        outputs=[runners[i].ask(read) for i in [1-writer,writer]];assert outputs[0]==outputs[1],outputs
        assert outputs[0][-3:]==[dict(value='9223372036854775807'),dict(value='cross π😄'.encode('utf-16-be').hex().upper()),dict(value='00FF42')],outputs
        if encrypted:
            bad=dict(ops=[open_db(path,'wrong password')]);out=[r.ask(bad) for r in runners];assert out[0]==out[1] and 'encryption error' in out[0][0]['exception'].lower(),out
        checks+=1
    return checks

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'native-h2-oracle';oracle.mkdir(exist_ok=True);sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NativeH2Oracle.java'];listing=build/'native-h2-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NativeH2Oracle']),Runner([str(build/'native_h2_runtime')])]
    failures=[];programs=operations=cross=0
    try:
        with tempfile.TemporaryDirectory(prefix='native-h2-fixtures-',dir=build) as scratch:
            folder=Path(scratch)/'db';cases=corpus(folder)
            for index,request in enumerate(cases):
                values=[]
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    folder.mkdir();value=runner.ask(request);validate(request,value);values.append(value)
                if values[0]!=values[1]:failures.append(dict(case=index,request=request,java=values[0],cpp=values[1]));break
                programs+=1;operations+=len(request['ops'])
            if not failures:cross=cross_files(runners,folder)
    except Exception as error:failures.append(dict(error=repr(error),case=programs))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,programs=programs,operations=operations,cross_runtime_files=cross,failures=failures,scope='Real H2 2.4.240 JDBC and native H2 shared library through the C++ SqlConnection API; actual SqlDB backup/restore/save/reopen, prepared bindings, schema/transaction failures, conversions, CLOBs, metadata and retained result views. JVM and native runtimes cross-open plaintext and AES MVStore files in both directions. No exception normalization. Native handle registry must be empty after every program. Full node, MySQL and arbitrary extension/runtime/reflection/resource-failure parity remain unverified.')
    (build/'native-h2-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
