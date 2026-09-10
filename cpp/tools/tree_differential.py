#!/usr/bin/env python3
"""Java/C++ tree weight, selection, traversal, trimming and node-wire comparisons."""
import argparse,json,random,subprocess,os
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve()
    oracle=build/'tree-oracle';oracle.mkdir(exist_ok=True)
    sources=[JAVA/'src/org/minima/utils/Streamable.java']
    sources += [JAVA/'src/org/minima'/s for s in ['objects/base/MiniByte.java','objects/base/MiniData.java','objects/base/MiniNumber.java','objects/base/MiniString.java','system/genesis/GenesisCoin.java','system/genesis/GenesisMMR.java','system/genesis/GenesisTxPoW.java','objects/Greeting.java','objects/Pulse.java','objects/TxBlock.java','objects/mmr/MegaMMR.java','objects/TxPoW.java','objects/Magic.java','objects/TxBody.java','objects/TxHeader.java','system/params/GlobalParams.java','objects/keys/TreeKey.java','objects/keys/TreeKeyNode.java','objects/mmr/MMR.java','system/params/GeneralParams.java','objects/Witness.java','objects/keys/Signature.java','objects/keys/SignatureProof.java','objects/keys/Winternitz.java','objects/Transaction.java','objects/Coin.java','objects/CoinProof.java','objects/ScriptProof.java','objects/mmr/MMREntryNumber.java','objects/mmr/MMREntry.java','objects/Address.java','objects/StateVariable.java','objects/Token.java','objects/mmr/MMRData.java','objects/mmr/MMRProof.java','utils/BaseConverter.java','utils/Crypto.java']]
    sources += list((JAVA/'src/org/minima/utils/json').glob('*.java'))
    sources += [JAVA/'src/org/minima/database/txpowtree'/s for s in ['TxPowTree.java','TxPoWTreeNode.java','TxPoWTreeNodeAction.java']]
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/TreeOracle.java')],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TreeOracle'];cpp=[str(build/'minima_tree')]
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180,env={**os.environ,'TZ':'UTC'})
        out=p.stdout.splitlines()
        if len(out)!=len(cases):raise RuntimeError(f'Unexpected output count {len(out)} for {len(cases)}\n{p.stdout[:1000]}')
        return out
    cases=[];rng=random.Random(5564)
    for count in [0,1,2,3,7,8,9,17,33]:
        for mode in range(8):cases.append(f'blocknode {count} {mode}')
    for i in range(450):
        count=rng.randrange(65);trim=rng.choice([-999,-1,0,1,2,3,10,100]);depth=rng.choice([-1,0,1,2,5,100]);spec=[];heights=[]
        for n in range(count):
            parent=rng.randrange(n) if n else -1
            height=heights[parent]+1 if parent>=0 else 0
            if i%7==0:height=rng.randrange(-3,10)
            heights.append(height);weight=rng.randrange(-3,15);ident=f'0x{n:02X}' if i%11 else f'0x{n%3:02X}'
            spec.append(f'{parent} {height} {weight} {ident}')
        cases.append(f'tree {count} {trim} {depth} '+ ' '.join(spec))
    cases.append('tree 513 -999 1 '+ ' '.join(f'{i-1} {i} 1 0x{i:04X}' for i in range(513)))
    expected=run(java,cases);actual=run(cpp,cases);derived=[]
    for case,value in zip(cases,expected):
        if not case.startswith('blocknode ') or value=='ERROR':continue
        wire=value.split('|')[0];derived.extend([f'readnode {wire}',f'readnode {wire}FFFF']);raw=bytes.fromhex(wire[2:])
        for cut in sorted({0,1,2,len(raw)//2,len(raw)-1}):derived.append(f'readnode 0x{raw[:cut].hex()}')
    cases+=derived;expected+=run(java,derived);actual+=run(cpp,derived)
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Tree GHOST selection and node wire/MMR without runtime wallet/SQL integrations; generic errors normalized'}
    (build/'tree-differential-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
