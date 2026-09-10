#!/usr/bin/env python3
"""Pinned logger order/values and date formatting; no production node started."""
import argparse,itertools,json,os,random,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA

def cases():
    result=[]
    for mode,text,notify,main in itertools.product(['string','defaultString','exception','defaultException','uncaught'],[None,'','plain','π😄','a\x00b','x\r\ny','\ud800','\udc00'],[False,True],[False,True]):
        result.append(dict(mode=mode,text=text,notify=notify,main=main,frames=['first','second']))
    for mode,flag,notify,main in itertools.product(['string','defaultString','exception','defaultException','uncaught'],['failString','failStack','nullFrames','nullError','failDate','failPrint','failPost','clearPrint','setPrint','clearPost','nested','mutatePost'],[False,True],[False,True]):
        result.append(dict(mode=mode,text='message',notify=notify,main=main,frames=['first','second'],**{flag:True}))
    for mode,frames in itertools.product(['exception','uncaught'],[[],[None],['first',None,'last'],['π😄','\ud800','\x00','line\nnext']]):
        result.append(dict(mode=mode,text='exception',notify=True,main=True,frames=frames))
    dates=[-2**63,-2**63+1000,-2**53-1,-62135769600000,-12219292800001,-12219292800000,-2208988800001,-2208988800000,-1,0,1,1000,1700000000000,2**53+1,2**63-1000,2**63-1]
    rng=random.Random(5564);dates.extend(rng.randrange(-2**63,2**63) for _ in range(80))
    result.extend(dict(date=str(date)) for date in dates);return result
def normalize(value):
    if isinstance(value,str):
        if value.startswith('java.lang.NullPointerException'):return 'java.lang.NullPointerException'
        if value.startswith('Minima @ '):return re.sub(r' \[[^\]]*\] : ', ' [<heap>] : ',value,count=1)
    if isinstance(value,list):return [normalize(v) for v in value]
    if isinstance(value,dict):return {k:normalize(v) for k,v in value.items()}
    return value
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'minima-logger-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/MinimaLoggerOracle.java'];listing=build/'minima-logger-java-sources.txt';listing.write_text('\n'.join('"'+str(path)+'"' for path in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();failures=[];count=0
    for zone in ['UTC','Europe/London','America/New_York','Asia/Tokyo']:
        commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MinimaLoggerOracle'],[str(build/'minima_logger')]];outputs=[]
        for command in commands:
            process=subprocess.run(command,input='\n'.join(json.dumps(case,ensure_ascii=True) for case in corpus)+'\n',capture_output=True,text=True,timeout=90,env={**os.environ,'TZ':zone,'UBSAN_OPTIONS':'halt_on_error=1'})
            if process.returncode:raise AssertionError(f'{command}: {process.returncode}\n{process.stderr[-6000:]}')
            rows=[normalize(json.loads(line)) for line in process.stdout.splitlines()]
            assert len(rows)==len(corpus),(len(rows),len(corpus),process.stderr[-2000:]);outputs.append(rows)
        for index,(java,cpp) in enumerate(zip(*outputs)):
            count+=1
            if java!=cpp:failures.append(dict(zone=zone,case=index,request=corpus[index],java=java,cpp=cpp));break
        if failures:break
    report=dict(passed=not failures,cases=count,failures=failures,scope='Unchanged pinned MinimaLogger, controlled DateFormat/PrintStream/Main and explicit Throwable frames. Runtime heap-dependent prefix and automatic NPE details normalized; formatSize has separate exact tests. Fixed source date pattern across four default timezones and signed-long timestamps. Full mutable SimpleDateFormat and native exception capture remain incomplete.')
    (build/'minima-logger-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
