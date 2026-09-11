#!/usr/bin/env python3
"""Actual SPHINCSSignature container with partial field/write state comparison."""
import argparse,hashlib,json,random,struct,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import ROOT,JAVA,hx,raw,number,data,digest

def wots(count):
    return number(count)+b''.join(data(bytes([i+1]))+data(b'sig')+number(i)+number(1)+bytes([i%2])+data(b'node')+number(i) for i in range(max(0,count)))

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');args=p.parse_args();build=args.build.resolve();oracle=build/'sphincs-signature-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['HORSTOracle.java','FORSOracle.java','SPHINCSSignatureOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(c,response_timeout=60) for c in [['java','-XX:-OmitStackTraceInFastThrow','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SPHINCSSignatureOracle'],[str(build/'minima_sphincs_signature')]]]
    vectors=json.loads((ROOT/'validation/fors-work-in-progress/normal/fors-known-vectors.json').read_text());actual=vectors[0];assert actual['java']==actual['cpp']
    smallinner=number(1)+data(b'secret')+number(2)+number(1)+b'\1'+data(b'inner')+number(3)
    smallfors=smallinner+number(1)+data(b'root')+number(5)+number(0)+number(1)+b'\0'+data(b'outer')+number(7)
    rootbytes=b'envelope root';base=dict(wots=hx(wots(2)),root=hx(rootbytes),sum='120',fors=hx(smallfors))
    groups={};failures=[];cases=0;assertions=0;transcript=hashlib.sha256();start=time.monotonic()
    def compare(group,q,wire=None):
        nonlocal cases,assertions
        values=[r.ask(q) for r in runners];cases+=1;groups[group]=groups.get(group,0)+1;transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=cases,group=group,request=q,java=values[0],cpp=values[1]))
        if wire is not None:
            for name,value in zip(['java','cpp'],values):
                assertions+=2;state=value['state'];assert state['wireHash']==digest(wire) and state['wireLength']==len(wire),(name,q,state)
                assert 'writeError' not in state and 'jsonError' not in state
        return values
    try:
        for count in [0,1,2,8,9]:
            for forsbytes in [number(0)+number(0),smallfors,raw(actual['java'])]:
                q=dict(base,action='construct',wots=hx(wots(count)),fors=hx(forsbytes));wire=wots(count)+data(rootbytes)+number(120)+forsbytes
                values=compare('construct',q,wire)
                for value in values:
                    assert value['wotsIdentity'] and value['rootIdentity'] and value['forsIdentity'];assertions+=3
        for mask in range(8):
            q=dict(base,action='construct')
            for bit,name in enumerate(['wots','root','fors']):
                if mask&(1<<bit):q[name]=None
            compare('null-components',q)
        compare('default',dict(base,action='construct',default=True))
        for change in [dict(addWots=True),dict(clearForsRoots=True),dict(badWotsProof=True),dict(root=hx(bytes(65))),dict(forsMutation=dict(rootData=hx(bytes(65)),rootSum='120')),dict(forsMutation=dict(oversizeOuterProof=0))]:compare('mutation-alias-and-write',dict(base,action='construct',**change))
        small=wots(2)+data(rootbytes)+number(120)+smallfors
        wires=[small[:cut] for cut in range(len(small)+1)]+[small+b'trailing']
        for prefix in [number(-1),number(2**32),number(2**32+1),b'\0\0',b'\0\x21',b'\0\xff']:
            wires.extend([prefix,prefix+data(rootbytes)+number(120)+smallfors,wots(2)+data(rootbytes)+number(120)+prefix])
        for length in [-1,65,2147483647]:wires.append(wots(2)+struct.pack('>i',length))
        full=wots(2)+data(raw(actual['root']['data']))+number(int(actual['root']['value']))+raw(actual['java'])
        cuts=set(range(96))|set(range(len(full)-48,len(full)))|set(range(0,len(full),96))|{len(full)}
        wires += [full[:cut] for cut in sorted(cuts)]+[full+b'trailing']
        for b in wires:
            compare('read-existing',dict(base,action='read',wire=hx(b)))
            compare('read-empty',dict(base,action='read',default=True,wire=hx(b)))
            compare('converter',dict(action='convert',wire=hx(b)))
        compare('null-converter',dict(action='convert',wire=None))
        compare('complete-read',dict(base,action='read',wire=hx(small)),small)
        compare('complete-convert',dict(action='convert',wire=hx(full)),full)
    finally:
        for r in runners:r.close()
    report=dict(cases=cases,passed=cases-len(failures),independent_assertions=assertions,groups=groups,failures=failures,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),full_fors_vector_sha256=hashlib.sha256(raw(actual['java'])).hexdigest(),scope='Pinned SPHINCSSignature constructor/getters, exact component wire and JSON, partial assignments/output, alias mutations, typed errors and converter logs. Full SPHINCS signing not exercised; synthetic WOTS proofs are structural inputs. Null SignatureProof internals/list entries, MiniData raw aliases, arbitrary subclasses/streams and concurrent mutation not certified. NPE/EOF prose and stack frames normalized.')
    (build/'sphincs-signature-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
