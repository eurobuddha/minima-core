#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'wallet-rows-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/WalletRowsOracle.java'];listing=build/'wallet-rows-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletRowsOracle'];cpp=[str(build/'minima_wallet_rows')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});lines=p.stdout.splitlines()
        if p.returncode or len(lines)!=len(cases):
            (build/'wallet-rows-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[];strings=[None,'','test-private-fixture','0x00','0xFFFF','RETURN TRUE','π Σ 😄 "\\\n']
    for kind in ['seed','key','script']:
        for i in range(250):
            fields={k:rng.choice(strings) for k in ['phrase','seed','modifier','privatekey','publickey','script']};fields['address']=rng.choice([None,'','0x','0x00','0x0123','0xGG','MxINVALID','0x'+rng.randbytes(32).hex()]);fields.update({k:rng.choice([None,0,-1,1,-2147483648,2147483647,rng.randint(-99999,99999)]) for k in ['size','depth','uses','maxuses','simple','defaultaddress','track']})
            for sql in [False,True]:cases.append(json.dumps({'type':kind,'sql':sql,'fail':0,'fields':fields},ensure_ascii=True))
            for fail in range(1,8):
                if i<10:cases.append(json.dumps({'type':kind,'sql':True,'fail':fail,'fields':fields},ensure_ascii=True))
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'SeedRow/KeyRow/ScriptRow direct and SQL column constructors, getters/JSON, null strings, nonzero integer booleans, Mx addresses, private-key JSON omission and SQL failure order; driver conversion/persistence excluded'}
    (build/'wallet-rows-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:4]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
