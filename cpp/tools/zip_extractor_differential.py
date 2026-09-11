#!/usr/bin/env python3
"""Compare pinned ZIP extraction, ordered logs, partial files and stream effects."""
import argparse
import collections
import hashlib
import io
import json
import os
from pathlib import Path
import random
import shutil
import struct
import subprocess
import tempfile
import zipfile
import zlib
from peers_checker_differential import Runner

ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'


def local(name=b'file',data=b'payload',method=8,flags=0,extra=b'',descriptor=True,wide=False,
          crc=None,size=None,compressed_size=None,payload=None):
    if isinstance(name,str):name=name.encode()
    if payload is None:
        compressor=zlib.compressobj(wbits=-15)
        payload=compressor.compress(data)+compressor.flush() if method==8 else data
    actual_crc=zlib.crc32(data)
    checksum=actual_crc if crc is None else crc
    uncompressed=len(data) if size is None else size
    compressed=len(payload) if compressed_size is None else compressed_size
    header=struct.pack('<IHHHHHIIIHH',0x04034b50,45 if wide else 20,flags,method,0,0,
                       0 if flags&8 else checksum,0xffffffff if wide else (0 if flags&8 else compressed),
                       0xffffffff if wide else (0 if flags&8 else uncompressed),len(name),len(extra))
    trailer=b''
    if flags&8:
        trailer=(struct.pack('<I',0x08074b50) if descriptor else b'')
        trailer+=struct.pack('<IQQ' if wide else '<III',checksum,compressed,uncompressed)
    return header+name+extra+payload+trailer


