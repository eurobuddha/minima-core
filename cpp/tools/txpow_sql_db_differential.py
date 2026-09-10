#!/usr/bin/env python3
"""Actual TxPoWSqlDB methods, prepared SQL and JDBC effects; no native engine claim."""
import argparse,itertools,json,random,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
MASK=(1<<64)-1

def project(output,request,native=False):
    assert output.get('defaultRetention')==259200000,output
    for result,op in zip(output['ops'],request['ops']):
        before=result.pop('before');after=result.pop('after');assert after>=before
        clock_events=0
        for event in result['events']:
            if event[0]!='prepared.setLong':continue
            if op['cmd']=='add' and event[2]==5:offset=0
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

def corpus(wires):
    cases=[]
    def op(cmd,**kw):return dict(cmd=cmd,rows=[],**kw) if 'rows' not in kw else dict(cmd=cmd,**kw)
    def add(*ops):cases.append(dict(ops=list(ops)))
    sample=[dict(txpowid='0x01',txpowdata=wires[0],tot=3),dict(txpowid=None,txpowdata=wires[1],tot=-1),dict(txpowid='π😄',txpowdata=wires[2],tot=2147483648)]
    commands=['add','get','children','relevant','latest','latestSize','relevantSize','size','custom','exists','clean','wipe','reopen']
    def command(cmd,**kw):return op(cmd,txpowid='0x01',parentid='0x00',limit=4,offset=-1,minTime=1700000000000,where='isblock=1',rows=sample,**kw)
    add(op('load'),*(command(c) for c in commands),op('save'),*(command(c) for c in commands),op('hard'))
    for cmd in commands:add(command(cmd))
    for cmd,fail,runtime in itertools.product(commands,range(1,29),[False,True]):
        add(op('load'),command(cmd,fail=fail,runtimeFailure=runtime),command('size'))
    for phase,fail in itertools.product(['load','create','wipe','reopen'],range(1,22)):
        add(op('load'),op(phase,fail=fail),*(command(c) for c in ['get','relevant','latestSize','size']))
    for fail in range(1,20):add(op('load',fail=fail),*(command(c) for c in ['get','children','relevant','latest','size']))
    for cmd,flag in itertools.product(commands,['nullPrepared','nullStatement','nullRows']):
        add(op('load',**{flag:True}),command(cmd,**{flag:True}),op('hard'),command(cmd))
    for cmd,rows in itertools.product(['get','children','relevant','latest','latestSize','size','relevantSize','custom','exists'],[[],[{}],sample,sample+sample]):
        add(op('load'),op(cmd,txpowid=None,parentid=None,where=None,limit=-(1<<31),offset=(1<<31)-1,minTime=-(1<<63),rows=rows))
    for i,mode,relevant,blocks,transactions in itertools.product(range(4),[0,1],[False,True],[False,True],[False,True]):
        add(op('load'),op('add',index=i,mode=mode,relevant=relevant,logBlocks=blocks,logTransactions=transactions,results=relevant))
    for blocks,transactions in itertools.product([False,True],repeat=2):add(op('load'),op('add',nullTxPoW=True,logBlocks=blocks,logTransactions=transactions))
    for condition in [None,'',';UPDATEDELETEinsert;',"txpowid='MiXeD';",'upupdatedate','deletdeletee','insinsertert','upd;ate','UPUPDATEdate DELETEinsert UPDATEINSERT','İΣ𐐀😄 UPDATE','\u0000delete','updatedeleteinsert'*12]:
        add(op('load'),op('custom',where=condition,rows=[dict(tot=9)]))
    for hard,retention,updated,now,logs in itertools.product([False,True],[-(1<<63),-1,0,259200000,(1<<63)-1],[-1,0,123,2147483647],[-(1<<63),(1<<63)-1,1700000000000],[False,True]):
        add(op('load'),op('clean',hard=hard,retention=retention,updated=updated,now=now,logBlocks=logs))
    for limit,offset in itertools.product([-(1<<31),-1,0,1,(1<<31)-1],repeat=2):
        add(op('load'),op('relevant',limit=limit,rows=sample),op('relevant',limit=limit,offset=offset,rows=sample),op('latest',limit=limit,offset=offset,rows=sample))
    # Every truncated prefix is passed through the actual TxPoW converter.
    for wire in wires:
        for length in range(0,len(wire)-2,2):
            partial=wire[:length+2]
            add(op('load'),op('get',rows=[dict(txpowdata=partial)]),op('relevant',limit=3,rows=[sample[0],dict(txpowdata=partial),sample[1]]))
        add(op('load'),op('get',rows=[dict(txpowdata=wire+'FEED')]))
    for transaction,invalid,fail in itertools.product([False,True],[False,True],range(0,15)):
        add(op('load'),op('add',index=2,transaction=transaction,invalidHash=invalid,logBlocks=True,logTransactions=True,fail=fail))
    # Locate the first superparent hash length in a real header: nonce,
    # chain ID, time, block height, difficulty, then the run count.
    encoded=bytes.fromhex(wires[0][2:]);at=0
    for kind in ['number','data','number','number','data']:
        if kind=='number':at+=2+encoded[at+1]
        else:at+=4+int.from_bytes(encoded[at:at+4],'big')
    at+=1
    invalid_hashes=['0x'+(encoded[:at]+length.to_bytes(4,'big',signed=True)+encoded[at+4:]).hex() for length in [-1,65]]
    for wire in ['0x0000','0x0021','0x00FF','0x000100FFFFFFFF']+invalid_hashes:
        add(op('load'),op('get',rows=[dict(txpowdata=wire)]),op('relevant',rows=[sample[0],dict(txpowdata=wire),sample[1]]))
    rng=random.Random(5564)
    for _ in range(100):
        ops=[op('load')]
        for _ in range(12):
            cmd=rng.choice(commands+['hard','create','clear']);ops.append(command(cmd,fail=rng.randrange(28),runtimeFailure=rng.choice([False,True]),updated=rng.randrange(-2,20)))
        add(*ops)
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'txpow-sql-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/TxPoWSqlDBOracle.java',ROOT/'support/ArchiveCacheOracle.java',ROOT/'support/RamOracle.java'];listing=build/'txpow-sql-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=driver_calls=0
    with tempfile.TemporaryDirectory(prefix='minima-txpow-sql-',dir=build) as root:
        runners=[Runner(['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxPoWSqlDBOracle',root]),Runner([str(build/'minima_txpow_sql_db'),root])]
        try:
            request={'ops':[dict(cmd='make',index=i,mode=mode,transaction=txn,rows=[]) for i,mode,txn in [(0,0,False),(2,1,False),(3,0,False),(2,0,True)]]}
            outputs=[project(runner.ask(request),request,i==1) for i,runner in enumerate(runners)]
            assert outputs[0]==outputs[1],outputs
            wires=[op['value'] for op in outputs[0]['ops']]
            for index,request in enumerate(corpus(wires)):
                outputs=[project(runner.ask(request),request,i==1) for i,runner in enumerate(runners)]
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);driver_calls+=sum(len(op['events']) for op in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,driver_calls=driver_calls,failures=failures,scope='Actual pinned TxPoWSqlDB methods through recording JDBC/native drivers: schema and prepared identities, serialized bytes and metadata, queries/pagination, single-pass filtering, cleanup arithmetic, reopen, SQL/runtime failures, null/partial results and every truncated prefix of four TxPoWs including a non-empty transaction; malformed MiniNumber and MiniData inputs, serialization failure timing and transaction logs. Clock values checked before normalization; NPE diagnostics and logger prefixes/stacks excluded. Native H2 execution, persistence and full runtime integration remain absent.')
    (build/'txpow-sql-db-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
