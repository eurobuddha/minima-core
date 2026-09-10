#!/usr/bin/env python3
"""Actual MySQLConnect bodies through shared recording SQL and real codecs."""
import argparse,copy,itertools,json,random,struct,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[])|kw
def request(*ops,**kw):return dict(locale='en_US',ops=list(ops))|kw

def project(raw,req):
 value=normalize(raw)
 for before,after,step in zip(raw['ops'],value['ops'],req['ops']):
  for event in after['events']:
   if step['cmd']=='blockSave' and event[0]=='prepared.setLong' and event[2]==3:
    assert before['before']<=event[3]<=before['after'],event
    event[3]='<current-time>'
  after.pop('before');after.pop('after')
 return value

def corpus(fixtures):
 cases=[];blocks=fixtures['blocks'];cascades=fixtures['cascades'];pows=fixtures['pows']
 rows=[dict(syncdata=blocks[1],fulldata=cascades[1],txpowdata=pows[1],block=1,tot=3,maxblock=17),dict(syncdata=blocks[0],fulldata=cascades[0],txpowdata=pows[0],block=0,tot=-1,maxblock=0)]
 def add(*ops,**kw):cases.append(request(*ops,**kw))
 def cmd(name,**kw):return op(name,index=1,start='0',id='0x01',number=1,spentAt=25,date='fixture date',rows=rows,**kw)
 names=['count','first','last','maxCoin','totalCoins','cascadeSave','cascadeLoad','blockSave','blockID','blockNum','range','txpowSave','txpowGet','coin','sql','shutdown']
 add(op('init'),*(cmd(name) for name in names))
 for name in names:add(cmd(name))
 for readOnly in [False,True]:
  add(op('debug',enabled=True),op('init'),op('shutdown'),op('init'),readOnly=readOnly)
  for field,value in itertools.product(['host','database','user','password'],[None,'','π 🐼']):add(op('init'),**{field:value,'readOnly':readOnly})
  for phase,fail,runtime in itertools.product(['init','wipe','wipeCoins'],range(1,25),[False,True]):
   add(op('init'),op(phase,fail=fail,runtimeFailure=runtime),*(cmd(name) for name in ['count','first','last','cascadeLoad','txpowGet']),readOnly=readOnly)
 for name,fail,runtime in itertools.product(names,range(1,24),[False,True]):add(op('init'),cmd(name,fail=fail,runtimeFailure=runtime),cmd('count'))
 for flag in ['nullConnection','nullStatement','nullPrepared']:
  for name in names:add(op('init',**{flag:True}),cmd(name),op('shutdown'))
 for name in ['count','first','last','maxCoin','totalCoins','cascadeLoad','blockID','blockNum','range','txpowGet']:
  for table in [[],[{}],[dict(tot=2147483648,block=-1,maxblock=None)],rows,rows[::-1]]:
   add(op('init'),op(name,index=1,start='0',id='0x01',rows=table))
  add(op('init'),cmd(name,nullRows=True))
 for name in ['cascadeSave','blockSave','txpowSave','coin']:
  add(op('init'),cmd(name,**{'null':True}));add(op('init'),cmd(name,badHash=True))
 add(op('init'),op('cascadeSave',index=0))
 for start in [None,'0','-1','1.5','-1.5','9223372036854775808','18446744073709551615','1e40']:
  add(op('init'),op('range',start=start,rows=rows))
 for name,column,wires in [('blockID','syncdata',blocks),('range','syncdata',blocks),('cascadeLoad','fulldata',cascades),('txpowGet','txpowdata',pows)]:
  for wire in wires:
   for end in sorted(set([2,4,6,8,10,20,len(wire)-4,len(wire)-2,len(wire),len(wire)+2])):
    add(op('init'),op(name,id='0x01',start='0',rows=[{column:(wire+'FF')[:end]}]))
  add(op('init'),op(name,id=None,start='0',rows=[{column:wires[0]},{column:'0x'}]))
 for index in [0,1,2,3,4,20]:
  for entry,scale in [('0',0),('-12345',3),('12345',3),('18446744073709551615',0),('1',-64),('123',2147483647),('12',-2147483648)]:
   add(op('init'),op('coin',index=index,entry=entry,scale=scale,spentAt=-1,date=None))
  for flag in ['noToken','nullState']:add(op('init'),op('coin',index=index,**{flag:True}))
 def sql(**kw):return op('sql',sql='select fixture',results=True,**kw)
 for hide in [False,True]:
  for name,typ,cell in itertools.product(['token','TOKEN','π 🐼'],['java.lang.String','java.sql.Clob','java.lang.Double',None],[None,'ordinary',dict(clob='large text',text='clob identity'),dict(doubleBits='3ff0000000000000')]):
   add(op('init'),sql(hideToken=hide,columns=[dict(name=name,**{'class':typ})],rows=[[cell]]))
 table=[['one',dict(clob='π 🐼',text='clob identity'),None],[None,None,'last']];columns=[dict(name=name,**{'class':typ}) for name,typ in [('a','java.lang.String'),('token','java.sql.Clob'),('b','java.lang.String')]]
 for hide,fail,runtime in itertools.product([False,True],range(1,33),[False,True]):add(op('init'),sql(hideToken=hide,columns=columns,rows=table,fail=fail,runtimeFailure=runtime))
 for flag in ['nullStatement','nullRows','nullMeta']:add(op('init'),sql(**{flag:True}))
 for text in [None,'','update fixture']:add(op('init'),op('sql',sql=text))
 for length in [-1,0,2147483647,2147483648,4294967296]:add(op('init'),sql(columns=[dict(name='value',**{'class':'java.sql.Clob'})],rows=[[dict(clob='fixed text',text='clob identity',length=length)]]))
 numbers=[0.,-0.,1.,-1.,1.2345675,1.2345685,0.0000005,-0.0000005,9.9999995,1e23,1e100,1.7976931348623157e308,5e-324,float('nan'),float('inf'),float('-inf')]
 rng=random.Random(5564);numbers += [struct.unpack('!d',rng.randbytes(8))[0] for _ in range(300)]
 for locale in ['en_US','en_GB','de_DE','fr_FR','ar_EG','fa_IR','bn_BD']:
  table=[[dict(doubleBits=struct.pack('!d',v).hex())] for v in numbers]
  add(op('init'),sql(columns=[dict(name='value',**{'class':'java.lang.Double'})],rows=table),locale=locale)
 return cases

