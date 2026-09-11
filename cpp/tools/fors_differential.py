#!/usr/bin/env python3
"""Full 16x65536 legacy FORS, nested wire failures and independent tree proofs."""
import argparse,hashlib,json,random,struct,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import ROOT,JAVA,hx,raw,digest,number,data,independent,proof_root

class ForestRunner(Runner):
    def close(self):
        # Same pipe/exit checks as Runner, allowing full-forest destruction.
        self.child.stdin.close()
        try:code=self.child.wait(timeout=120)
        except subprocess.TimeoutExpired:self.child.kill();self.child.wait();raise
        if code:raise RuntimeError(self.errors())
        self.err.close()

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');args=p.parse_args();build=args.build.resolve();oracle=build/'fors-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/HORSTOracle.java',ROOT/'support/FORSOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-XX:-OmitStackTraceInFastThrow','-Xmx4g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','FORSOracle'],[str(build/'minima_fors')]]
    runners=[ForestRunner(command,response_timeout=1800) for command in commands];failures=[];groups={};count=0;checks=0;vectors=[];transcript=hashlib.sha256();start=time.monotonic()
    def compare(group,request,expected=None):
        nonlocal count,checks
        values=[r.ask(request) for r in runners];count+=1;groups[group]=groups.get(group,0)+1;transcript.update(json.dumps([request,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=count,group=group,request=request,java=values[0],cpp=values[1]))
        if expected is not None:
            checks+=2
            for name,value in zip(['java','cpp'],values):
                if value!=expected:failures.append(dict(index=count,group=group+'-independent',runtime=name,request=request,expected=expected,actual=value))
        if count%50==0:(build/'fors-progress.json').write_text(json.dumps(dict(cases=count,failures=failures,groups=groups),indent=2)+'\n')
        return values
    def ok(**kw):return dict(**kw,logCount=0,logKinds=[])
    def valid(b):return ok(valid=b)
    rng=random.Random(5564);seed=bytes.fromhex('00112233');inner=[]
    try:
        for i in range(16):
            child=hashlib.sha3_256(data(seed)+number(i)).digest();expected,_,_=independent(child);inner.append(expected['root'])
        nodes=[(hashlib.sha3_256(number(0)+data(raw(root['data']))+number(i)).digest(),i) for i,root in enumerate(inner)]
        while len(nodes)>1:nodes=[(hashlib.sha3_256(number(1)+data(lh)+data(rh)+number(ls+rs)).digest(),ls+rs) for (lh,ls),(rh,rs) in zip(nodes[::2],nodes[1::2])]
        root=dict(data=hx(nodes[0][0]),value=str(nodes[0][1]))
        compare('full-forest',dict(action='init',seed=hx(seed)),ok(root=root,rootIdentity=True))
        print(json.dumps(dict(progress='full forest constructed and independently checked',cases=count,failures=len(failures))),flush=True)
        for nonce in range(100000):
            repeated=nonce.to_bytes(4,'big');hashed=hashlib.sha3_256(repeated).digest();refs=[hashed[i:i+2] for i in range(0,32,2)]
            if len(set(refs))<16:break
        for message in [bytes.fromhex('FFEEDD'),b'',rng.randbytes(137),repeated]:
            values=compare('sign',dict(action='sign',message=hx(message)));hashed=hashlib.sha3_256(message).digest()
            for value in values:
                assert value['root']==root and len(value['json']['horstroots'])==16
                for i in range(16):
                    ref=int.from_bytes(hashed[2*i:2*i+2],'big');child=hashlib.sha3_256(data(seed)+number(i)).digest();secret=hashlib.sha3_256(data(child)+number(ref)).digest();public=hashlib.sha3_256(secret).digest()
                    part=value['json']['horstsignature']['values'][i];outer=value['json']['horstroots'][i]
                    assert part['sigvalue']==hx(secret);assert outer['horstroot']==inner[i]
                    assert proof_root(raw(part['sigproof']),public,ref)==inner[i]
                    assert proof_root(raw(outer['horstproof']),raw(inner[i]['data']),i,4)==root;checks+=4
            record=dict(seed=hx(seed),message=hx(message),root=root,java=values[0]['wire'],cpp=values[1]['wire'],json=values[0]['json']);vectors.append(record)
            (build/'fors-known-vectors.json').write_text(json.dumps(vectors,indent=2)+'\n')
            for writer in ['java','cpp']:
                request=dict(action='verify',message=hx(message),wire=record[writer],root=root['data'],sum=root['value'])
                compare('cross-writer',request,valid(True));compare('wrong-message',dict(request,message=hx(message)+'FF'),valid(False));compare('wrong-root',dict(request,root='0x00'),valid(False));compare('wrong-sum',dict(request,sum='121'),valid(False));compare('extra',dict(request,extra=True),valid(True))
        record=vectors[0];request=dict(action='verify',message=record['message'],wire=record['java'],root=root['data'],sum=root['value'])
        compare('aliases',dict(action='alias',message=record['message']),ok(sharedRoots=True,sharedSecrets=True,distinctProofs=True,distinctLists=True,aliasSize=15,otherSize=16))
        compare('null-sign',dict(action='sign',message=None))
        for i in range(16):
            for field in ['values','proofs','roots','outerProofs','nullValue','nullProof','nullRoot','nullOuterProof']:
                compare('missing-null',dict(request,**{field:i}))
            compare('wrong-secret',dict(request,index=i,value='0x00'),valid(False))
            compare('wrong-inner-root',dict(request,index=i,rootData='0x00',rootSum='2147450880'),valid(False))
            compare('wrong-inner-sum',dict(request,index=i,rootData=inner[i]['data'],rootSum='2147450881'),valid(False))
            for field in ['proof','outerProof']:compare('empty-proof',dict(request,index=i,**{field:hx(number(0)+number(0))}),valid(False))
        for changes in [dict(message=None),dict(wire=None),dict(root=None),dict(message=None,wire=None,root=None),dict(values=0,root='0x00'),dict(nullValue=0,root='0x00'),dict(nullOuterProof=0,values=0),dict(rootData='0x00',rootSum='1',values=0),dict(nullRoot=1,value='0x00'),dict(roots=0,values=0)]:compare('failure-order',dict(request,**changes))
        for field,proof in [('proof',record['json']['horstsignature']['values'][0]['sigproof']),('outerProof',record['json']['horstroots'][0]['horstproof'])]:
            b=raw(proof)
            for pos in sorted(set([0,2,3,5,6,7,10,11,20,35,len(b)-1])):
                for xor in [1,2,128,255]:
                    changed=bytearray(b);changed[pos]^=xor;compare('proof-bytes',dict(request,**{field:hx(changed)}))
        smallinner=number(2)+data(b'abc')+number(4)+number(1)+b'\x01'+data(b'xyz')+number(7)+data(b'')+number(0)+number(0)
        small=smallinner+number(2)+data(b'root')+number(7)+number(3)+number(1)+b'\x00'+data(b'sibling')+number(9)+data(b'')+number(0)+number(0)+number(0)
        wires=[small[:cut] for cut in range(len(small)+1)]+[small+b'trailing']
        for n in [-1,0,1,3,2**32,2**32+1]:wires.append(smallinner+number(n)+small[len(smallinner)+3:])
        for count_wire in [b'\0\0',b'\0\x21',b'\0\xff',number(-1),number(0)]:wires.extend([count_wire,smallinner+count_wire])
        for n in [-1,65,2147483647]:wires.append(smallinner+number(1)+struct.pack('>i',n))
        full=raw(record['java']);cuts=set(range(48))|set(range(len(full)-48,len(full)))|set(range(0,len(full),96))|{len(full)}
        wires += [full[:cut] for cut in sorted(cuts)]+[full+b'trailing']
        for b in wires:
            compare('partial-read',dict(action='read',initial=record['java'],wire=hx(b)))
            compare('converter',dict(action='convert',wire=hx(b)))
        compare('null-convert',dict(action='convert',wire=None))
        for i in range(16):
            for field in ['values','proofs','roots','outerProofs','nullValue','nullProof','nullRoot','nullOuterProof','oversizeProof','oversizeOuterProof']:
                compare('partial-write-json',dict(action='mutate',wire=record['java'],**{field:i}))
            compare('unused-root-serialization',dict(action='mutate',wire=record['java'],index=i,rootData=hx(bytes(65)),rootSum='2147450880'))
        print(json.dumps(dict(progress='wire and verification comparisons completed',cases=count,failures=len(failures))),flush=True)
        # Release the cached million-leaf forests before running the mains.
        for runner in runners:runner.close()
        runners=[]
        demos=[subprocess.run(command+['--demo'],capture_output=True,text=True,check=True,timeout=1800).stdout for command in commands]
        checks+=1
        if demos[0]!=demos[1]:failures.append(dict(group='source-main',java=demos[0],cpp=demos[1]))
        assert demos[0].endswith('Verify : true\n');(build/'fors-demo-output.json').write_text(json.dumps(dict(stdout=demos[1]),indent=2)+'\n')
    finally:
        for runner in runners:runner.close()
    report=dict(cases=count,passed=count-len({f['index'] for f in failures if 'index' in f}),independent_assertions=checks,groups=groups,failures=failures,seconds=round(time.monotonic()-start,3),full_forest_leaves=1048576,transcript_sha256=transcript.hexdigest(),scope='Full-size pinned FORS and nested signature APIs, both source mains, independent million-leaf forest and proof reconstruction; NPE/EOF prose and stack frames normalized. Raw MiniData aliases, arbitrary subclasses, concurrent mutation and full SPHINCS/node parity not certified.')
    (build/'fors-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');(build/'fors-known-vectors.json').write_text(json.dumps(vectors,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
