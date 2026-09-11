#!/usr/bin/env python3
"""Actual full-size legacy HORST, cross-writer signatures, independent SHA3 tree."""
import argparse
import hashlib
import json
import random
import struct
import subprocess
import time
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def hx(b):return '0x'+b.hex().upper()
def raw(s):return bytes.fromhex(s[2:])
def digest(b):return hx(hashlib.sha3_256(b).digest())
def number(n):
    length=max(1,(n.bit_length()+8)//8)
    b=n.to_bytes(length,'big',signed=True)
    return bytes([0,length])+b

def data(b):return struct.pack('>i',len(b))+b

def independent(seed):
    secrets=[hashlib.sha3_256(data(seed)+number(i)).digest() for i in range(65536)]
    publics=[hashlib.sha3_256(s).digest() for s in secrets]
    nodes=[(hashlib.sha3_256(number(0)+data(p)+number(i)).digest(),i) for i,p in enumerate(publics)]
    while len(nodes)>1:
        nodes=[(hashlib.sha3_256(number(1)+data(lh)+data(rh)+number(ls+rs)).digest(),ls+rs)
               for (lh,ls),(rh,rs) in zip(nodes[::2],nodes[1::2])]
    return {'root':{'data':hx(nodes[0][0]),'value':str(nodes[0][1])},
            'privateHash':digest(b''.join(secrets)),'publicHash':digest(b''.join(publics)),
            'privateSize':65536,'publicSize':65536,'rootIdentity':True,'logCount':0,'logKinds':[]},secrets,publics

def proof_root(encoded,public,index):
    offset=0
    def num():
        nonlocal offset
        scale,length=encoded[offset:offset+2];offset+=2
        value=int.from_bytes(encoded[offset:offset+length],'big',signed=True);offset+=length
        assert scale==0
        return value
    def blob():
        nonlocal offset
        length=int.from_bytes(encoded[offset:offset+4],'big',signed=True);offset+=4
        value=encoded[offset:offset+length];offset+=length
        return value
    assert num()==0
    count=num();assert count==16
    current=hashlib.sha3_256(number(0)+data(public)+number(index)).digest();total=index
    for _ in range(count):
        left=encoded[offset];offset+=1;sibling=blob();total+=num()
        children=data(sibling)+data(current) if left==1 else data(current)+data(sibling)
        current=hashlib.sha3_256(number(1)+children+number(total)).digest()
    assert offset==len(encoded)
    return {'data':hx(current),'value':str(total)}

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');args=p.parse_args();build=args.build.resolve()
    oracle=build/'horst-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/HORSTOracle.java']
    listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-XX:-OmitStackTraceInFastThrow','-Xmx2g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HORSTOracle'],[str(build/'minima_horst')]]
    runners=[Runner(command,response_timeout=300) for command in commands]
    failures=[];groups={};count=0;checks=0;transcript=hashlib.sha256();vectors=[];start=time.monotonic()
    def compare(group,request,expect=None):
        nonlocal count,checks
        values=[r.ask(request) for r in runners];count+=1;groups[group]=groups.get(group,0)+1
        transcript.update(json.dumps([request,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=count,group=group,request=request,java=values[0],cpp=values[1]))
        if expect is not None:
            checks+=2
            for runtime,value in zip(['java','cpp'],values):
                if value!=expect:failures.append(dict(index=count,group=group+'-independent',runtime=runtime,request=request,expected=expect,actual=value))
        return values
    def ok(**kw):return dict(**kw,logCount=0,logKinds=[])
    def valid(value):return ok(valid=value)
    rng=random.Random(5564)
    try:
        # Public getKeyRef is not restricted to sixteen chunks. Signed int
        # multiplication wraps, while each byte access still checks its index.
        for length in [0,1,2,3,4,31,32,33,34,257]:
            b=rng.randbytes(length)
            for i in [-2147483648,-1073741824,-1,0,1,15,16,17,127,128,1073741824,2147483647]:
                offset=((i*2+2**31)%2**32)-2**31
                expected=ok(ref=int.from_bytes(b[offset:offset+2],'big')) if 0<=offset and offset+1<length else None
                compare('reference-indices',dict(action='ref',index=i,data=hx(b)),expected)
        for pair in [b'\0\0',b'\0\xff',b'\xff\0',b'\xff\xff']:
            compare('reference-boundaries',dict(action='ref',index=0,data=hx(pair)),ok(ref=int.from_bytes(pair,'big')))
        compare('null-reference',dict(action='ref',index=0,data=None))
        compare('null-public',dict(action='publicNull'))
        compare('null-private',dict(action='privateNull'),dict(size=65536,firstNull=True,lastNull=True,logCount=65536,logKinds=['java.lang.NullPointerException']*2))
        records=[]
        # Find a real message whose hash repeats an index; never modify the
        # production hash or substitute a smaller key space for this test.
        for nonce in range(100000):
            repeated=nonce.to_bytes(4,'big');hashed=hashlib.sha3_256(repeated).digest()
            refs=[int.from_bytes(hashed[i:i+2],'big') for i in range(0,32,2)]
            if len(set(refs))<16:break
        else:raise AssertionError('No repeated-index message in deterministic search')
        for seed in [bytes.fromhex('00112233'),b'',rng.randbytes(33)]:
            expected,secrets,publics=independent(seed)
            compare('full-tree',dict(action='init',seed=hx(seed)),expected)
            for i in [0,1,127,128,255,256,32767,32768,65534,65535,-1,-2,65536,2147483647,-2147483648]:
                for kind in ['private','public','proof']:
                    expected_key=ok(data=hx(secrets[i] if kind=='private' else publics[i]),**({'identity':True} if kind=='private' else {})) if 0<=i<65536 and kind!='proof' else None
                    compare('key-access',dict(action='key',index=i,kind=kind),expected_key)
            for message in [bytes.fromhex('FFEEDD'),b'',rng.randbytes(137),repeated]:
                values=compare('sign',dict(action='sign',message=hx(message)))
                for value in values:
                    assert 'wire' in value and value['identity'] and value['root']==expected['root'],value
                    hashed=hashlib.sha3_256(message).digest()
                    for i,item in enumerate(value['json']['values']):
                        index=int.from_bytes(hashed[2*i:2*i+2],'big')
                        assert item['sigvalue']==hx(secrets[index])
                        assert proof_root(raw(item['sigproof']),publics[index],index)==expected['root']
                        checks+=2
                rec=dict(message=hx(message),root=expected['root'],java=values[0]['wire'],cpp=values[1]['wire'],firstProof=values[0]['json']['values'][0]['sigproof'])
                records.append(rec);vectors.append(dict(seed=hx(seed),**rec))
                # Every output is consumed by both implementations, including
                # the opposite writer; expected True is checked independently.
                for writer in ['java','cpp']:
                    request=dict(action='verify',message=rec['message'],wire=rec[writer],root=rec['root']['data'],sum=rec['root']['value'])
                    compare('cross-writer',request,valid(True))
                    compare('changed-message',dict(request,message=rec['message']+'FF'),valid(False))
                    compare('changed-root',dict(request,root='0x00'),valid(False))
                    compare('changed-root-sum',dict(request,sum='2147450881'),valid(False))
                    compare('extra-entries',dict(request,extra=True),valid(True))
            compare('live-list-alias',dict(action='alias',message='0xFFEEDD'))
            compare('null-sign-message',dict(action='sign',message=None))
            print(json.dumps({'progress':'full trees and signatures','cases':count,'failures':len(failures)}),flush=True)
        rec=records[0];request=dict(action='verify',message=rec['message'],wire=rec['java'],root=rec['root']['data'],sum=rec['root']['value'])
        for i in range(16):
            compare('changed-secret',dict(request,index=i,value=hx(rng.randbytes(32))),valid(False))
            compare('missing-values',dict(request,values=i))
            compare('missing-proofs',dict(request,proofs=i))
            compare('null-value',dict(request,nullValue=i))
            compare('null-proof',dict(request,nullProof=i))
            compare('empty-proof',dict(request,index=i,proof=hx(number(0)+number(0))),valid(False))
        for i in range(1,16):compare('swapped-secret',dict(request,swap=i),valid(False))
        for changes in [dict(message=None),dict(wire=None),dict(root=None),dict(message=None,wire=None,root=None),
                        dict(values=0,root=None),dict(value='0x00',nullProof=1),dict(root='0x00',nullValue=1),
                        dict(value='0x00',proofs=0),dict(nullValue=0,proofs=0)]:compare('failure-precedence',dict(request,**changes))
        # Proof directions, sibling hashes, sums, block times and counts. Mutate
        # one complete proof at a time, keeping decode failures distinct.
        proof=raw(rec['firstProof'])
        for at in sorted(set([0,2,3,5,6,7,10,11,20,35,len(proof)-1])):
            for xor in [1,2,128,255]:
                changed=bytearray(proof);changed[at]^=xor
                compare('mutated-proof-wire',dict(request,index=0,proof=hx(changed)))
        # A small, independently encoded signature exposes every partial read
        # boundary without requiring a 12KB response for every signature byte.
        empty=number(0);small=number(2)+data(b'abc')+number(4)+number(1)+b'\x01'+data(b'xyz')+number(7)+data(b'')+number(0)+number(0)
        smallhex=hx(small);initial=rec['java']
        wires=[small[:cut] for cut in range(len(small)+1)]+[small+b'trailing',empty,number(-1),number(2**32),number(2**32+1),b'\x01\x01\x0f']
        # Count validation; negative/oversized data/hash lengths; zero-byte
        # BigIntegers (runtime exceptions must escape convert's IOException catch).
        wires += [bytes([0,n]) for n in [0,33,127,128,255]]
        wires += [number(1)+struct.pack('>i',n) for n in [-1,536870913,2147483647]]
        wires += [number(1)+data(b'x')+number(0)+number(1)+b'\x01'+struct.pack('>i',n) for n in [-1,65,2147483647]]
        full=raw(rec['java']);cuts=set(range(80))|set(range(len(full)-80,len(full)))
        # Each 64-byte stride samples all sixteen independently decoded entries.
        cuts.update(range(0,len(full),64));cuts.update([len(full),len(full)//2])
        wires += [full[:cut] for cut in sorted(cuts)]+[full+b'trailing']
        for wire in wires:
            compare('partial-read',dict(action='read',initial=initial,wire=hx(wire)))
            compare('converter',dict(action='convert',wire=hx(wire)))
        compare('null-converter',dict(action='convert',wire=None))
        for i in range(16):
            for field in ['nullValue','nullProof','values','proofs','oversizeProof']:
                compare('partial-write-json',dict(action='mutate',wire=rec['java'],**{field:i}))
        # The mapped demonstration is executed, rather than recreated in fixtures.
        demos=[subprocess.run(command+['--demo'],capture_output=True,text=True,check=True,timeout=300).stdout for command in commands]
        checks+=1
        if demos[0]!=demos[1]:failures.append(dict(group='source-main',java=demos[0],cpp=demos[1]))
        if not demos[0].endswith('Verify : true\n'):raise AssertionError(demos[0])
        (build/'horst-demo-output.txt').write_text(demos[1])
    finally:
        for runner in runners:runner.close()
    report={'cases':count,'passed':count-len({f['index'] for f in failures if 'index' in f}),
            'independent_assertions':checks,'groups':groups,'failures':failures,'seconds':round(time.monotonic()-start,3),
            'full_tree_leaves':65536,'full_tree_seeds':3,'cross_writer_signatures':len(records)*2,
            'transcript_sha256':transcript.hexdigest(),
            'scope':'Pinned full-size HORST four classes, native primitives; root/all-key digests independently reproduced with Python SHA3; exact wire/JSON, aliases, typed errors, partial input/output state and converter log categories. NPE/EOF prose and runtime stack frames normalized. Other SPHINCS classes, arbitrary subclass overrides, raw MiniData byte-array mutation and concurrent mutation not certified.'}
    (build/'horst-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    (build/'horst-known-vectors.json').write_text(json.dumps(vectors,indent=2)+'\n')
    print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
