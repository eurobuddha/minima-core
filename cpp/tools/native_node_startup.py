#!/usr/bin/env python3
"""Real, unmodified Java/native Main startup and cross-runtime node reopen."""
import argparse,json,os,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def replies(output):
    decoder=json.JSONDecoder();result=[];offset=0
    while offset<len(output):
        if output[offset] in '{[':
            try:
                value,end=decoder.raw_decode(output,offset)
                if isinstance(value,dict) and 'command' in value:result.append(value)
                offset=end
                continue
            except json.JSONDecodeError:pass
        end=output.find('\n',offset)
        if end<0:break
        offset=end+1
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'native-node-oracle';oracle.mkdir(exist_ok=True)
    listing=build/'native-node-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sorted((JAVA/'src').rglob('*.java')))+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    env={k:v for k,v in os.environ.items() if not k.lower().startswith('minima_')};env.update(TZ='UTC',UBSAN_OPTIONS='halt_on_error=1')
    checks=[];failures=[]
    try:
        for encrypted in [False,True]:
            for writer in [0,1]:
                with tempfile.TemporaryDirectory(prefix='native-node-',dir=build) as scratch:
                    data=Path(scratch)/'data';original=None
                    for step,runtime in enumerate([writer,1-writer,writer]):
                        args=['-data',str(data),'-solo','-noconnect','-host','127.0.0.1','-port','0','-hashtest','1000','-nossl']
                        if encrypted:args+=['-dbpassword','fixture password']
                        process=subprocess.run(commands[runtime]+args,input='hash data:hello\nstatus\n'+('quit compact:true\n' if step==2 else 'quit\n'),capture_output=True,text=True,timeout=150,env=env,cwd=scratch)
                        assert process.returncode==0,dict(code=process.returncode,out=process.stdout,err=process.stderr)
                        assert 'Shut down completed OK..' in process.stdout,process.stdout
                        assert 'ERROR' not in process.stdout+process.stderr,(process.stdout,process.stderr)
                        rows=replies(process.stdout);assert [r['command'] for r in rows]==(['hash','status','quit'] if step==2 else ['hash','status']),rows
                        assert all(r['status'] for r in rows),rows
                        assert rows[0]['response']['hash']=='0x3338BE694F50C5F338814986CDF0686453A888B84F424D792AF4B9202398F392',rows[0]
                        status=rows[1]['response'];assert status['network']['connected']==0 and status['network']['connecting']==0,status
                        assert status['version']=='1.1.2-TEST.8' and not status['locked'],status
                        snapshot={k:status[k] for k in ['version','locked','minima','coins','length','weight']}
                        snapshot['chain']={k:status['chain'][k] for k in ['block','hash','difficulty','length','branches','weight']}
                        assert snapshot['minima']=='1000000000' and snapshot['coins']=='2' and snapshot['chain']['block']==1,snapshot
                        if original is None:original=snapshot
                        else:assert snapshot==original,dict(original=original,reopened=snapshot)
                        db=data/'1.1/databases'
                        for file in ['walletsql/wallet.mv.db','archivesql/archive.mv.db','txpowsql/txpow.mv.db','userprefs.db','cascade.db','chaintree.db']:
                            assert (db/file).stat().st_size>0,file
                        checks.append(dict(encrypted_wallet=encrypted,writer=['java','cpp'][writer],runtime=['java','cpp'][runtime],step=step,compact=step==2,chain_preserved=True))
                        print(f'Validated node process {len(checks)}/12: {checks[-1]}',flush=True)
    except Exception as error:failures.append(dict(error=repr(error)))
    report=dict(passed=not failures,node_processes=len(checks),checks=checks,failures=failures,scope='Real pinned Main and native MinimaCoreC++ with H2: solo genesis startup, hash/status/quit, graceful database save, then other runtime and original runtime reopen the same complete node directory; plaintext and encrypted wallets, compact shutdown. Exact selected stable status/chain fields and SHA3 value; timestamps, random genesis across fresh directories, memory/disk size, uptime, log destinations/order and stack text are not compared. No public-peer connection, long-running mining/sync, RPC/daemon or full-node parity certification.')
    (build/'native-node-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
