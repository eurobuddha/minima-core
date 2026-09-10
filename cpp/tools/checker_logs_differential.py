#!/usr/bin/env python3
"""Pinned checker rejection/log failure behavior using proven consensus fixtures."""
import argparse,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
import re

def normalize(row):
    def clean(v):
        if isinstance(v,str):return re.sub(r'java\.lang\.NullPointerException(?::.*)?','java.lang.NullPointerException:<diagnostic>',v)
        if isinstance(v,list):return [clean(x) for x in v]
        if isinstance(v,dict):return {k:clean(x) for k,x in v.items()}
        return v
    return clean(row)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'checker-logs-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','ConsensusOracle.java','CheckerLogsOracle.java']];listing=build/'checker-logs-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CheckerLogsOracle'];p=subprocess.run(java+['fixture'],capture_output=True,text=True,timeout=60);assert p.returncode==0,(p.stdout,p.stderr);fixtures=json.loads(p.stdout);(build/'checker-logs-fixtures.json').write_text(p.stdout)
    requests=[]
    for f in fixtures:
        for method in (['basic'] if f['mode']>=30 else ['basic','simple','scripts','mmr','signatures']):requests.append(dict(wire=f['wire'],method=method,kind='none',failAt=0,log=True))
        if f['mode']<30:requests.append(dict(wire=f['wire'],method='simple',kind='none',failAt=0,log=True,nullMMR=True))
        if f['mode'] in [0,3,9,22,23]:
            requests += [dict(wire=f['wire'],method='mmr',kind='none',failAt=0,log=log,badMMR=True) for log in [False,True]]
            requests += [dict(wire=f['wire'],method='mmr',kind='none',failAt=0,log=True,nullMMR=True)]
            requests += [dict(wire=f['wire'],method='simple',kind='none',failAt=0,log=False,maxSize='0')]
            requests += [dict(wire=f['wire'],method='scripts',kind='none',failAt=0,trace=True)]
    failures=[];cases=0;runners=[Runner(java),Runner([str(build/'checker_logs')])]
    def compare(request):
        nonlocal cases
        outputs=[normalize(r.ask(request)) for r in runners];cases+=1
        if outputs[0]!=outputs[1]:failures.append(dict(request=request,java=outputs[0],cpp=outputs[1]))
        return outputs[0]
    try:
        for request in requests:
            baseline=compare(request)
            if baseline['logs']:
                for kind in ['runtime','error','oom']:compare({**request,'kind':kind,'failAt':1})
                if len(baseline['logs'])>1:compare({**request,'kind':'error','failAt':2})
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,cases=cases,fixtures=len(fixtures),failures=failures,scope='Actual pinned basic/simple/script/MMR/signature checks and native methods using source consensus fixtures, duplicate-ID set capacity transitions and invalid input amount. Exact rejection logs, optional MMR logging, token script labels, selected full VM traces, failed logger propagation, cache flags and transaction IDs. Java-generated NPE diagnostic expressions excluded; categories and all other text compared. Complete object aliases, null list entries and full node integration remain unverified.')
    (build/'checker-logs-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
