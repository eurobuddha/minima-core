#!/usr/bin/env python3
"""Pinned TransactionTest public methods and the actual full-size source main."""
import argparse,decimal,hashlib,json,re,subprocess,time
from pathlib import Path
from horst_differential import ROOT,JAVA,hx,number,data
from peers_checker_differential import Runner
from fors_differential import checked_demo
from sphincs_utils_differential import token

def javahex(s):
    b=s.removeprefix('0x');b=('0' if len(b)%2 else '')+b
    return '0x'+b.upper() if b else ''
def expected(q):
    i=q.get('index',0);address=javahex(q.get('address','0x001122'));tid=javahex(q.get('tokenID',token if i%2 else '0x00'));keep=q.get('keep',i%2==0)
    with decimal.localcontext() as c:
        c.prec=64;c.rounding=decimal.ROUND_DOWN
        amount=decimal.Decimal(q.get('amount','0.125'));scale=int(decimal.Decimal(q.get('scale','3' if i%2 else '0')))
        for _ in range(max(0,scale)):amount*=10
        text=format(amount,'f')
        if '.' in text:text=text.rstrip('0').rstrip('.')
        if amount==0:text='0'
    return address+'AMOUNT'+text+'TOKENID'+tid+('TRUE' if keep else 'FALSE')

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');a=p.parse_args();build=a.build.resolve();oracle=build/'transaction-test-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['ObjectOracle.java','HORSTOracle.java','TransactionTestOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-XX:-OmitStackTraceInFastThrow','-Xmx2g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TransactionTestOracle'],[str(build/'minima_transaction_test')]]
    runners=[Runner(c) for c in commands];cases=0;checks=0;failures=[];transcript=hashlib.sha256();start=time.monotonic()
    def compare(q,expect):
        nonlocal cases,checks
        values=[r.ask(q) for r in runners];cases+=1;transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=cases,request=q,java=values[0],cpp=values[1]))
        for v in values:
            checks+=1
            if v!=expect:failures.append(dict(index=cases,request=q,expected=expect,actual=v))
    try:
        for n in range(32):q=dict(action='format',index=n);compare(q,dict(value=expected(q),printed=''))
        for n in [0,1]:
            for amount in ['0','-0.125','1.2500','999999999.125','0.000000000000000000000000000000000000001']:
                for scale in ['-1','0','1','3','8','1.9']:
                    for keep in [False,True]:
                        q=dict(action='format',index=n,amount=amount,scale=scale,keep=keep);compare(q,dict(value=expected(q),printed=''))
        for field in ['address','tokenID']:
            for value in ['0x','0x0','0xF','0xabc','0x'+'AA'*65]:
                q=dict(action='format',index=1,**{field:value});compare(q,dict(value=expected(q),printed=''))
        compare(dict(action='format',nullCoin=True),dict(error=dict(exception='java.lang.NullPointerException',message=''),printed=''))
        for message in [None,'','hello','students 🐼','a\nb','a\0b']:
            compare(dict(action='log',message=message),dict(printed=('null' if message is None else message)+'\n'))
        assert not runners[1].errors(),'Native helper stderr'
    finally:
        for r in runners:r.close()
    partial=dict(cases=cases,passed=cases-len({f['index'] for f in failures if 'index' in f}),independent_assertions=checks,failures=failures,phase='public APIs complete; actual mains next');(build/'transaction-test-progress.json').write_text(json.dumps(partial,indent=2)+'\n');print(json.dumps(partial),flush=True)
    # Each main still creates all sixteen 65536-leaf trees. The fixed wrong
    # script public key is preserved; do not short-circuit signing or rewrite it.
    demos=[checked_demo(c,timeout=3600) for c in commands]
    if demos[0].stdout!=demos[1].stdout:failures.append(dict(main_java=demos[0].stdout,main_cpp=demos[1].stdout))
    stdout=demos[0].stdout;lines=stdout.splitlines();message_line=next(l[len('Message Str : '):] for l in lines if l.startswith('Message Str : '));encoded=next(l[len('Message : '):] for l in lines if l.startswith('Message : '));assert encoded==hx(message_line.encode());checks+=1
    # Independently reconstruct the demonstration's token and signing message.
    demo_token=hx(hashlib.sha3_256(data(data(b'\0')+data(b'RETURN TRUE')+number(1)+number(1)+data(b'paddy')+number(0))).digest())
    expected_message='7SPHINCS2SPHINCS'+''.join(f'0x0{i}' for i in range(7))+'COINJOIN'+('0xAAAMOUNT10TOKENID'+demo_token+'FALSE')*2
    assert message_line==expected_message;checks+=1
    assert 'Valid Contract : false' in lines;checks+=1
    variables=json.loads(next(l[len('Variables      : '):] for l in lines if l.startswith('Variables      : ')))
    script=(JAVA/'src/org/minima/utils/sphincs/TransactionTest.java').read_text();fixed=re.search(r'LET sphincspublickey=(0x[0-9A-F]+)',script)[1]
    assert variables['sphincspublickey']==fixed;checks+=1
    (build/'transaction-test-demo-output.json').write_text(json.dumps(dict(java_stdout=stdout,cpp_stdout=demos[1].stdout,native_stderr=demos[1].stderr,returncodes=[d.returncode for d in demos],UBSAN_OPTIONS='halt_on_error=1'),indent=2)+'\n')
    report=dict(cases=cases,passed=cases-len({f['index'] for f in failures if 'index' in f}),independent_assertions=checks,failures=failures,source_mains_matched=demos[0].stdout==demos[1].stdout,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),scope='Actual pinned TransactionTest public formatter/log and full-size demonstration, exact stdout and independent message/token checks. Fixed script key/one-iteration loop and no explicit coin-ID recomputation preserved. Full node parity and arbitrary subclasses/null Coin internals remain unverified.')
    (build/'transaction-test-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
