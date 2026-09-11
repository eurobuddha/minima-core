#!/usr/bin/env python3
"""Exact pinned tutorial variants and public white-paper constant/replies."""
import argparse,hashlib,json,re,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def literal(source,marker):
    start=source.index(marker)+len(marker);end=source.index(';\n',start);expression=source[start:end]
    pattern=r'"(?:[^"\\]|\\.)*"';parts=re.findall(pattern,expression)
    assert re.sub(pattern,'',expression).replace('+','').strip()==''
    return ''.join(json.loads(part) for part in parts)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();start=time.monotonic()
    oracle=build/'text-commands-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/TextCommandsOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    expected={}
    for kind in ['tutorial','base/tutorial','whitepaper']:
        source=(JAVA/'src/org/minima/system/commands'/f'{kind}.java').read_text();expected[kind]=literal(source,'public static final String WP =' if kind=='whitepaper' else 'String tutorial =')
    assert expected['tutorial']!=expected['base/tutorial'];assert len(expected['whitepaper'])==43554
    cases=[]
    def add(kind,op='run',params=None,**kw):cases.append(dict(kind=kind,op=op,params=params or {})|kw)
    for kind in expected:
        add(kind,'meta');add(kind,'meta',{'ignored':'value'})
        for params in [{},{'x':None},{'x':True},{'x':1},{'x':{}},{'x':[]},{'x':'é🐼\n\0'}, {'response':'replacement'}]:
            add(kind,params=params);add(kind,'alias',params)
    add('whitepaper','constant')
    for caller in ['0x00','0x1234']:
        for text in ['tutorial','whitepaper','TUTORIAL','WHITEPAPER','tutorial;whitepaper','whitepaper;tutorial','tutorial bad:true','whitepaper bad:true']:
            add('tutorial','runner',caller=caller,text=text)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TextCommandsOracle']),Runner([str(build/'minima_text_commands')])];failures=[];count=checks=0;transcript=hashlib.sha256()
    def verify(kind,reply):
        nonlocal checks
        assert reply['status'] is True and reply['pending'] is False
        value=reply['response']['whitepaper'] if kind=='whitepaper' else reply['response'];assert value==expected[kind];checks+=2
    try:
        for index,request in enumerate(cases):
            values=[r.ask(request) for r in runners];count+=1
            if values[0]!=values[1]:failures.append(dict(case=index,request=request,java=values[0],cpp=values[1]))
            for output in values:
                op=request['op'];kind=request['kind']
                if op in ['run','alias']:
                    reply=output['value'];verify(kind,reply)
                    assert ('params' in reply)==bool(request['params']);checks+=1
                    if op=='alias' and request['params']:assert reply['params']['later']=='changed';checks+=1
                elif op=='constant':assert output['value']==expected['whitepaper'];checks+=1
                elif op=='runner':
                    for reply in output['value']:
                        if reply['status']:verify(reply['command'],reply)
                        else:assert reply['error']=='Invalid parameter : bad';checks+=1
                else:assert output['params']==[] and 'params' not in output['fresh'];checks+=2
            transcript.update(json.dumps([request,*values],sort_keys=True,separators=(',',':')).encode())
        assert runners[1].errors()=='','Native fixture stderr: '+runners[1].errors()
    except Exception as error:failures.append(dict(phase='execution',cases=count,error=repr(error)))
    finally:
        for name,r in zip(['java','cpp'],runners):
            try:r.close()
            except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,cases=count,planned_cases=len(cases),independent_assertions=checks,seconds=round(time.monotonic()-start,3),text_payloads={k:dict(utf8_bytes=len(v.encode()),sha256=hashlib.sha256(v.encode()).hexdigest()) for k,v in expected.items()},transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual pinned tutorial variants and whitepaper: complete exact payloads, public WP constant, constructor metadata, base-only full-help override, fresh factories, direct ignored parameters, reply parameter aliases, actual registered root tutorial, chained commands, case-insensitive parsing and non-admin callers. Native empty stderr. Does not validate historical paper claims or execute tutorial examples; full project parity remains unverified.')
    (build/'text-commands-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
