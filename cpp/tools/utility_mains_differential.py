#!/usr/bin/env python3
"""Five complete source entry points plus JSONPretty debug behavior."""
import argparse,hashlib,itertools,json,os,re,subprocess,selectors
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'utility-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','UtilityMainsOracle.java']];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','UtilityMainsOracle'],[str(build/'utility_mains')]]
    report=dict(passed=False,main_comparisons=0,debug_comparisons=0,nonreturning_comparisons=0,assertions=0,observations=[])
    source=(JAVA/'src/org/minima/utils/MiniFormat.java').read_text();fragment=source[source.index('String tester = '):source.index('JSONArray obj = convertToJSON(tester);')]
    sample=''.join(json.loads(x) for x in re.findall(r'"(?:\\.|[^"\\])*"',fragment))
    native=(ROOT/'src/org/minima/utils/MiniFormat.cpp').read_text().split('R"MINIMA_DEMO(',1)[1].split(')MINIMA_DEMO"',1)[0]
    assert native==sample,'MiniFormat literal differs from pinned source'
    report['literal']=dict(bytes=len(sample.encode()),sha256=hashlib.sha256(sample.encode()).hexdigest())
    def check(value,detail):report['assertions']+=1;assert value,detail
    def run(command,args):
        p=subprocess.run(command+args,capture_output=True,text=True,timeout=60,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if p.returncode or p.stderr:raise AssertionError((args,p.returncode,p.stdout[:500],p.stderr))
        return p.stdout
    def nonreturning(command,args):
        p=subprocess.Popen(command+args,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        try:
            with selectors.DefaultSelector() as selector:
                selector.register(p.stdout,selectors.EVENT_READ)
                if not selector.select(10):raise AssertionError('No formatter entry marker')
            if p.stdout.readline()!='ENTER\n':raise AssertionError('Formatter entry marker differs')
            try:
                out,err=p.communicate(timeout=.3)
                raise AssertionError(('Leading unmatched quote returned',args,out,err))
            except subprocess.TimeoutExpired:pass
            p.kill();out,err=p.communicate(timeout=5)
            if out or err:raise AssertionError(('Unexpected output during nonreturning case',out,err))
            return dict(entered=True,still_running_after_seconds=.3,output_after_entry='',fixture_terminated=True)
        finally:
            if p.poll() is None:p.kill();p.wait()
            p.stdout.close();p.stderr.close()
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            for name in ['Maths','BaseConverter','MiniFormat','POW','REPLACE']:
                futures=[pool.submit(run,c,[name]) for c in commands];outputs=[f.result() for f in futures]
                for index,text in enumerate(outputs):(build/f'utility-main-{name}-{index}.txt').write_text(text)
                normalized=[]
                for value in outputs:
                    if name=='POW':
                        lines=value.splitlines();check(len(lines)==1001 and re.fullmatch(r'Time : -?\d+',lines[-1]) is not None,('POW output/loop length',len(lines)));normalized.append('\n'.join(lines[:-1])+'\nTime : <elapsed>\n')
                    else:normalized.append(value)
                check(normalized[0]==normalized[1],('main differs',name,normalized[0][:500],normalized[1][:500]));report['main_comparisons']+=1
                expected={'Maths':'1\n','REPLACE':'3333\n','BaseConverter':'HEX : 8 0x000001\nB32 : 3 Mx1\nEXCEPTION:java.lang.NumberFormatException: Invalid HEX string in decode16 : Mx1\n'}
                for value in normalized:
                    if name in expected:check(value==expected[name],(name,value))
                    elif name=='POW':check(all(line=='0' for line in value.splitlines()[:-1]),('power underflows source decimal scale',value[:100]))
                    else:
                        line=value.splitlines()[0];check(line.startswith('JSONPRETTY len:') and json.loads(line.split(' obj:',1)[1])==json.loads(sample)[0],('complete source sample debug rendering',line[:100]))
                report['observations'].append(dict(main=name,bytes=[len(x.encode()) for x in outputs],sha256=[hashlib.sha256(x.encode()).hexdigest() for x in outputs],normalized_sha256=hashlib.sha256(normalized[0].encode()).hexdigest()))
            samples=['{}','  {} \n','{"🐼":"é"}','{"x":"unterminated','"','{"x":"a\\n/b"}']
            for raw,debug,fail in itertools.product(samples,[False,True],[0,1,2]):
                if raw=='\"' and (not debug or fail!=1):
                    args=['hang',raw,str(debug).lower(),str(fail)];futures=[pool.submit(nonreturning,c,args) for c in commands];rows=[f.result() for f in futures];check(rows[0]==rows[1],(args,rows));report['nonreturning_comparisons']+=1;report['observations'].append(dict(nonreturning=args[1:],java=rows[0],cpp=rows[1]));continue
                args=['debug',raw,str(debug).lower(),str(fail)];futures=[pool.submit(run,c,args) for c in commands];rows=[json.loads(f.result()) for f in futures];check(rows[0]==rows[1],(args,rows));report['debug_comparisons']+=1
                for row in rows:
                    check(row['renders']==(2 if debug else 1),(args,row));check(row['notifications']==int(debug and fail!=1),(args,row))
                    if not debug:check(row['logs']==[] and row['error']=='none',(args,row))
                    else:
                        check(row['logs'][0]=='JSONPRETTY len:'+str(len(raw.strip(' \t\r\n').encode('utf-16-le'))//2)+' obj:'+raw,(args,row))
                        if fail==1:check(row['error']=='java.lang.IllegalStateException: logger' and row['output'] is None,(args,row))
                report['observations'].append(dict(debug=args[1:],java=rows[0],cpp=rows[1]))
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Five actual main methods with unused null arguments, exact source sample and complete 1000-iteration POW loop. Raw stdout retained; only logger runtime prefix and final POW elapsed value normalized for comparison. 36 JSONPretty matrix cases: 31 finite comparisons of exact output/error/logs, notifications, render counts, trim, UTF-16 length, fallback and logger failures, plus five bounded observations of the original leading-unmatched-quote loop in fixture-owned child processes. Bounded liveness observation alone is not a proof of nontermination; source control flow supplies the loop evidence. Complete APIs/IO failures, localization and arbitrary subclass behavior remain unverified.'
        (build/'utility-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
