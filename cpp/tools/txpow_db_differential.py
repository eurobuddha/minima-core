#!/usr/bin/env python3
"""Pinned owning TxPoWDB + actual RAM/SQL/searcher components, recorded JDBC/owners."""
import argparse,itertools,json,random,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
MASK=(1<<64)-1
FIXTURES=[dict(index=0),dict(index=1,transaction=True),dict(index=2,transaction=True),dict(index=1,transaction=True),dict(index=2,mode=1)]

def project(output,request,native=False):
    access={}
    for result,op in zip(output['ops'],request['ops']):
        before=result.pop('before');after=result.pop('after');assert after>=before
        for row in (result['pool'] or [])+(result['held'] or []):
            identity=row['object'];time=row.pop('access')
            if identity not in access or access[identity]!=time:
                assert before<=time<=after,(op,row,time,before,after)
                assert op['cmd'] in ['add','get','all','children'],(op,row,time)
            access[identity]=time
        sqlbefore=result.pop('sqlNow') if native else before
        sqlafter=sqlbefore if native else after
        count=0
        for event in result['events']:
            if event[0]!='prepared.setLong':continue
            if op['cmd'] in ['add','addSQL'] and event[2]==5:offsets=[0]
            elif op['cmd']=='cleanSQL' and event[2]==1:offsets=[-259200000,-500654080]
            else:continue
            valid=[offset for offset in offsets if ((event[3]-(sqlbefore+offset))&MASK)<=sqlafter-sqlbefore]
            assert len(valid)==1,(op,event,sqlbefore,sqlafter)
            event[3]='<validated-clock:'+str(valid[0])+'>';count+=1
        if native:
            clocks=result.pop('clockCalls')
            if op['cmd'] in ['add','addSQL']:assert clocks==count
            elif op['cmd']=='cleanSQL':assert count<=clocks<=2
            else:assert clocks==0
    return normalize(output)

