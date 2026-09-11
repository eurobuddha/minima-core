#!/usr/bin/env python3
"""Focused audit of println(null), after fixing the Java fixture's null capture."""
import argparse,json,subprocess
from pathlib import Path
from horst_differential import ROOT,JAVA
from peers_checker_differential import Runner
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve();oracle=build/'sphincs-log-audit-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['HORSTOracle.java','FORSOracle.java','SPHINCSOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(c) for c in [['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SPHINCSOracle'],[str(build/'minima_sphincs')]]];records=[]
    try:
        for message in [None,'','student log']:
            q=dict(action='log',message=message);values=[r.ask(q) for r in runners]
            expected=dict(state=dict(public=None,private=None,total=None),samePublic=True,samePrivate=True,sameTotal=True,logs=['null' if message is None else message])
            assert values==[expected,expected],values;records.append(dict(request=q,java=values[0],cpp=values[1]))
        assert not runners[1].errors(),'Unexpected native stderr'
    finally:
        for runner in runners:runner.close()
    report=dict(cases=3,passed=3,independent_assertions=6,failures=[],records=records,scope='Actual SPHINCS.log null/empty/text with corrected null-safe Java println capture; no production changes. Supersedes the original full driver null-log fixture mismatch only.')
    (build/'sphincs-log-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('SPHINCS log audit: 3 cases, 6 independent assertions passed')
if __name__=='__main__':main()
