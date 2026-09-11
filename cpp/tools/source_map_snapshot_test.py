#!/usr/bin/env python3
"""Verify exported mapping checks without Git or the original reuse catalogue."""
import hashlib,json,shutil,subprocess,sys,tempfile
from pathlib import Path
CPP=Path(__file__).resolve().parents[1]
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    checks=[]
    with tempfile.TemporaryDirectory(prefix='source-map-snapshot-') as temporary:
        folder=Path(temporary);root=folder/'project';cpp=root/'cpp';java=root/'upstream-study'
        (cpp/'tools').mkdir(parents=True);shutil.copy2(CPP/'tools/source_map.py',cpp/'tools/source_map.py')
        for name in ['source-map.json','SOURCE_MAP.md']:shutil.copy2(CPP/name,cpp/name)
        mapping=json.loads((cpp/'source-map.json').read_text())
        for row in mapping['files']:
            target=java/row['java'];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(CPP.parent/'upstream-study'/row['java'],target)
            if row['cpp_exists']:
                target=cpp/row['cpp'];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(CPP/row['cpp'],target)
        manifest=dict(upstream_pin=mapping['commit'],file_sha256={str(p.relative_to(folder)):digest(p) for p in folder.rglob('*') if p.is_file()})
        manifest_path=folder/'delivery-manifest.json';encoded=json.dumps(manifest);manifest_path.write_text(encoded)
        command=[sys.executable,str(cpp/'tools/source_map.py')]
        def check(name,flag,expected,needle):
            result=subprocess.run(command+([flag] if flag else []),cwd=folder,capture_output=True,text=True)
            assert result.returncode==expected and needle in result.stdout+result.stderr,(name,result.returncode,result.stdout,result.stderr)
            checks.append(name)
        check('valid exported source and frozen search provenance','--check',0,'456 counterparts')
        check('full parity remains incomplete','--require-complete',1,'457 files lack verified parity')
        manifest_path.unlink();check('missing manifest rejected','--check',1,'snapshot checks require');manifest_path.write_text(encoded)
        manifest['upstream_pin']='wrong';manifest_path.write_text(json.dumps(manifest));check('wrong pin rejected','--check',1,'source pin differs');manifest_path.write_text(encoded)
        source=java/mapping['files'][0]['java'];before=source.read_bytes();source.write_bytes(before+b'\n');check('modified Java rejected','--check',1,'Java source hash differs');source.write_bytes(before)
        added=java/'Added.java';added.write_text('class Added {}');check('extra Java rejected','--check',1,'Java file inventory differs');added.unlink()
        source.unlink();check('missing Java rejected','--check',1,'Java file inventory differs');source.write_bytes(before)
        map_path=cpp/'source-map.json';before_map=map_path.read_bytes();map_path.write_bytes(before_map+b'\n');check('modified mapping rejected','--check',1,'mapping hash differs');map_path.write_bytes(before_map)
        row=next(row for row in mapping['files'] if row['cpp_exists']);target=cpp/row['cpp'];before_cpp=target.read_bytes();target.unlink();check('missing native counterpart rejected','--check',1,'Stale map');target.write_bytes(before_cpp)
        check('snapshot cannot refresh source inventory','',1,'Source refresh requires')
    print(json.dumps(dict(passed=True,checks=checks,scope='Exported source-map checks and deliberate tampering; no port parity claim'),indent=2))
if __name__=='__main__':main()
