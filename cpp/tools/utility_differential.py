#!/usr/bin/env python3
"""Java/C++ crypto interoperability, formatting, gzip and isolated persistence tests."""
import argparse,json,random,subprocess,os,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve()
    oracle=build/'utility-oracle';oracle.mkdir(exist_ok=True)
    sources=[JAVA/'src/org/minima/utils/Streamable.java']
    sources += [JAVA/'src/org/minima'/s for s in ['objects/base/MiniByte.java','objects/base/MiniData.java','objects/base/MiniNumber.java','objects/base/MiniString.java','system/genesis/GenesisCoin.java','system/genesis/GenesisMMR.java','system/genesis/GenesisTxPoW.java','objects/Greeting.java','objects/Pulse.java','objects/TxBlock.java','objects/mmr/MegaMMR.java','objects/TxPoW.java','objects/Magic.java','objects/TxBody.java','objects/TxHeader.java','system/params/GlobalParams.java','objects/keys/TreeKey.java','objects/keys/TreeKeyNode.java','objects/mmr/MMR.java','system/params/GeneralParams.java','objects/Witness.java','objects/keys/Signature.java','objects/keys/SignatureProof.java','objects/keys/Winternitz.java','objects/Transaction.java','objects/Coin.java','objects/CoinProof.java','objects/ScriptProof.java','objects/mmr/MMREntryNumber.java','objects/mmr/MMREntry.java','objects/Address.java','objects/StateVariable.java','objects/Token.java','objects/mmr/MMRData.java','objects/mmr/MMRProof.java','utils/BaseConverter.java','utils/Crypto.java']]
    sources += list((JAVA/'src/org/minima/utils/json').glob('*.java'))
    sources += [JAVA/'src/org/minima/database/cascade'/s for s in ['Cascade.java','CascadeNode.java']]
    sources += list((JAVA/'src/org/minima/utils/encrypt').glob('*.java'))
    sources += [JAVA/'src/org/minima/utils'/s for s in ['MiniFile.java','MiniFormat.java']]
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/UtilityOracle.java')],check=True)
    temp=tempfile.TemporaryDirectory(prefix='utility-fixtures-',dir=build);scratch=Path(temp.name)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','UtilityOracle',str(scratch/'java')];cpp=[str(build/'minima_utility'),str(scratch/'cpp')]
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180,env={**os.environ,'TZ':'UTC'})
        out=[x for x in p.stdout.splitlines() if not x.startswith('Minima @ ')]
        if len(out)!=len(cases):raise RuntimeError(f'Unexpected output count {len(out)} for {len(cases)}\n{p.stdout[:1000]}')
        return out
    def hx(s):return '0x'+(s.encode() if isinstance(s,str) else s).hex()
    cases=[];rng=random.Random(5564)
    # SunJCE accepts no ciphertext as a successful empty padded decryption.
    for size in [16,24,32]:cases.append(f'aesdec {hx(bytes(16))} {hx(bytes(size))} 0x')
    for password in ['', 'password','🐼 é','a\x00b','  spaces  ']:
        for length in [0,1,8,16,32]:cases.append(f'derive {hx(password)} {hx(rng.randbytes(length))}')
    for length in [0,1,2,15,16,17,31,32,33,255,512,2048]:
        for kl in [0,1,16,24,32,33]:
            for il in [0,16,17]:cases.append(f'aesenc {hx(rng.randbytes(il))} {hx(rng.randbytes(kl))} {hx(rng.randbytes(length))}')
        for data in [bytes(length),rng.randbytes(length),b'abcdef'*(length//6)]:cases.append(f'gzip {hx(data)}')
    for n in [-2**63,-1,0,1,999,1000,1023,1024,1280,1536,2**63-1]+[rng.randrange(2**63) for _ in range(100)]:cases.extend([f'size {n}',f'time {n}'])
    for k in range(1,7):
        for n in [-1,0,1,256,512,768]:cases.append(f'size {2**(k*10)+n}')
    for n in ['0','-1.1','1.9','12345678901234567890']:
        for size in [-1,0,1,4,32]:cases.append(f'pad {size} {n}')
    for ext in ['html','htm','css','js','txt','xml','jpg','jpeg','png','gif','svg','ico','ttf','zip','pdf','wasm','mp3','wav','HTML','','abc']:cases.append(f'type {hx("path/file."+ext)}')
    for p,c in [('/tmp/a','/tmp/a'),('/tmp/a','/tmp/ab/file'),('/tmp/a','/tmp/a/../b'),('/tmp/a/','/tmp/a'),('.', './child'),('/','/tmp/a')]:cases.append(f'child {hx(p)} {hx(c)}')
    for obj in ['{}','[]','[{}]','[{},{}]','[1]','{"a":[1,2],"b":{"x":"y"}}','{"a":"a\\n/\\\\b"}','{"quote":"a\\"b","x":1}','{"🐼":"é"}','null']:
        cases.append(f'pretty {hx(obj)}')
    cases += [f'files {hx(rng.randbytes(n))}' for n in [0,1,16,256,2048]]
    keypairs=[]
    for cmd in [java,cpp]:
        for value in run(cmd,['gen','gen']):
            if value=='ERROR':raise RuntimeError('Key generation failed')
            keypairs.append(value.split('|'))
    for pub,priv in keypairs:
        cases += [f'key public {pub}',f'key private {priv}',f'key public {pub}FF',f'key private {priv}FF',f'key public {priv}',f'key private {pub}',f'key public {pub[:-2]}',f'key private {priv[:-2]}']
        # Private-key encryption uses deterministic PKCS#1 type-1 padding.
        # These are direct calls to the source cipher factory, beyond the wrappers.
        for n in [0,1,32,116,117,118,128]:cases.append(f'rsacipher private 1 {priv} {hx(rng.randbytes(n))}')
        for n in [0,1,32,117,118,128,2048]:cases.append(f'sign {priv} {hx(rng.randbytes(n))}')
    expected=run(java,cases);actual=run(cpp,cases);derived=[]
    for case,value in zip(cases,expected):
        if value=='ERROR':continue
        p=case.split();op=p[0]
        if op=='aesenc':
            prefix=f'aesdec {p[1]} {p[2]}'
            derived.extend([f'{prefix} {value}',f'{prefix} {value}00',f'{prefix} {value[:-2]}',f'{prefix} {value[:-2]}00'])
        elif op=='rsacipher':
            pub=next(pub for pub,priv in keypairs if priv==p[3]);prefix=f'rsacipher public 2 {pub}'
            derived.extend([f'{prefix} {value}',f'{prefix} {value[:-2]}',f'{prefix} {value}00',f'{prefix} {hx(bytes(128))}'])
        elif op=='gzip':derived.extend([f'ungzip {value}',f'ungzip {value}00',f'ungzip {value[:-2]}',f'ungzip {value}{value[2:]}'])
        elif op=='sign':
            pub=next(pub for pub,priv in keypairs if priv==p[1]);prefix=f'verify {pub} {p[2]}'
            derived.extend([f'{prefix} {value}',f'{prefix} {value[:-2]}',f'{prefix} {value}00',f'{prefix} {value[:-2]}00',f'verify {pub} {p[2]}00 {value}'])
    # Cross decrypt randomized encryption produced by each language.
    random_cases=[];plain=[];decrypt_prefix=[]
    for pub,priv in keypairs:
        for n in [0,1,32,117,118,1024]:
            data=hx(rng.randbytes(n));random_cases += [f'rsaenc {pub} {data}',f'packenc {pub} {data}'];plain += [data.upper().replace('0X','0x') if n else '']*2;decrypt_prefix += [f'rsadec {priv}',f'packdec {priv}']
    for n in [0,1,255,512,1024,4096]:
        data=hx(rng.randbytes(n));random_cases.append(f'pwenc {hx("🐼 password")} {data}');plain.append(data.upper().replace('0X','0x') if n else '');decrypt_prefix.append(f'pwdec {hx("🐼 password")}')
    cross=[]
    for cmd in [java,cpp]:
        encrypted=run(cmd,random_cases)
        for case,value,prefix,plaintext in zip(random_cases,encrypted,decrypt_prefix,plain):
            if value=='ERROR':
                if case.startswith('rsaenc ') and len(bytes.fromhex(case.split()[2][2:]))>117:continue
                raise RuntimeError('Encryption failed: '+case[:60])
            cross.append((f'{prefix} {value}',plaintext));derived += [f'{prefix} {value[:-2]}',f'{prefix} {value}00']
    derived += [c for c,_ in cross]
    cases+=derived;expected+=run(java,derived);actual+=run(cpp,derived)
    # Independent recovery checks require the original bytes, not just agreement.
    reverse=[(f'rsacipher public 2 {next(pub for pub,priv in keypairs if priv==c.split()[3])} {v}', c.split()[4].upper().replace('0X','0x') if c.split()[4]!='0x' else '') for c,v in zip(cases,expected) if c.startswith('rsacipher private 1 ') and v!='ERROR']
    reverse_cases=[c for c,_ in reverse]
    cases+=reverse_cases;expected+=run(java,reverse_cases);actual+=run(cpp,reverse_cases)
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    for (c,plain),j,n in zip(reverse,expected[-len(reverse):],actual[-len(reverse):]):
        if j!=plain or n!=plain:failures.append({'case':c,'expected_plaintext':plain,'java':j,'cpp':n})
    # Independently require all successful cross decryptions to recover their original data.
    for (c,p),j,n in zip(cross,expected[-len(cross)-len(reverse):-len(reverse)],actual[-len(cross)-len(reverse):-len(reverse)]):
        if j!=p or n!=p:failures.append({'case':c,'expected_plaintext':p,'java':j,'cpp':n})
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'reverse_rsa_recoveries':len(reverse),'scope':'Direct RSA private-encrypt/public-recover plus crypto interoperability/formatting/gzip and normal isolated file persistence; runtime integration and IO failure parity remain under review'}
    (build/'utility-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({**report,'failures':[{**f,'case':f['case'][:200]} for f in failures[:8]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
