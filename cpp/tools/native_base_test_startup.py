#!/usr/bin/env python3
"""Actual Java/native test command with H2, followed by orderly node restart."""
import argparse,json,os,re,subprocess,tempfile,time
from pathlib import Path
from native_restore_startup import Node,ready,key_state
from p2p_functions_differential import ROOT,JAVA

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();start=time.monotonic()
    # Reuse the pinned classes compiled for the database differential fixture.
    oracle=build/'base-test-command-oracle'
    if not (oracle/'org/minima/Minima.class').exists():raise RuntimeError('Run base_test_command_differential.py first')
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    checks=[];failures=[];live=[];runtime='setup'
    try:
        for runtime,command in zip(['java','cpp'],commands):
            with tempfile.TemporaryDirectory(prefix='base-test-node-',dir=build) as scratch:
                data=Path(scratch)/'data';expected=None
                for phase in ['fresh','restart']:
                    node=Node(command,data,scratch);live.append(node)
                    keys=ready(node,7) if phase=='fresh' else key_state(node)
                    if expected is None:expected=set(keys)
                    assert set(keys)==expected and len(keys)>=8,keys
                    for text in ['test','test show:ignored action:ignored','test']:
                        offset=len(node.output);result=node.ask(text);assert result['status'] and not result['pending'] and 'response' not in result,result
                        printed=node.output[offset:];assert 'About to close and reopen DBs' in printed and 'DBs reopened..' in printed,printed
                        status=node.ask('status');assert status['status'],status
                    assert set(key_state(node))==expected
                    files=sorted(str(p.relative_to(data)) for p in data.rglob('*.mv.db'));assert any('archive' in p for p in files) and any('txpow' in p for p in files),files
                    # Keep an independent descriptor so shutdown diagnostics are
                    # still checked after the shared helper closes its handle.
                    with os.fdopen(os.dup(node.err.fileno()),'r') as errors:
                        node.finish(quit=True);errors.seek(0);stderr=errors.read()
                    live.remove(node)
                    if runtime=='cpp':
                        # Existing node services use clog for ordinary lifecycle logs.
                        # Retain that output, require exit0 above and UBSANhalt from
                        # Node, and separately reject native sanitizer diagnostics.
                        assert not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|LeakSanitizer|runtime error:|DEADLYSIGNAL|SUMMARY:',stderr),stderr
                    checks.append(dict(runtime=runtime,phase=phase,keys=len(keys),commands=3,sql_files=files,graceful_shutdown=True,stderr=stderr))
                    print(json.dumps(checks[-1]),flush=True)
    except Exception as error:failures.append(dict(runtime=runtime,error=repr(error),nodes=[dict(stdout=n.output[-6000:],stderr=n.errors()) for n in live]))
    finally:
        for node in live:node.cancel()
    report=dict(passed=not failures,node_processes=len(checks),checks=checks,failures=failures,seconds=round(time.monotonic()-start,3),scope='Actual pinned Java and native solo/noconnect nodes, real H2 archive/TxPoW databases, three test commands before and after restart, completion logs, valid status replies, preserved key sets, orderly exit, and no native sanitizer diagnostics including shutdown. No public network, SQL fault injection on durable storage or unused classloader-helper coverage.')
    (build/'native-base-test-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