def corpus():
    cases=[];rng=random.Random(5564)
    def add(label,data=b'',**kw):cases.append(dict(label=label,data=data,**kw))
    for method in [0,8]:
        for length in [0,1,15,511,512,513,1023,1024,1025,2049,8192,65536,100000]:
            for data in [bytes(length),rng.randbytes(length)]:
                buffer=io.BytesIO()
                with zipfile.ZipFile(buffer,'w',compression=method) as archive:archive.writestr('nested/file',data)
                add('standard',buffer.getvalue(),expected={'target/nested/file':data.hex()})
        for name in ['', '/', './', '.', '..', '../outside/marker', '../target-copy/marker','dir/../file',
                     'missing/../file','/absolute-file','double//slash/file','a\\b','C:\\file','é/🐼','a\x00b']:
            add('paths',local(name,b'path payload',method)+bytes(30))
        for setup in ['file','directory','parent-file','inside-link','outside-link','dangling-inside','dangling-outside','cycle-link']:
            for name in ['file','nested/file','link/file','link','file/']:
                add('existing-'+setup,local(name,b'replacement',method)+bytes(30),setup=setup)
        for entries in [[('file',b'one'),('file',b'two')],[('directory/',b''),('directory/file',b'child')],
                        [('first',b'kept'),('../outside/marker',b'bad')],[('first',b'kept'),('broken',b'not returned')]]:
            raw=b''.join(local(name,data,method) for name,data in entries)
            add('multiple',raw+bytes(30))
        data=rng.randbytes(2050)
        add('crc-timing',local('file',data,method,crc=zlib.crc32(data)^1)+bytes(30),
            partial=('target/file',data[:2048 if method==0 else len(data)].hex()))
        add('empty-crc',local('file',b'',method,crc=123)+bytes(30))
        raw=local('file',rng.randbytes(1050),method)+bytes(30)
        cuts=sorted(set(list(range(40))+[len(raw)-n for n in [0,1,2,7,29,30,31,100,512,1024]]))
        for cut in cuts:add('truncated',raw[:cut])
        for field in ['size','compressed_size','crc']:
            for value in [0,1,16,0xffffffff]:add('size-crc',local('file',b'abcdef'*30,method,**{field:value})+bytes(30))
    for name in [b'\xff',b'\xed\xa0\x80',b'\xe2\x82',b'\xc0\x80',b'\xf4\x90\x80\x80']:
        for flags in [0,1,2048,2049]:add('invalid-name',local(name,flags=flags)+bytes(30))
    for method in [0,1,8,9,99]:
        for flags in [1,8,9,2048,2049]:add('method-flags',local(method=method,flags=flags)+bytes(30))
    for wide in [False,True]:
        for signature in [False,True]:
            for data in [b'',b'abc',rng.randbytes(5000)]:
                compressor=zlib.compressobj(wbits=-15);packed=compressor.compress(data)+compressor.flush()
                extra=struct.pack('<HHQQ',1,16,len(data),len(packed)) if wide else b''
                raw=local('file',data,flags=8,descriptor=signature,wide=wide,extra=extra)
                add('descriptor',raw+bytes(30),expected={'target/file':data.hex()})
                for cut in [0,1,4,8,12,16,20]:add('descriptor-truncation',raw[:-cut] if cut else raw)
                for field in ['size','compressed_size','crc']:
                    add('descriptor-mismatch',local('file',data,flags=8,descriptor=signature,wide=wide,extra=extra,**{field:123})+bytes(30))
    for method in [0,8]:
        data=b'ZIP64'
        compressor=zlib.compressobj(wbits=-15);packed=compressor.compress(data)+compressor.flush() if method==8 else data
        for sizes in [(len(data),len(packed)),(0,0),(2**64-1,len(packed)),(len(data),2**64-1)]:
            extra=struct.pack('<HHQQ',1,16,*sizes)
            add('zip64-sizes',local('file',data,method,wide=True,extra=extra)+bytes(30))
        for extra in [b'\x00',b'\x01\x00\x00\x00',b'\x01\x00\x01\x00\x00',b'\x01\x00\xff\xff\x00',
                      struct.pack('<HH',1,8)+bytes(8),struct.pack('<HH',0x5455,0)+b'\x00',
                      struct.pack('<HH',0xa,32)+bytes(32)]:
            for flags in [0,8]:add('extra-fields',local('file',data,method,wide=True,flags=flags,extra=extra)+bytes(30))
    for data in [b'',b'not a ZIP',bytes(29),bytes(30),bytes(500),b'PK\x03\x04'+bytes(26),b'PK\x05\x06'+bytes(40)]:
        add('empty-or-invalid-signature',data)
        add('null-destination',data,destination=None)
    for name,extra in [('n'*65489,b''),('n'*65490,b''),('n'*100,b'\x00'*65400),('🐼'*16000,b'')]:
        add('long-fields',local(name,extra=extra)+bytes(30))
    for payload in [b'\x07',bytes(50),b'\xff'*30,b'\x01\x02\x00\xff\xffab']:
        add('invalid-deflate',local(payload=payload)+bytes(30))
    raw=local('directory/',b'directory payload')+local('file',b'hello world')+bytes(30)
    for chunk in [1,2,7,29,30,31,64,511,512,513,4096]:
        add('short-reads',raw,chunk=chunk,expected={'target/file':b'hello world'.hex()})
    for fail in list(range(0,45))+[50,60,70,80,90,100,len(raw)]:add('read-failure',raw,failAt=fail,chunk=7)
    add('close-failure',raw,failClose=True)
    add('null-input',raw,nullInput=True)
    add('null-input-null-destination',raw,nullInput=True,destination=None)
    for destination in ['target','target/','target//','target/./','target/../target',None,'','/']:
        for name in [None,'','file','.','..','../outside/marker','/file','a\x00b','nested/file']:
            add('path-helper',destination=destination,helper=True,name=name)
    for setup in ['cycle-link','inside-link','outside-link','dangling-inside','dangling-outside']:
        for name in ['link','link/','link/file','link/..','link/../file','missing/../../target/file']:
            add('canonical-helper',helper=True,name=name,setup=setup)
    for length in [255,256,300,512,900,1024]:
        add('path-length',local('n'*length)+bytes(30))
    add('null-destination-valid',local()+bytes(30),destination=None)
    return cases


