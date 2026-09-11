#!/usr/bin/env python3
"""Compare decryptbackup results and every fixture file, including partial output."""
import argparse,gzip,json,random,shutil,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def hx(data):return '0x'+data.hex()
def project(value):
    # Existing native stream/parameter boundaries preserve categories but not
    # JVM-specific EOF, helpful-NPE or class-cast diagnostic prose.
    value=dict(value)
    error=value.get('exception','')
    for category in ['java.io.EOFException','java.lang.NullPointerException','java.lang.ClassCastException']:
        if error==category or error.startswith(category+':'):value['exception']=category
    return value

def corpus(java):
    result=[];rng=random.Random(5564)
    def encrypt(data,password='minima'):
        value=java.ask(dict(op='make',salt='0x0102030405060708',iv='0x000102030405060708090A0B0C0D0E0F',password=password,compressed=hx(data)))
        assert 'data' in value,value
        return bytes.fromhex(value['data'][2:])
    def add(data=None,params=None,expected=None,**extra):
        result.append(dict(data=data,params=dict(file='input.bak') if params is None else params,expected=expected,**extra))
    valid=encrypt(gzip.compress(b'fixture payload',mtime=0))
    result.append(dict(op='meta',data=None,params={}))
    for params in [{},{'file':None},{'file':3},{'file':False},{'file':''},{'file':'   '},{'file':'missing'},
                   {'file':'input.bak','password':None},{'file':'input.bak','password':''},{'file':'input.bak','password':'  '},
                   {'file':'input.bak','output':None},{'file':'input.bak','output':''}]:add(valid,params)
    for password in ['minima','custom password','π😄中文','a\x00b']:
        payload=b'Unicode password payload';data=encrypt(gzip.compress(payload,mtime=0),password)
        add(data,dict(file='input.bak',password=password),payload)
        add(data,dict(file='input.bak',password='wrong'),label='wrong-password')
    add(valid,dict(file='input.bak',output='output.bin'),b'fixture payload',existing=True)
    add(valid,dict(file='input.bak',output='nested/output.bin'),b'fixture payload')
    add(valid,dict(file='input.bak',output='input.bak'),label='same-file')
    add(valid,dict(file='input.bak',output='directory'),directory='empty',expected=b'fixture payload')
    add(valid,dict(file='input.bak',output='directory'),directory='nonempty')
    add(None,dict(file='directory'),directory='nonempty')
    for length in [0,1,2,3,4,5,11,12,15,16,31,35,36,37,len(valid)-1]:add(valid[:length],label='truncated-frame-or-cipher')
    for frame in [b'\xff'*4,b'\x20\x00\x00\x01',b'\x00'*4+valid[12:],valid[:12]+b'\x00'*4,valid[:12]+b'\x00\x00\x00\x0f'+bytes(15),valid[:12]+b'\x00\x00\x00\x11'+bytes(17)]:add(frame,label='invalid-frame')
    for length in [0,1,15,16,31,255,511,512,513,8191,8192,8193,16383,16384,16385,65536,100000]:
        for payload in [bytes(length),rng.randbytes(length)]:
            compressed=gzip.compress(payload,mtime=0);data=encrypt(compressed);add(data,expected=payload,label='valid')
            for cut in sorted({0,1,7,15,16,17,len(data)//2}):add(data[:-cut] if cut else data+b'\x00',label='damaged-cipher',original=payload,truncated=cut!=0)
            damaged=bytearray(data);damaged[-1]^=255;add(bytes(damaged),label='padding')
            if length in [0,512,8192,65536]:
                for stream in [compressed[:-1],compressed[:-8],compressed[:-8]+bytes(8),compressed[:10],compressed[:len(compressed)//2],compressed+b'trailing',compressed+gzip.compress(b'second',mtime=0)]:
                    add(encrypt(stream),label='damaged-or-concatenated-gzip')
    for data in [b'',b'x',bytes(100),b'\x1f\x8b',b'\x1f\x8b\x09'+bytes(30),b'\x1f\x8b\x08\x08'+bytes(6)+b'unterminated']:
        add(encrypt(data),label='gzip-header')
    for command,caller in [('decryptbackup file:input.bak','0x00'),('decryptbackup file:input.bak','0x1234'),('decryptbackup file:input.bak bad:x','0x00'),('decryptbackup','0x00'),('decryptbackup file:input.bak password:wrong','0x00'),('decryptbackup file:input.bak;hash data:hello','0x00')]:
        result.append(dict(data=valid,op='runner',command=command,caller=caller,params={}))
    return result

def prepare(folder,case):
    if folder.exists():shutil.rmtree(folder)
    folder.mkdir()
    if case.get('data') is not None:(folder/'input.bak').write_bytes(case['data'])
    if case.get('existing'):(folder/'output.bin').write_bytes(b'old output')
    if case.get('directory'):
        (folder/'directory').mkdir()
        if case['directory']=='nonempty':(folder/'directory/child').write_bytes(b'child')

def snapshot(folder):
    return {str(p.relative_to(folder)):('directory' if p.is_dir() else p.read_bytes().hex()) for p in sorted(folder.rglob('*'))}

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'decrypt-backup-oracle';oracle.mkdir(exist_ok=True);listing=build/'decrypt-backup-sources.txt'
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/DecryptBackupOracle.java'];listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','DecryptBackupOracle']),Runner([str(build/'decrypt_backup_runtime')])]
    failures=[];completed=partial=0
    try:
        cases=corpus(runners[0])
        with tempfile.TemporaryDirectory(prefix='decrypt-backup-',dir=build) as scratch:
            folder=Path(scratch)/'files'
            for index,case in enumerate(cases):
                request={k:v for k,v in case.items() if k in ['op','params','command','caller']};request['folder']=str(folder)
                # MiniFile treats a slash-containing name as an explicit path,
                # independent of BASE_FILE_FOLDER. Keep that fixture isolated.
                output_name=request.get('params',{}).get('output')
                if isinstance(output_name,str) and '/' in output_name:
                    request['params']={**request['params'],'output':str(folder/request['params']['output'])}
                values=[]
                for runner in runners:
                    prepare(folder,case);value=project(runner.ask(request));files=snapshot(folder)
                    if case.get('expected') is not None:
                        target=case['params'].get('output','decrypted-input.bak')
                        assert 'result' in value and files[target]==case['expected'].hex(),dict(case=index,result=value,files=files)
                    values.append(dict(reply=value,files=files))
                if values[0]!=values[1]:failures.append(dict(case=index,label=case.get('label'),request=request,input_hex=case['data'].hex() if case.get('data') is not None else None,java=values[0],cpp=values[1]));break
                if case.get('label')=='damaged-cipher' and 'result' in values[0]['reply']:
                    payload=bytes.fromhex(values[0]['files']['decrypted-input.bak'])
                    if case['truncated']:
                        assert case['original'].startswith(payload),'Truncation changed already decoded bytes'
                    if 0<len(payload)<len(case['original']):partial+=1
                completed+=1
            if not failures:assert partial>0,'No partial-success behavior exercised'
    except Exception as error:failures.append(dict(error=repr(error),case=completed))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,partial_success_cases=partial,failures=failures,scope='Unmodified pinned decryptbackup metadata/direct calls and real registry, exact output snapshots, password/framing gates, output-before-read ordering, aliasing, directories, Unicode passwords, empty/large/random payloads, damaged CBC padding, truncated/corrupt/concatenated gzip and source-swallowed payload failures. Only EOF/NPE/class-cast diagnostic prose normalized for direct exceptions. Virtual threads, resource exhaustion, special-device write failures and private unused helpers remain unverified.')
    (build/'decrypt-backup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[str(f)[:4000] for f in failures]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
