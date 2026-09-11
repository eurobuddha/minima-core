#!/usr/bin/env python3
"""Actual pinned JNI callers with explicit test-only native-library boundary."""
import argparse,copy,decimal,hashlib,json,os,shutil,subprocess,sys,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
decimal.getcontext().prec=128
D=decimal.Decimal

def signed(n):
    length=max(1,(n.bit_length()+8)//8);b=n.to_bytes(length,'big',signed=True)
    while len(b)>1 and ((b[0]==0 and b[1]<128) or (b[0]==255 and b[1]>=128)):b=b[1:]
    return b

def number(value):
    d=D(str(value)).normalize();t=d.as_tuple();unscaled=int(''.join(map(str,t.digits)) or '0')*(-1 if t.sign else 1)
    b=signed(unscaled);return bytes([(-t.exponent)&255,len(b)])+b

def read_number(data,offset=0):
    scale=int.from_bytes(data[offset:offset+1],'big',signed=True);length=data[offset+1];end=offset+2+length
    assert len(data)>=end
    value=D(int.from_bytes(data[offset+2:end],'big',signed=True)).scaleb(-scale)
    return value,end

def hx(b):return '0x'+b.hex().upper()

def corpus():
    cases=[]
    for name in ['benchmark','jnitest','jniminingtest']:
        key='hashes' if name=='benchmark' else 'maxattempts'
        def add(params=None,mode='run',**kw):cases.append(dict(name=name,mode=mode,params={} if params is None else params,**kw))
        add({'unused':'ignored'},'meta')
        add({key:'0'},'unloaded')
        add({'targetdifficulty':'0x'+'FF'*32}) # Keeps the real default amount; mining stops at first valid hash.
        for amount in ['0','-1','1','127','128','255','256','513','1.9','4294967296']:
            add({key:amount,'targetdifficulty':'0x00'})
        for amount in ['1','2','513']:
            add({key:amount,'targetdifficulty':'0x'+'FF'*32})
        for nonce in ['0','-1','-12345.54321','0.00000000000000000000000000000000000000001','18446744073709551615']:
            add({key:'0','testnonce':nonce,'targetdifficulty':'0x00'})
        for payload in [None,'0x','0x00','0x0000','0x0021','0x0001','0x000100AABB','0xFF0101','0x00028000']:
            add({key:'0','targetdifficulty':'0x00'},result=payload)
        for mode in ['hello_error','hash_error']:
            add({key:'0'},mode)
        for badkey,value in [(key,''),(key,None),(key,True),(key,'bad'),('targetdifficulty','xyz'),('testnonce','bad')]:
            add({key:'0',badkey:value})
        for amount in ['0','1']:
            add({key:amount,'targetdifficulty':'0x'+'FF'*32},mutate=0)
            add({key:amount,'targetdifficulty':'0x'+'00'*32},mutate=255)
        # Accepted but unused parameter/duplicate valid-parameter names are retained.
        add({key:'0','unused':'ignored','maxattempts':'0'})
    return cases

def execute(command):
    r=subprocess.run(command,capture_output=True,text=True,timeout=30)
    if r.returncode or r.stderr:raise AssertionError(dict(command=command,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr))
    return json.loads(r.stdout,parse_float=D)

def normalize(request,result):
    result=copy.deepcopy(result);checks=0;events=result['events'];name=request['name']
    if 'header' in events:
        assert events['hello'] is True;checks+=1
        header=bytes.fromhex(events['header'][2:]);nonce,nonce_end=read_number(header)
        expected_start=bytearray(number('100000000000000000.00000000000000000000000000000000000000001'));expected_start[4]=0
        assert header[:nonce_end]==expected_start;checks+=1
        key='hashes' if name=='benchmark' else 'maxattempts';amount=int(D(request['params'].get(key,1000000 if name=='jniminingtest' else 1000)))
        amount=(amount+2**31)%2**32-2**31
        assert events['attempts']==amount;checks+=1
        assert events['nonce']==hx(number(request['params'].get('testnonce','12345.54321')));checks+=1
        payload=request.get('result',events['nonce'])
        response=result.get('value',{}).get('response')
        if response is not None:
            data=bytes.fromhex(payload[2:]);final,_=read_number(data)
            final_header=number(final)+header[nonce_end:]
            txid=hx(hashlib.sha3_256(final_header).digest())
            comparison_diff=events['difficulty'] if name=='jniminingtest' and amount>0 else events.get('mutateddifficulty',events['difficulty'])
            found=int(txid[2:],16)<int(comparison_diff[2:] or '0',16)
            if name!='benchmark':assert response['targetdifficulty']==events.get('mutateddifficulty',events['difficulty']);checks+=1
            nativepart=response if name=='jnitest' else response.get('jni')
            if name!='benchmark':
                assert D(str(nativepart['nonce']))==final;assert nativepart['txpowid']==txid
                assert nativepart['success' if name=='jnitest' else 'found']==found;checks+=3
                nativepart['txpowid']='<checked against captured header>'
            if name=='jnitest':assert response['result']==''.join(str(int.from_bytes(bytes([x]),'big',signed=True))+',' for x in data);checks+=1
            if name=='jniminingtest':
                # Independently replay this caller's full requested loop with hashlib;
                # no reduced attempts or easy-difficulty substitution in hard cases.
                candidate=bytearray(header);found_java=False;java_nonce=D(0);attempts=0
                for i in range(max(0,amount)):
                    b=signed(i);candidate[4:4+len(b)]=b;attempts+=1
                    if int.from_bytes(hashlib.sha3_256(candidate).digest(),'big')<int(events['difficulty'][2:] or '0',16):
                        found_java=True;java_nonce,_=read_number(candidate);break
                j=response['java'];assert j['attempts']==attempts and j['found']==found_java and D(str(j['nonce']))==java_nonce;checks+=3
                javaid=hx(hashlib.sha3_256(number(java_nonce)+header[nonce_end:]).digest());assert j['txpowid']==javaid;checks+=1
                j['txpowid']='<checked against captured header>'
            times=[response] if name=='jnitest' else [response['java'],response['jni']]
            for value in times:
                assert isinstance(value['millitime'],int) and value['millitime']>=0;checks+=1;value['millitime']='<elapsed milliseconds>'
        # Different executions create headers at different times. Normalize only
        # the timestamp after validating each hash against its own original bytes.
        chain_length=int.from_bytes(header[nonce_end:nonce_end+4],'big');time_start=nonce_end+4+chain_length
        timestamp,time_end=read_number(header,time_start);assert abs(timestamp-D(time.time()*1000))<60000;checks+=1
        assert header[-36:-32]==bytes([0,0,0,32]);checks+=1
        # The source TxBody constructor uses 32 random PRNG bytes; its body hash
        # necessarily differs per call. Preserve its bytes for independent ID
        # checks above, then normalize only this final hash for cross-run comparison.
        events['header']=hx(header[:time_start]+number(0)+header[time_end:-32]+bytes(32))
    key='hashes' if name=='benchmark' else 'maxattempts'
    params=request['params'];expected_error=None
    for p in [key,'targetdifficulty','testnonce']:
        if p in params:
            v=params[p]
            if v is None:expected_error='java.lang.NullPointerException';break
            if not isinstance(v,str):expected_error='java.lang.ClassCastException';break
            if not v.strip():expected_error='org.minima.system.commands.CommandException';break
            if v in ['bad','xyz']:
                expected_error='org.minima.system.commands.CommandException' if p=='targetdifficulty' else 'java.lang.NumberFormatException';break
    if expected_error is None:
        if request['mode']=='unloaded':expected_error='java.lang.UnsatisfiedLinkError'
        elif request['mode'] in ['hello_error','hash_error']:expected_error='java.lang.IllegalStateException'
        elif 'result' in request:
            payload=request['result']
            if payload is None:expected_error='java.lang.NullPointerException'
            elif payload in ['0x','0x00','0x0001']:expected_error='java.io.EOFException'
            elif payload=='0x0000':expected_error='java.lang.NumberFormatException'
            elif payload=='0x0021':expected_error='java.io.IOException'
    assert result.get('error')==expected_error;checks+=1
    if expected_error is None:assert 'value' in result;checks+=1
    if request['mode']=='meta':
        assert result['value']['byteformat']==''.join(str(i if i<128 else i-256)+',' for i in range(256));checks+=1
    if result.get('error') in ['java.lang.NullPointerException','java.lang.ClassCastException','java.lang.NumberFormatException']:
        # JVM-specific enhanced NPE/cast/numeric parsing prose, already tested by
        # the existing command/base-value suites. Preserve the exact error class.
        result['message']='<runtime diagnostic>'
    return result,checks

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();start=time.monotonic()
    oracle=build/'jni-command-oracle';oracle.mkdir(exist_ok=True)
    javac=Path(shutil.which('javac')).resolve();java_home=javac.parent.parent
    library=oracle/('libfixture.dylib' if sys.platform=='darwin' else 'libfixture.so')
    include=java_home/'include';platform='darwin' if sys.platform=='darwin' else 'linux'
    subprocess.run(['c++','-std=c++20','-shared','-fPIC','-I'+str(include),'-I'+str(include/platform),str(ROOT/'support/jni_command_fixture.cpp'),'-o',str(library)],check=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/JNICommandOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run([str(javac),'-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cases=corpus();observations=[];failures=[];checks=0
    for index,request in enumerate(cases):
        values=[];raw=[]
        try:
            for command in [['java','--enable-native-access=ALL-UNNAMED','-cp',f'{oracle}:{JAVA}/jar/minima.jar','JNICommandOracle'],[str(build/'minima_jni_commands')]]:
                command=command+[json.dumps(request)]+([str(library)] if command[0]=='java' else [])
                result=execute(command);raw.append(result);value,count=normalize(request,result);checks+=count;values.append(value)
            if values[0]!=values[1]:raise AssertionError('Java/C++ mismatch')
            observations.append(dict(request=request,normalized=values[0]))
        except Exception as error:
            failures.append(dict(case=index,request=request,raw=raw,normalized=values,error=repr(error)));break
    report=dict(passed=not failures,programs=len(observations),planned_programs=len(cases),independent_checks=checks,seconds=round(time.monotonic()-start,3),failures=failures,observations=observations,
                scope='Unchanged Java commands use an explicit recording JNI fixture, native callers use a corresponding provider. Metadata, signed-byte formatting, defaults, full nonce loops, fixed-offset header mutation, failure propagation, nonce decoding and independently recomputed SHA3 IDs are checked. Only timing, validated creation timestamps/IDs and random body hashes and selected JVM diagnostic prose are normalized. Target-buffer mutation and warmed versus cold numeric-cache behavior are checked with fixture writes. No original JNI library algorithm, arbitrary JNI buffer/lifetime behavior, JNI loader or full caller/runtime parity is claimed.')
    (build/'jni-command-callers-results.json').write_text(json.dumps(report,indent=2,default=str)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2,default=str));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
