#!/usr/bin/env python3
"""Pinned full-size SPHINCS composition, state failures and cross-writer verification."""
import argparse,copy,hashlib,json,random,subprocess,time
from pathlib import Path
from horst_differential import ROOT,JAVA,hx,raw,number,data,proof_root
from fors_differential import ForestRunner,checked_demo

def sha(b):return hashlib.sha3_256(b).digest()
def mmr(root):return data(raw(root['data']))+number(int(root['value']))
def proof(p):return number(int(p['blocktime']))+number(len(p['proof']))+b''.join(bytes([int(c['left'])])+mmr(c['data']) for c in p['proof'])
def envelope(j):
    w=j['WOTSsignature']['signatures'];f=j['FORSsignature'];values=f['horstsignature']['values'];roots=f['horstroots']
    return (number(len(w))+b''.join(data(raw(s['publickey']))+data(raw(s['signature']))+proof(s['proof']) for s in w)
            +mmr(j['FORSroot'])+number(len(values))+b''.join(data(raw(s['sigvalue']))+raw(s['sigproof']) for s in values)
            +number(len(values))+b''.join(mmr(s['horstroot'])+raw(s['horstproof']) for s in roots))
def wots_public(seed):
    state=int.from_bytes(seed[:32],'little');result=b''
    for _ in range(34):
        secret=sha(state.to_bytes(32,'little'));state=(state+int.from_bytes(secret,'little')+1)%(2**256)
        for _ in range(255):secret=sha(secret)
        result+=secret
    return sha(result)
