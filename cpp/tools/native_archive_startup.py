#!/usr/bin/env python3
"""Real-node archive import/reset and opposite-runtime chain/wallet reopen."""
import argparse,hashlib,json,shutil,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from native_restore_startup import Node,ready,key_state,chain
from peers_checker_differential import Runner

def coins(node):
    row=node.ask('status');assert row['status'],row
    return {k:row['response'][k] for k in ['coins','minima']}

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'archive-node-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['ArchiveCommandOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing=build/'archive-node-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    classpath=f'{oracle}:{JAVA}/jar/minima.jar'
    commands=[['java','-cp',classpath,'org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    checks=[];failures=[];live=[];runners=[];processes=0;exports=[];stage='baseline'
    try:
        with tempfile.TemporaryDirectory(prefix='archive-node-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline'
            node=Node(commands[0],baseline,scratch);live.append(node)
            original_keys=ready(node,7);original_chain=chain(node);original_coins=coins(node)
            assert original_chain['block']==1,original_chain
            backup=root/'baseline.bak';row=node.ask(f'backup file:{backup} password:fixture-archive');assert row['status'] and backup.is_file(),row
            node.finish(quit=True);live.remove(node);processes+=1
            stage='export saved chain'
            runners=[Runner(['java','-cp',classpath,'ArchiveCommandOracle']),Runner([str(build/'archive_command_runtime')])]
            tree_wires=None;files={}
            for runtime,runner in enumerate(runners):
                folder=root/f'export-{runtime}';shutil.copytree(baseline/'1.1',folder)
                row=runner.ask(dict(op='seedTree',folder=str(folder)));assert row.get('seeded') and len(row['blocks'])==1,row
                if tree_wires is None:tree_wires=row['blocks']
                else:assert row['blocks']==tree_wires,'Saved tree encodings differ'
                files[runtime]={}
                for action,suffix in [('export','gzip'),('exportraw','dat')]:
                    path=root/f'archive-{runtime}.{suffix}'
                    row=runner.ask(dict(folder=str(folder),params=dict(action=action,file=str(path))));assert row.get('result',{}).get('status'),row
                    files[runtime][suffix]=path
                    exports.append(dict(runtime=['java','cpp'][runtime],format=suffix,bytes=path.stat().st_size,sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
            for suffix in ['gzip','dat']:assert files[0][suffix].read_bytes()==files[1][suffix].read_bytes(),suffix
            for runner in runners:runner.close()
            runners=[]
            workflows=[(runtime,suffix,False) for runtime in [0,1] for suffix in ['dat','gzip']]+[(0,'dat',True),(1,'gzip',True)]
            for index,(runtime,suffix,reset) in enumerate(workflows):
                stage=f'workflow {index}: runtime {runtime}, {suffix}, reset {reset}'
                target=root/f'target-{index}';shutil.copytree(baseline,target)
                node=Node(commands[runtime],target,scratch);live.append(node)
                before=ready(node,100);assert before=={k:100 for k in original_keys},before
                archive=files[1-runtime][suffix]
                text=f'reset action:restore archivefile:{archive} file:{backup} password:fixture-archive' if reset else f'archive action:import file:{archive}'
                row=node.ask(text,timeout=300);assert row['status'],row
                imported=row['response']['chainsync'] if reset else row
                if reset:assert row['response']['restore']['status'],row
                assert imported['status'] and imported['response']['archiveresync']['status'],row
                synced=imported['response']['archiveresync']['response'];assert synced['start']=='1' and synced['end']=='1',row
                node.finish();live.remove(node);processes+=1
                assert not (target/'1.1/archiverestore').exists() and not (target/'1.1/restore').exists()
                node=Node(commands[1-runtime],target,scratch);live.append(node)
                uses=263 if reset else 100
                expected={k:uses for k in original_keys};assert key_state(node)==expected
                assert chain(node)==original_chain and coins(node)==original_coins
                public=sorted(expected)[0];signed=node.ask(f'sign publickey:{public} data:0x010203');assert signed['status'],signed
                signature=signed['response'];verified=node.ask(f'verify publickey:{public} data:0x010203 signature:{signature}');assert verified['status'] and verified['response']=='Signature valid',verified
                rejected=node.ask(f'verify publickey:{public} data:0x010204 signature:{signature}');assert not rejected['status'] and rejected['message']=='Signature NOT valid',rejected
                assert key_state(node)=={**expected,public:uses+1}
                node.finish(quit=True);live.remove(node);processes+=1
                checks.append(dict(importer=['java','cpp'][runtime],writer_and_reopen=['java','cpp'][1-runtime],format=suffix,reset_restore=reset,keys=len(expected),uses_after_reopen=uses,chain_and_coins_preserved=True,real_resync_processor=True,graceful_shutdown=True,signs_and_rejects_modified_data=True,signing_use_increment=1))
                print('Verified archive workflow: '+json.dumps(checks[-1]),flush=True)
    except Exception as error:
        failures.append(dict(stage=stage,error=repr(error),nodes=[dict(stdout=node.output[-10000:],stderr=node.errors()) for node in live]))
    finally:
        for node in live:node.cancel()
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,node_processes=processes,exports=exports,checks=checks,failures=failures,scope='Actual pinned Java and native solo nodes with real H2. Valid saved genesis exported identically as H2/raw by both runtimes. Both import formats in both runtimes, real nested resync and worker shutdown, reset restore in both runtimes, opposite-runtime chain/coin/key reopen and signing. Genesis only: cascades, multi-batches, seed replacement, network retries and complete node parity remain unverified.')
    (build/'native-archive-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)

if __name__=='__main__':raise SystemExit(main())
