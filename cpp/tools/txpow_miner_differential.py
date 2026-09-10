#!/usr/bin/env python3
"""Actual pinned miner: byte-patched work, reservations, messages and failure state."""
import argparse,itertools,json,random,re,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,**kw)
def cases():
    result=[]
    def add(ops,txs=None,**kw):result.append(dict(txs=txs or [dict(inputs=[0],burn=[1])],ops=ops,**kw))
    for seed,first,inputs,burn in itertools.product(range(1,9),['ff','80','10','01'],[[],[0],[0,0,2]],[[],[0,1]]):
        txs=[dict(seed=seed,difficulty='0x'+first+'ff'*31,inputs=inputs,burn=burn)]
        add([op('queue'),op('next',logs=True),op('sync',post=False,maxima=True,logs=True)],txs)
    for failAt,automine,logs in itertools.product(range(5),[False,True],[False,True]):
        add([op('queue'),op('process',automine=automine,logs=logs,failAt=failAt),op('sync',maxima=True,logs=logs,failAt=failAt),op('next')])
    for delay in [-1001,-1,0,1,3,50000,9223372036854775807,-9223372036854775808]:
        add([op('process',automine=True),op('pulse'),op('pulse',blockNode=True),op('block',blockNode=True),op('unknown')],delay=delay)
    for cmd in ['queue','sync','process']:
        add([op(cmd,tx=-1),op('next')])
        if cmd!='process':add([op('clearBody'),op(cmd)])
    for post,maxima,logs in itertools.product([False,True],[False,True],[False,True]):
        add([op('sync',post=post,maxima=maxima,logs=logs,limit=-1),op('difficulty',value='0x'+'ff'*32),op('sync',post=post,maxima=maxima,logs=logs)], [dict(inputs=[0,0,2],burn=[0,1],difficulty='0x00')])
    add([op('queue'),op('queue',tx=1),op('next'),op('next')],[dict(inputs=[0,2]),dict(inputs=[0],burn=[1])])
    add([op('queue'),op('input',coin=3),op('next'),op('queue'),op('stop'),op('next'),op('sync')])
    for stopped in [True,False]:
        add(([op('stop')] if stopped else [])+[op('process',automine=True),op('check',id=None),op('check',id=''),op('check',id='0x01'),op('check',id='π😄')])
    rng=random.Random(5564)
    for _ in range(120):
        txs=[dict(seed=rng.randrange(1,100),inputs=[rng.randrange(6) for _ in range(rng.randrange(4))],burn=[rng.randrange(6) for _ in range(rng.randrange(3))],difficulty='0x80'+'ff'*31) for _ in range(2)]
        add([op(rng.choice(['queue','sync','process']),tx=rng.randrange(2),post=rng.choice([False,True]),automine=rng.choice([False,True]),failAt=rng.randrange(4)) for _ in range(5)],txs)
    return result

def signed(value):return (value+(1<<63))%(1<<64)-(1<<63)
def project(output,request):
    assert 'ops' in output and len(output['ops'])==len(request['ops']),output
    for row,command in zip(output['ops'],request['ops']):
        before=row.pop('before');after=row.pop('after');assert after>=before
        for timer in row['timers']:
            delay=request.get('delay',50000)
            if command['cmd']=='pulse':delay=30000
            elif timer['type']=='TXPOWMINER_TXBLOCKMINER':delay=abs(delay)//2*(1 if delay>=0 else -1)
            creation=signed(timer['deadline']-delay);assert before<=creation<=after,(timer,before,after,delay)
            timer['deadline']={'validatedDelay':delay}
        for index,line in enumerate(row['logs']):
            match=re.fullmatch(r'(MINING TXPOW FINISHED time:|MINING MAXIMA FINISHED : )(\d+)',line)
            if match:assert int(match[2])<=after-before,(line,before,after);row['logs'][index]=match[1]+'<validated-elapsed>'
    return normalize(output)
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'txpow-miner-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','SqlDBOracle.java','ArchiveManagerOracle.java','ObjectOracle.java','TxPoWMinerOracle.java']];listing=build/'txpow-miner-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();failures=[];operations=0;runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxPoWMinerOracle']),Runner([str(build/'minima_txpow_miner')])]
    try:
        for index,request in enumerate(corpus):
            outputs=[project(r.ask(request),request) for r in runners]
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
            operations+=len(request['ops'])
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,failures=failures,scope='Actual pinned mining loops with real SHA3, full TxPoW wire/IDs and retained message identity, synchronous/async reservations, timeout/post exceptions, mutation and stopped-loop state, automine/no-tip timers. Clock values validated before normalization; Java NPE expressions excluded. Generator/network branches and full Main runtime remain pending.')
    (build/'txpow-miner-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
