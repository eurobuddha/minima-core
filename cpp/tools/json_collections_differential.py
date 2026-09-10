#!/usr/bin/env python3
"""Compare ordered JSON storage, retained children and iterator transitions."""
import argparse,json,subprocess
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def op(name,**kw):return dict(op=name,**kw)
def cases():
    result=[]
    def add(kind,steps):result.append(dict(kind=kind,ops=steps))
    for kind in ['array','object']:
        for count in range(4):
            base=[op('add',value=str(i)) if kind=='array' else op('put',key=str(i),value=str(i)) for i in range(count)]
            mutations=([op('add',value='tail'),op('remove',value='0'),op('remove',value='missing')] if kind=='array' else [op('put',key='tail',value=None),op('put',key='0',value='changed')])
            for consumed in range(count+1):
                for mutation in mutations:
                    add(kind,base+[op('iter')]+[op('next')]*consumed+[mutation,op('has'),op('next'),op('has'),op('next'),op('serial')])
            add(kind,base+[op('iter'),op('replace'),op('has')]+[op('next')]*(count+1)+[op('serial')])
        child=op('childStore',key='child')
        mutate=op('add',value='outside-copy') if kind=='array' else op('put',key='extra',value='outside-copy')
        add(kind,[op('childPut',key='π🐼',value='before'),child,op('copy'),mutate,op('childPut',key='π🐼',value='after'),op('serial'),op('copySerial')])
    for index in [-1,0,1,2,2147483647]:add('array',[op('add',value=None),op('add',value='π'),op('get',index=index)])
    add('array',[op('add',value='π'),op('add',value='π'),op('contains',value='π'),op('remove',value='π'),op('serial'),op('contains',value='π'),op('remove',value='π'),op('contains',value='π')])
    add('object',[op('put',key='π🐼',value=None),op('put',key='first',value=1),op('put',key='π🐼',value=False),op('get',key='missing'),op('serial')])
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'json-collections-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src/org/minima/utils/json').rglob('*.java'))+[ROOT/'support/JSONCollectionsOracle.java']
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources)],check=True)
    corpus=cases();runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','JSONCollectionsOracle']),Runner([str(build/'json_collections')])];failures=[]
    try:
        for index,request in enumerate(corpus):
            outputs=[runner.ask(request) for runner in runners]
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]))
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=len(corpus),operations=sum(len(c['ops']) for c in corpus),failures=failures,scope='Pinned JSONArray/JSONObject: iterator hasNext/next before/between/after structural changes, map value replacement, empty/exhausted iterators, outer copy independence with shared children, original iterator ownership after variable replacement, array string removal and indexed reads. Single-thread deterministic schedules; native concurrent stress is a separate test. Full inherited Java collection API is not certified.')
    (build/'json-collections-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:3]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
