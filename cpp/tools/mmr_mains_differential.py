#!/usr/bin/env python3
"""Exact debug output, including NUL bytes, and diagnostic nonmutation checks."""
import argparse,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'mmr-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/MMRMainsOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MMRMainsOracle'],[str(build/'mmr_mains')]]
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    cases=[['MMR',mode] for mode in ['null','empty','args']]+[['MegaMMR',mode,flag] for flag in ['true','false'] for mode in ['null','empty','args']]
    cases += [[op,mode] for op in ['printinfo','printmmrtree'] for mode in ['null','empty']]
    cases += [[op,'many',str(n)] for op in ['printinfo','printmmrtree'] for n in [1,10,20]]
    cases += [['printmmrtree','entry',str(row),str(index),str(scale),str(value),star] for row,index,scale,value,star in [(0,0,0,1,'true'),(0,2,0,-2147483648,'true'),(0,341,0,12,'false'),(0,342,0,1,'false'),(0,-1,0,1,'false'),(0,2**32,0,1,'false'),(0,19,1,2,'false'),(10,0,0,1,'false'),(30,0,0,1,'false'),(31,0,0,1,'false'),(32,0,0,1,'false')]]
    try:
        for args in cases:
            rows=[]
            for command in commands:
                p=subprocess.run(command+args,capture_output=True,text=True,timeout=30,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
                check(p.returncode==0 and not p.stderr,(args,p.returncode,p.stderr));rows.append(json.loads(p.stdout))
            report['observations'].append(dict(args=args,java=rows[0],cpp=rows[1]))
            comparable=[{k:v for k,v in r.items() if k!='message' or r['error']!='java.lang.NullPointerException'} for r in rows]
            check(comparable[0]==comparable[1],(args,rows));report['comparisons']+=1
            row=rows[0]
            if row['error']=='none' and args[0] in ['printinfo','printmmrtree']:check(row['before']==row['after'],args)
            if args[0]=='MMR':check(row['stdout'].startswith('** MMR Tree Prune POC **\n') and '\0'*1536 in row['stdout'] and row['stdout'].endswith('Tree Peak Sum Value : 10\n\n'),args)
            if args[0]=='MegaMMR':check(row['stdout'].count('Total Entries       : ')==6 and row['stdout'].count('Tree Peak Sum Value : ')==6 and '\0'*1536 in row['stdout'],args)
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Exact complete main/printer output including embedded NULs, source arguments, pruning on/off, direct empty/multileaf/overflow/negative/fractional-index diagnostics and unchanged serialized tree. Null diagnostic error class compared, VM-generated helpful-null message not equated. Actual randomized MegaMMR coin draws remain, but random identities are not printed and their distribution is not certified. Full MMR API/concurrency parity remains unverified.'
        (build/'mmr-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
