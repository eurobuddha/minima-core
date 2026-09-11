#!/usr/bin/env python3
"""Common Streamable dispatch, partial reads/writes, and existing generic callers."""
import argparse,hashlib,json,os,re,subprocess,tempfile,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import ROOT,JAVA,hx,number,data
from sphincs_signature_differential import wots

def statevar(n):return bytes([n,4])+data(f'[state{n}]'.encode()) if n%2 else bytes([n,2])+number(n+10)
def address(n):return data(bytes([n+80]))+data(f'script{n}'.encode())
def scriptproof(n):return data(f'script{n}'.encode())+proof(n)
def token(n):return data(bytes([n+16]))+data(f'script{n}'.encode())+number(n)+number(n+2)+data(f'name{n}'.encode())+number(n+3)
def coin(n):return data(bytes([n+48]))+data(bytes([n+64]))+number(n+4)+data(b'\x00')+b'\x01'+entrynumber(n)+bytes([n%2])+number(n+6)+number(1)+statevar(n)+bytes([n%2])+(token(n) if n%2 else b'')
def coinproof(n):return coin(n)+proof(n)
def script_root(n):
    current=hashlib.sha3_256(number(0)+data(f'script{n}'.encode())+number(0)).digest();total=0
    for i in range(n):
        sibling=bytes([i+3]);total+=i+11;left,right=(sibling,current) if i%2 else (current,sibling)
        current=hashlib.sha3_256(number(1)+data(left)+data(right)+number(total)).digest()
    return hx(current)

