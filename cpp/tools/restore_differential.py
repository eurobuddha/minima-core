#!/usr/bin/env python3
"""Restore exact source files into copied real H2 databases, observing partial state."""
import argparse,copy,gzip,hashlib,json,shutil,struct,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def frames(data):
    result=[];offset=0
    while offset<len(data):
        start=offset;size=struct.unpack_from('>i',data,offset)[0];offset+=4
        assert 0<=size<=len(data)-offset
        result.append((start,data[offset:offset+size]));offset+=size
    assert len(result)==6;return result

def corpus(valid,cppvalid,plain,crypto):
    result=[]
    def encrypted(data,password='minima',compressed=False):
        value=crypto.ask(dict(op='make',salt='0x0102030405060708',iv='0x000102030405060708090A0B0C0D0E0F',password=password,compressed='0x'+(data if compressed else gzip.compress(data,mtime=0)).hex()));assert 'data' in value,value;return bytes.fromhex(value['data'][2:])
    def add(data=valid,params=None,**kw):result.append(dict(data=data,params=dict(file='input.bak') if params is None else params,**kw))
    add(op='meta');add(expected=True);add(params=dict(file='input.bak',shutdown='false'),expected=True);add(cppvalid,expected=True)
    for key in ['file','password','shutdown']:
        for value in [None,3,False,'','   ','invalid']:
            p=dict(file='input.bak');p[key]=value;add(params=p)
    add(params={});add(params=dict(file='missing'));add(params=dict(file='directory'),directory='input')
    for params in [{},{'file':None},{'file':'input.bak'},{'file':'input.bak','shutdown':'false'}]:add(params=params,keys=False)
    add(readyFail=True);add(realReady=True)
    for password in ['secret','π😄中文','a\x00b']:
        add(encrypted(plain,password),dict(file='input.bak',password=password),expected=True)
        add(encrypted(plain,password),dict(file='input.bak',password='wrong'))
    for length in [0,1,3,4,7,11,12,15,16,31,32,33,len(valid)-1,len(valid)-16]:add(valid[:length],label='cipher-prefix')
    for header in [b'\xff'*4,struct.pack('>i',536870913),struct.pack('>i',0)+valid[12:],valid[:12]+struct.pack('>i',0)]:add(header,label='outer-frame')
    for directory in ['restore','wallet.sql','cascade.db','chaintree.db','userprefs.db','p2p.db']:add(directory=directory,label='destination-directory')
    sections=frames(plain)
    for index,(start,section) in enumerate(sections):
        for cut in sorted(set([start,start+1,start+3,start+4,start+4+len(section)//2,start+4+len(section)-1])):
            add(encrypted(plain[:cut]),label=f'truncated-section-{index}')
        for length in [-1,536870913]:add(encrypted(plain[:start]+struct.pack('>i',length)+plain[start+4:]),label=f'invalid-length-{index}')
    def replace(index,data):return b''.join(struct.pack('>i',len(data if i==index else section))+(data if i==index else section) for i,(_,section) in enumerate(sections))
    for sql in [b'',b'not SQL;',b'DROP ALL OBJECTS;',b'CREATE TABLE fixture_only(id INT);']:
        add(encrypted(replace(0,sql)),label='wallet-sql-failure')
    for history in [b'',b'\x00',struct.pack('>i',1),struct.pack('>i',-1),struct.pack('>i',0)]:add(encrypted(replace(5,history)),label='history-payload')
    compressed=gzip.compress(plain,mtime=0)
    for changed in [compressed[:-1],compressed[:-8],compressed[:-8]+bytes(8),compressed+b'trailing',compressed+gzip.compress(b'extra',mtime=0)]:add(encrypted(changed,compressed=True),label='gzip-tail')
    add(encrypted(plain+b'extra section'),expected=True,label='ignored-extra-plaintext')
    for command in ['restore file:input.bak shutdown:false','restore file:input.bak bad:x','restore','restore file:input.bak password:wrong']:
        add(op='runner',command=command)
    return result

def project(value):
    result=copy.deepcopy(value)
    if 'exception' in result:
        for name in ['java.io.EOFException','java.lang.NullPointerException','java.lang.ClassCastException']:
            if result['exception'].startswith(name):result['exception']=name
    return result

def snapshot(folder):
    files={}
    for path in sorted(folder.rglob('*')):
        relative=str(path.relative_to(folder))
        if relative=='input.bak':continue
        if path.is_dir():files[relative]='directory'
        elif '.mv.db' not in path.name and '.trace.db' not in path.name:
            data=path.read_bytes();files[relative]=dict(size=len(data),sha256=hashlib.sha256(data).hexdigest())
    return files

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'restore-oracle';oracle.mkdir(exist_ok=True);listing=build/'restore-sources.txt';sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['RestoreOracle.java','DecryptBackupOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{JAVA}/jar/minima.jar';runners=[Runner(['java','-cp',cp,'RestoreOracle']),Runner([str(build/'restore_runtime')])];crypto=Runner(['java','-cp',cp,'DecryptBackupOracle'])
    failures=[];completed=successes=partial=0;observations=[]
    try:
        with tempfile.TemporaryDirectory(prefix='restore-compare-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline';baseline.mkdir();assert runners[0].ask(dict(op='seed',role='target',folder=str(baseline))).get('seeded')
            sources=[]
            for i,runner in enumerate(runners):
                source=root/f'source-{i}';source.mkdir();assert runner.ask(dict(op='seed',role='source',folder=str(source))).get('seeded')
                sources.append((source/'source.bak').read_bytes())
            source=root/'source-0';answer=crypto.ask(dict(folder=str(source),params=dict(file='source.bak',output='plain.bin')));assert 'result' in answer,answer;plain=(source/'plain.bin').read_bytes()
            for index,case in enumerate(corpus(*sources,plain,crypto)):
                values=[];folder=root/'node'
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    shutil.copytree(baseline,folder);(folder/'input.bak').write_bytes(case['data'])
                    directory=case.get('directory')
                    if directory:
                        path=folder/('directory' if directory=='input' else 'restore' if directory=='restore' else 'restore/wallet.sql' if directory=='wallet.sql' else 'databases/'+directory)
                        if path.is_file():path.unlink()
                        path.mkdir(parents=True,exist_ok=True);(path/'child').write_text('fixture child')
                    request={k:v for k,v in case.items() if k in ['op','params','keys','readyFail','realReady','command']};request['folder']=str(folder)
                    value=project(runner.ask(request));assert 'fixtureError' not in value,value
                    values.append(dict(reply=value,files=snapshot(folder)))
                if values[0]!=values[1]:failures.append(dict(case=index,label=case.get('label'),params=case['params'],java=values[0],cpp=values[1]));break
                result=values[0]['reply'];successful='result' in result and case.get('op')!='runner'
                if case.get('expected'):
                    assert successful,result
                    assert result['user']['fixture_marker']=='source' and result['user']['minidapps_uninstalled']==[],result
                    assert result['keys']['rows']==[{'PUBLICKEY':'0x01','USES':'261'},{'PUBLICKEY':'0x02','USES':'356'}],result['keys']
                    assert len(result['history']['rows'])==6 and all(r['ISRELEVANT']=='1' for r in result['history']['rows']),result['history']
                    assert result['allowSave'] and not result['archiveFile'] and 'restore' not in values[0]['files'],result
                if successful:successes+=1
                if not result['allowSave']:partial+=1
                observations.append(dict(case=index,label=case.get('label'),success=successful,allow_save=result['allowSave'],restoring=result['restoring'],temporary_folder='restore' in values[0]['files']))
                completed+=1
            if not failures:assert successes>=8 and partial>=15,(successes,partial)
    except Exception as error:failures.append(dict(case=completed,error=repr(error)))
    finally:
        for runner in [*runners,crypto]:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,successful_restores=successes,save_disabled_cases=partial,observations=observations,failures=failures,scope='Real H2 source/target stores with controlled Main.restoreReady and real shutdown notification; Java- and C++-generated backups; exact restored SQL rows/key uses, source state, files, caches, closure flags and partial-failure effects. Direct EOF/NPE/class-cast diagnostic prose normalized. SQL observations can reopen after recording cache/closure state. Real running-node shutdown is a separate process test; full parity remains unverified.')
    (build/'restore-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
