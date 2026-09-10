#!/usr/bin/env python3
"""Pinned ArchiveManager bodies and wire codecs; recording SQL is not an engine."""
import argparse,itertools,json,random,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
MASK=(1<<64)-1
FIXTURES=[dict(index=3),dict(index=1),dict(index=2,mode=1),dict(index=3,nonce=99)]
CASCADES=[dict(count=3),dict(count=0),dict(count=1),*[dict(count=3,mode=m) for m in [1,2,4,8]]]

def project(output,request,native=False):
    assert 'ops' in output,output
    retention=(2000*request.get('days',50))&MASK
    if retention>>63:retention-=1<<64
    assert output['defaultRetention']==retention,output
    for result,op in zip(output['ops'],request['ops']):
        before=result.pop('before');after=result.pop('after');assert after>=before
        clocks=0
        for event in result['events']:
            if op['cmd']=='add' and event[0]=='prepared.setLong' and event[2]==3:
                assert before<=event[3]<=after,(event,before,after)
                event[3]='<validated-clock-value>';clocks+=1
        if native:assert result.pop('clockCalls')==clocks
    return normalize(output)

def request(ops,full=False,**kw):
    # Build only referenced test inputs. This changes no archive operation or
    # expected row: encoded JDBC payloads still come from the full bootstrap.
    def count(command):return max((entry.get('index',-1) for entry in ops if entry['cmd']==command and entry.get('index') is not None),default=-1)+1
    return dict(fixtures=FIXTURES if full else FIXTURES[:count('add')],cascades=CASCADES if full else CASCADES[:count('checkCascade')],ops=ops,**kw)
def op(cmd,**kw):return dict(cmd=cmd,**({'rows':[],'columns':[]}|kw))