def validate(req,out):
 for step,result in zip(req['ops'],out['ops']):
  if step['cmd']=='init' and 'exception' not in result:
   effects=result['events'];queries=[x[2] for x in effects if x[0]=='connection.prepareStatement'];assert len(queries)==14
   assert queries[3]=='SELECT syncdata FROM syncblock WHERE block>=? ORDER BY block ASC LIMIT 250'
   assert queries[4].endswith('ORDER BY block ASC LIMIT 1') and queries[5].endswith('ORDER BY block DESC LIMIT 1')
   assert sum(x[0]=='statement.execute' for x in effects)==(0 if req.get('readOnly') else 4)
  if step['cmd']=='coin' and 'exception' not in result and not result['logs']:
   bindings=[x for x in result['events'] if x[0].startswith('prepared.set')];assert [x[2] for x in bindings]==list(range(1,15))
   assert bindings[3][0]==bindings[13][0]=='prepared.setDouble'

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'mysql-connect-oracle';oracle.mkdir(exist_ok=True)
 sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['SqlDBOracle.java','MySQLConnectOracle.java','ArchiveManagerOracle.java','NIOClientOracle.java','ObjectOracle.java']]
 listing=build/'mysql-connect-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
 runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MySQLConnectOracle']),Runner([str(build/'minima_mysql_connect')])];failures=[];operations=effects=0
 try:
  bootstrap=request();initial=[r.ask(bootstrap) for r in runners];assert initial[0]==initial[1],initial
  cases=corpus(initial[0])
  for index,req in enumerate(cases):
   outputs=[project(r.ask(req),req) for r in runners]
   if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=req,java=outputs[0],cpp=outputs[1]));break
   validate(req,outputs[0]);operations+=len(req['ops']);effects+=sum(len(o['events']) for o in outputs[0]['ops'])
 finally:
  for r in runners:r.close()
 report=dict(passed=not failures,programs=index+1,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual MySQLConnect bodies and codecs, exact SQL effects and normalized runtime diagnostics. Clock bindings checked against real operation windows before labeling. Shared recording SQL is not a live MySQL driver, durable database or complete Main/node integration; demo main and exhaustive concurrency/platform parity remain open.')
 (build/'mysql-connect-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:1]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