def wots_verify(message,signature):
    digits=list(sha(message));checksum=8192-sum(digits);digits += [checksum&255,checksum>>8];result=b''
    assert len(signature)==1088
    for i,digit in enumerate(digits):
        part=signature[i*32:(i+1)*32]
        for _ in range(255-digit):part=sha(part)
        result+=part
    return sha(result)

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');p.add_argument('--resume-vectors',type=Path);p.add_argument('--response-timeout',type=int,default=3600);args=p.parse_args();build=args.build.resolve();oracle=build/'sphincs-oracle';oracle.mkdir(exist_ok=True)
    resumed=args.resume_vectors is not None;prefix='sphincs-resumed' if resumed else 'sphincs'
    saved=json.loads(args.resume_vectors.read_text()) if resumed else []
    if len(saved)>3:raise ValueError('At most three completed signature records may be resumed')
    saved_hash=hashlib.sha256(args.resume_vectors.read_bytes()).hexdigest() if resumed else None
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/s for s in ['HORSTOracle.java','FORSOracle.java','SPHINCSOracle.java']]
    listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-XX:-OmitStackTraceInFastThrow','-Xmx2g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SPHINCSOracle'],[str(build/'minima_sphincs')]]
    runners=[ForestRunner(c,response_timeout=args.response_timeout) for c in commands];cases=0;checks=0;failures=[];groups={};vectors=[];transcript=hashlib.sha256();start=time.monotonic()
    def check(condition,description):
        nonlocal checks
        checks+=1
        if not condition:raise AssertionError(description)
    def compare(group,q):
        nonlocal cases
        values=[r.ask(q) for r in runners];cases+=1;groups[group]=groups.get(group,0)+1
        transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=cases,group=group,request=q,java=values[0],cpp=values[1]))
        (build/(prefix+'-progress.json')).write_text(json.dumps(dict(cases=cases,groups=groups,failures=failures,independent_assertions=checks),indent=2)+'\n')
        return values
    def verify(group,q,expected=None):
        values=compare(group,q)
        if expected is not None:
            for v in values:check(v.get('valid')==expected,(group,v))
        return values
    seed=bytes.fromhex('0011223344');private=sha(seed+b'WOTS')+sha(seed+b'FORS');rng=random.Random(5564)
    try:
        compare('default',dict(action='state'));compare('uninitialized-sign',dict(action='sign',message='0x00'));compare('null-sign',dict(action='sign',message=None))
        for initialized in [False,True]:
            if initialized:compare('initialize',dict(action='seed',seed=hx(seed)))
            compare('null-seed',dict(action='seed',seed=None))
            for length in range(64):
                values=compare('short-private',dict(action='private',private=hx(bytes(length))))
                for v in values:
                    index=0 if length==0 else length if length>=32 else 32
                    check(v.get('inputIdentity') is True and v['state']['private']==(hx(bytes(length)) if length else ''),v)
                    check(v.get('error')==dict(exception='java.lang.ArrayIndexOutOfBoundsException',message=f'Index {index} out of bounds for length {length}'),v)
                    check(v['samePublic'] and v['sameTotal'],v)
            compare('null-private',dict(action='private',private=None))
        for s in [b'',rng.randbytes(33),seed]:
            values=compare('seed-derivation',dict(action='seed',seed=hx(s)))
            for v in values:
                check(v['state']['private']==hx(sha(s+b'WOTS')+sha(s+b'FORS')),v)
                check(v['state']['total']==str(192**4),v)
        public=values[0]['state']['public']
        for extra in [b'',b'X',bytes(32)]:
            values=compare('private-roundtrip-extra',dict(action='private',private=hx(private+extra)))
            for v in values:check(v['state']['public']==public and v['state']['private']==hx(private+extra) and v['inputIdentity'],v)
        # These failures preserve the initialized signing seeds. The first real
        # signature below therefore also tests recovery after failed reinitialization.
        compare('failed-reinitialize',dict(action='private',private='0x01'));compare('failed-seed-reinitialize',dict(action='seed',seed=None))
        for position,(action,message) in enumerate([('sign',bytes.fromhex('FFEEDD')),('static',bytes.fromhex('FFEEDD')),('sign',b'')]):
            if position<len(saved):
                prior=saved[position]
                check((prior['action'],prior['message'],prior['private'],prior['public'])==(action,hx(message),hx(private),public),'resume record inputs match this corpus')
                check(prior['java']==prior['cpp'] and hx(envelope(prior['json']))==prior['java'],'resume record retains exact matched Java/native wire')
                # These are saved observations, not newly executed sign calls.
                # All independent proof checks and cross-writer verifiers below
                # still run; the original generating run remains separate evidence.
                values=[dict(wire=prior[writer],json=prior['json']) for writer in ['java','cpp']]
                print(json.dumps(dict(progress='checking saved matched signature',record=position,action=action,message=hx(message))),flush=True)
            else:
                print(json.dumps(dict(progress='starting full-size signature',action=action,message=hx(message),cases=cases)),flush=True)
                values=compare('full-sign',dict(action=action,message=hx(message),private=hx(private)))
            hashed=sha(message);index=int.from_bytes(hashed,'big')%(192**4);text=str(index);indexbytes=bytes.fromhex(('0' if len(text)%2 else '')+text);forsseed=sha(private[32:]+indexbytes)
            for v in values:
                j=v['json'];check(hx(envelope(j))==v['wire'],'independently encoded full envelope')
                check(j['FORSroot']['value']=='120' and len(j['WOTSsignature']['signatures'])==4,'full size components')
                for i in range(16):
                    ref=int.from_bytes(hashed[2*i:2*i+2],'big');child=sha(data(forsseed)+number(i));secret=sha(data(child)+number(ref));part=j['FORSsignature']['horstsignature']['values'][i];outer=j['FORSsignature']['horstroots'][i]
                    check(part['sigvalue']==hx(secret),'selected FORS secret')
                    check(proof_root(raw(part['sigproof']),sha(secret),ref)==outer['horstroot'],'inner authentication path')
                    check(proof_root(raw(outer['horstproof']),raw(outer['horstroot']['data']),i,4)==j['FORSroot'],'outer authentication path')
                nodes=[index//(192**n)%192 for n in [3,2,1,0]];node_seed=private[:32];signatures=j['WOTSsignature']['signatures']
                for i,sig in enumerate(signatures):
                    leaf_seed=sha(number(nodes[i])+data(node_seed));check(hx(wots_public(leaf_seed))==sig['publickey'],'selected WOTS leaf public key')
                    check(proof_root(proof(sig['proof']),raw(sig['publickey']),0,len(sig['proof']['proof']))['data']==sig['rootkey'],'WOTS MMR path')
                    signed=raw(signatures[i+1]['rootkey'] if i<3 else j['FORSroot']['data']);check(hx(wots_verify(signed,raw(sig['signature'])))==sig['publickey'],'WOTS chain verification')
                    node_seed=sha(number(nodes[i])+data(sha(data(node_seed))))
                check(signatures[0]['rootkey']==public,'WOTS root is SPHINCS public key')
            record=dict(action=action,message=hx(message),private=hx(private),public=public,index=index,index_bytes=hx(indexbytes),fors_seed=hx(forsseed),java=values[0]['wire'],cpp=values[1]['wire'],json=values[0]['json']);vectors.append(record)
            (build/(prefix+'-known-vectors.json')).write_text(json.dumps(vectors,indent=2)+'\n')
            if action=='static':check(vectors[0]['java']==record['java'] and vectors[0]['cpp']==record['cpp'],'static and instance deterministic equality after failed reinit')
            for writer in ['java','cpp']:
                q=dict(action='verify',wire=record[writer],message=hx(message),public=public)
                verify('cross-writer',q,True);verify('wrong-message',dict(q,message=hx(message+b'X')),False);verify('wrong-public',dict(q,public='0x00'),False)
        record=vectors[0];q=dict(action='verify',wire=record['java'],message=record['message'],public=public)
        for mask in range(8):
            for msg in [q['message'],None]:
                for pub in [public,None]:verify('null-order',dict(q,nulls=mask,message=msg,public=pub))
        verify('null-envelope',dict(q,wire=None));verify('all-null',dict(q,wire=None,message=None,public=None))
        j=record['json']
        for count in [0,1,2,3,5,8,9]:
            changed=copy.deepcopy(j);changed['WOTSsignature']['signatures']=(j['WOTSsignature']['signatures']*3)[:count]
            verify('wots-count',dict(q,wire=hx(envelope(changed))),False)
            if count in [0,9]:verify('wots-count-before-null-public',dict(q,wire=hx(envelope(changed)),public=None),False)
        for i in range(4):
            for field in ['publickey','signature']:
                changed=copy.deepcopy(j);b=bytearray(raw(changed['WOTSsignature']['signatures'][i][field]));b[len(b)//2]^=1;changed['WOTSsignature']['signatures'][i][field]=hx(b)
                verify('wots-tamper',dict(q,wire=hx(envelope(changed))),False)
            for length in [0,1,1087,1089]:
                changed=copy.deepcopy(j);changed['WOTSsignature']['signatures'][i]['signature']=hx(bytes(length));verify('wots-length',dict(q,wire=hx(envelope(changed))))
        for field,value in [('data','0x00'),('value','121')]:
            changed=copy.deepcopy(j);changed['FORSroot'][field]=value;values=verify('root-tamper',dict(q,wire=hx(envelope(changed))),False)
            for v in values:check(v['logs']==['SPHINCS VERIFY '+('wots' if field=='data' else 'fors')+'  : FALSE'],'stage-specific failure log')
        for i in range(16):
            for field in ['sigvalue','sigproof','horstroot','horstproof']:
                changed=copy.deepcopy(j);f=changed['FORSsignature']
                if field=='sigvalue':f['horstsignature']['values'][i][field]='0x00'
                elif field=='sigproof':f['horstsignature']['values'][i][field]=hx(number(0)+number(0))
                elif field=='horstroot':f['horstroots'][i][field]['data']='0x00'
                else:f['horstroots'][i][field]=hx(number(0)+number(0))
                verify('fors-tamper',dict(q,wire=hx(envelope(changed))),False)
        for text in [None,'','student log']:compare('log',dict(action='log',message=text))
        # The runner enables UBSAN halt; inspect stderr before its temporary file closes.
        check(not runners[1].errors(),'native runner stderr must be empty')
    except Exception as error:
        failures.append(dict(phase='execution',cases=cases,error=repr(error)))
    finally:
        diagnostics=[dict(runtime=name,stderr=runner.errors()) for name,runner in zip(['java','cpp'],runners)]
        for name,runner in zip(['java','cpp'],runners):
            try:runner.close()
            except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
        (build/(prefix+'-process-results.json')).write_text(json.dumps(dict(cases=cases,groups=groups,failures=failures,diagnostics=diagnostics,resumed_signature_records=len(saved),resume_vectors_sha256=saved_hash),indent=2)+'\n')
    if any(f.get('phase') in ['execution','cleanup'] for f in failures):
        print(json.dumps(dict(phase='incomplete execution',cases=cases,failures=failures)),flush=True);return 1
    print(json.dumps(dict(progress='comparisons complete, running actual source mains',cases=cases,failures=len(failures))),flush=True)
    demos=[checked_demo(c,timeout=args.response_timeout) for c in commands];check(demos[0].stdout==demos[1].stdout,'actual source main stdout')
    (build/(prefix+'-demo-output.json')).write_text(json.dumps(dict(stdout=demos[0].stdout,native_stderr=demos[1].stderr,returncodes=[d.returncode for d in demos],UBSAN_OPTIONS='halt_on_error=1'),indent=2)+'\n')
    report=dict(cases=cases,passed=cases-len(failures),resumed_signature_records=len(saved),new_signatures=3-len(saved),resume_vectors_sha256=saved_hash,independent_assertions=checks,groups=groups,failures=failures,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),full_forest_leaves_per_sign=1048576,source_mains_matched=True,scope='Actual pinned SPHINCS with TreeKey192/4 and full FORS, private-key partial state, all signature bytes/JSON, cross-writer verification, independent selected secrets/WOTS chains/MMR paths, source mains. NPE/EOF prose normalized. Raw MiniData byte aliases, package-private field mutation, null SignatureProof internals/list entries, arbitrary subclasses/concurrency and full node parity not certified.')
    (build/(prefix+'-differential-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
