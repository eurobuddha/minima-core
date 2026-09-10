#!/usr/bin/env python3
import argparse,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'command-runner-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/CommandRunnerOracle.java'];listing=build/'command-runner-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CommandRunnerOracle'];cpp=[str(build/'minima_command_runner')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'command-runner-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    texts=['',' ',';;;','\t\n','unknown','hash','HASH data:hello','hash data:0x12 type:sha2','hash data:hello type:wrong','hash data:"hello world"','hash data:{"a":1,"b":["hi",null]}','hash data:[1,2,3]','fixture value:a value:b','fixture fail:true;hash data:hello','send data:foo','fixture bad:x; hash data:x','fixture text:"one:two"','fixture path:C:\\temp\\thing','fixture text:"[ one ]"','fixture data:{"x":"}"} value:a','txnstate state:[ HELLO WORLD ]','fixture text:"hello;world"; fixture data:later']
    for _ in range(500):
        text=rng.choice(['fixture','send','txnstate'])
        for i in range(rng.randrange(7)):text+=rng.choice([' ','  ','\t','\n'])+rng.choice(['data','value','text','path','amount','bad'])+rng.choice([':',': ', ' :',' : '])+rng.choice(['hello','0x12','true','false','"abc def"','"x:y"','"unterminated','[]','{}','{"x": [1,2]}','[ hello ]','{bad}','a:b:c','C:\\temp\\file','π 😄',''])
        texts.append(text)
    cases=[]
    for text in texts:
        for op in ['split','parse']:cases.append(json.dumps({'op':op,'text':text},ensure_ascii=True))
    for text in texts[:22]:
        for caller in ['0x00','0x1234']:cases.append(json.dumps({'op':'run','text':text,'caller':caller},ensure_ascii=True))
        cases.append(json.dumps({'op':'single','text':text},ensure_ascii=True))
    for text in ['hash','HASH','fixture','send','txnstate','missingcmd','unknown']:
        cases.append(json.dumps({'op':'only','text':text}))
    names=['send','sendpoll','sendsign','multisig','tokencreate','consolidate','cointrack','sign','txnauto','txnsign','mds','backup','removescript','restore','restoresync','vault','archive','mysql','mysqlcoins','rpc','magic','quit','seedrandom','megammrsync','megammr','decryptbackup','reset','hash','fixture']
    for name in names:
        for text in [name,name.upper(),' '+name+' ',name+' x']:cases.append(json.dumps({'op':'allowed','text':text}))
    # Keep automine's active no-op body separate from runner parameter validation.
    for params in [{},*({'enable':v} for v in ['true','false','single',' other ','',' ',None,1,True,{},[]]),{'unused':'value'}]:
        cases.append(json.dumps({'op':'direct','text':'automine','params':params}))
    cases.append(json.dumps({'op':'meta','text':'automine'}))
    for text in ['automine','AUTOMINE','automine enable:true','automine enable:false','automine enable:single','automine enable:" "','automine bad:value','automine; hash data:hello','automine enable:true; hash data:hello']:
        for caller in ['0x00','0x1234']:cases.append(json.dumps({'op':'run','text':text,'caller':caller}))
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'CommandRunner fast/JSON tokenization, malformed quotes/JSON, Windows paths, duplicate parameters, multi-command stopping, parameter validation, write permissions and concrete hash; fake commands isolate dispatch without wallet/network actions. Also concrete automine direct calls, metadata, fresh factory, inherited empty valid-parameter list and runner dispatch. Direct malformed-parameter errors normalize to ERROR; complete registry and runtime exception diagnostics excluded.'}
    (build/'command-runner-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
