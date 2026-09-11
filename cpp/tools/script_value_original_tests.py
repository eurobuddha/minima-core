#!/usr/bin/env python3
"""Run the unchanged pinned JUnit assertions and their one-to-one native test."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build')
    parser.add_argument('--test-class',choices=['CONCATTests','BOOLTests','HEXTests','NUMBERTests','SCRIPTTests','REVTests','LENTests','MiniStringTests','ScriptValueTests','HexValueTests','NumberValueTests','ABSTests','INCTests','DECTests','POWTests','SIGDIGTests','BITCOUNTTests','BITGETTests','BITSETTests','FLOORTests','CEILTests','MINTests','MAXTests','MinimaFunctionTests'],default='ScriptValueTests')
    args=parser.parse_args();build=args.build.resolve();test=args.test_class
    prefix,driver,methods,assertions={'CONCATTests':('concat','CONCATOriginalTests',3,21),'BOOLTests':('bool-cast', 'BOOLCastOriginalTests', 3, 21),'HEXTests':('hex-cast', 'HEXCastOriginalTests', 3, 24),'NUMBERTests':('number-cast', 'NUMBERCastOriginalTests', 3, 19),'SCRIPTTests':('script-cast', 'SCRIPTCastOriginalTests', 3, 23),'REVTests':('rev','REVOriginalTests',3,12),'LENTests':('len','LENOriginalTests',3,12),'MiniStringTests':('ministring','MiniStringOriginalTests',2,5),'ScriptValueTests':('script-value','ScriptValueOriginalTests',1,10),'HexValueTests':('hex-value','HexValueOriginalTests',2,35),'NumberValueTests':('number-value','NumberValueOriginalTests',4,99),'ABSTests':('abs','ABSOriginalTests',3,23),'INCTests':('inc','INCOriginalTests',3,30),'DECTests':('dec','DECOriginalTests',3,30),'BITGETTests':('bitget','BITGETOriginalTests',3,272),'BITSETTests':('bitset','BITSETOriginalTests',3,271),'BITCOUNTTests':('bitcount','BITCOUNTOriginalTests',3,34),'POWTests':('pow','POWOriginalTests',3,25),'SIGDIGTests':('sigdig','SIGDIGOriginalTests',3,89),'FLOORTests':('floor','FLOOROriginalTests',3,30),'CEILTests':('ceil','CEILOriginalTests',3,26),'MINTests':('min','MINOriginalTests',3,120023),'MAXTests':('max','MAXOriginalTests',3,120023),'MinimaFunctionTests':('minima-function','MinimaFunctionOriginalTests',2,11)}[test]
    package={'CONCATTests':'functions/base','BOOLTests':'functions/cast','HEXTests':'functions/cast','NUMBERTests':'functions/cast','SCRIPTTests':'functions/cast','REVTests':'functions/base','LENTests':'functions/base','BITGETTests':'functions/maths','BITSETTests':'functions/maths','BITCOUNTTests':'functions/maths','POWTests':'functions/maths','SIGDIGTests':'functions/maths','INCTests':'functions/maths','DECTests':'functions/maths','ABSTests':'functions/maths','FLOORTests':'functions/maths','CEILTests':'functions/maths','MINTests':'functions/maths','MAXTests':'functions/maths','MinimaFunctionTests':'functions'}.get(test,'values')
    oracle=build/(prefix+'-original-oracle');oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[
        (JAVA/'test/org/minima/objects/base'/ (test+'.java') if test=='MiniStringTests' else JAVA/'test/org/minima/tests/kissvm'/package/(test+'.java')),
        ROOT/'support'/(driver+'.java')]
    if test=='MinimaFunctionTests':sources.append(JAVA/'test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java')
    listing=build/(prefix+'-original-sources.txt')
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    dependencies=f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac','-cp',dependencies,'-d',str(oracle),'@'+str(listing)],check=True)
    timeout=180 if test in ['MINTests','MAXTests'] else 30
    java=subprocess.run(['java','-cp',f'{oracle}:{dependencies}',driver],capture_output=True,text=True,check=True,timeout=timeout)
    native=subprocess.run([str(build/test)],capture_output=True,text=True,check=True,timeout=timeout)
    if test=='NumberValueTests' and java.stdout!=native.stdout:
        raise AssertionError(f'Numeric error observations differ: Java={java.stdout!r}, C++={native.stdout!r}')
    report={'passed':True,'original_methods':methods,'assertions_per_runtime':assertions,
            'java_stdout':java.stdout,'native_stdout':native.stdout,
            'scope':'Unchanged pinned '+test+' active assertions and the mapped native test. Executed assertion count includes source loops. No claim of complete production API parity.'}
    if test=='CONCATTests':
        assert native.stderr=='', 'Native CONCATTests diagnostics: '+native.stderr
        report.update(native_stderr=native.stderr)
    if test=='MiniStringTests':
        helper_java=subprocess.run(['java','-cp',f'{oracle}:{dependencies}',driver,'--unannotated-helper'],capture_output=True,text=True,check=True,timeout=30)
        helper_native=subprocess.run([str(build/test),'--unannotated-helper'],capture_output=True,text=True,check=True,timeout=30)
        report.update(supplemental_unannotated_methods=1,supplemental_assertions_per_runtime=1,helper_java_stdout=helper_java.stdout,helper_native_stdout=helper_native.stdout)
        report['scope']='Two unchanged pinned @Test-annotated methods (direct calls after checking annotation membership) and their native counterpart: five successful-path non-null assertions. Separately execute the one unannotated helper and its one successful-path assertion. Static read result is discarded in the source. No decoded-content, error-branch or full production parity claim.'
    if test=='BITCOUNTTests':
        table_java=subprocess.run(['java','-cp',f'{oracle}:{dependencies}',driver,'--mutated-table'],capture_output=True,text=True,check=True,timeout=30)
        table_native=subprocess.run([str(build/test),'--mutated-table'],capture_output=True,text=True,check=True,timeout=30)
        assert table_java.stdout==table_native.stdout=='84\n42\n-2147483648\n'
        report.update(conditional_table_checks_per_runtime=256,supplemental_table_mutation_observations=3,table_observations=table_java.stdout)
    if test=='NumberValueTests':report['supplemental_error_comparisons']=11
    if test in ['MINTests','MAXTests']:
        report['random_trials_per_runtime']=60000
        report['random_input_scope']='Java runs unchanged unseeded Random tests. Native uses reproducible signed samples assembled from existing JavaRandom48 bounded draws; same properties and loop counts, not identical random inputs or a PRNG parity test.'
    (build/(prefix+'-original-results.json')).write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__':main()