def corpus(wires,cascades):
    cases=[]
    def add(*ops,**kw):cases.append(request(list(ops),**kw))
    sample=[dict(syncdata=wires[0],fulldata=cascades[0],block=3,tot=4),dict(syncdata=wires[1],fulldata=cascades[2],block=1,tot=-1),dict(syncdata=wires[2],block=2),dict(syncdata=wires[3],block=3)]
    commands=['add','get','exists','number','first','last','sync','range','cascade','checkCascade','size','clean','reopen','hack']
    def command(cmd,**kw):return op(cmd,**(dict(index=0,id='0x01',start='260',end='300',rows=sample)|kw))
    add(op('load',debug=True),*(command(c) for c in commands),op('save'),*(command(c) for c in commands))
    for cmd in commands:add(command(cmd))
    # Check every driver boundary, including retry after runtime exceptions,
    # partial list results, and prepared field replacement during reopen.
    for cmd,fail,runtime in itertools.product(commands,range(1,30),[False,True]):
        add(op('load'),command(cmd,fail=fail,runtimeFailure=runtime),command('size'))
    for phase,fail,runtime in itertools.product(['load','create'],range(1,25),[False,True]):
        add(op('load'),op(phase,fail=fail,runtimeFailure=runtime,debug=True),*(command(c) for c in ['add','first','last','cascade','sync']))
    for cmd,flag in itertools.product(commands,['nullConnection','nullStatement','nullPrepared','nullRows']):
        add(op('load',**{flag:True}),command(cmd,**{flag:True}),op('hard'),command(cmd))
    for cmd,rows in itertools.product(['get','number','first','last','exists','sync','range','cascade','size','clean'],[[],[{}],[dict(block=-1,tot=2147483648,syncdata=wires[0],fulldata=cascades[0])],sample,sample+sample]):
        add(op('load'),command(cmd,rows=rows))
    for cmd,start,end in itertools.product(['number','sync','range'],[None,'-1','0','1','256','257','258','259.99','-1.5','9223372036854775808','18446744073709551615','1e40'],[None,'1','300']):
        add(op('load'),command(cmd,start=start,end=end),command(cmd,start=start,end=end,descending=False))
    for null,shutdown,after in itertools.product([False,True],[False,True],[0,1,2,3,4,5]):
        add(op('load'),command('sync',nullMain=null,shutdown=shutdown,shutdownAfter=after))
    for index in [None,0,1,2,3]:
        add(op('load'),command('add',index=index),op('hack'),command('add',index=index),op('reopen'),command('add',index=index))
    for cmd,fail,runtime in itertools.product(['add','checkCascade'],range(15),[False,True]):
        add(op('load'),command(cmd,badHash=True,fail=fail,runtimeFailure=runtime,rows=[dict(block=3,fulldata=cascades[2],syncdata=wires[0])]),command('size'))
    for rows in [[],sample,[dict(block=50,fulldata=cascades[0],syncdata=wires[0])],[dict(block=3,fulldata=cascades[2],syncdata=wires[0])],[dict(block=-1,fulldata=cascades[0])],[dict(block=3,fulldata=cascades[1],syncdata=wires[0])]]:
        for index in [None,*range(len(CASCADES))]:
            add(op('load'),command('checkCascade',rows=rows,index=index))
    # No stored cascade => validation + DELETE then INSERT. DELETE's failure
    # JSON is ignored by the actual source, so insertion can still follow it.
    for index,fail,runtime in itertools.product([0,2,3,4,5,6],range(1,20),[False,True]):
        add(op('load'),command('checkCascade',rows=[],index=index,fail=fail,runtimeFailure=runtime))
    for archive,retention,height in itertools.product([False,True],[-(1<<63),-1,0,100000,(1<<63)-1],[-(1<<63),-1,0,100001,(1<<63)-1]):
        add(op('load'),command('clean',archive=archive,retention=retention,rows=[dict(block=height)],updated=-7))
    for days in [-(1<<63),-2147483648,-1,0,50,2147483647,(1<<63)-1]:
        add(op('load'),command('clean',updated=2),days=days)
    # Every truncated prefix of actual block and cascade serialization must
    # preserve I/O logging/null conversion. Runtime decoding errors propagate.
    for wire in wires[:3]:
        for end in range(2,len(wire),2):
            rows=[dict(syncdata=wire[:end])]
            add(op('load'),command('get',rows=rows),command('range',rows=rows),command('sync',rows=rows))
    for wire in cascades[:3]:
        for end in range(2,len(wire),2):add(op('load'),command('cascade',rows=[dict(fulldata=wire[:end])]))
    for data in ['0x','0xFF','0x00000000','0x0000000000','0x00FFFFFFFF','0x0000000100','0x0000000120','0x0000000140',wires[0]+'FFFF']:
        for cmd in ['get','range','sync']:
            add(op('load'),command(cmd,rows=[dict(syncdata=data)]),command(cmd,rows=[dict(syncdata=data),sample[0]]))
    rng=random.Random(5564)
    for _ in range(100):
        add(op('load'),*(command(rng.choice(commands+['hard','clear','create']),fail=rng.randrange(30),runtimeFailure=rng.choice([False,True]),descending=rng.choice([False,True]),rows=rng.choice([[],sample,sample[:1]])) for _ in range(10)))
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'archive-manager-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/NIOClientOracle.java',ROOT/'support/ArchiveManagerOracle.java'];listing=build/'archive-manager-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=driver_calls=0
    with tempfile.TemporaryDirectory(prefix='minima-archive-',dir=build) as root:
        runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ArchiveManagerOracle',root]),Runner([str(build/'minima_archive_manager'),root])]
        try:
            bootstrap=request([],full=True);outputs=[project(runner.ask(bootstrap),bootstrap,i==1) for i,runner in enumerate(runners)];assert outputs[0]==outputs[1],outputs
            for index,req in enumerate(corpus(outputs[0]['fixtures'],outputs[0]['cascades'])):
                outputs=[project(runner.ask(req),req,i==1) for i,runner in enumerate(runners)]
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=req,java=outputs[0],cpp=outputs[1]));break
                operations+=len(req['ops']);driver_calls+=sum(len(result['events']) for result in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,driver_calls=driver_calls,failures=failures,scope='Actual pinned ArchiveManager schema/prepared SQL, save retry, lookups, high/low two-query reads, ranges/order, shutdown boundary, cleanup long arithmetic, cascade validation/replacement and reopen with recording JDBC/native drivers. Serialized fixture/output bytes and I/O logs compared; every truncated prefix of three blocks and three cascades; checked/runtime/null/partial failures. Clock values checked against operation windows before normalization; Java NPE expression text and logger prefixes/stacks excluded. Native H2 SQL execution, persistence, concurrent lifecycle, demonstration main and full node integration remain absent/unverified.')
    (build/'archive-manager-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
