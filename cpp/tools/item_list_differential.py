#!/usr/bin/env python3
"""Public ItemList API and retained List/Object behavior against pinned Java."""
import argparse,itertools,json,random,subprocess
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

def op(name,**kw):return dict(op=name,**kw)
def corpus():
    result=[]
    def add(*steps,**kw):result.append(dict(ops=list(steps),**kw))
    samples=[None,'','a',' a , b, ',':a::b:',',,,',' a\t\r\n\x00','\u00a0a\u00a0','aπbπ','a😄b😄','\ud800a\udc00','a\ud83d\ude04b\ud83d','one::two::','a||b|']
    separators=[None,',',':','::','|','π','😄','\ud83d','\ude04','a b','\x00']
    for arity in range(4):
        combos=[(None,None,False)] if arity==0 else [(s,None,False) for s in samples] if arity==1 else [(s,sep,multi) for s,sep,multi in itertools.product(samples,separators,[False,True] if arity==3 else [False])]
        for s,sep,multi in combos:
            steps=[op('new',arity=arity,s=s,sep=sep,multi=multi),op('join'),op('join',sep=None),op('array'),op('alias'),op('add',s=' after '),op('join'),op('get',index=0),op('clear'),op('add',s='one'),op('add',s='two'),op('join'),op('reset'),op('add',s='one'),op('add',s='two'),op('join')]
            add(*steps,constructor=True)
    for s,sep,multi in itertools.product(samples,separators+[ ''],[False,True]):
        if sep=='' and not multi and s is not None:
            add(op('split',s=s,sep=sep,multi=False,limited=True),nonadvance=True)
            add(op('split',s=s,sep=sep,null=True),nullAppend=True)
        else:
            for null in [False,True]:add(op('split',s=s,sep=sep,multi=multi,null=null),op('other',s='x:y',sep=':',multi=False),op('all',object=True),op('join'))
    for arity,sep in itertools.product([1,2,3],[None,'|','::','😄']):
        for s in samples:
            steps=[op('add',s='head')]
            step=op('all',s=s)
            if arity>=2:step['sep']=sep
            if arity==3:step['multi']=True
            add(*steps,step,op('join'))
    for index,s in itertools.product([-2147483648,-1,0,1,2,3,2147483647],[None,'',' x ','😄','\ud800']):
        add(op('add',s='first'),op('add',index=index,s=s),op('get',index=index),op('join'))
    for sep in [None,'','|','😄','\ud800']:
        add(op('setSP',sep=sep),op('join'),op('add',s='a'),op('join'),op('add',s='b'),op('join'),op('all',object=True,self=True),op('join'),op('all',object=True,null=True),op('reset'),op('join'))
    for value in [None,True,False,0,42,-17,1.25,{},[],{'a':'x'},[1,None],' x ','\ud800']:
        add(op('raw',value=value),op('get',index=0),op('join'),op('array'),op('all',object=True,self=True),op('join'),op('rawSet',index=0,value='changed'),op('join'),op('rawRemove',index=0),op('join'))
    add(op('alias'),op('new',arity=1,s='new,owner'),op('raw',value='old alias'),op('join'),op('alias'),op('rawClear'),op('join'))
    add(op('raw',probe=True),op('join'),op('probe',s='after'),op('join'),op('probe',s='error',fail=True),op('join'),op('probe',s=None),op('join'),rawProbe=True)
    add(op('raw',probe=True),op('probe',s='mutating',append=True),op('join'),op('join'),mutatingProbe=True)
    rng=random.Random(5564)
    for _ in range(120):
        s=''.join(rng.choice(' ab,:|π😄\ud800\udc00\t\u00a0') for _ in range(rng.randrange(30)))
        sep=''.join(rng.choice(' ,:|π😄\ud800\udc00') for _ in range(rng.randrange(1,5)))
        add(op('new',arity=3,s=s,sep=sep,multi=rng.choice([False,True])),op('alias'),op('split',s=s,sep=sep,alias=True,multi=rng.choice([False,True])),op('join'),op('array'))
    return result

def project(rows):
    for row in rows:
        if 'exception' in row and row['exception'].startswith('java.lang.NullPointerException:'):row['exception']='java.lang.NullPointerException'
    return rows

def hx(value):return value.encode('utf-16-be',errors='surrogatepass').hex().upper()
def validate(request,rows):
    if request.get('nonadvance'):
        assert rows[0]['exception']=='java.lang.IllegalStateException: fixture append limit',rows
        assert rows[0]['append']==['']*4,rows
    if request.get('nullAppend'):assert rows[0]['exception']=='java.lang.NullPointerException',rows
    if request.get('constructor'):
        assert rows[3]['exception']=='java.lang.ClassCastException',rows
        assert rows[-1]['value']==hx('one,two'),rows
    if request.get('rawProbe'):
        assert rows[1]['value']==hx('before') and rows[3]['value']==hx('after'),rows
        assert rows[5]['exception']=='java.lang.IllegalStateException: fixture render failure',rows
        assert rows[7]['value']==hx('null') and rows[7]['probeCalls']==4,rows
    if request.get('mutatingProbe'):
        assert rows[2]['value']==hx('mutating,added during render') and rows[3]['value']==rows[2]['value'],rows

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'item-list-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src/org/minima/utils/json').rglob('*.java'))+[ROOT/'support/ItemListOracle.java']
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources)],check=True)
    cases=corpus();failures=[];programs=operations=0
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ItemListOracle']),Runner([str(build/'item_list_runtime')])]
    try:
        for index,request in enumerate(cases):
            outputs=[]
            for runtime,runner in zip(['java','cpp'],runners):
                try:rows=project(runner.ask(request));validate(request,rows);outputs.append(rows)
                except Exception as error:failures.append(dict(case=index,runtime=runtime,request=request,error=repr(error),stderr=runner.errors()));break
            if failures:break
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
            programs+=1;operations+=len(request['ops'])
    finally:
        for runtime,runner in zip(['java','cpp'],runners):
            try:runner.close()
            except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
    report=dict(passed=not failures,programs=programs,total_programs=len(cases),operations=operations,failures=failures,scope='All public ItemList constructors/method overloads; UTF-16 exact separator versus code-point tokenizer; trimming, nulls, bounds, raw values, mutable List aliases, shared mutating/throwing objects, self-add and getArray cast failure. Empty exact separator observed with test append-list throwing after four additions; production loop unchanged. String code units and state exact; enhanced NPE/CCE provider prose normalized. Full inherited Java List API/concurrency/arbitrary Object and resource-limit parity not certified.')
    (build/'item-list-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
