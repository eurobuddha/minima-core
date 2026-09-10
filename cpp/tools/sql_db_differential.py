#!/usr/bin/env python3
"""Exercise actual pinned SqlDB against a recording JDBC driver, not an H2 substitute."""
import argparse,itertools,json,random,re,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def normalize(value):
    if isinstance(value,list):return [normalize(v) for v in value]
    if isinstance(value,dict):return {k:normalize(v) for k,v in value.items()}
    if isinstance(value,str):
        # Enhanced NPE and class-cast diagnostics name Java expressions/classes.
        value=re.sub(r'java.lang.(NullPointerException|ClassCastException):.*',r'java.lang.\1',value)
    return value

def cases(root):
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    def op(cmd,**kw):return dict(cmd=cmd,**kw)
    for debug,encrypted,password in itertools.product([False,True],[False,True],[None,'','fixture-password','π 😄']):
        load=op('encrypted' if encrypted else 'load',password=password,debug=debug)
        add(op('open'),load,op('open'),op('check'),op('save',compact=True),op('open'),op('check',logs=False),op('hard'),op('check'),op('clear'),op('check'))
    # Encryption is intentionally sticky after a subsequent plain load.
    add(op('encrypted',password='fixture-only'),op('load',file=root+'/second'),op('hard'),op('check'))
    for cmd in ['open','check','hard','save','backup','restore','sql']:
        add(op(cmd,sql='select 1'))
    for cmd,file in itertools.product(['load','encrypted','backup','restore'],[None,'bare-file',root+"/nested/a'b"]):
        add(op(cmd,file=file),op('check'),op('open'))
    for file in ['', '/', '///', 'bare-file/', root+'//nested///file/', root+'/nested/../main', root+'/nul\0file', root+'/nul\0dir/file']:
        add(op('load',file=file),op('check'),op('hard'),op('check'))
    for file in [root+'//nested///file/', root+'/nul\0file']:
        add(op('load'),op('backup',file=file),op('restore',file=file))
    for cmd,gzip,fail in itertools.product(['backup','restore'],[False,True],range(7)):
        add(op('load'),op(cmd,file=root+"/backup/quote'π.sql",gzip=gzip,touch=True,fail=fail),op('open'))
    for cmd,fail in itertools.product(['load','encrypted','check','hard','save'],range(1,8)):
        prefix=[op('load')]
        if cmd=='check':prefix+=[op('hard')]
        add(*prefix,op(cmd,password='fixture',fail=fail,debug=True,compact=True),op('open'),op('check'))
    for cmd,flag in itertools.product(['load','sql','save','backup','restore'],['nullConnection','nullStatement','nullRows','nullMeta']):
        add(op('load'),op(cmd,sql='select fixture',results=True,columns=[],rows=[],**{flag:True}),op('open'))
    for sql in [None,'','select 1','DROP TABLE fixture','π😄;\n\t',"select 'x'"]:
        add(op('load'),op('sql',sql=sql),op('sql',sql=sql,results=True,columns=[],rows=[[],[]]))
    columns=[dict(name='A',**{'class':'java.lang.String'}),dict(name='B',**{'class':'java.sql.Clob'}),dict(name='A',**{'class':'java.lang.Integer'}),dict(name='π😄',**{'class':'java.lang.String'})]
    rows=[['one',dict(clob='large π😄\nvalue',text='clob-object'),None,''],[None,None,'123','two']]
    for fail in range(46):add(op('load'),op('sql',sql='select fixture',results=True,columns=columns,rows=rows,fail=fail),op('sql',sql='update fixture'))
    for length in [0,-1,2147483647,2147483648,4294967295,4294967296,9223372036854775807]:
        add(op('load'),op('sql',sql='clob',results=True,columns=[columns[1]],rows=[[dict(clob='sample',length=length,text='object')]]))
    for cls,cell in itertools.product([None,'java.sql.Clob','org.h2.jdbc.JdbcClob','java.lang.String'],[None,'text',dict(clob='π😄',text='CLOB-fixture')]):
        add(op('load'),op('sql',sql='types',results=True,columns=[dict(name='C',**{'class':cls})],rows=[[cell]]))
    rng=random.Random(5564)
    for _ in range(250):
        count=rng.randrange(6);cols=[dict(name=rng.choice(['A','B','','π😄']),**{'class':rng.choice(['java.lang.String','java.lang.Integer','java.sql.Clob'])}) for _ in range(count)]
        table=[]
        for _ in range(rng.randrange(6)):
            table.append([rng.choice([None,dict(clob=rng.choice(['','text','π😄']),text='object')]) if c['class']=='java.sql.Clob' else rng.choice([None,'','0','false','π😄','\n\t\\"']) for c in cols])
        add(op('load'),op('sql',sql='random fixture',results=True,columns=cols,rows=table,fail=rng.randrange(60)))
    for data in [None,{},dict(a=1,b=True,c=None),dict(text='π😄\n',list=[1,False,'x']),[],1,'text']:
        wire=json.dumps(data,ensure_ascii=False,separators=(',',':')).encode()
        add(op('decode',hex='0x'+wire.hex()))
        if data is None or isinstance(data,dict):add(op('encode',json=data))
    # MiniString's malformed UTF-8 handling precedes parsing.
    for raw in [b'{"x":"\xff"}',b'{"x":"\xe2\x82"}',b'{"x":"\xed\xa0\x80"}']:
        add(op('decode',hex='0x'+raw.hex()))
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'sql-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java'];listing=build/'sql-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=0;event_count=0
    with tempfile.TemporaryDirectory(prefix='minima-sqldb-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SqlDBOracle',root]),Runner([str(build/'minima_sql_db'),root])]
        corpus=cases(root)
        try:
            for index,request in enumerate(corpus):
                outputs=[normalize(runner.ask(request)) for runner in runners]
                if outputs[0]!=outputs[1] or 'ops' not in outputs[0]:
                    failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);event_count+=sum(len(op['events']) for op in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,driver_calls=event_count,failures=failures,scope='Pinned SqlDB lifecycle, sticky encrypted reopen URL/credentials, failure and partial-state order, shutdown, backup/restore SQL and file deletion, generic SQL metadata/row/null/CLOB handling, failure-result reset, statement close, JSON/MiniData conversion. Exact recording driver events and logger bodies except Java-specific NPE/class-cast messages and stack/date/memory prefixes. JDBC driver/storage/encryption execution, prepared statements, full logger notifications, and full node integration remain absent/unverified.')
    (build/'sql-db-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
