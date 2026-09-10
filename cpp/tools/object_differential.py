#!/usr/bin/env python3
"""Java-produced object fixtures, wire round trips, mutation and truncation tests."""
import argparse,json,random,subprocess,os
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve()
    oracle=build/'object-oracle';oracle.mkdir(exist_ok=True)
    sources=[JAVA/'src/org/minima/utils/Streamable.java']
    sources += [JAVA/'src/org/minima'/s for s in ['objects/base/MiniByte.java','objects/base/MiniData.java','objects/base/MiniNumber.java','objects/base/MiniString.java','system/genesis/GenesisCoin.java','system/genesis/GenesisMMR.java','system/genesis/GenesisTxPoW.java','objects/Greeting.java','objects/Pulse.java','objects/TxBlock.java','objects/mmr/MegaMMR.java','objects/TxPoW.java','objects/Magic.java','objects/TxBody.java','objects/TxHeader.java','system/params/GlobalParams.java','objects/keys/TreeKey.java','objects/keys/TreeKeyNode.java','objects/mmr/MMR.java','system/params/GeneralParams.java','objects/Witness.java','objects/keys/Signature.java','objects/keys/SignatureProof.java','objects/keys/Winternitz.java','objects/Transaction.java','objects/Coin.java','objects/CoinProof.java','objects/ScriptProof.java','objects/mmr/MMREntryNumber.java','objects/mmr/MMREntry.java','objects/Address.java','objects/StateVariable.java','objects/Token.java','objects/mmr/MMRData.java','objects/mmr/MMRProof.java','utils/BaseConverter.java','utils/Crypto.java']]
    sources += list((JAVA/'src/org/minima/utils/json').glob('*.java'))
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/ObjectOracle.java')],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ObjectOracle'];cpp=[str(build/'minima_objects')]
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180,env={**os.environ,'TZ':'UTC'})
        out=p.stdout.splitlines()
        if len(out)!=len(cases):raise RuntimeError(f'Unexpected output count {len(out)} for {len(cases)}\n{p.stdout[:2000]}')
        return out
    def hx(s):return '0x'+s.encode().hex()
    cases=[]
    for script in ['', 'RETURN TRUE','RETURN FALSE','LET x = 1 RETURN x EQ 1','🐼 "\\\n\t', '[hello]']:
        cases += [f'address {hx(script)}',f'leaf {hx(script)} 1.125',f'json {hx(script)} {hx(script)}']
    for port in [-1,0,127,128,255,256]:
        for value in ['0x','0x00','0xabc','TRUE','False','[hello]','1.00','1E3','-1E-45','',' ',' 1','[a\n🐼]']:
            cases.append(f'state {port} {hx(value)}')
    for scale in [-1,0,1,6,18,44]:
        for name in ['Panda','{"name":"Panda"}','  {invalid','🐼 "\\\n']:
            cases.append(f'token 0x00ff {scale} 0.00000001 {hx(name)} {hx("RETURN TRUE")} 7')
    rng=random.Random(5564)
    for i in range(90):
        data='0x'+rng.randbytes(i%65).hex();cases.extend([f'mx {data}',f'b32 {data}'])
        nodes=' '.join(f'{rng.randrange(2)} {hx(str(rng.randrange(100000)))} {rng.randrange(10)}.25' for _ in range(i%9))
        cases.append(f'proof {i} {hx("leaf")} 7.125 {nodes}'.strip())
    cases += [f'unb32 {s}' for s in ['Mx0','Mx1','MxFF','MxWYZ','MxILO','mx+0','Mx-0','Mx-1','Mx','MxX','MxVVVVVVVV','Mx00001']]
    # Escaping boundary code points plus malformed UTF-8 passed through MiniString.
    for n in list(range(33))+[127,159,160,0x1fff,0x2000,0x20ff,0x2100,0xffff,0x10000,0x1f43c]:cases.append(f'json {hx(chr(n))} 0x')
    for n in [0,1,-1,2,3,10,100,-101,2**128,2**256+1]:
        for scale in [-10,-1,0,1,2,15]:cases.append(f'entry {n} {scale}')
        for row in [0,1,255]:cases.append(f'mmrentry {row} {n}')
    for n in range(32):cases.extend([f'coin {n}',f'coinproof {n}'])
    for n in [0,1,4,7,31]:
        for growth in [0,1,17,257]:
            for mode in range(11):cases.append(f'coinalias {n} {growth} {mode}')
    cases += [f'scriptproof {hx(s)}' for s in ['', 'RETURN TRUE', 'RETURN FALSE','LET x = [🐼] RETURN TRUE']]
    for amounts in ['5 2','5 5','5 6','0 0','-1 1','1000000000 1000000000','1000000001 1']:
        for tokens in ['0x00 0x00','0x11 0x11','0x11 0x22','0x00 0xFF','0x00 -','- 0x00','- -']:
            for duplicate in [0,1]:cases.append(f'txn {amounts} {tokens} {duplicate}')
    for length in [0,31,32,33,64]:
        for msglen in [0,1,32,65]:cases.append(f'wots 0x{rng.randbytes(length).hex()} 0x{rng.randbytes(msglen).hex()}')
    cases += [f'witness {i}' for i in [0,1,2,5]]
    cases += [f'signature {i}' for i in [0,1,2,5]]
    cases += ['sigproof 0x00 0x','sigproof 0x00 0x123456']
    for size in [0,1,2,3,4,5,7,8,9,15,16,17,31,32,33]:
        for mode in ['normal','prune','scan','child']:cases.append(f'mmr {size} {mode}')
    for size,depth,uses in [(1,1,0),(2,1,0),(2,1,1),(2,2,0),(2,2,3),(3,2,8),(2,3,3),(0,1,0),(2,9,0),(2,0,0),(2,1,-1)]:
        cases.append(f'treekey 0x000102 {size} {depth} {uses} 2 0x1234')
    cases.append('treeverify 0x00 0x12 0x000100')
    for values in ['65536 1024 256','1 1 1','131072 2048 512','999999999 999999999 999999999','65537.125 1025.25 257.5']:
        for work in ['0x','0x00','0x01','0x'+('FF'*32)]:cases.append(f'magic {values} {work}')
    for count in [0,1,3]:cases.append(f'body 0x000102 {count}')
    for timestamp in [0,-1,99999999,1788883200000,2147483648000]:
        for run_length in [0,1,2,7,32]:cases.append(f'header {timestamp} {run_length}')
    maxhash=2**256-1
    for difficulty in [0,1,maxhash,maxhash//2,maxhash*2//3,maxhash*9//10,maxhash*20]:
        for mode in [0,1,2,3]:
            for nonce in [0,1,7]:cases.append(f'pow {nonce} 0x{difficulty:x} {mode}')
    for size in [1,3,5,8,17]:
        for flags in range(17):
            for time in [0,42]:cases.append(f'txblock {size} {flags} {time}')
    for size in [1,2,3,7,8,9,17,33]:
        for flags in range(16):
            for rounds in [1,3]:cases.append(f'mega {size} {flags} {rounds}')
    cases.append('mega 0 0 0')
    cases += [f'genesis {address}' for address in ['0x','0x00','0xFF','0x'+('12'*32),'0x123456']]
    for top in [-1,0,1,123,10**12,10**12+1]:
        for count in [0,1,3,65]:cases.append(f'greeting {top} {count} {hx("Welcome 🐼")}')
    cases += [f'pulse {count}' for count in [0,1,3,60,61,128]]
    expected=run(java,cases);actual=run(cpp,cases)
    derived=[]
    for case,value in zip(cases,expected):
        if value=='ERROR':continue
        typ=case.split()[0]
        if typ in ['address','state','token','leaf','proof','entry','mmrentry','coin','coinproof','scriptproof','txn','signature','sigproof','witness','mmr','magic','body','header','pow','txblock','mega','greeting','pulse']:
            wire=value.split('|')[0];derived.append(f'read {typ} {wire}');derived.append(f'read {typ} {wire}FFFF')
            raw=bytes.fromhex(wire[2:])
            for cut in sorted(set([0,1,2,len(raw)//2,len(raw)-1])):
                derived.append(f'read {typ} 0x{raw[:cut].hex()}')
        elif typ=='treekey':
            parts=value.split('|');pub=parts[0];message=case.split()[6]
            for i in range(2,len(parts),3):
                sig=parts[i+1]
                derived += [f'treeverify {pub} {message} {sig}',f'treeverify {pub} {message}FF {sig}',f'treeverify 0x00 {message} {sig}',f'treeverify {pub} {message} {sig[:-2]}']
        elif typ=='wots':
            pub,sig,_=value.split('|');message=case.split()[2]
            derived += [f'verify {pub} {message} {sig}',f'verify {pub} {message}FF {sig}',f'verify 0x00 {message} {sig}',f'verify {pub} {message} {sig[:-2]}',f'verify {pub} {message} {sig}00']
        elif typ=='mx':derived += [f'unmx {value}',f'unmx {value[:-1]}'+('1' if value[-1]!='1' else '0')]
        elif typ=='b32':derived.append(f'unb32 {value}')
    for version in [0,1,2]:derived.append(f'read pulse 0x0001{version:02x}0001010000000100')
    for payload in ['{}','{invalid','null','[]','123','{,"x":::1}','{"x":1e7}']:
        value=b'1.1.2.8';extra=payload.encode();wire=len(value).to_bytes(4,'big')+value+len(extra).to_bytes(4,'big')+extra+bytes.fromhex('000100000100');derived.append(f'read greeting 0x{wire.hex()}')
    cases+=derived;expected+=run(java,derived);actual+=run(cpp,derived)
    # A matching pair of generic ERROR replies is not evidence of alias fidelity.
    for case,value in zip(cases,expected):
        if case.startswith('coinalias ') and (value=='ERROR' or len(value.split('|'))!=38):
            raise RuntimeError(f'Invalid Java alias fixture: {case}: {value[:200]}')
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'coin_alias_cases':sum(c.startswith('coinalias ') for c in cases),'scope':'Object wire/hash/JSON fixtures plus Coin mutation aliases through transaction inputs/outputs, CoinProof and Witness after vector growth/removal/clear; explicit deepCopy, renamed-copy and wire-copy isolation. State-list identity across different Coin objects, MMRProof identity and full node parity remain unverified'}
    (build/'object-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({**report,'failures':failures[:15]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
