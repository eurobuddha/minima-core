#!/usr/bin/env python3
"""Exercise the shipped launcher after ZIP extraction to a path containing spaces."""
import argparse, hashlib, json, os, subprocess, tempfile, time, traceback, zipfile
from pathlib import Path
from native_node_startup import replies

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('archive',type=Path)
    parser.add_argument('--report',type=Path,required=True)
    parser.add_argument('--port',type=int,help='Use a separate verification port via the actual launcher argument forwarding')
    parser.add_argument('--resume-folder',type=Path,help='Recheck an existing extracted preview after a recorded interrupted/failed verification; never label this a fresh start.')
    args=parser.parse_args()
    if args.port is not None and not 1 <= args.port <= 65535:parser.error('Invalid verification port')
    archive=args.archive.resolve()
    if args.resume_folder:
        folder=args.resume_folder.resolve();scratch=folder.parent
        assert (folder/'data/1.1/databases/chaintree.db').stat().st_size>0
    else:
        scratch=Path(tempfile.mkdtemp(prefix='Minima preview relocation ')).resolve()
        subprocess.run(['ditto','-x','-k',str(archive),str(scratch)],check=True)
        folders=[p for p in scratch.iterdir() if p.is_dir()]
        assert len(folders)==1
        folder=folders[0]
    manifest=json.loads((folder/'manifest.json').read_text())
    port=args.port if args.port is not None else manifest.get('node_port',21001)
    with zipfile.ZipFile(archive) as contents:
        assert all('/data/' not in name for name in contents.namelist()),'Release ZIP must not contain wallet/chain data'
        assert json.loads(contents.read(folder.name+'/manifest.json'))==manifest
    checks=[]
    for name,digest in manifest['file_sha256'].items():
        assert hashlib.sha256((folder/name).read_bytes()).hexdigest()==digest,name
    checks.append('Every extracted payload file matches its manifest SHA-256')
    if not args.resume_folder:assert not (folder/'data').exists()
    launcher=folder/'Start MinimaCoreC++.command'
    assert os.access(launcher,os.X_OK),'Executable permission survives ZIP extraction'
    env={k:v for k,v in os.environ.items() if not k.lower().startswith(('minima_','dyld_'))}
    env['PATH']='/usr/bin:/bin:/usr/sbin:/sbin'
    help_result=subprocess.run([str(launcher),'-help'],cwd='/',env=env,text=True,capture_output=True,timeout=30)
    # Pinned ParamConfigurer.java deliberately exits 1 after printing help.
    assert help_result.returncode==1 and 'Minima Help' in help_result.stdout and not help_result.stderr,(help_result.returncode,help_result.stdout,help_result.stderr)
    checks.append('Relocated launcher runs -help with system-only PATH')
    observed=[];failure=None;failure_traceback=None
    try:
        for step in (['resumed-reopen'] if args.resume_folder else ['fresh','reopen']):
            stdout=scratch/(step+'-stdout.log');stderr=scratch/(step+'-stderr.log')
            with stdout.open('w') as out,stderr.open('w') as err:
                process=subprocess.Popen([str(launcher)]+(['-port',str(port)] if args.port is not None else []),cwd='/',env=env,text=True,stdin=subprocess.PIPE,stdout=out,stderr=err)
                try:
                    process.stdin.write('hash data:hello\nlogs networking:true\n');process.stdin.flush()
                    deadline=time.monotonic()+150;comparison=None;last_ping=0
                    while time.monotonic()<deadline:
                        if process.poll() is not None:raise AssertionError(f'Node exited unexpectedly: {process.returncode}; {stderr.read_text()}')
                        process.stdin.write('status\n');process.stdin.flush()
                        time.sleep(5)
                        rows=replies(stdout.read_text())
                        pings=[r['response']['extradata'] for r in rows if r.get('command')=='ping' and r.get('response',{}).get('valid')]
                        statuses=[r['response'] for r in rows if r.get('command')=='status' and r.get('status')]
                        if statuses and statuses[-1]['chain']['block']>0 and time.monotonic()-last_ping>=15:
                            last_ping=time.monotonic()
                            process.stdin.write('ping host:eurobuddha.com:9001\nstatus\n');process.stdin.flush()
                            continue
                        for ping in pings[-1:]:
                            for status in statuses[-1:]:
                                chain=status['chain']
                                # A saved tip can match before the delayed connection starts.
                                # Wait for every readiness condition within the same deadline.
                                if chain['block']==int(ping['topblock']) and chain['hash']==ping['tophash'] and status['network']['connected']>=1:
                                    assert chain['block']>=2309561
                                    assert status['version']=='1.1.2.8'
                                    assert status['network']['port']==port and not status['network']['rpc']['enabled']
                                    assert status['network']['connected']>=1
                                    assert Path(status['data']).resolve()==(folder/'data/1.1').resolve()
                                    comparison=dict(step=step,block=chain['block'],hash=chain['hash'],peer='eurobuddha.com:9001',matches_peer=True)
                                    break
                            if comparison:break
                        if comparison:break
                    assert comparison,'Packaged node did not match the designated peer tip within 150 seconds'
                    hashes=[r for r in replies(stdout.read_text()) if r.get('command')=='hash']
                    assert len(hashes)==1 and hashes[0]['response']['hash']=='0x3338BE694F50C5F338814986CDF0686453A888B84F424D792AF4B9202398F392'
                    process.stdin.write('quit\n');process.stdin.flush();process.wait(timeout=45)
                    assert process.returncode==0
                    assert 'Shut down completed OK..' in stdout.read_text()
                    for name in ['walletsql/wallet.mv.db','archivesql/archive.mv.db','txpowsql/txpow.mv.db','cascade.db','chaintree.db']:
                        assert (folder/'data/1.1/databases'/name).stat().st_size>0,name
                    if step in ['reopen','resumed-reopen']:
                        assert 'Loading TxPowTree size : 0 bytes' not in stderr.read_text()
                        if observed:assert comparison['block']>=observed[0]['block']
                    observed.append(comparison)
                    print(json.dumps(comparison),flush=True)
                finally:
                    if process.poll() is None:
                        try:process.stdin.write('quit\n');process.stdin.flush();process.wait(timeout=45)
                        except (BrokenPipeError,subprocess.TimeoutExpired):
                            process.terminate();process.wait(timeout=15)
            checks.append(step+': exact SHA3 command, matching live peer height/hash, isolated data/port, saved databases and clean exit')
    except Exception as error:
        failure=repr(error);failure_traceback=traceback.format_exc()
    report=dict(passed=failure is None,resumed_existing_data=bool(args.resume_folder),verification_port=port,launcher_port_override=args.port is not None,source_head=manifest['source_head'],archive_sha256=hashlib.sha256(archive.read_bytes()).hexdigest(),checks=checks,observations=observed,failure=failure,failure_traceback=failure_traceback,relocated_folder=str(folder),scope='Actual shipped ZIP and launcher (port override recorded), no modified Minima source, system-only PATH; mainnet lifecycle steps explicitly listed in observations; resumed verification does not claim a new fresh start. Tests packaging and this workflow, not full node parity. Test wallet data remains only in the temporary extracted directory.')
    args.report.parent.mkdir(parents=True,exist_ok=True);args.report.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2));return bool(failure)

if __name__=='__main__':raise SystemExit(main())
