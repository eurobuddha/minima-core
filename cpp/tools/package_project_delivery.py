#!/usr/bin/env python3
"""Assemble the committed project, teaching material and existing native runtime package."""
import argparse,hashlib,json,os,shutil,subprocess,sys,tarfile,tempfile
from datetime import datetime,timezone
from pathlib import Path
CPP=Path(__file__).resolve().parents[1];ROOT=CPP.parent

def run(*args,cwd=ROOT):
    return subprocess.check_output([str(a) for a in args],cwd=cwd,text=True).strip()

def sha(path):
    digest=hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda:stream.read(1024*1024),b''):digest.update(block)
    return digest.hexdigest()

def export_tree(repo,revision,prefix,destination,paths=None):
    # Export Git objects rather than copying a working tree containing databases.
    with tempfile.TemporaryFile() as stream:
        command=['git','archive','--format=tar','--prefix='+prefix,revision]
        if paths:command+=['--',*paths]
        subprocess.run(command,cwd=repo,stdout=stream,check=True);stream.seek(0)
        with tarfile.open(fileobj=stream,mode='r:') as archive:
            for item in archive.getmembers():
                p=Path(item.name)
                if p.is_absolute() or '..' in p.parts or item.issym() or item.islnk():
                    raise RuntimeError('Unsafe/unexpected source archive member: '+item.name)
            members=archive.getmembers()
            excluded=[item.name for item in members if Path(item.name).name=='.DS_Store']
            archive.extractall(destination,members=[item for item in members if item.name not in excluded],filter='data')
            return excluded

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True);parser.add_argument('--build',type=Path,default=CPP/'build');args=parser.parse_args()
    output=args.output.resolve();archive=output.with_suffix('.zip')
    if output.exists() or archive.exists():parser.error('Choose a new delivery destination')
    if run('git','status','--porcelain','--','cpp'):parser.error('Commit C++ sources, documentation and evidence before delivery')
    head=run('git','rev-parse','HEAD');pin=run('git','rev-parse','HEAD:upstream-study')
    if run('git','rev-parse','HEAD',cwd=ROOT/'upstream-study')!=pin:parser.error('Pinned upstream checkout differs')
    if run('git','status','--porcelain',cwd=ROOT/'upstream-study'):parser.error('Pinned upstream checkout has local edits')
    output.mkdir(parents=True);runtime_parent=output/'runtime';runtime_parent.mkdir()
    runtime=runtime_parent/'MinimaCoreC++-macos-arm64'
    subprocess.run([sys.executable,str(CPP/'tools/package_macos_preview.py'),'--build',str(args.build.resolve()),'--output',str(runtime)],check=True)
    # Keep the standalone runtime ZIP beside the full project ZIP, not duplicated inside it.
    runtime_archive=output.parent/(output.name+'-runtime.zip')
    if runtime_archive.exists() or runtime_archive.with_suffix('.zip.sha256').exists():raise RuntimeError('Standalone runtime archive exists')
    shutil.move(runtime.with_suffix('.zip'),runtime_archive)
    runtime.with_suffix('.zip.sha256').unlink()
    runtime_archive.with_suffix('.zip.sha256').write_text(sha(runtime_archive)+'  '+runtime_archive.name+'\n')
    top=run('git','ls-tree','--name-only',head).splitlines()
    excluded=export_tree(ROOT,head,'project/',output,[p for p in top if p not in ['graphify-out','upstream-study']])
    excluded+=export_tree(ROOT/'upstream-study',pin,'project/upstream-study/',output)
    # Source/docs stay in their repository layout, preserving every relative student link.
    handover=output/'project/cpp/delivery'
    if not (handover/'START_HERE.md').is_file():raise RuntimeError('Delivery overview missing from checkpoint')
    (output/'START_HERE.md').write_text('''# MinimaCoreC++ delivery

The runnable Apple Silicon build is in `runtime/MinimaCoreC++-macos-arm64/`.
Double-click its `Start MinimaCoreC++.command` launcher. The default port is 21001.
The package creates a separate wallet and chain directory on first use.

Start the team handover at [project/cpp/delivery/START_HERE.md](project/cpp/delivery/START_HERE.md).
Student lessons start at [project/cpp/student-guide/README.md](project/cpp/student-guide/README.md).
The complete source snapshot, pinned original Java and validation evidence are under `project/`.
This source archive omits unrelated graphify outputs, Finder .DS_Store metadata and untracked working files.
It contains no `.git` working directory. Source identity and file hashes are in `delivery-manifest.json`.

Full behavioral parity remains unverified. Read the known gaps before relying on this build.
''')
    forbidden=[]
    for path in output.rglob('*'):
        if path.is_file() and (path.suffix=='.db' or path.name.endswith('.mv.db') or path.name in ['.env','.DS_Store']):forbidden.append(str(path.relative_to(output)))
    if forbidden:raise RuntimeError('Unexpected private/runtime file in delivery: '+repr(forbidden))
    manifest=dict(source_head=head,upstream_pin=pin,utc=datetime.now(timezone.utc).isoformat(),full_parity=False,
        source_scope='All committed root entries except unrelated graphify-out and Finder metadata, plus pinned upstream; no untracked files, wallet databases or .git configuration',
        excluded_finder_metadata=excluded,
        runtime_archive=runtime_archive.name,runtime_archive_sha256=sha(runtime_archive),
        file_sha256={str(p.relative_to(output)):sha(p) for p in sorted(output.rglob('*')) if p.is_file()})
    (output/'delivery-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    run('ditto','-c','-k','--keepParent',output,archive)
    archive.with_suffix('.zip.sha256').write_text(sha(archive)+'  '+archive.name+'\n')
    # Read the actual ZIP payloads back, not just the staging files.
    import zipfile
    with zipfile.ZipFile(archive) as contents:
        if contents.testzip() is not None:raise RuntimeError('Delivery ZIP CRC failure')
        for name,digest in manifest['file_sha256'].items():
            if hashlib.sha256(contents.read(output.name+'/'+name)).hexdigest()!=digest:raise RuntimeError('Delivery ZIP hash mismatch: '+name)
        if json.loads(contents.read(output.name+'/delivery-manifest.json'))!=manifest:raise RuntimeError('Manifest differs in ZIP')
    result=dict(folder=str(output),archive=str(archive),sha256=sha(archive),source_head=head,upstream_pin=pin,
        runtime_archive=str(runtime_archive),payload_files=len(manifest['file_sha256']),all_archive_files_verified=True)
    archive.with_suffix('.verification.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
if __name__=='__main__':main()
