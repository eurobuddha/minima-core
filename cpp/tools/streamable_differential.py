#!/usr/bin/env python3
"""Common Streamable dispatch, partial reads/writes, and existing generic callers."""
import argparse,hashlib,json,os,re,subprocess,tempfile,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import ROOT,JAVA,hx,number,data
from sphincs_signature_differential import wots

def entrynumber(n):return number(n)+data(bytes([n+42]))
def mmrdata(n):return data(bytes([n+50]))+number(n+22)
def entry(n):return number(n+3)+entrynumber(n)+mmrdata(n)
def signatureproof(n):return data(bytes([n+4]))+data(bytes([n+10]))+proof(n)
def proof(n):return number(n+7)+number(n)+b''.join(bytes([i%2])+data(bytes([i+3]))+number(i+11) for i in range(n))
def horst(n):return number(n)+b''.join(data(bytes([i+21,i+22]))+proof(n) for i in range(n))
def fors(n):return horst(n)+number(n)+b''.join(data(bytes([i+31]))+number(i+40)+proof(n) for i in range(n))
def sphincs(n):return wots(n)+data(bytes([n+61]))+number(n+70)+fors(n)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'streamable-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/StreamableOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    adopted={'MiniByte','MMRProof','HORSTSignature','FORSSignature','SPHINCSSignature','MMRData','MMREntryNumber','MMREntry','MMRProofChunk','SignatureProof','Signature','MiniData','MiniNumber','MiniString'};implementations=[]
    for path in sorted((JAVA/'src').rglob('*.java')):
        source=re.sub(r'/\*.*?\*/|//[^\n]*','',path.read_text(),flags=re.S)
        for name in re.findall(r'\bclass\s+(\w+)[^{]*?\bimplements\s+Streamable\b',source):implementations.append(dict(java=str(path.relative_to(JAVA)),type=name,native_interface_adopted=name in adopted))
    check_names={r['type'] for r in implementations if r['native_interface_adopted']};assert check_names==adopted
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','StreamableOracle']),Runner([str(build/'minima_streamable')])];failures=[];count=checks=0;cases=[];transcript=hashlib.sha256()
    def check(ok,label):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(label)
    try:
        with tempfile.TemporaryDirectory(prefix='streamable-',dir=build) as scratch:
            initial={'byte':b'\x11','proof':proof(1),'horst':horst(1),'fors':fors(1),'sphincs':sphincs(1)};changed={'byte':b'\xff','proof':proof(2),'horst':horst(2),'fors':fors(2),'sphincs':sphincs(2)}
            for kind,encode in [('mmrdata',mmrdata),('entrynumber',entrynumber),('entry',entry),('chunk',lambda n:bytes([n%2])+mmrdata(n)),('signatureproof',signatureproof),('signature',wots)]:initial[kind]=encode(1);changed[kind]=encode(2)
            initial.update(data=data(b'\x11\x22'),number=number(7),string=data('old'.encode()))
            changed.update(data=data(b'\x80\x00\xff'),number=number(-129),string=data('new🐼'.encode()))
            for kind,base in initial.items():
                target=changed[kind]
                cases.append(dict(kind=kind,op='write',initial=hx(base)))
                cases.append(dict(kind=kind,op='callers',initial=hx(base),wire=hx(target),file=str(Path(scratch)/(kind+'.dat'))))
                for offset in [0,2]:
                    for cut in range(len(target)+1):cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(b'\xaa'*offset+target[:cut]),offset=offset))
                    cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(b'\xaa'*offset+target+b'\x88\x99'),offset=offset))
                if kind not in ['byte','data','string']:
                    for prefix in [number(-1),number(2**32),b'\x00\xff',b'\x00\x21']:cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(prefix),offset=0))
            for cache in [False,True]:
                for mode in [{},{'hashRead':True},{'expectedSize':-1},{'expectedSize':-2},{'expectedSize':0},{'expectedSize':3},{'expectedSize':4}]:
                    for payload in [changed['data'][:cut] for cut in range(len(changed['data'])+1)]+[data(b''),bytes.fromhex('ffffffff'),bytes.fromhex('fffffffe'),bytes.fromhex('20000001'),bytes.fromhex('00000041')]:
                        # Avoid large allocations: oversized declarations reject before allocating.
                        if ('expectedSize' in mode and mode['expectedSize']!=-1) or 'hashRead' in mode or payload!=bytes.fromhex('00000041'):
                            cases.append(dict(kind='data',op='read',initial=hx(initial['data']),wire=hx(payload),offset=0,cache=cache,scalarExtra=True,**mode))
            for cache in [False,True]:
                for cut in range(4,8):
                    wire=changed['data'][:cut]
                    cases.append(dict(kind='data',op='read',initial=hx(data(wire)),wire=hx(wire),offset=0,cache=cache,selfInput=True,scalarExtra=True))
            for raw in [b'',bytes.fromhex('EDA080'),bytes.fromhex('F09F90'),bytes.fromhex('E08080'),bytes.fromhex('C080'),bytes(range(256))]:
                encoded=data(raw)
                for cut in range(len(encoded)+1):cases.append(dict(kind='string',op='read',initial=hx(initial['string']),wire=hx(encoded[:cut]),offset=0,scalarExtra=True))
            number_payloads=[b'\x00'+bytes([len(raw)])+raw for raw in [bytes.fromhex(h) for h in ['0001','000080','ffff','ffff80','ff7f','0080','ff80','00000000','ffffff00']]]
            for payload in number_payloads+[b'\x00\x00',b'\x00\xff',b'\x00\x21',b'\x80\x01\x01',b'\x7f\x01\x01',b'\x00\x20'+b'\xff'*32,b'\x00\x20'+b'\x7f'+b'\xff'*31]:
                for cut in range(len(payload)+1):cases.append(dict(kind='number',op='read',initial=hx(initial['number']),wire=hx(payload[:cut]),offset=0,scalarExtra=True))
            for raw in [bytes.fromhex(h) for h in ['0001','000080','ffff','ffff80','ff7f','0080','ff80','00000000','ffffff00']]:
                n=int.from_bytes(raw,'big',signed=True);canonical=n.to_bytes(max(1,((~n if n<0 else n).bit_length()+8)//8),'big',signed=True)
                cases.append(dict(kind='number',op='read',initial=hx(initial['number']),wire=hx(b'\x00'+bytes([len(raw)])+raw),offset=0,scalarExtra=True,expectedState=hx(b'\xaa\x55\x00'+bytes([len(canonical)])+canonical)))
                target=number(0)+data(raw)
                for cut in range(len(target)+1):
                    q=dict(kind='entrynumber',op='read',initial=hx(initial['entrynumber']),wire=hx(target[:cut]),offset=0,scalarExtra=True)
                    if cut==len(target):q['expectedState']=hx(b'\xaa\x55'+number(0)+data(canonical))
                    cases.append(q)
            for raw,canonical in [(b'\xff'*65536,b'\xff'),(b'\x00'*65536,b'\x00'),(b'\x00'*65536+b'\x80',b'\x00\x80'),(b'\xff'*65536+b'\x7f',b'\xff\x7f')]:
                cases.append(dict(kind='entrynumber',op='read',initial=hx(initial['entrynumber']),wire=hx(number(0)+data(raw)),offset=0,scalarExtra=True,expectedState=hx(b'\xaa\x55'+number(0)+data(canonical))))
            cases.append(dict(kind='entrynumber',op='read',initial=hx(initial['entrynumber']),wire=hx(number(0)+data(b'')),offset=0))
            for null_data in [False,True]:
                cases.append(dict(kind='entry',op='write',initial='0x',empty=True,nullData=null_data))
                for cut in range(len(changed['entry'])+1):cases.append(dict(kind='entry',op='read',initial='0x',empty=True,nullData=null_data,wire=hx(changed['entry'][:cut]),offset=0))
            for kind in ['mmrdata','entry','chunk','signatureproof','signature']:cases.append(dict(kind=kind,op='badwrite',initial='0x'))
            for fail in [-1,0,1,2,3]:
                cases.append(dict(kind='failing',op='write',initial='0x010203',fail=fail))
                for cut in range(4):cases.append(dict(kind='failing',op='read',initial='0x010203',wire=hx(b'\x04\x05\x06'[:cut]),offset=0,fail=fail))
            for index,q in enumerate(cases):
                values=[r.ask(q) for r in runners];count+=1
                if values[0]!=values[1]:failures.append(dict(case=index,request=q,java=values[0],cpp=values[1]))
                for value in values:
                    check('error' not in value,'fixture initialization succeeds')
                    if 'expectedState' in q:check(value['state']==dict(appended=q['expectedState']),'signed coefficient reserializes to minimal two-complement bytes')
                    if q['kind']=='data':
                        wire=bytes.fromhex(q.get('wire',q['initial'])[2:]);previous=bytes.fromhex(q['initial'][2:])[4:];expected=previous
                        if q['op']=='callers':expected=wire[4:]
                        elif q['op']=='read' and len(wire)-q['offset']>=4:
                            start=q['offset'];length=int.from_bytes(wire[start:start+4],'big',signed=True)
                            allowed=0<=length<=(64 if 'hashRead' in q else 512*1024*1024) and ('expectedSize' not in q or q['expectedSize']==-1 or length==q['expectedSize'])
                            if allowed:expected=(wire[start+4:start+4+length]+b'\x00'*length)[:length]
                        check(value['state']==dict(appended=hx(b'\xaa\x55'+data(expected))),'data read allocates zeroed bytes before copying available payload')
                        numeric=int.from_bytes(previous if q.get('cache') else expected,'big')
                        check(value['numeric']==str(numeric) and value['less128']==(numeric<128),'cached numeric value survives read; otherwise derive current bytes')
                        for key,n in [('shiftLeft',numeric<<1),('shiftRight',numeric>>1)]:check(value[key]==hx(n.to_bytes(max(1,(n.bit_length()+7)//8),'big')),'shifts use the source numeric cache')
                    if q.get('scalarExtra'):
                        check(value['offset']<=len(bytes.fromhex(q['wire'][2:])),'extra scalar consumes available bytes only')
                        if q['kind'] in ['number','string','entrynumber'] and 'readError' in value:check(value['state']==dict(appended=hx(b'\xaa\x55'+initial[q['kind']])),'failed scalar read retains previous complete value')
                        continue
                    if q['kind']=='mmrdata':check(value['unspendable'],'read does not reset unspendable flag')
                    if q['kind']=='entry':check(value['empty']==(q.get('empty',False) and not q.get('nullData',False) and q['op']=='write'),'explicit empty flag clears before first read')
                    if q['kind']=='signature':check(value['oldListSize']==1 and value['sameList']==(q['op'] in ['write','badwrite']),'retained signature list survives replacement');check(value['aliasCount']>=0,'signature alias observes current list')
                    if q['op']=='badwrite':
                        signature_prefix=data(b'\xa1')+data(b'\xb2')+number(7)+number(1)+b'\x01'
                        prefix={'mmrdata':b'','entry':number(7)+number(0)+data(b'\x04'),'chunk':b'\x01','signatureproof':signature_prefix,'signature':number(1)+signature_prefix}[q['kind']]
                        check(value['state']==dict(appended=hx(b'\xaa\x55'+prefix),writeError='java.io.IOException'),'oversized nested hash preserves all preceding fields')
                    elif q.get('empty',False):
                        if q['op']=='write':check(value['state']==dict(appended=hx(b'\xaa\x55'+number(7)+number(0)+data(b'\x04')),writeError='java.lang.NullPointerException'),'empty entry writes row/entry before null data error')
                        else:check(value['offset']<=len(bytes.fromhex(q['wire'][2:])),'empty-entry read consumes only available bytes')
                    elif q['kind']=='failing':
                        expected=bytearray(b'\x01\x02\x03');n=0
                        if q['op']=='read':n=len(bytes.fromhex(q['wire'][2:]));expected[:n]=b'\x04\x05\x06'[:n];check(value['offset']==n,'failing-object consumed bytes');check(('readError' in value)==(n<3),'read EOF boundary')
                        fail=q['fail'];check(value['state']['appended']==hx(b'\xaa\x55'+expected[:fail if fail>=0 else 3]),'partial writer output remains appended');check(('writeError' in value['state'])==(fail>=0),'writer exception propagates through base')
                    elif q['op']=='write':check(value['state']==dict(appended=hx(b'\xaa\x55'+initial[q['kind']])),'base write appends exact source wire')
                    elif q['op']=='callers':
                        base=initial[q['kind']];target=changed[q['kind']]
                        check(value['data']==hx(base) and value['file']==hx(base),'MiniData and MiniFile preserve bytes through base')
                        check(value['appendedFile']==hx(base+base),'file append via base')
                        check(value['hash']==hx(hashlib.sha3_256(base).digest()) and value['pairHash']==hx(hashlib.sha3_256(base+base).digest()),'independent hash of base dispatch bytes')
                        check(value['nio']==hx(b'\x0d'+base),'network payload type then object')
                        check(value['state']==dict(appended=hx(b'\xaa\x55'+target)),'MiniFile loads into concrete object via base reader')
                    else:
                        wire=bytes.fromhex(q['wire'][2:]);offset=q['offset'];target=changed[q['kind']]
                        check(offset<=value['offset']<=len(wire),'read offset stays in supplied span')
                        if wire[offset:].startswith(target):check(value['offset']==offset+len(target) and value['state']==dict(appended=hx(b'\xaa\x55'+target)) and 'readError' not in value,'complete base read consumes exact object and leaves tail')
                        if q['kind']=='byte' and len(wire)==offset:check(value['state']==dict(appended=hx(b'\xaa\x55'+initial['byte'])) and value['readError']=='java.io.EOFException','failed byte read preserves previous value')
                stable=dict(q);stable.pop('file',None);transcript.update(json.dumps([stable,*values],sort_keys=True,separators=(',',':')).encode())
    except Exception as error:failures.append(dict(phase='execution',cases=count,error=repr(error)))
    finally:
        for which,r in enumerate(runners):
            with os.fdopen(os.dup(r.err.fileno()),'r') as err:
                try:r.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=which,error=repr(error)))
                err.seek(0);stderr=err.read()
                if which==1 and stderr:failures.append(dict(phase='native stderr',stderr=stderr))
    report=dict(passed=not failures,cases=count,planned_cases=len(cases),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,explicit_source_implementations=implementations,adopted_types=len(adopted),scope='Actual Java/C++ common Streamable references: MiniByte, MMRProof, HORSTSignature, FORSSignature, SPHINCSSignature, MMRData, MMREntryNumber, MMREntry, MMRProofChunk, SignatureProof, Signature, MiniData, MiniNumber and MiniString. Prefix writes, every small-vector read truncation, offset/trailing bytes and partial reserialization, synthetic failing implementation, actual MiniFile in-place load/write/append, MiniData conversion, Crypto single/pair hashing and NIO message construction. Exception classes compared; diagnostic prose excluded. Native complete stderr empty. Includes per-field MMR/signature readers, explicit flags, retained lists, scalar in-place assignment timing, MiniData zero-filled partial payloads and retained numeric cache with comparisons/shifts. Prior eleven-type coverage retained. Remaining source implementers, arbitrary Java stream backends, allocation failures, concurrency and complete node/API parity remain unfinished.')
    (build/'streamable-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
