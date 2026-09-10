#!/usr/bin/env python3
"""Pinned processor with real mined block fixtures and controlled receivers."""
import argparse,itertools,json,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,**kw)
def cases():
    out=[]
    def add(ops,**kw):out.append(dict(ops=ops,**kw))
    add([op('root'),op('add',tx=8)]+sum(([op('post',tx=i),op('next')] for i in range(1,6)),[]))
    add([op('root'),op('post',tx=5),op('next'),op('post',tx=4),op('next'),op('post',tx=3),op('next'),op('post',tx=2),op('next'),op('post',tx=1),op('next'),op('add',tx=8),op('post',tx=3),op('next')])
    for kind,slave,test in itertools.product(['post','postBlock'],[False,True],[False,True]):
        add([op('root',blockNode=slave,test=test),op('add',tx=8)]+sum(([op(kind,tx=i),op('next')] for i in [3,2,1,5,4,6,7]),[])+[op('nonce',tx=5,value=123),op('recalc')])
    for kind,override,slave,restoring,shutting in itertools.product(['ibd','archive','sync'],[False,True],[False,True],[False,True],[False,True]):
        add([op(kind,list=[0,1,2,3,4,5],override=override,blockNode=slave,restore=restoring,shutdown=shutting),op('next'),op('info',shutdown=False,restore=False),op('check'),op('next')])
    for kind,listing in itertools.product(['ibd','archive','sync'],[[],[0],[1],[0,1,1,2],[2,1,0],[0,5,1],[0,-1,1],[0,1,6,7],[0,1,2,3,4,5,6,7]]):
        add([op(kind,list=listing,override=True),op('next'),op('recalc')])
    for kind,uid,restore in itertools.product(['ibd','archive','sync','ask'],[None,'','0x00','c0','π🐼'],[False,True]):
        add([op(kind,list=[],uid=uid,override=True,restore=restore),op('next')])
    for cmd,relevant,notify,slave,shutdown,restore in itertools.product(['post','postBlock'],[False,True],[False,True],[False,True],[False,True],[False,True]):
        add([op(cmd,relevant=relevant,notifyAll=notify,blockNode=slave,shutdown=shutdown,restore=restore),op('next')])
    for cascade,listing in itertools.product([-1,0,1,3],[[],[0],[1],[2],[4],[0,1,2]]):
        add([op('ibd',list=listing,cascade=cascade,override=True),op('next')])
    for depth,frequency,mega in itertools.product([1,2,4],[0,1,2],[False,True]):
        add([op('archive',list=list(range(6)),mega=mega),op('next'),op('recalc')],depth=depth,frequency=frequency)
    for fail,kind in itertools.product(range(1,35),['ibd','archive']):
        add([op(kind,list=list(range(6)),override=True),op('next',fail=fail),op('recalc'),op('check'),op('next')],depth=2,frequency=1)
    for fail in range(1,12):
        add([op('root'),op('post',tx=1),op('next',fail=fail),op('recalc')])
    for mode in ['chain','parent','time']:
        add([op('root'),op(mode,tx=1,value=10000000000000),op('post',tx=1),op('next')])
    for uid,noSync in itertools.product(['0x00','c0',None],[False,True]):add([op('ask',uid=uid,noSync=noSync)])
    for null in [True,False]:add([op('ibd',nullIBD=null,override=True),op('next'),op('timer'),op('check'),op('next')])
    for tx in [-1,0]:add([op('post',tx=tx),op('postBlock',tx=tx,blockNode=True),op('next')])
    for logs in [False,True]:add([op('root',logs=logs,ibdLogs=logs),op('ibd',list=list(range(6)),override=True),op('next')])
    add([op('root'),op('link',tx=1,other=8),op('time',tx=1,value=4102444800000),op('ibd',list=[1]),op('next')])
    for kind,count in itertools.product(['ibd','archive'],[256,257,258,1000,1001,1002]):
        add([op(kind,list=[0]*count,override=True),op('next',ibdLogs=True)])
    for origin,listing in itertools.product(['root','last'],[[4,3,2,1,0],[3,2,1,0],[5,4,3],[4,2,1],[6,1,0]]):
        add([op(origin,tx=5),op('sync',list=listing),op('next')])
    for cmd,value,kind in itertools.product(['height','work','magic','super','time'],[-1,0,10000000000000],['post','postBlock']):
        add([op('root',blockNode=kind=='postBlock'),op(cmd,tx=1,value=value),op(kind,tx=1),op('next')])
    for height,test in itertools.product([999,1000,1001],[False,True]):
        add([op('height',tx=0,value=height),op('height',tx=1,value=height+1),op('root',test=test,logs=True),op('post',tx=1),op('next')])
    return out

def project(output,request):
    assert 'ops' in output and len(output['ops'])==len(request['ops']),output
    for row in output['ops']:
        before=row.pop('before');after=row.pop('after');assert after>=before
        for index,line in enumerate(row['logs']):
            m=re.fullmatch(r'Processing main IBD finished (\d+)ms',line)
            if m:assert int(m[1])<=after-before;row['logs'][index]='Processing main IBD finished <validated elapsed>ms'
            m=re.fullmatch(r'(\[VALID:(?:true|false)\] Block checker time : )\d+ms( @ .*)',line)
            if m:row['logs'][index]=m[1]+'<elapsed>ms'+m[2]
    return normalize(output)
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'txpow-processor-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['NIOClientOracle.java','SqlDBOracle.java','ArchiveManagerOracle.java','TxPoWProcessorOracle.java']];listing=build/'txpow-processor-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxPoWProcessorOracle']),Runner([str(build/'minima_txpow_processor')])];failures=[];operations=0
    try:
        fixture=runners[0].ask(dict(fixture=True));assert 'blocks' in fixture,fixture
        (build/'txpow-processor-fixture.json').write_text(json.dumps(fixture)+'\n')
        for index,case in enumerate(cases()):
            request=fixture|case;outputs=[project(r.ask(request),request) for r in runners]
            if index==0:
                # Guard against a matching but inert fixture: both processors
                # must attach the complete six-block chain with original owners.
                for output in outputs:
                    assert output['ops'][-1]['length']==6,output
                    assert all(row==[False] or row[1] is True for row in output['ops'][-1]['nodes']),output
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
            operations+=len(case['ops'])
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,failures=failures,scope='Actual pinned processor/checker/tree/MMR with Java-mined block bytes. Controlled archive, TxPoW index and Main/network receivers; actual RAM flags. Queue, IBD, cascading, failure ordering, aliases and notifications. Native H2/full runtime and complete API parity remain pending.')
    (build/'txpow-processor-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