def witness_lists(n):return [[encode(i+1) for i in range(n)] for encode in [wots,coinproof,scriptproof]]
def witness(n):return b"".join(number(len(items))+b"".join(items) for items in witness_lists(n))

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
    adopted={'MiniByte','MMRProof','HORSTSignature','FORSSignature','SPHINCSSignature','MMRData','MMREntryNumber','MMREntry','MMRProofChunk','SignatureProof','Signature','MiniData','MiniNumber','MiniString','StateVariable','Address','ScriptProof','Token','Coin','CoinProof','Witness'};implementations=[]
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
            for kind,encode in [('state',statevar),('address',address),('scriptproof',scriptproof),('token',token),('coin',coin),('coinproof',coinproof),('witness',witness)]:initial[kind]=encode(1);changed[kind]=encode(2)
            for kind,base in initial.items():
                target=changed[kind]
                cases.append(dict(kind=kind,op='write',initial=hx(base)))
                cases.append(dict(kind=kind,op='callers',initial=hx(base),wire=hx(target),file=str(Path(scratch)/(kind+'.dat'))))
                for offset in [0,2]:
                    for cut in range(len(target)+1):cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(b'\xaa'*offset+target[:cut]),offset=offset))
                    cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(b'\xaa'*offset+target+b'\x88\x99'),offset=offset))
                if kind not in ['byte','data','string']:
                    for prefix in [number(-1),number(2**32),b'\x00\xff',b'\x00\x21']:cases.append(dict(kind=kind,op='read',initial=hx(base),wire=hx(prefix),offset=0))
            for op in ['witnessClear','witnessDuplicates']:
                cases.append(dict(kind='witness',op=op,initial=hx(witness(1)),witnessExtra=True))
            for target in [number(0)*3,number(-1)*3,number(2**32)*3]:
                cases.append(dict(kind='witness',op='read',initial=hx(witness(1)),wire=hx(target),offset=0,witnessExtra=True,expectedCounts=[0,0,0],expectedSame=[False]*3,expectedState=hx(b'\xaa\x55'+number(0)*3)))
            cases.append(dict(kind='witness',op='read',initial=hx(witness(1)),wire=hx(witness(2)),offset=0,clearHeld=True,witnessExtra=True))
            duplicates=b''.join(number(2)+items[0]*2 for items in witness_lists(1))
            cases.append(dict(kind='witness',op='read',initial=hx(witness(1)),wire=hx(duplicates),offset=0,witnessExtra=True,expectedCounts=[2]*3,expectedSame=[False]*3,expectedState=hx(b'\xaa\x55'+duplicates)))
            sigs,coins,scripts=[number(1)+items[0] for items in witness_lists(1)]
            bad_sig=data(b'\xa1')+data(b'\xb2')+number(7)+number(1)+b'\x01'
            for fault,partial in [('signature',number(2)+wots(1)+number(1)+bad_sig),('coinproof',sigs+number(2)+coinproof(1)),('scriptproof',sigs+coins+number(2)+scriptproof(1)+data(b'RETURN TRUE')+number(7)+number(1)+b'\x01')]:
                cases.append(dict(kind='witness',op='badwrite',initial='0x',base=hx(witness(1)),fault=fault,witnessExtra=True,expectedWrite=dict(appended=hx(b'\xaa\x55'+partial),writeError='java.io.IOException')))
            for kind in ['coin','coinproof']:
                target=coin(3) if kind=='coin' else coinproof(3)
                for cut in range(len(target)+1):cases.append(dict(kind=kind,op='read',initial=hx(initial[kind]),wire=hx(target[:cut]),offset=0,objectExtra=True))
            for tag in [0,3,5,16,255]:
                cases.append(dict(kind='state',op='read',initial=hx(initial['state']),wire=hx(bytes([99,tag,0xff])),offset=0,objectExtra=True,expectedState=hx(b'\xaa\x55'+bytes([99,tag])),expectedText='[state1]',expectedError='java.io.IOException'))
            for byte in [0,1,2,255]:
                cases.append(dict(kind='state',op='read',initial=hx(initial['state']),wire=hx(bytes([99,8,byte])),offset=0,objectExtra=True,expectedState=hx(bytes([0xaa,0x55,99,8,int(byte!=0)])),expectedText='TRUE' if byte!=0 else 'FALSE'))
            prefix=data(b'\x01')+data(b'\x02')+number(3)+data(b'\x00')+b'\x01'+number(0)+data(b'\x00')+b'\x00'+number(0)
            for kind in ['address','scriptproof','token','coin','coinproof']:
                faults=['coinid','address','tokenid','state','token'] if kind in ['coin','coinproof'] else ['none']
                if kind=='coinproof':faults.append('proof')
                for fault in faults:
                    partial=b''
                    if kind=='scriptproof':partial=data(b'RETURN TRUE')+number(7)+number(1)+b'\x01'
                    elif fault=='address':partial=data(b'\x01')
                    elif fault=='tokenid':partial=data(b'\x01')+data(b'\x02')+number(3)
                    elif fault=='state':partial=prefix
                    elif fault=='token':partial=prefix+number(0)+b'\x01'
                    elif fault=='proof':partial=prefix+number(0)+b'\x00'+number(7)+number(1)+b'\x01'
                    cases.append(dict(kind=kind,op='badwrite',initial='0x',fault=fault,objectExtra=True,expectedWrite=dict(appended=hx(b'\xaa\x55'+partial),writeError='java.lang.NullPointerException' if fault=='state' else 'java.io.IOException'),expectedLogs=['java.io.IOException'] if kind=='token' or fault=='token' else []))
            for payload,logs,error,converted in [(b'', ['java.io.EOFException'],None,False),(bytes.fromhex('00000041'),['java.io.IOException'],None,False),(token(2),[],None,True),(data(b'\x11')+data(b'script1')+b'\x00\x00',[],'java.lang.NumberFormatException',None)]:
                cases.append(dict(kind='token',op='tokenConvert',initial=hx(initial['token']),wire=hx(payload),objectExtra=True,expectedLogs=logs,expectedConvertError=error,expectedConverted=converted))
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
                    check(value['logs']==q.get('expectedLogs',[]),'actual logger records expected exception classes')
                    if q['kind'] in ['token','scriptproof'] and q['op']!='badwrite':
                        n=2 if q['op']=='callers' or (q['op']=='read' and 'readError' not in value) else 1
                        expected=hx(hashlib.sha3_256(data(token(n))).digest()) if q['kind']=='token' else script_root(n)
                        check(value['derived']==expected,'derived identity updates only after complete prerequisite reads')
                    if q['kind']=='witness':
                        check(value['witnessAlias']==value['state'] and value['containingWitness']==value['state'],'Witness copy and TxPoW setter retain the same underlying object')
                        if 'expectedWrite' in q:check(value['state']==q['expectedWrite'],'Witness nested failure preserves prior lists, count and child prefix')
                        if q['op'] in ['write','callers','read']:
                            expected_old=[0 if q.get('clearHeld') else 1]*3;check(value['oldSizes']==expected_old and value['oldState']==dict(appended=hx(b'\xaa\x55'+(number(0)*3 if q.get('clearHeld') else witness(1)))),'old lists and their complete contents remain live and independent after replacement')
                            if q['op']=='write':check(value['sameLists']==[True]*3 and value['currentSizes']==[1]*3,'write leaves original lists intact')
                            if q['op']=='callers' or q.get('clearHeld'):check(value['sameLists']==[False]*3 and value['currentSizes']==[2]*3,'completed read replaces all three lists')
                            if q['op']=='read':
                                incoming=bytes.fromhex(q['wire'][2:])[q['offset']:];target=witness(2)
                                if target.startswith(incoming) or incoming.startswith(target):
                                    consumed=min(len(incoming),len(target));check(value['offset']==q['offset']+consumed and value.get('readError')==('java.io.EOFException' if consumed<len(target) else None),'Witness consumes each available prefix and stops at the exact complete boundary');start=0;expected_counts=[];expected_same=[];partial=[]
                                    for items,old_items in zip(witness_lists(2),witness_lists(1)):
                                        replaced=consumed>=start;completed_children=0;end=start+len(number(2))
                                        for item in items:
                                            end+=len(item)
                                            if consumed>=end:completed_children+=1
                                        expected_counts.append(completed_children if replaced else 1);expected_same.append(not replaced)
                                        kept=items[:completed_children] if replaced else old_items;partial.append(number(len(kept))+b''.join(kept));start=end
                                    check(value['currentSizes']==expected_counts and value['sameLists']==expected_same,'each list is replaced before its count, appending only complete children')
                                    check(value['state']==dict(appended=hx(b'\xaa\x55'+b''.join(partial))),'independent partial Witness serialization')
                        if 'expectedCounts' in q:check(value['currentSizes']==q['expectedCounts'] and value['sameLists']==q['expectedSame'] and value['state']==dict(appended=q['expectedState']),'negative and wrapped list counts execute zero iterations')
                        if q['op']=='witnessClear':check(value['oldSizes']==[0]*3 and value['currentSizes']==[0]*3 and value['sameLists']==[True]*3,'clear mutates retained lists in place')
                        if q['op']=='witnessDuplicates':check(value['oldSizes']==[1,2,1] and value['currentSizes']==[1,2,1] and value['sameLists']==[True]*3,'signature/script deduplication and unrestricted coin proof append')
                        if q.get('witnessExtra'):continue
                    if q.get('objectExtra'):
                        if 'expectedWrite' in q:check(value['state']==q['expectedWrite'],'nested writer preserves exact prefix before failure')
                        if 'expectedState' in q:check(value['state']==dict(appended=q['expectedState']) and value['stateText']==q['expectedText'],'state tag and retained data match source')
                        if 'expectedError' in q:check(value.get('readError')==q['expectedError'],'unknown state read fails with IOException')
                        if q['op']=='tokenConvert':check(value.get('convertError')==q['expectedConvertError'] and value.get('converted')==q['expectedConverted'],'Token converter logs IO exceptions and lets runtime exceptions escape')
                        if q['op']=='read':check(value['offset']<=len(bytes.fromhex(q['wire'][2:])),'object read consumes available bytes only')
                        continue
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
    except Exception as error:failures.append(dict(phase='execution',cases=count,error=repr(error),request=locals().get('q'),observations=locals().get('values')))
    finally:
        for which,r in enumerate(runners):
            with os.fdopen(os.dup(r.err.fileno()),'r') as err:
                try:r.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=which,error=repr(error)))
                err.seek(0);stderr=err.read()
                if which==1 and stderr:failures.append(dict(phase='native stderr',stderr=stderr))
    if count!=len(cases):failures.append(dict(phase='case accounting',executed=count,planned=len(cases)))
    report=dict(passed=not failures,cases=count,planned_cases=len(cases),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,explicit_source_implementations=implementations,adopted_types=len(adopted),scope='Actual Java/C++ common Streamable references: MiniByte, MMRProof, HORSTSignature, FORSSignature, SPHINCSSignature, MMRData, MMREntryNumber, MMREntry, MMRProofChunk, SignatureProof, Signature, MiniData, MiniNumber, MiniString, StateVariable, Address, ScriptProof, Token, Coin, CoinProof and Witness. Prefix writes, every small-vector read truncation, offset/trailing bytes and partial reserialization, synthetic failing implementation, actual MiniFile in-place load/write/append, MiniData conversion, Crypto single/pair hashing and NIO message construction. Exception classes compared; diagnostic prose excluded. Native complete stderr empty. Includes per-field MMR/signature readers, explicit flags, retained lists, scalar in-place assignment timing, MiniData zero-filled partial payloads and retained numeric cache with comparisons/shifts. Adds source-ordered object reads, derived identity timing, nested coin/state/token writer prefixes and actual Token failure logging. Witness shared container aliases, owned list replacement, clear/deduplication, TxPoW setter aliases and independent partial-list serialization. Prior twenty-type coverage retained. Remaining source implementers, arbitrary Java stream backends, allocation failures, concurrency and complete node/API parity remain unfinished.')
    (build/'streamable-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
