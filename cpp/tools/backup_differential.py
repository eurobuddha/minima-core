#!/usr/bin/env python3
"""Pinned Java/native backup command with real H2 and four-way cross-decryption."""
import argparse,copy,gzip,hashlib,json,random,re,shutil,struct,subprocess,tempfile,time
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def cases():
    result=[dict(op='meta',params={})]
    def add(params=None,**extra):result.append(dict(params=dict(file='output.bak') if params is None else params,**extra))
    add();add({});add(tip=True);add(dict(file='output.bak',auto='true'));add(dict(auto='false'),autoInitial=True)
    for key in ['file','password','confirm','auto','complete','debug','maxhistory']:
        for value in [None,3,False,'','   ','invalid']:
            add(dict(file='output.bak',**{key:value}) if key!='file' else dict(file=value))
    for password in ['custom password','π😄中文','a\x00b']:
        add(dict(file='output.bak',password=password,confirm=password));add(dict(file='output.bak',password=password,confirm='wrong'))
    for value in ['0','1','3','100','-1','2.9','4294967296','2147483648']:
        add(dict(file='output.bak',maxhistory=value))
    for params in [dict(auto='false',password=None,file=None),dict(auto='true',password=None),dict(auto='false'),dict(file='output.bak')]:
        add(params,keys=False);add(params,autoInitial=True)
    for file in ['directory','missing/child.bak','backup/wallet.sql','backup/inside.bak','with space.bak','nul\x00file']:
        add(dict(file=file),directory=True)
    for name in ['wallet.sql','cascade.bak','chaintree.bak','userdb.bak','p2p.bak']:
        add(obstruct=name)
    for value in ['true','false']:
        add(dict(file='output.bak',complete=value));add(dict(file='output.bak',debug=value))
    rng=random.Random(5564)
    for length in [511,512,8192,65536,100000]:
        add(payload=rng.randbytes(length).hex())
    for command in ['backup file:output.bak','backup auto:false','backup file:output.bak complete:true','backup file:output.bak bad:x','backup auto:true password:wrong confirm:no','backup file:output.bak;hash data:hello']:
        add(op='runner',command=command)
    return result

def normalize(value):
    value=copy.deepcopy(value)
    if 'exception' in value:
        for category in ['java.lang.NullPointerException','java.lang.ClassCastException']:
            for prefix in ['', 'org.minima.system.commands.CommandException: ']:
                if value['exception'].startswith(prefix+category):value['exception']=prefix+category
    return value

