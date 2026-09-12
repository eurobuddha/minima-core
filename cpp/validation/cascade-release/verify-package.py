import hashlib,json,os,subprocess,sys,tempfile,zipfile
from pathlib import Path
sys.path.insert(0,str(Path('cpp/tools').resolve()))
from native_node_startup import replies
archive=Path(sys.argv[1]).resolve();report=Path(sys.argv[2]).resolve()
scratch=Path(tempfile.mkdtemp(prefix='Minima cascade replacement relocation '))
subprocess.run(['ditto','-x','-k',str(archive),str(scratch)],check=True)
folders=[p for p in scratch.iterdir() if p.is_dir()];assert len(folders)==1;folder=folders[0]
manifest=json.loads((folder/'manifest.json').read_text())
with zipfile.ZipFile(archive) as contents:
 assert all('/data/' not in n for n in contents.namelist())
 assert json.loads(contents.read(folder.name+'/manifest.json'))==manifest
for name,digest in manifest['file_sha256'].items():assert hashlib.sha256((folder/name).read_bytes()).hexdigest()==digest,name
assert not (folder/'data').exists();launcher=folder/'Start MinimaCoreC++.command';assert os.access(launcher,os.X_OK)
env={k:v for k,v in os.environ.items() if not k.lower().startswith(('minima_','dyld_'))};env['PATH']='/usr/bin:/bin:/usr/sbin:/sbin'
help_result=subprocess.run([str(launcher),'-help'],cwd='/',env=env,capture_output=True,text=True,timeout=30)
assert help_result.returncode==1 and 'Minima Help' in help_result.stdout and not help_result.stderr
checks=[];previous=None
for step in ['fresh','reopen']:
 command=[str(launcher),'-connect','','-solo','-noconnect','-port','0','-hashtest','1000']
 process=subprocess.run(command,input='hash data:hello\nstatus\nquit\n',cwd='/',env=env,capture_output=True,text=True,timeout=150)
 assert process.returncode==0 and 'Shut down completed OK..' in process.stdout,(process.returncode,process.stderr)
 assert 'ERROR' not in process.stdout+process.stderr
 rows=replies(process.stdout);assert [r['command'] for r in rows]==['hash','status'] and all(r['status'] for r in rows)
 assert rows[0]['response']['hash']=='0x3338BE694F50C5F338814986CDF0686453A888B84F424D792AF4B9202398F392'
 status=rows[1]['response'];assert status['chain']['block']==1 and status['network']['connected']==0 and status['network']['connecting']==0
 assert Path(status['data']).resolve()==(folder/'data/1.1').resolve()
 snapshot={k:status[k] for k in ['version','locked','minima','coins','length','weight']};snapshot['chain']={k:status['chain'][k] for k in ['block','hash','difficulty','length','branches','weight']}
 if previous is not None:assert snapshot==previous
 previous=snapshot
 for name in ['walletsql/wallet.mv.db','archivesql/archive.mv.db','txpowsql/txpow.mv.db','cascade.db','chaintree.db']:assert (folder/'data/1.1/databases'/name).stat().st_size>0
 checks.append(dict(step=step,passed=True,chain_height=1,clean_shutdown=True,isolated_wallet=True))
uuid=subprocess.check_output(['dwarfdump','--uuid',str(folder/'bin/MinimaCoreC++')],text=True).split()[1]
result=dict(passed=True,source_head=manifest['source_head'],binary_uuid=uuid,archive_sha256=hashlib.file_digest(archive.open('rb'),'sha256').hexdigest(),manifest_files_verified=len(manifest['file_sha256']),system_only_path=True,help_passed=True,observations=checks,scope='Actual shipped ZIP and launcher relocated to a path containing spaces; empty connect override and solo fixture keep this verification offline; hash/status/quit, durable databases and exact stable chain fields after reopen. No mainnet-tip or overnight-stability claim. Reuses verify_macos_preview.py checks and native_node_startup.py reply parser and assertions.',relocated_folder=str(folder))
report.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
