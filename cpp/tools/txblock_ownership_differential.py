#!/usr/bin/env python3
"""TxBlock main-object identity and in-place restoration against pinned Java."""
import argparse,itertools,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,**kw)
def cases(prefixes):
    out=[]
    for size,flags,time in itertools.product([1,2,5],[0,1,2,3,4,8],[0,1,7]):
        out.append(dict(size=size,flags=flags,time=time,ops=[op('info'),op('held',nonce=123),op('read'),op('held',nonce=456),op('target',nonce=789)]))
    for prefix in range(prefixes+1):out.append(dict(ops=[op('read',length=prefix),op('held',nonce=123),op('read'),op('held',nonce=456)]))
    for ops in [[op('simple'),op('node'),op('main',nonce=123),op('target',nonce=456)], [op('simple',null=True)], [op('construct'),op('main',nonce=123)], [op('construct',nullParent=True)], [op('construct',nullMain=True)], [op('listNull'),op('construct')], [op('link'),op('listNull'),op('construct')], [op('link'),op('empty'),op('construct')], [op('link'),op('construct'),op('main',nonce=123)]]:out.append(dict(ops=ops))
    for index in [0,1,2]:out.append(dict(ops=[op('link'),op('nullEntry',index=index),op('construct')]))
    for count,mode,timed in itertools.product([0,1,2,5],range(6),[False,True]):out.append(dict(checker=True,count=count,mode=mode,timed=timed))
    return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'txblock-ownership-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['ObjectOracle.java','ArchiveManagerOracle.java','SqlDBOracle.java','NIOClientOracle.java','TxBlockOwnershipOracle.java']];listing=build/'txblock-ownership-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxBlockOwnershipOracle']),Runner([str(build/'txblock_ownership')])];failures=[];operations=0
    try:
        prefixes=runners[0].ask(dict(ops=[op("info")]))["bytes"]
        for index,request in enumerate(cases(prefixes)):
            outputs=[normalize(r.ask(request)) for r in runners]
            if outputs[0]!=outputs[1] or not ('ops' in outputs[0] or 'steps' in outputs[0]):failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
            operations+=len(request.get('ops',[None]*3))
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,failures=failures,scope='TxBlock main TxPoW and tree aliases, shared/null constructor arguments, exact missing-transaction errors, in-place read/truncated-prefix offsets and partial state, held old TxPoW and removed-state retention; shared checker lists, repeated transaction aliases, cache mutation and null-worker completion. Collection aliases and full node runtime remain incomplete; NPE expressions normalized.')
    (build/'txblock-ownership-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
