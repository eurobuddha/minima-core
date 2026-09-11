#!/usr/bin/env python3
"""Separate ordinary/hash virtual dispatch, nested prefix writes and mutable limits."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from horst_differential import number,data,hx
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
IO='java.io.IOException';EOF_ERROR='java.io.EOFException'
def rawtext(b):return hx(b) if b else ''
def write_model(q):
    raw=b'\x44'*q['length'];framed=data(raw);context=q['context'];method=q['method'];is_hash=method in ['hash','hashSerial'] or context in ['transaction','prng','id']
    dispatch=('hash' if is_hash else 'ordinary') if (q['kind']=='hash' if is_hash else q['kind']!='base') else None
    trace=[];written=b'';error=None;mutated=False
    if dispatch:trace.append(dispatch);written=bytes([0xB2 if is_hash else 0xA1]);mutated=q['replace']
    if dispatch is None or q['mode'].startswith('super'):
        if is_hash and len(raw)>q.get('hashLimit',64):error=f'{IO}: Write Error : HASH Length greater than {q.get("hashLimit",64)}! {len(raw)}'
        else:written+=framed
    if dispatch and error is None:
        trace.append(dispatch+'-after')
        if q['mode'].endswith('io'):error=f'{IO}: {dispatch} fixture'
    empty_tx=number(0)*3+data(b'\x00');empty_witness=number(0)*3
    capacities=number(65536)+number(1024)+number(256);work=data(((2**256-1)//10000).to_bytes(31,'big'))
    prefix,tail={
        'direct':(b'',b''),'transaction':(number(0)*3,b''),
        'prng':(b'',data(b'\x22')+empty_tx+empty_witness+empty_tx+empty_witness+number(0)),
        'id':(data(b'\x11')+data(b'\x22')+empty_tx+empty_witness+empty_tx+empty_witness+number(2)+data(b'\x33'),b''),
        'magic':(capacities,capacities+work)}[context]
    if context=='direct' and method in ['serial','hashSerial'] and error:written=b''
    result=dict(wire=hx(b'\xaa\x55'+prefix+written+(b'' if error else tail)),trace=trace)
    if error:result['error']=error
    if context!='direct':result['current']='0x99' if mutated else rawtext(raw)
    return result

def cases():
    result=[]
    for length,limit,kind,mode,method in itertools.product([0,1,63,64,65,128],[-1,0,1,64,65,128],['base','ordinary','hash'],['return','io','super','superio'],['write','serial','hash','hashSerial']):
        if kind=='base' and mode!='return':continue
        q=dict(context='direct',length=length,hashLimit=limit,kind=kind,mode=mode,method=method,replace=False);result.append((q,write_model(q)))
    for context,length,kind,mode,replace in itertools.product(['transaction','prng','id','magic'],[0,1,64,65],['base','ordinary','hash'],['return','io','super','superio'],[False,True]):
        if kind=='base' and (mode!='return' or replace):continue
        q=dict(context=context,length=length,kind=kind,mode=mode,method='write',replace=replace);result.append((q,write_model(q)))
    for method,limit,length in itertools.product(['read','readHash'],[-2,-1,0,1,63,64,65,128],[-2,-1,0,1,63,64,65,128]):
        header=length.to_bytes(4,'big',signed=True)
        for cut in sorted({0,max(0,length-1),max(0,length)}):
            payload=b'\x55'*cut;q=dict(context='direct',kind='base',mode='return',method=method,length=3,hashLimit=limit,dataLimit=limit,replace=False,wire=hx(header+payload))
            expected=dict(wire='0xAA55',trace=[],offset=4,value='0x444444')
            if length>limit:expected['errorClass']=IO
            elif length<0:expected['errorClass']=IO if method=='readHash' else 'java.lang.NegativeArraySizeException'
            else:
                expected['offset']+=min(length,cut);expected['value']=rawtext(payload[:length]+b'\x00'*max(0,length-cut))
                if cut<length:expected['errorClass']=EOF_ERROR
            result.append((q,expected))
    for method,cut in itertools.product(['read','readHash'],range(4)):
        q=dict(context='direct',kind='base',mode='return',method=method,length=3,replace=False,wire=hx(b'\x00'*cut));result.append((q,dict(wire='0xAA55',trace=[],offset=cut,value='0x444444',errorClass=EOF_ERROR)))
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'hash-writers-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/HashWritersOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();runners=[];failures=[];count=checks=0;transcript=hashlib.sha256()
    try:
        runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HashWritersOracle']))
        runners.append(Runner([str(build/'hash_writers')]))
        for index,(q,expected) in enumerate(corpus):
            values=[runner.ask(q) for runner in runners];count+=1
            if values[0]!=values[1]:failures.append(dict(case=index,request=q,java=values[0],cpp=values[1]))
            for runtime,value in enumerate(values):
                for key,answer in expected.items():
                    checks+=1;actual=value.get('error','').split(':',1)[0] if key=='errorClass' else value.get(key)
                    if actual!=answer:failures.append(dict(case=index,runtime=runtime,key=key,expected=answer,actual=actual))
                if 'error' not in expected and 'errorClass' not in expected:
                    checks+=1
                    if 'error' in value:failures.append(dict(case=index,runtime=runtime,unexpected_error=value['error']))
            transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
    except Exception as error:failures.append(dict(phase='execution',case=count,error=repr(error)))
    finally:
        for runtime,runner in enumerate(runners):
            with os.fdopen(os.dup(runner.err.fileno()),'r') as errors:
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
                errors.seek(0);stderr=errors.read()
                if stderr:failures.append(dict(phase='stderr',runtime=runtime,stderr=stderr))
    if count!=len(corpus):failures.append(dict(phase='accounting',executed=count,planned=len(corpus)))
    report=dict(passed=not failures,cases=count,planned_cases=len(corpus),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual ordinary/hash writers and readers; independent byte prefixes, virtual and qualified-base dispatch, source mutable length limits including negatives and 64/65 boundaries; actual Transaction link, TxBody PRNG/ID and Magic work writers with self-replacement while executing. Both native convenience serializers model staged output explicitly. Diagnostic text compared for represented errors. Java arbitrary stream implementations, byte-array identity/overridden non-stream methods, allocation and concurrency remain open.')
    (build/'hash-writers-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:4]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
