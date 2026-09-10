#!/usr/bin/env python3
"""Actual command dispatch errors and logger failures; controlled command body."""
import argparse,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--baseline',action='store_true')
    args=parser.parse_args();build=args.build.resolve();oracle=build/'command-runner-errors-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/CommandRunnerErrorsOracle.java']
    listing=build/'command-runner-errors-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cases=[]
    for kind in ['success','command','runtime','io','sql','sqlruntime','format','arithmetic','error','oom','null','missingstatus','nullstatus','wrongstatus','false']:
        for emit in ['false','true']:
            for text in [f'fixture kind:{kind} emit:{emit}',f'fixture;fixture kind:{kind} emit:{emit};fixture',f'send kind:{kind} emit:{emit};fixture',f'fixture kind:{kind} bad:value;fixture']:
                for caller in ['0x00','0x1234']:
                    for fail_at,logger_kind in [(0,'runtime'),(1,'runtime'),(2,'runtime'),(1,'error'),(2,'error'),(1,'oom'),(2,'oom')]:
                        cases.append(dict(text=text,caller=caller,failAt=fail_at,loggerKind=logger_kind))
    payload=''.join(json.dumps(x)+'\n' for x in cases)
    def run(command):
        process=subprocess.run(command,input=payload,capture_output=True,text=True,check=True,timeout=90)
        output=[json.loads(line) for line in process.stdout.splitlines()]
        if len(output)!=len(cases):raise AssertionError(f'Expected{len(cases)}results, got{len(output)}')
        return output
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CommandRunnerErrorsOracle'])
    actual=run([str(build/'command_runner_errors')])
    def normalize(value):
        result=value['result']
        if isinstance(result,str) and result.startswith(('escaped:java.lang.NullPointerException','escaped:java.lang.ClassCastException')):
            value={**value,'result':result.split(': ',1)[0]}
        return value
    failures=[dict(case=case,java=j,cpp=c) for case,j,c in zip(cases,expected,actual) if normalize(j)!=normalize(c)]
    # Independent assertions protect the oracle setup and the important catch boundary.
    for kind,exception in [('null','NullPointerException'),('missingstatus','NullPointerException'),('nullstatus','NullPointerException'),('wrongstatus','ClassCastException')]:
        index=cases.index(dict(text=f'fixture;fixture kind:{kind} emit:false;fixture',caller='0x00',failAt=0,loggerKind='runtime'))
        for output in [expected[index],actual[index]]:
            assert output['result'].startswith('escaped:java.lang.'+exception),output
            assert output['events']==['fixture:success','fixture:'+kind],output
            assert output['logs']==[],output
    report=dict(passed=not failures,cases=len(cases),failures=failures,scope='Actual CommandRunner and MinimaLogger. Controlled command effects, exact JSON/errors/log bodies, permission and invalid-param gates, stop-after-failure, logger failure on first/second non-frame print. Only logger timestamp/memory prefixes, native/Java frame lines, and escaped NPE/CCE diagnostic prose excluded. Null replies and missing/null/wrong/false status exercise the status read outside the command catch block. Full registry/Main integration not covered.')
    name='command-runner-errors-baseline-results.json' if args.baseline else 'command-runner-errors-results.json'
    (build/name).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2))
    return bool(failures)
if __name__=='__main__':raise SystemExit(main())
