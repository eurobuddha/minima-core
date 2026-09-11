#!/usr/bin/env python3
"""Preserve inactive source accurately and validate the active delay helper boundary."""
import argparse,json,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def run(command):
    result=subprocess.run(command,text=True,capture_output=True,timeout=30)
    if result.returncode or result.stderr:raise RuntimeError(dict(command=command,**vars(result)))
    return result.stdout

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'original-processor-oracle';oracle.mkdir(exist_ok=True)
    source=JAVA/'test/org/minima/system/brains/TxPoWProcessorTest.java'
    # Confirm that the actual original tests are empty, not omitted by our runner.
    active=re.sub(r'//[^\n]*','',source.read_text())
    bodies=re.findall(r'public void (testTxPoWProcessor2?)\(\)\s*\{([^}]*)\}',active)
    if len(bodies)!=2 or any(body.strip() for _,body in bodies):raise AssertionError('Original tests gained active behavior')
    inactive='src/org/minima/system/network/p2p/messages/P2PSwapLink'
    j=(JAVA/(inactive+'.java')).read_text();n=(ROOT/(inactive+'.cpp')).read_text()
    if any(line.strip() and not line.lstrip().startswith('//') for line in j.splitlines()+n.splitlines()):raise AssertionError('Inactive source gained active code')
    if not n.endswith(j):raise AssertionError('Historical comments changed')
    dependencies=f'{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac','-cp',dependencies,'-d',str(oracle),str(source),str(JAVA/(inactive+'.java')),str(ROOT/'support/OriginalProcessorTest.java')],check=True)
    if list(oracle.rglob('P2PSwapLink.class')):raise AssertionError('Inactive Java unexpectedly generated a class')
    java=run(['java','-cp',f'{oracle}:{dependencies}','OriginalProcessorTest'])
    native=run([str(build/'TxPoWProcessorTest')])
    expected=['original_methods=2;original_assertions=0','default=100ms','delay(0)=returned','delay(1)=returned']
    expected += [f'delay({v})=java.lang.IllegalArgumentException: timeout value is negative' for v in [-1,-100,-9223372036854775808]]
    expected += [f'interrupted({v})=returned;status=false' for v in [0,1,9223372036854775807]]
    expected += ['interrupted(-1)=java.lang.IllegalArgumentException: timeout value is negative;status=true']
    passed=java==native and java.splitlines()==expected
    report=dict(passed=passed,original_methods=2,original_assertions=0,delay_comparisons=10,known_outcome_checks=20,
                native_unrelated_exception_check=True,inactive_source_checks=4,java=java,cpp=native,
                scope='Original empty tests provide no processor coverage. Default/zero/positive/negative delays use real sleep. Native interrupted observations use an injected runtime callback; no native Thread.interrupt implementation is claimed. P2PSwapLink preserves comments only and is not functional conversion credit.')
    (build/'original-processor-results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2));return not passed
if __name__=='__main__':raise SystemExit(main())
