#!/usr/bin/env python3
"""Actual TxPoWOnChainDB methods, prepared SQL and JDBC effects; no native engine claim."""
import argparse,itertools,json,random,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
MASK=(1<<64)-1

def project(output,request,native=False):
    assert output.get('defaultRetention')==500654080,output
    for result,op in zip(output['ops'],request['ops']):
        before=result.pop('before');after=result.pop('after');assert after>=before
        clock_events=0
        for event in result['events']:
            if event[0]!='prepared.setLong':continue
            if op['cmd']=='add' and event[2]==4:offset=0
            elif op['cmd']=='clean' and event[2]==1:offset=100000 if op.get('hard',False) else -result['retention']
            else:continue
            # Retain the actual arithmetic: the observed timestamp must fall in
            # the operation's real clock window, including Java long wraparound.
            assert ((event[3]-(before+offset))&MASK)<=after-before,(event,before,after,offset)
            event[3]='<validated-clock-value>';clock_events+=1
        if native:
            count=result.pop('clockCalls')
            if op['cmd']=='add':assert count==clock_events,(op,result,count)
            elif op['cmd']=='clean':
                expected=2 if op.get('hard',False) else 1
                assert count in [0,expected]
                if clock_events:assert count==expected
            else:assert count==0
    return normalize(output)

def corpus():
    cases=[]
    def op(cmd,**kw):return dict(cmd=cmd,rows=[],**kw) if 'rows' not in kw else dict(cmd=cmd,**kw)
    def add(*ops):cases.append(dict(ops=list(ops)))
    sample=[dict(txpowid='0x01',blockid='0xAA',block=42,timemilli=1700000000123,tot=3),dict(txpowid=None,blockid='',block=-1,timemilli=0,tot=-1),dict(txpowid='π😄',blockid=None,block=9223372036854775807,timemilli=-1,tot=2147483648)]
    commands=['add','get','inblock','inheight','first','last','size','clean','wipe']
    def command(cmd,**kw):return op(cmd,blockid='0xAA',txpowid='0x01',block='42',height=42,rows=sample,**kw)
    add(op('load'),*(command(c) for c in commands),op('save'),*(command(c) for c in commands),op('hard'))
    for cmd in commands:add(command(cmd))
    # Every early JDBC boundary, checked/runtime failures and a subsequent read
    # exercise partial prepared-statement replacement and partial returned rows.
    for cmd,fail,runtime in itertools.product(commands,range(1,25),[False,True]):
        add(op('load'),command(cmd,fail=fail,runtimeFailure=runtime),command('size'))
    for phase,fail in itertools.product(['load','create','wipe'],range(1,17)):
        add(op('load'),op(phase,fail=fail),*(command(c) for c in ['get','inblock','inheight','first','last','size']))
    for fail in range(1,16):add(op('load',fail=fail),*(command(c) for c in ['get','first','last','size']))
    for cmd,flag in itertools.product(commands,['nullPrepared','nullStatement','nullRows']):
        add(op('load',**{flag:True}),command(cmd,**{flag:True}),op('hard'),command(cmd))
    for cmd,empty in itertools.product(['get','inblock','inheight','first','last','size'],[[],[{}],sample,sample+sample]):
        add(op('load'),op(cmd,blockid=None,txpowid=None,height=-(1<<63),rows=empty))
    for block,bid,tid in itertools.product([None,'0','-1','1.99','-1.99','9223372036854775808','18446744073709551615','1e40','1e-12'],[None,'',"quote'π😄"],[None,'txp']):
        add(op('load'),op('add',block=block,blockid=bid,txpowid=tid))
    for block in ['-1e40','1.23e40','18446744073709551616','-18446744073709551616','18446744073709551615.0001','18446744073709551615.0000','9'*70,'-'+'9'*70,'18446744073709551615.'+'0'*44+'1']:
        add(op('load'),op('add',block=block,blockid='fixture',txpowid='fixture'))
    for hard,retention,updated,now in itertools.product([False,True],[-(1<<63),-1,0,500654080,(1<<63)-1],[-1,0,123,2147483647],[-(1<<63),(1<<63)-1,1700000000000]):
        add(op('load'),op('clean',hard=hard,retention=retention,updated=updated,now=now))
    for time in [-(1<<63),-12219292800001,-2208988800000,-1,0,1,1700000000123,(1<<63)-1]:
        for cmd in ['first','last']:add(op('load'),op(cmd,rows=[dict(txpowid='fixture',blockid='b',block=-1,timemilli=time)]))
    rng=random.Random(5564)
    for _ in range(100):
        ops=[op('load')]
        for _ in range(12):
            cmd=rng.choice(commands+['hard','create','clear']);ops.append(command(cmd,fail=rng.randrange(22),runtimeFailure=rng.choice([False,True]),updated=rng.randrange(-2,20)))
        add(*ops)
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'onchain-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/OnChainDBOracle.java'];listing=build/'onchain-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=driver_calls=0
    with tempfile.TemporaryDirectory(prefix='minima-onchain-',dir=build) as root:
        runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','OnChainDBOracle',root]),Runner([str(build/'minima_onchain_db'),root])]
        try:
            for index,request in enumerate(corpus()):
                outputs=[project(runner.ask(request),request,i==1) for i,runner in enumerate(runners)]
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);driver_calls+=sum(len(op['events']) for op in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,driver_calls=driver_calls,failures=failures,scope='Actual pinned TxPoWOnChainDB schema/prepared SQL, wipe, add, lookup, both block query overloads, first/last/date, count and cleanup through recording JDBC/native drivers. Exact statement identities, call order, result/null/partial-error behavior, checked versus runtime failures, long/int wrap and parameters. Live clock parameters validated against call windows before normalization; Java-specific NPE text and logger prefixes/stack traces excluded. Real native H2 SQL execution, persistence and full runtime integration remain absent.')
    (build/'onchain-db-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
