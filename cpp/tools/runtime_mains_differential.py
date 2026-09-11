#!/usr/bin/env python3
"""Real full-work miner, fixed-seed successive TreeKey signing and Cascade mains."""
import argparse,json,os,subprocess,tempfile,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'runtime-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','RuntimeMainsOracle.java','CascadeOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{JAVA}/jar/minima.jar';commands=[['java','-cp',cp,'RuntimeMainsOracle'],[str(build/'runtime_mains')]]
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    with tempfile.TemporaryDirectory(prefix='runtime-main-fixture-',dir=build) as folder:
        scratch=Path(folder);literal=scratch/r'C:\Users\spartacusrex\.minima\1.0\databases\cascade.db'
        # Existing pinned-source cascade fixture generates deterministic valid
        # and invalid-parent files. Both mains read the exact same bytes.
        input_cases=['chain 256 0 1 0','chain 256 3 1 0','chain 256 3 1 8']
        generated=subprocess.run(['java','-cp',cp,'CascadeOracle'],input='\n'.join(input_cases)+'\n',capture_output=True,text=True,check=True)
        wires=[line.split('|')[0] for line in generated.stdout.splitlines() if line.startswith('0x')];check(len(wires)==3,generated.stdout)
        cases=[(name,mode,None) for name in ['TxPoWMiner','TreeKey'] for mode in ['null','empty','args']]
        cases += [('Cascade',mode,kind) for mode in ['null','empty','args'] for kind in ['missing','empty','valid','invalid']]
        for name,mode,kind in cases:
            if kind=='missing':literal.unlink(missing_ok=True)
            elif kind is not None:literal.write_bytes(bytes.fromhex(wires[['empty','valid','invalid'].index(kind)][2:]))
            before=literal.read_bytes() if literal.exists() else None;rows=[];durations=[]
            for cmd in commands:
                start=time.monotonic();p=subprocess.run(cmd+[name,mode],cwd=scratch,capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});durations.append(round(time.monotonic()-start,3))
                check(p.returncode==0 and not p.stderr,(name,mode,kind,p.returncode,p.stderr));rows.append(json.loads(p.stdout))
            report['observations'].append(dict(name=name,mode=mode,file=kind,java=rows[0],cpp=rows[1],seconds=durations));check(rows[0]==rows[1],(name,mode,kind,rows));report['comparisons']+=1
            row=rows[0];check(row['error']=='none',row)
            if name=='TxPoWMiner':check(row['stdout']=='' and row['notifications']==0,row)
            elif name=='TreeKey':
                output=row['stdout'];check(output.startswith('1\n') and row['notifications']==4,row)
                decoder=json.JSONDecoder();first,offset=decoder.raw_decode(output[2:]);rest=output[2+offset:].lstrip();check(rest.startswith('2\n'),rest[:100]);second,end=decoder.raw_decode(rest[2:]);check(not rest[2+end:].strip() and first!=second,(first,second))
            else:
                check(row['stdout'].endswith('Correct:'+('false' if kind=='invalid' else 'true')+'\n'),row)
                check((literal.read_bytes() if literal.exists() else None)==before,'Cascade main changed its input file')
                check(row['notifications']==(1 if kind in ['missing','invalid'] else 0),row)
    report['passed']=True;report['scope']='18 exact Java/native main comparisons with null/empty/unused arguments. Full miner work retained (10000+1000000 old hashes, 1000000 new header hashes); silent completion/timing recorded, internal invocation counts not instrumented. Two complete fixed-seed signature JSON outputs and logger notifications. Cascade missing/empty/valid/invalid-parent files under fixture-owned cwd with exact shared source bytes, read-only input and notifications. Full class/provider/concurrency parity and malformed file error logging remain unverified.'
    (build/'runtime-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
