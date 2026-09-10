#!/usr/bin/env python3
"""Pinned CoinDB with real coin codecs and filesystem, recording JDBC boundary."""
import argparse,itertools,json,random,shutil,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def corpus(wires):
    cases=[]
    def op(cmd,**kw):
        result=dict(cmd=cmd,rows=[],tree='0x0001',block='42',height=0)
        result.update(kw);return result
    def add(*ops):cases.append(dict(ops=list(ops)))
    rows=[dict(coindata=wire,tot=3) for wire in wires]
    commands=['insert','get','clear','size','reopen','save','hard','open','load','encrypted']
    add(op('create'),op('hold'),op('insert'),op('get',rows=rows),op('clear'),op('size',rows=rows),op('save',compact=True),op('get',rows=rows),op('create'),op('insert',held=True),op('hard'),op('reopen'))
    for cmd,fail,runtime in itertools.product(commands,range(1,25),[False,True]):
        add(op('create'),op(cmd,rows=rows,fail=fail,runtimeFailure=runtime),op('size',rows=rows))
    for fail,runtime in itertools.product(range(1,14),[False,True]):
        add(op('create'),op('hold'),op('create',fail=fail,runtimeFailure=runtime,seed='file',debug=True),op('size',rows=rows),op('get',rows=rows),op('size',held=True,rows=rows))
    for flag,cmd in itertools.product(['nullConnection','nullStatement','nullPrepared','nullRows'],commands):
        add(op('create',**{flag:True}),op(cmd,rows=rows,**{flag:True}),op('hard'),op('get',rows=rows))
    for closed,cmd,fail in itertools.product([False,True],['insert','get','clear','size'],range(1,22)):
        add(op('create'),op('hard') if closed else op('open'),op(cmd,rows=rows,fail=fail),op('open'))
    for cmd,table in itertools.product(['get','size'],[[],[{}],rows,rows+rows,[dict(coindata='0x')]+rows]):
        add(op('create'),op(cmd,rows=table),op(cmd,tree=None,rows=table))
    for index,tree,block,nullcoin,invalid in itertools.product([0,1,7,31],[None,'0x','0x001122'],[None,'0','-1','9223372036854775808','-9223372036854775809'],[False,True],[False,True]):
        add(op('create'),op('insert',index=index,tree=tree,block=block,nullCoin=nullcoin,invalidHash=invalid))
    for seed,fail in itertools.product(['file','empty','nonempty'],[0,1,4,9]):
        add(op('create',seed=seed,fail=fail),op('hold'),op('create',file=None),op('instance'),op('open',held=True))
    for file in ['db/other','db//nested///coin/','db/../coin','db/./coin','db/π 😄','db/nul\0coin','db/nul\0dir/coin']:
        add(op('create',file=file),op('hard'),op('reopen'),op('get',rows=rows))
    for height in [-(1<<63),-(1<<31),-1,0,1,(1<<31)-1,1<<31,(1<<63)-1]:
        add(op('create'),op('clear',height=height),op('size',rows=[dict(tot=height)]))
    for wire in wires:
        for end in range(2,len(wire),2):
            add(op('create'),op('get',rows=[rows[0],dict(coindata=wire[:end]),rows[-1]]))
        add(op('create'),op('get',rows=[dict(coindata=wire+'FEED')]))
    encoded=bytes.fromhex(wires[0][2:]);offset=0
    for _ in range(2):offset+=4+int.from_bytes(encoded[offset:offset+4],'big')
    malformed=['0x'+(n.to_bytes(4,'big',signed=True)+encoded[4:]).hex() for n in [-1,65]]
    malformed+=['0x'+(encoded[:offset]+v+encoded[offset+2:]).hex() for v in [b'\0\0',b'\0\x21',b'\0\xff']]
    for wire in malformed:add(op('create'),op('get',rows=[rows[0],dict(coindata=wire),rows[-1]]))
    rng=random.Random(5564)
    for _ in range(60):
        add(op('create'),*[op(rng.choice(commands+['create']),rows=rows,fail=rng.randrange(20),runtimeFailure=rng.choice([False,True]),index=rng.randrange(32)) for _ in range(10)])
    return cases

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'coin-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/CoinDBOracle.java',ROOT/'support/ObjectOracle.java'];listing=build/'coin-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=effects=0
    with tempfile.TemporaryDirectory(prefix='minima-coin-db-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CoinDBOracle',root]),Runner([str(build/'minima_coin_db'),root])]
        def ask(request):
            outputs=[]
            for runner in runners:
                # Reset only this test's generated files before each implementation.
                shutil.rmtree(Path(root)/'files',ignore_errors=True)
                outputs.append(normalize(runner.ask(request)))
            return outputs
        try:
            request=dict(ops=[dict(cmd='instance',rows=[])]+[dict(cmd='make',rows=[],index=i) for i in [0,1,7]])
            outputs=ask(request);assert outputs[0]==outputs[1],outputs;assert outputs[0]['ops'][0]['value']==-1,outputs
            wires=[result['value']['wire'] for result in outputs[0]['ops'][1:]]
            for index,request in enumerate(corpus(wires)):
                outputs=ask(request)
                if outputs[0]!=outputs[1] or 'ops' not in outputs[0]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                operations+=len(request['ops']);effects+=sum(len(op['events']) for op in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,operations=operations,effects=effects,failures=failures,scope='Actual pinned CoinDB and Coin codec, static instance/held-owner identity, temporary filesystem deletion, constructor publication, schema/prepared statements, SQL/runtime failures, partial/null rows, malformed/truncated coins and serialization failure timing. JDBC calls are recorded; no native H2 storage or full node integration. Java NPE expression details and logger prefixes/stacks excluded.')
    (build/'coin-db-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
