#!/usr/bin/env python3
"""Pinned help catalogue, actual registry metadata, and UTF-16 padding semantics."""
import argparse,hashlib,json,os,random,re,subprocess,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic()
    oracle=build/'help-command-oracle';oracle.mkdir(exist_ok=True);sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/HelpCommandOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    source=(JAVA/'src/org/minima/system/commands/help.java').read_text();active=re.sub(r'//[^\n]*','',source);curated=re.findall(r'addCommand\(details, new (\w+)\(\)\);',active);assert len(curated)==94
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HelpCommandOracle']),Runner([str(build/'minima_help_command')])];failures=[];count=checks=0;transcript=hashlib.sha256();missing=[];cases=[];reference=None
    def check(ok,label):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(label)
    def add(op='run',params=None,**kw):cases.append(dict(op=op,params={} if params is None else params,**kw))
    def normalize(value):
        if isinstance(value,list):return [normalize(x) for x in value]
        if isinstance(value,dict):
            value={k:normalize(v) for k,v in value.items()}
            if 'error' in value:
                for kind in ['java.lang.NullPointerException','java.lang.ClassCastException']:
                    if value['error'].startswith(kind):value['error']=kind
            return value
        return value
    try:
        names=[r.ask(dict(op='registry',params={}))['names'] for r in runners];missing=sorted(set(names[0])-set(names[1]));check(names[0]==names[1] and len(names[1])==116,'all 116 pinned command names present');check(not set(names[1])-set(names[0]),'no invented native commands');check(set(curated)<=set(names[1]),'every curated constructor available');transcript.update(json.dumps(names).encode())
        for params in [{},{'ignored':'value'},{'command':'hash'}]:add('meta',params)
        for op in ['run','alias']:
            for params in [{},{'ignored':None},{'ignored':[]},{'command':'help'},{'command':'hash','ignored':True}]:add(op,params)
        for name in names[1]:add(params={'command':name});add(params={'command':' \t'+name+'\n '})
        for value in ['', ' ', '\t\n\0', 'HASH','Help','not-a-command','nodecount','missingcmd','hash data:x','é😄','\u00a0',None,True,False,0,1,{},[]]:add(params={'command':value})
        for short,full in [('probe short','probe full'),(None,'full without short'),('short without full',None),(None,None),('','')]:
            probe=dict(name='fixture',help=short,fullhelp=full);add(params={'command':'fixture'},probe=probe);add(probe=probe)
        rng=random.Random(5564);unit_cases=[[],[0],[65],[0xd83d,0xde04],[0xd800],[0xdc00],[65,0xd83d,0xde04,0,0xffff]]
        unit_cases += [[rng.randrange(65536) for _ in range(rng.randrange(0,35))] for _ in range(50)]
        for units in [None,*unit_cases]:
            for length in [-2147483648,-1,0,1,2,15,32,128]:add('pad',units=units,length=length)
        for caller in ['0x00','0x1234']:
            for text in ['help','HELP','help command:hash','HELP command:hash','help command:HASH','help command:" hash "','help command:" "','help command:null','help command:{}','help command:[]','help command:hash bad:true','help bad:true;help','help command:missing;help','help command:hash;help command:help','help command:hash command:help']:
                add('runner',caller=caller,text=text)
        for index,request in enumerate(cases):
            actual=[r.ask(request) for r in runners];count+=1;values=[normalize(v) for v in actual]
            if values[0]!=values[1]:failures.append(dict(case=index,request=request,java=actual[0],cpp=actual[1]))
            for output in values:
                op=request['op']
                if op=='pad':
                    units=request['units'];length=request['length']
                    if units is None:check(output['exception']=='java.lang.NullPointerException','null checked before substring range')
                    elif length<0:check(output['exception']=='java.lang.StringIndexOutOfBoundsException','negative desired length throws')
                    else:check(output['units']==units[:length]+[32]*max(0,length-len(units)),'independent UTF-16 truncation/space padding')
                elif op=='meta':check(output['params']==['command'] and output['distinct'] and 'params' not in output['fresh'],'fresh factory and valid params');check(output['help']==output['fullhelp'],'inherited full help')
                elif 'value' in output and op!='runner':
                    reply=output['value'];check(reply['status'] and not reply['pending'],'reply envelope');params=request['params']
                    check(('params' in reply)==bool(params),'empty parameter omission / nonempty alias')
                    if op=='alias' and params:check(reply['params']['later']=='changed','reply retains parameter object alias')
                    response=reply['response']
                    if 'command' not in params:
                        check(list(response)==[n[:15].ljust(15) for n in curated],'exact independently extracted curated keys and insertion order')
                        if reference is None:reference=response
                        check(response==reference,'curated output unaffected by registry replacement')
                        check('nodecount'.ljust(15) not in response,'commented command excluded')
                    elif 'probe' in request:check(response==dict(command='fixture',help=request['probe']['help'],fullhelp=request['probe']['fullhelp']),'nullable dynamic metadata, target body not executed')
                    else:check(response['command']==params['command'].strip(),'trimmed detailed lookup name')
                elif op=='runner':
                    check(len(output['value'])<=2,'chain reply count')
                    if 'bad:' in request['text'] or 'command:missing;' in request['text']:check(len(output['value'])==1 and not output['value'][0]['status'],'invalid command stops chain')
            transcript.update(json.dumps([request,*values],sort_keys=True,separators=(',',':')).encode())
    except Exception as error:failures.append(dict(phase='execution',cases=count,error=repr(error)))
    finally:
        for which,r in enumerate(runners):
            with os.fdopen(os.dup(r.err.fileno()),'r') as errors:
                try:r.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=which,error=repr(error)))
                errors.seek(0);stderr=errors.read()
                if which==1 and stderr:failures.append(dict(phase='native stderr',stderr=stderr))
    report=dict(passed=not failures,cases=count,planned_cases=len(cases),independent_assertions=checks,curated_commands=len(curated),missing_native_registry_commands=missing,transcript_sha256=transcript.hexdigest(),seconds=round(time.monotonic()-started,3),failures=failures,scope='Actual pinned help and native counterpart: full 94-entry curated catalogue, detailed help for all 116 registry commands, exact metadata/nulls, fresh factory/parameter aliases, trimmed/blank/wrong-type input, case-sensitive lookup, actual chained runner/non-admin calls, controlled metadata-only target, UTF-16 code-unit padding/truncation/null/negative ranges. All previous cases retained plus detailed help for three JNI commands. NPE/classcast diagnostic prose normalized; native complete stderr empty. Successful JNI calls and full node/API/platform parity are not certified.')
    (build/'help-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