def sections(data):
    result=[];offset=0
    for name in ['wallet','cascade','chain','user','p2p','txpow']:
        size=struct.unpack_from('>i',data,offset)[0];offset+=4
        assert 0<=size<=len(data)-offset,(name,size,len(data),offset)
        value=data[offset:offset+size];offset+=size
        result.append(dict(name=name,size=size,sha256=hashlib.sha256(value).hexdigest()))
        if name=='wallet':assert b'CREATE' in value and b'SEED' in value.upper(), 'Missing actual wallet SQL'
        if name=='txpow':result[-1]['rows']=struct.unpack_from('>i',value)[0]
    assert offset==len(data),(offset,len(data));return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'backup-oracle';oracle.mkdir(exist_ok=True);listing=build/'backup-sources.txt'
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['BackupOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','BackupOracle']),Runner([str(build/'backup_runtime')])]
    failures=[];completed=backups=decryptions=stream_cases=0;evidence=[]
    try:
        with tempfile.TemporaryDirectory(prefix='backup-compare-',dir=build) as scratch:
            root=Path(scratch)
            rng=random.Random(9876)
            for length in [0,1,15,16,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,512,513,8192,16384,65536,100000]:
                for data in [bytes(length),rng.randbytes(length)]:
                    parts=[data[:4],data[4:511],b'',data[511:]]
                    for fail,after in [(0,False),(0,True),(1,False),(2,False),(3,False),(15,False)]:
                        request=dict(op='stream',folder=str(root),parts=['0x'+p.hex() for p in parts],fail=fail,after=after)
                        values=[r.ask(request) for r in runners];assert values[0]==values[1],dict(stream_case=stream_cases,request=request,java=values[0],cpp=values[1])
                        if 'exception' not in values[0]:assert gzip.decompress(bytes.fromhex(values[0]['data'][2:]))==data
                        stream_cases+=1
            baseline=root/'baseline';folder=root/'node';baseline.mkdir()
            seeded=runners[0].ask(dict(op='seed',folder=str(baseline)));assert seeded.get('seeded'),seeded
            for index,case in enumerate(cases()):
                values=[];plaintexts=[]
                for runtime,runner in enumerate(runners):
                    if folder.exists():shutil.rmtree(folder)
                    shutil.copytree(baseline,folder)
                    (folder/'output.bak').write_bytes(b'previous backup fixture')
                    request=copy.deepcopy(case);request['folder']=str(folder)
                    name=request['params'].get('file')
                    if isinstance(name,str) and '/' in name:request['params']['file']=str(folder/name)
                    if case.get('directory'):
                        (folder/'directory').mkdir();(folder/'directory/child').write_text('fixture child')
                    if case.get('obstruct'):
                        path=folder/'backup'/case['obstruct'];path.mkdir(parents=True);(path/'child').write_text('fixture child')
                    before=int(time.time()*1000);reply=normalize(runner.ask(request));after=int(time.time()*1000)
                    assert 'fixtureError' not in reply,reply
                    result=reply.get('result',{});result=result[0] if isinstance(result,list) else result
                    description=result.get('backup',{});output=description.get('file');decoded=None;plain=None
                    if output:
                        output=Path(output)
                        if 'file' not in case['params']:
                            stamp=int(re.fullmatch(r'minima-backup-(\d+)\.bak',output.name)[1]);assert before<=stamp<=after
                            description['file']=str(folder/'minima-backup-TIMESTAMP.bak')
                        if output.is_file():
                            encoded=output.read_bytes();assert encoded[:4]==struct.pack('>i',8) and encoded[12:16]==struct.pack('>i',16)
                            assert len(encoded)>32 and (len(encoded)-32)%16==0
                            raw=[]
                            for decoder in runners:
                                target=root/'decoded.bin';observation=decoder.ask(dict(op='decode',folder=str(folder),file=str(output),output=str(target),password=case['params'].get('password','minima')))
                                assert 'data' in observation,observation
                                raw.append(bytes.fromhex(observation['data'][2:]));decryptions+=1
                            assert raw[0]==raw[1],'Cross-decryption mismatch'
                            plain=raw[0];decoded=sections(plain);backups+=1
                            if runtime==0:evidence.append(dict(case=index,sections=decoded))
                    plaintexts.append(plain)
                    saved={name:hashlib.sha256((folder/'databases'/name).read_bytes()).hexdigest() for name in ['userprefs.db','cascade.db','chaintree.db','p2p.db','p2p2.db'] if (folder/'databases'/name).is_file()}
                    previous=folder/'output.bak';file_state=None
                    if previous.is_file():
                        content=previous.read_bytes();file_state=dict(size=len(content),previous=content==b'previous backup fixture')
                    values.append(dict(reply=reply,decoded=decoded,backup_folder=(folder/'backup').exists(),output_exists=bool(output and output.exists()),saved=saved,destination=file_state))
                if plaintexts[0]!=plaintexts[1]:raise AssertionError('Java/native backup plaintext bytes differ')
                if values[0]!=values[1]:failures.append(dict(case=index,request=case,java=values[0],cpp=values[1]));break
                assert values[0]['reply']['lock']=='[Write locks = 0, Read locks = 0]',values[0]['reply']
                if index in [1,2,3]:assert values[0]['decoded'],'Expected a working backup'
                completed+=1
            if not failures:assert backups>=30 and decryptions==backups*2,(backups,decryptions)
    except Exception as error:failures.append(dict(case=completed,error=repr(error)))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,stream_cases=stream_cases,backups=backups,cross_decryptions=decryptions,failures=failures,evidence=evidence,scope='Real H2, pinned Java/native command metadata, parameters, key gate, auto side effects, history limits, six sections, registry, lock counts and cleanup. Native and Java decrypt every surviving file; source-random encryption compared through exact plaintext. Direct and wrapped NPE/class-cast prose and validated default timestamp names normalized. Full API, concurrent mutations, resource exhaustion and special-device failures remain uncertified.')
    (build/'backup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='evidence'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
