#!/usr/bin/env python3
"""Real registry initialization, exact pinned ordering/metadata and JNI dispatch."""
import argparse,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'command-registry-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/CommandRegistryOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    source=re.sub(r'//[^\n]*','',(JAVA/'src/org/minima/system/commands/CommandRunner.java').read_text());start=source.index('public static final Command[] ALL_COMMANDS');source=source[start:source.index('};',start)]
    names=re.findall(r'new (\w+)\(\)',source);assert len(names)==116 and len(set(names))==116
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CommandRegistryOracle'],[str(build/'minima_command_registry')]];outputs=[];checks=0
    for command in commands:
        r=subprocess.run(command,text=True,capture_output=True,timeout=30,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});assert r.returncode==0 and not r.stderr,(r.returncode,r.stdout,r.stderr)
        value=json.loads(r.stdout);assert [row['name'] for row in value['commands']]==names;checks+=1
        for key in ['sameRegistry','mutationVisible','restored']:assert value[key] is True;checks+=1
        for row in value['commands']:assert row['fresh'] is True and row['freshparams']=={};checks+=2
        assert '19005' in next(row['help'] for row in value['commands'] if row['name']=='rpc');checks+=1
        assert value['jniMissingLibrary']==["java.lang.UnsatisfiedLinkError: 'void org.minima.utils.jni.jnifunctions.sayHello()'"]*3;checks+=3
        outputs.append(value)
    differences=[]
    for index,(java,cpp) in enumerate(zip(outputs[0]['commands'],outputs[1]['commands'])):
        if java!=cpp:differences.append(dict(index=index,java=java,cpp=cpp))
    passed=outputs[0]==outputs[1]
    report=dict(passed=passed,registered_commands=116,independent_checks=checks,concurrent_first_call_threads=8,outputs=outputs,differences=differences,scope='Every pinned registered command in exact order with complete help/params/factory metadata; 8-way first access retains one registry, array element mutation and restoration remain visible, RPC constructor captures configured port, and all three JNI commands dispatch to the explicit missing-library error. No successful JNI library or complete command runtime parity claim.')
    (build/'command-registry-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='outputs'},indent=2));return not passed
if __name__=='__main__':raise SystemExit(main())
