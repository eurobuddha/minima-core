#!/usr/bin/env python3
"""Actual node systemcheck reports, details and asynchronous checker delivery."""
import argparse, json, re, subprocess, tempfile, time
from pathlib import Path
from p2p_functions_differential import ROOT, JAVA
from native_restore_startup import Node, ready
from systemcheck_commands_corpus import LABELS, NAMES


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--build',type=Path,default=ROOT/'build')
    build=parser.parse_args().build.resolve()
    oracle=build/'systemcheck-node-oracle';oracle.mkdir(exist_ok=True)
    listing=build/'systemcheck-node-sources.txt'
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sorted((JAVA/'src').rglob('*.java')))+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    checks=[];failures=[];live=[];stage='start'
    try:
        for runtime,command in enumerate(commands):
            stage=['java','cpp'][runtime]
            with tempfile.TemporaryDirectory(prefix='systemcheck-node-',dir=build) as scratch:
                node=Node(command,Path(scratch)/'data',scratch);live.append(node)
                ready(node,7)
                start=len(node.output)
                row=node.ask('systemcheck');assert row['status'],row
                report=row['response']
                for label in LABELS:
                    assert isinstance(report[label]['stack'],int) and report[label]['stack']>=0,report
                    assert report[label]['lastmessage'] is None or isinstance(report[label]['lastmessage'],str),report
                assert isinstance(report['TimerProcessor'],int) and report['TimerProcessor']>=0,report
                assert re.fullmatch(r'java.util.concurrent.locks.ReentrantReadWriteLock@[0-9a-f]+\[Write locks = \d+, Read locks = \d+\]',report['RWLockInfo']),report
                assert isinstance(report['writelockthread'],str) and isinstance(report['writelockthreadstate'],bool) and report['Shutting Down'] is False,report
                expected=['TXPOWPROCESSOR Checker Call Recieved..','MAIN Checker Call Recieved.. timer:false','MAIN Checker Call Recieved.. timer:true']
                deadline=time.monotonic()+30
                while not all(message in node.output[start:] for message in expected):
                    assert time.monotonic()<deadline,node.output[start:]
                    time.sleep(.1)
                    status=node.ask('status');assert status['status'],status
                for name in NAMES+['sendpollmanager','unknown']:
                    row=node.ask('systemcheck action:details processor:'+name)
                    assert row['status'] and row['response']==dict(details='Sent to Minima Log'),row
                row=node.ask('systemcheck action:unknown');assert row['status'] and row['response']=={},row
                node.finish(quit=True);live.remove(node)
                checks.append(dict(runtime=stage,processors=len(LABELS),details_selectors=len(NAMES)+2,
                                   real_checker_delivery=expected,report_shapes_valid=True,graceful_shutdown=True))
                print('Verified live systemcheck: '+json.dumps(checks[-1]),flush=True)
    except Exception as error:
        failures.append(dict(stage=stage,error=repr(error),nodes=[dict(stdout=n.output[-10000:],stderr=n.errors()) for n in live]))
    finally:
        for node in live:node.cancel()
    report=dict(passed=not failures,node_processes=len(checks),checks=checks,failures=failures,
                scope='Actual pinned Java/native solo nodes and real H2, seven processor report shapes, all details selectors, real TxPoWProcessor/Main/immediate and delayed timer checker delivery, graceful shutdown. Live queue counts and scheduling are not compared for equality; no atomic report or exhaustive concurrency claim.')
    (build/'native-systemcheck-startup-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2));return bool(failures)


if __name__=='__main__':raise SystemExit(main())
