#!/usr/bin/env python3
"""Actual Java/native mysqlcoins commands with real H2 and isolated MySQL storage."""
import argparse,json,os,re,tempfile,time
from pathlib import Path
from native_restore_startup import Node,ready,key_state
from p2p_functions_differential import ROOT,JAVA

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--fixture',type=Path,required=True);args=ap.parse_args();build=args.build.resolve();state=json.loads(args.fixture.read_text());start=time.monotonic()
    assert state['address'].startswith('127.0.0.1:') and state['database']=='minima_cpp' and state['name'].startswith('minima-cpp-mysql-')
    oracle=build/'native-mysql-oracle'
    if not (oracle/'org/minima/Minima.class').exists():raise RuntimeError('Run native_mysql_differential.py first')
    commands=[['java','-cp',f'{oracle}:{JAVA}/lib/mysql-connector-java-8.0.24.jar:{JAVA}/jar/minima.jar','org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    login=f"mysqlcoins host:{state['address']} database:{state['database']} user:{state['user']} password:{state['password']}"
    checks=[];failures=[];live=[];transcripts=[];runtime='setup'
    try:
        for runtime,command in zip(['java','cpp'],commands):
            transcript=[]
            with tempfile.TemporaryDirectory(prefix='mysql-node-',dir=build) as scratch:
                data=Path(scratch)/'data';expected=None
                for phase in ['fresh','restart']:
                    node=Node(command,data,scratch);live.append(node)
                    keys=ready(node,7) if phase=='fresh' else key_state(node)
                    if expected is None:expected=set(keys)
                    assert set(keys)==expected and len(keys)>=8,keys
                    def ask(params):
                        value=node.ask(login+' '+params);assert value['status'] and not value['pending'],value
                        transcript.append(value);return value
                    if phase=='fresh':
                        assert ask('action:wipe')['respnse']=='CoinsDB wiped'
                        assert ask('action:info')['response']==dict(total=0,lastblock=0,autobackup=False)
                        empty=ask('action:update')['response'];duration=empty.pop('duration');assert re.fullmatch(r'\d+ seconds',duration);assert empty==dict(blocks=0,firstblock=-1,lastblock=-1,coinsadded=0,message='MySQL CoinsDB Updated..'),empty
                        assert ask('action:search where:"1=0"')['response']['rows']==[]
                        sql="INSERT INTO coins(coinid,address,amount,amountdouble,tokenid,storestate,state,mmrentrynumber,spent,blockcreated,blockspent,date,token,tokenamount) VALUES('0x01','0xFF','1.25',1.25,'0x00',1,'[]',9,0,42,0,'fixture π😄','fixture-token',1.25)"
                        assert ask('action:search query:"'+sql+'"')['response']==dict(sql=sql,status=True,results=False)
                    assert ask('action:info readonly:true')['response']==dict(total=1,lastblock=42,autobackup=False)
                    row=ask('action:search where:"coinid=\'0x01\'"')['response'];assert row['count']==1 and row['status'],row
                    record=row['rows'][0];assert record['amountdouble']=='1.250000' and record['date']=='fixture π😄' and record['token']=='fixture-token',record
                    hidden=ask('action:search address:0xFF spent:false limit:1 hidetoken:true')['response'];assert hidden['count']==1 and hidden['rows'][0]['token']=='_hidden_',hidden
                    assert ask('action:search query:"SELECT NULL AS absent, 7 AS number"')['response']['rows']==[dict(number='7')]
                    bad=ask('action:search query:"SELECT * FROM NATIVE_MISSING"')['response'];assert bad['error'] and not bad['status'],bad
                    if phase=='restart':
                        assert ask('action:wipe')['respnse']=='CoinsDB wiped'
                        assert ask('action:info')['response']==dict(total=0,lastblock=0,autobackup=False)
                    assert set(key_state(node))==expected
                    with os.fdopen(os.dup(node.err.fileno()),'r') as errors:
                        node.finish(quit=True);errors.seek(0);stderr=errors.read()
                    live.remove(node)
                    if runtime=='cpp':assert not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|LeakSanitizer|runtime error:|DEADLYSIGNAL|SUMMARY:',stderr),stderr
                    checks.append(dict(runtime=runtime,phase=phase,keys=len(keys),graceful_shutdown=True,stderr=stderr));print(json.dumps(checks[-1]),flush=True)
            transcripts.append(transcript)
        assert transcripts[0]==transcripts[1],transcripts
    except Exception as error:failures.append(dict(runtime=runtime,error=repr(error),nodes=[dict(stdout=n.output[-6000:],stderr=n.errors()) for n in live]))
    finally:
        for node in live:node.cancel()
    report=dict(passed=not failures,node_processes=len(checks),commands_per_runtime=len(transcripts[0]) if transcripts else 0,checks=checks,transcripts=transcripts,failures=failures,seconds=round(time.monotonic()-start,3),scope='Actual pinned Java and native solo/noconnect nodes with real H2 and the isolated MySQL fixture: mysqlcoins info, wipe (source respnse spelling), empty-archive update, durable query insert/search, null omission, double formatting, token hiding, SQL errors, read-only reconnect and node restart. Exact command replies except elapsed update duration validated by shape. All eight H2 wallet keys preserved; graceful shutdown and complete stderr checked. Nonempty archive update, autologin/autobackup, reconnect faults, TLS policies and public network remain outside this test.')
    (build/'native-mysql-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
