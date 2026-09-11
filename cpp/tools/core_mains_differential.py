#!/usr/bin/env python3
"""Four real entry points, complete Contract trace, availability exception boundary."""
import argparse,json,os,subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'core-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','CoreMainsOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CoreMainsOracle'],[str(build/'core_mains')]]
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def run(command,args):
        p=subprocess.run(command+args,capture_output=True,text=True,timeout=30,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if p.returncode or p.stderr:raise AssertionError((args,p.returncode,p.stdout,p.stderr))
        return json.loads(p.stdout)
    cases=[[name,mode] for name in ['send','TxPoWTreeNode','Contract','P2PFunctions'] for mode in ['null','empty','args']]
    cases += [['P2PFunctions','null',stage,kind] for stage in ['open','connect','input','close'] for kind in ['io','error','oom']]
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            for args in cases:
                rows=[f.result() for f in [pool.submit(run,c,args) for c in commands]];report['observations'].append(dict(args=args,java=rows[0],cpp=rows[1]));check(rows[0]==rows[1],(args,rows));report['comparisons']+=1
                row=rows[0];out=row['stdout'];name=args[0]
                if name=='send':
                    coins=[json.loads(line) for line in out.splitlines()];check([(c['address'],c['amount']) for c in coins]==[('0xDD','10'),('0xDD','1'),('0xEE','2'),('0xEE','0.5'),('0xEE','0.2'),('0xFF','1')],coins);check(row['notifications']==0 and row['error']=='none',row)
                elif name=='TxPoWTreeNode':check(json.loads(out)=={'test':'this','transactions':'[one, two, three]'},row)
                elif name=='Contract':
                    check(out.startswith('\nScript:LET g = 0xFF << -8\nClean :'),row);check('Contract finished     : false' in out and 'Contract instructions :' in out,row);check(row['notifications']==len(out.splitlines()) and row['error']=='none',row)
                else:
                    stages=['open:https://www.google.com','connect','input','close'];failed=len(args)>2;index=['open','connect','input','close'].index(args[2]) if failed else 3
                    check(row['events']==stages[:index+1],row);check(row['notifications']==0,row)
                    if failed and args[3]!='io':check(out=='' and row['error']==('java.lang.AssertionError: fixture' if args[3]=='error' else 'java.lang.OutOfMemoryError: fixture'),row)
                    else:check(out==('NET:false\n' if failed else 'NET:true\n') and row['error']=='none',row)
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Four actual mains, null/empty/unused arguments; complete stdout except logger runtime prefix, full coin JSON/list-string/VM traces and logger notifications. Existing offline URL transport boundary covers success and IO/AssertionError/OutOfMemoryError at open/connect/input/close. Does not certify real HTTPS provider semantics or complete class parity.'
        (build/'core-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
