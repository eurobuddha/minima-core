#!/usr/bin/env python3
"""Independent Java/C++ cascade wire, weight, pruning and parent-validation comparisons."""
import argparse,json,random,subprocess,os
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve()
    oracle=build/'cascade-oracle';oracle.mkdir(exist_ok=True)
    sources=[JAVA/'src/org/minima/utils/Streamable.java']
    sources += [JAVA/'src/org/minima'/s for s in ['objects/base/MiniByte.java','objects/base/MiniData.java','objects/base/MiniNumber.java','objects/base/MiniString.java','system/genesis/GenesisCoin.java','system/genesis/GenesisMMR.java','system/genesis/GenesisTxPoW.java','objects/Greeting.java','objects/Pulse.java','objects/TxBlock.java','objects/mmr/MegaMMR.java','objects/TxPoW.java','objects/Magic.java','objects/TxBody.java','objects/TxHeader.java','system/params/GlobalParams.java','objects/keys/TreeKey.java','objects/keys/TreeKeyNode.java','objects/mmr/MMR.java','system/params/GeneralParams.java','objects/Witness.java','objects/keys/Signature.java','objects/keys/SignatureProof.java','objects/keys/Winternitz.java','objects/Transaction.java','objects/Coin.java','objects/CoinProof.java','objects/ScriptProof.java','objects/mmr/MMREntryNumber.java','objects/mmr/MMREntry.java','objects/Address.java','objects/StateVariable.java','objects/Token.java','objects/mmr/MMRData.java','objects/mmr/MMRProof.java','utils/BaseConverter.java','utils/Crypto.java']]
    sources += list((JAVA/'src/org/minima/utils/json').glob('*.java'))
    sources += [JAVA/'src/org/minima/database/cascade'/s for s in ['Cascade.java','CascadeNode.java']]
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/CascadeOracle.java')],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CascadeOracle'];cpp=[str(build/'minima_cascade')]
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180,env={**os.environ,'TZ':'UTC'})
        out=[line for line in p.stdout.splitlines() if not line.startswith('Minima @ ')]
        if len(out)!=len(cases):raise RuntimeError(f'Unexpected output count {len(out)} for {len(cases)}\n{p.stdout[:1000]}')
        return out
    cases=[];rng=random.Random(5564)
    for a in [0,1,-1,9999999,99999995,99999985,123456750,123456650,10**40+17]:
        for scale in [-10,-1,0,1,6,7,10,50]:
            for factor in [1,-2,2**31]:cases.append(f'weight {a} {scale} {factor} {rng.randrange(-100000,100000)} {rng.randrange(-10,11)}')
    for i in range(250):cases.append(f'weight {rng.randrange(-10**80,10**80)} {rng.randrange(-40,41)} {rng.randrange(-10**10,10**10)} {rng.randrange(-10**80,10**80)} {rng.randrange(-40,41)}')
    for n in range(12):
        for d in [1,2,3,7]:
            for level in [-1,0,1,7,23,31,64]:cases.append(f'node {n} {d} {level}')
    for quota in [1,2,3,256]:
        for n in [0,1,2,9,33]:
            for mode in [0,1,2,4,8,16,32,33,34,36,40,48,64,65,66,68,72,80]:cases.append(f'chain {quota} {n} {1+n%3} {mode}')
    cases += ['chain 256 257 1 32','chain 256 513 1 32','chain 1 100 1 33']
    expected=run(java,cases);actual=run(cpp,cases);derived=[]
    for case,value in zip(cases,expected):
        if value=='ERROR':continue
        op=case.split()[0]
        if op not in ['chain','node']:continue
        wire=value.split('|')[0];prefix='read '+case.split()[1] if op=='chain' else 'readnode'
        derived.extend([f'{prefix} {wire}',f'{prefix} {wire}FFFF'])
        if rng.randrange(5)==0:
            raw=bytes.fromhex(wire[2:])
            for cut in sorted({0,1,len(raw)//2,len(raw)-1}):derived.append(f'{prefix} 0x{raw[:cut].hex()}')
    derived += ['read 256 0x0001FF','read 256 0x000100']
    cases+=derived;expected+=run(java,derived);actual+=run(cpp,derived)
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Cascade wire, arithmetic, pruning, validity and display; generic errors normalized; file persistence pending'}
    (build/'cascade-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