def prepare(folder,case):
    if folder.exists():shutil.rmtree(folder)
    folder.mkdir();(folder/'outside').mkdir();(folder/'outside/marker').write_bytes(b'unchanged')
    (folder/'target-copy').mkdir();(folder/'target-copy/marker').write_bytes(b'unchanged')
    target=folder/'target'
    setup=case.get('setup')
    if setup or case.get('helper'):target.mkdir()
    if setup=='file':(target/'file').write_bytes(b'old file')
    if setup=='directory':(target/'file').mkdir()
    if setup=='parent-file':(target/'nested').write_bytes(b'parent blocker')
    if setup=='inside-link':
        (target/'real').mkdir();(target/'link').symlink_to('real')
    if setup=='outside-link':(target/'link').symlink_to('../outside')
    if setup=='dangling-inside':(target/'link').symlink_to('missing')
    if setup=='dangling-outside':(target/'link').symlink_to('../missing')
    if setup=='cycle-link':(target/'link').symlink_to('link')


def snapshot(folder):
    result={}
    def visit(directory):
        for path in sorted(directory.iterdir()):
            relative=str(path.relative_to(folder))
            if path.is_symlink():result[relative]={'symlink':os.readlink(path)}
            elif path.is_dir():result[relative]='directory';visit(path)
            else:result[relative]=path.read_bytes().hex()
    visit(folder);return result


def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'zip-extractor-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ZipExtractorOracle.java']
    listing=build/'zip-extractor-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ZipExtractorOracle']),Runner([str(build/'minima_zip_extractor')])]
    cases=corpus();failures=[];completed=0;groups=collections.Counter();observations=[]
    try:
        with tempfile.TemporaryDirectory(prefix='zip-extractor-',dir=build) as temp:
            folder=Path(temp)/'files'
            for index,case in enumerate(cases):
                request={k:v for k,v in case.items() if k in ['chunk','failAt','failClose','nullInput','helper','name']}
                destination=case.get('destination','target')
                if destination not in [None,'','/']:destination=str(folder/destination)
                request.update(bytes='0x'+case['data'].hex(),destination=destination)
                values=[]
                for runner in runners:
                    prepare(folder,case);reply=runner.ask(request);files=snapshot(folder)
                    if case.get('expected'):
                        assert reply.get('success'),dict(index=index,reply=reply)
                        for path,want in case['expected'].items():assert files.get(path)==want,dict(index=index,path=path)
                    if case.get('partial'):
                        path,want=case['partial'];assert files.get(path)==want,dict(index=index,path=path,reply=reply,actual=files.get(path))
                    assert files['outside/marker']==b'unchanged'.hex() and files['target-copy/marker']==b'unchanged'.hex()
                    values.append(dict(reply=reply,files=files))
                if values[0]!=values[1]:
                    failures.append(dict(index=index,label=case['label'],request=request,java=values[0],cpp=values[1]))
                groups[case['label']]+=1;completed+=1
                if case['label'] in ['crc-timing','empty-crc','close-failure','multiple']:
                    observations.append(dict(index=index,label=case['label'],result=values[0]))
    except Exception as error:failures.append(dict(error=repr(error),index=completed))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,total_cases=len(cases),groups=dict(groups),observations=observations,failures=failures,
                corpus_sha256=hashlib.sha256(json.dumps(cases,default=lambda b:b.hex(),sort_keys=True).encode()).hexdigest(),
                scope='Pinned ZipExtractor and native library with actual filesystem extraction: exact files, symlinks, ordered logs, exception category/message except NPE prose, reader calls/position and close count. STORED/DEFLATED/ZIP64/descriptors, short reads, malformed/truncated entries, failures and canonical containment. No central-directory use (source ignores it), concurrent filesystem replacement, Java GC descriptor timing, special-device write failures or actual >4GiB archives certification.')
    (build/'zip-extractor-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2));return bool(failures)


if __name__=='__main__':raise SystemExit(main())