def corpus(fixtures,root):
    cases=[];ids=[fixture['id'] for fixture in fixtures]
    rows=[dict(txpowid=ids[1],txpowdata=fixtures[1]['wire'],tot=7),dict(txpowid=ids[2],txpowdata=fixtures[2]['wire'],tot=9)]
    def op(cmd,**kw):return dict(cmd=cmd,rows=[],**kw) if 'rows' not in kw else dict(cmd=cmd,**kw)
    def add(*ops):cases.append(dict(fixtures=FIXTURES,ops=list(ops)))
    baseline=[op('load'),op('hold'),op('add',index=1,addresses=['0x1122']),op('add',index=2),op('add',index=3,rows=rows),op('get',index=1),op('unused'),op('chain',index=1),op('unused'),op('cascade',index=2),op('coin',coinid='0x03'),op('clearChain'),op('unused'),op('cleanRAM'),op('remove',index=1),op('get',index=1,rows=rows),op('wipeRAM'),op('sqlSize',rows=rows),op('cleanSQL'),op('save'),op('hard')]
    add(*baseline)
    commands=['load','save','hard','add','addSQL','get','exists','all','children','ramSize','sqlSize','file','cleanRAM','wipeRAM','cleanSQL','clearChain','chain','cascade','unused','remove','coin','hold']
    def command(cmd,**kw):return op(cmd,index=1,ids=[ids[1],ids[2],ids[1],'missing'],coinid='0x02',rows=rows,**kw)
    for cmd in commands:add(command(cmd))
    for cmd,fail,runtime in itertools.product(['load','save','hard','add','addSQL','get','exists','all','children','sqlSize','cleanSQL'],range(1,44),[False,True]):
        add(op('load'),op('add',index=2),command(cmd,fail=fail,runtimeFailure=runtime),op('get',index=2),op('sqlSize'))
    for cmd,index,rowspec in itertools.product(['add','addSQL'],[None,0,1,2,3,4],[[],rows]):
        for problem in [{},{'nullMain':True},{'nullDB':True},{'nullWallet':True},{'serviceFail':'main'},{'serviceFail':'wallet'},{'serviceFail':'address'}]:
            add(op('load'),op(cmd,index=index,rows=rowspec,**problem),op('get',index=index,rows=rows),op('unused'))
    # SQL miss must add in both stores; duplicate RAM references remain original,
    # while every add posts a fresh message referring to the supplied object.
    for fail,relevant in itertools.product(range(0,25),[False,True]):
        add(op('load'),op('add',index=1,addresses=['0x1122'] if relevant else [],fail=fail),op('add',index=3,rows=rows),op('exists',index=1),op('get',index=1))
    for cmd,idvalue in itertools.product(['get','exists','children','chain','cascade','remove'],[None,'','unknown','π😄']):add(op('load'),op('add',index=1),op(cmd,id=idvalue,rows=rows))
    for values in [None,[],[None],[ids[1],None],[ids[2],ids[1],ids[1],'missing']]:add(op('load'),op('add',index=1),op('all',ids=values,rows=rows))
    for children in [[],[dict(txpowid=None)],[dict(txpowid=ids[1])],rows+rows]:add(op('load'),op('add',index=1),op('children',id=None,rows=children))
    for retention in [-(1<<62),-1,0,3600000,(1<<62)]:add(op('load'),op('add',index=1),op('hold'),op('cleanRAM',retention=retention),op('wipeRAM'),op('unused'))
    for filename in [None,'','/','///','bare','bare/',root+'//db///txpow/',root+'/db/../txpow',root+'/nul\0file']:
        add(op('load',file=filename),op('sqlSize'),op('hard'),op('cleanSQL'))
    for flag in ['nullPrepared','nullStatement','nullRows']:
        add(op('load',**{flag:True}),op('add',index=1,**{flag:True}),op('get',index=1),op('hard'),op('addSQL',index=1))
    add(op('load'),op('add',index=1),op('hold'),op('mutate',index=1,nonce=900),op('get',id=ids[1]),op('get',index=1),op('cleanRAM'),op('get',index=1),op('wipeRAM'))
    # Reuse SearcherOracle's transaction state fixture to reach wallet key
    # membership and address/key short-circuit paths in the owning database.
    for state,addresses,keys,fail in itertools.product(['0xAB','[text]','TRUE'],[[],['0xAB']],[[],['0xAB']],['','address','key']):
        cases.append(dict(fixtures=[dict(index=0,state=state)],ops=[op('load'),op('add',index=0,addresses=addresses,keys=keys,serviceFail=fail),op('get',index=0)]))
    # Larger actual RAM map and retained references exercise enumeration and flags.
    large=[dict(index=i,transaction=True) for i in range(80)]
    cases.append(dict(fixtures=large,ops=[op('load')]+[op('add',index=i) for i in range(80)]+[op('hold'),op('unused')]+[op('chain',index=i) for i in range(0,80,3)]+[op('cascade',index=i) for i in range(0,80,4)]+[op('unused'),op('clearChain'),op('unused'),op('cleanRAM'),op('wipeRAM')]))
    rng=random.Random(5564)
    for _ in range(80):
        ops=[op('load')]
        for _ in range(15):
            cmd=rng.choice(commands);ops.append(command(cmd,fail=rng.randrange(30),runtimeFailure=rng.choice([False,True]),addresses=['0x1122']))
        add(*ops)
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'txpow-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['SqlDBOracle.java','TxPoWSqlDBOracle.java','ArchiveCacheOracle.java','RamOracle.java','NIOClientOracle.java','TxPoWDBOracle.java']]
    listing=build/'txpow-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=effects=0
    with tempfile.TemporaryDirectory(prefix='minima-txpow-db-',dir=build) as root:
        runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxPoWDBOracle',root]),Runner([str(build/'minima_txpow_db'),root])]
        try:
            request=dict(fixtures=FIXTURES,ops=[]);outputs=[r.ask(request) for r in runners];assert outputs[0]==outputs[1],outputs
            for index,request in enumerate(corpus(outputs[0]['fixtures'],root)):
                outputs=[project(r.ask(request),request,i==1) for i,r in enumerate(runners)]
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);effects+=sum(len(op['events']) for op in outputs[0]['ops'])
        finally:
            for r in runners:r.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,effects=effects,failures=failures,scope='Actual pinned owning TxPoWDB, RAM/SQL subclasses, TxPoWSearcher and Wallet membership methods. Exact JDBC and owner-call order, shared TxPoW/message/RamData/map identities, null/error side effects, both-store lifecycle and relevance decisions. SQL clocks checked before normalization; RAM access timestamps checked at creation/change against operation windows. Java NPE expression diagnostics and logger prefixes/stacks excluded. JDBC and Main/wallet owner boundaries are fixtures; real native SQL storage and full node runtime remain incomplete.')
    (build/'txpow-db-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
