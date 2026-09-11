#!/usr/bin/env python3
"""Actual Java/native nodes: all default keys, backup, opposite-runtime restore, reopen."""
import argparse,json,os,select,shutil,subprocess,tempfile,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from native_node_startup import replies
class Node:
    def __init__(self,command,data,cwd):
        env={k:v for k,v in os.environ.items() if not k.lower().startswith('minima_')};env.update(TZ='UTC',UBSAN_OPTIONS='halt_on_error=1')
        self.err=tempfile.TemporaryFile(mode='w+t');self.output='';self.pending=''
        args=['-data',str(data),'-solo','-noconnect','-host','127.0.0.1','-port','0','-hashtest','1000','-nossl']
        self.child=subprocess.Popen(command+args,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=self.err,bufsize=0,env=env,cwd=cwd)
    def errors(self):self.err.seek(0);return self.err.read()[-6000:]
    def ask(self,text,timeout=180):
        self.child.stdin.write((text+'\n').encode());self.child.stdin.flush();deadline=time.monotonic()+timeout;name=text.split()[0]
        while time.monotonic()<deadline:
            rows=replies(self.pending)
            if rows:
                assert len(rows)==1 and rows[0]['command']==name,rows
                self.pending='';return rows[0]
            if not select.select([self.child.stdout],[],[],min(1,max(0,deadline-time.monotonic())))[0]:continue
            raw=os.read(self.child.stdout.fileno(),65536)
            if not raw:raise RuntimeError('Node exited: '+self.output[-6000:]+self.errors())
            value=raw.decode('utf-8',errors='replace');self.pending+=value;self.output+=value
        raise TimeoutError('Node command timeout: '+text+' '+self.output[-3000:])
    def finish(self,quit=False):
        if quit:self.child.stdin.write(b'quit\n');self.child.stdin.flush()
        output,_=self.child.communicate(timeout=60);self.output+=output.decode('utf-8',errors='replace');assert self.child.returncode==0,(self.output[-6000:],self.errors());self.err.close()
    def cancel(self):
        if self.child.poll() is None:self.child.kill();self.child.wait(timeout=10)
        self.err.close()
def key_state(node):
    row=node.ask('keys');assert row['status'],row;return {r['publickey']:r['uses'] for r in row['response']['keys']}
def ready(node,uses):
    result=node.ask(f'keys action:createallkeys keyuses:{uses}');assert result['status'],result;deadline=time.monotonic()+1200;last=-1
    while time.monotonic()<deadline:
        keys=key_state(node)
        if len(keys)!=last:print(f'Actual node key creation: {len(keys)} keys',flush=True);last=len(keys)
        if len(keys)>=8 and all(int(u)==uses for u in keys.values()):
            gate=node.ask('backup auto:false');assert gate['status'],gate;return keys
        time.sleep(1)
    raise TimeoutError('Default keys did not finish')
def chain(node):
    row=node.ask('status');assert row['status'],row;status=row['response'];return {k:status['chain'][k] for k in ['block','hash','length','branches','weight']}
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--restore-command',choices=['restore','restoresync'],default='restore');args=parser.parse_args();build=args.build.resolve();restore_command=args.restore_command
    oracle=build/f'{restore_command}-node-oracle';oracle.mkdir(exist_ok=True);listing=build/f'{restore_command}-node-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sorted((JAVA/'src').rglob('*.java')))+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.Minima'],[str(build/'MinimaCoreC++')]];checks=[];failures=[];live=[];processes=0
    try:
        with tempfile.TemporaryDirectory(prefix='restore-node-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline';original=None
            for writer in [0,1]:
                source=baseline if writer==0 else root/'native-source'
                if writer:shutil.copytree(baseline,source)
                node=Node(commands[writer],source,scratch);live.append(node);keys=ready(node,7);current=chain(node)
                if original is None:original=current
                else:assert current==original,(current,original)
                backup=root/f'backup-{writer}.bak';result=node.ask(f'backup file:{backup} password:fixture-restore');assert result['status'] and backup.is_file(),result
                node.finish(quit=True);live.remove(node);processes+=1
                target=root/f'target-{writer}';shutil.copytree(baseline,target)
                reader=1-writer;node=Node(commands[reader],target,scratch);live.append(node);changed=ready(node,100);assert set(changed)==set(keys)
                host=' host:invalid-host' if restore_command=='restoresync' and writer==1 else '';result=node.ask(f'{restore_command} file:{backup} password:fixture-restore'+host,timeout=240);assert result['status'] and 'restore' in result,result
                node.finish();live.remove(node);processes+=1
                assert not (target/'1.1/restore').exists()
                node=Node(commands[writer],target,scratch);live.append(node);restored=key_state(node)
                assert restored=={k:int(v)+256 for k,v in keys.items()},dict(expected_count=len(keys),restored_count=len(restored),uses=sorted(set(restored.values())))
                assert chain(node)==original
                public=sorted(restored)[0];signed=node.ask(f'sign publickey:{public} data:0x010203');assert signed['status'],signed
                signature=signed['response'];verified=node.ask(f'verify publickey:{public} data:0x010203 signature:{signature}');assert verified['status'] and verified['response']=='Signature valid',verified
                rejected=node.ask(f'verify publickey:{public} data:0x010204 signature:{signature}');assert not rejected['status'] and rejected['message']=='Signature NOT valid',rejected
                after_sign=key_state(node);assert after_sign=={**restored,public:264},after_sign
                node.finish(quit=True);live.remove(node);processes+=1
                checks.append(dict(writer=['java','cpp'][writer],restorer=['java','cpp'][reader],reopen=['java','cpp'][writer],keys=len(keys),backup_uses=7,destination_uses_before=100,restored_uses=263,chain_preserved=True,real_worker_shutdown=True,restored_key_signs=True,altered_data_rejected=True,signing_use_count=264));print('Verified restore direction: '+json.dumps(checks[-1]),flush=True)
    except Exception as error:failures.append(dict(error=repr(error)))
    finally:
        for node in live:node.cancel()
    report=dict(passed=not failures,restore_command=restore_command,recent_invalid_host_ignored=restore_command=='restoresync' and len(checks)==2,node_processes=processes,checks=checks,failures=failures,scope='Actual unmodified pinned Java Minima and native MinimaCoreC++ with real H2, all eight solo-mode default keys, both backup/restore directions, real Main worker shutdown, persisted key-use increments and original chain on opposite-runtime reopen. For restoresync the recent-backup path is tested with absent and invalid host; remote sync remains unverified here. Plaintext SQL storage and password-encrypted backups; public network, encrypted wallet database, damaged running-node restores and full parity remain unverified.')
    (build/f'native-{restore_command}-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
