#!/usr/bin/env python3
"""Historical burn windows, retained accumulators and public median helper."""
import argparse,copy,itertools,json,random,subprocess,tempfile
from decimal import Decimal,localcontext,ROUND_DOWN
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from scan_chain_differential import project,op
from wallet_balance_corpus import fixture_request

def cmd(params=None,**kw):return op(name='burn',params=params or {},replyText=True,**kw)
def run(**kw):return op('burnRun',params={},replyText=True,**kw)
def fixtures(runner):
    request=fixture_request();request['groups']=request['groups'][:2];coins=runner.ask(request)
    native=coins['groups'][0]['coins'][0];token=coins['groups'][1]['coins'][0]
    def coin(amount,wire=native):return dict(wire=wire,amount=str(amount))
    specs=[{},dict(inputs=[coin(0)]),dict(inputs=[coin(5)],outputs=[coin(2)]),dict(burnInputs=[coin(7)],burnOutputs=[coin(2)]),dict(inputs=[coin(4)],outputs=[coin(6)]),dict(inputs=[coin('0.25')]),dict(inputs=[coin('2000000000')]),dict(inputs=[coin(99,token)],burnInputs=[coin(8)],burnOutputs=[coin(1)]),dict(inputs=[coin(4)],outputs=[coin(1)],burnInputs=[coin(5)],burnOutputs=[coin(2)])]
    def build(specs):return runner.ask(dict(kind='txpowSearchFixtures',wires=runner.ask(dict(kind='historyFixtures',cases=specs))))
    leaves=build(specs);ids=[x['id'] for x in leaves]
    blocks=build([dict(block=str(i+1),transactions=ids[:6]+ids[2:3]+['0xFF'])|specs[i%len(specs)] for i in range(53)])
    nodes=[]
    for block in blocks:
        node=copy.deepcopy(coins['nodes'][0]);node.update(pow=block['wire'],coins=[],relevant=[]);nodes.append(node)
    # Values follow explicit coin amounts, independently of either burn implementation.
    return dict(nodes=nodes,leaves=leaves,values=[None,Decimal(0),Decimal(3),Decimal(5),Decimal(-2),Decimal('.25'),Decimal(2000000000),Decimal(106),Decimal(6)])

def corpus(f):
    cases=[];nodes=f['nodes'];leaves=f['leaves']
    def add(*steps,chain=nodes,ram=True,stores=True,**kw):
        setup=([op('txpowStores')] if stores else [])+[op('chain',history='8',nodes=chain)]
        if ram:setup += [op('txpowRAM',data=x['wire']) for x in leaves]
        cases.append(dict(ops=setup+list(steps),**kw))
    add(op('meta',name='burn'),metadata=True)
    for length in [0,1,2,9,10,11,49,50,51,52,53]:
        add(cmd(),chain=nodes[:length],chainLength=length)
        add(op('burnNew'),run(),run(),run(fresh=True),chain=nodes[:length],retainedLength=length)
    for action in [None,True,False,0,1,{},[],'','list','LIST','send',' list ','java.lang.NullPointerException: text']:
        add(cmd({'action':action}))
    for caller,text in itertools.product(['0x00','0x1234'],['burn','burn action:list','burn action:send','burn invalid:true','burn;burn','burn action:{};burn']):add(op('run',text=text,caller=caller,replyText=True))
    for i,leaf in enumerate(leaves):
        for counter in [-2147483648,-1,0,1,9,10,49,50,2147483647]:
            add(op('burnNew'),op('burnCheck',data=leaf['wire'],counter=counter),run(),chain=[],helper=(i,counter))
        add(op('burnNew'),run(),op('burnCheck',data=leaf['wire'],counter=0),run(),chain=[],afterEmpty=i)
        add(op('burnNew'),op('burnCheck',data=leaf['wire'],counter=0,bodyless=True),run(),chain=[])
    add(op('burnNew'),op('burnCheck',nullTxPoW=True,counter=0),run(),chain=[])
    add(op('burnCheck',nullTxPoW=True,counter=0),chain=[])
    add(op('burnNew'),op('burnCheck',data=leaves[2]['wire'],counter=0),op('burnRun',params={'action':'unknown'},replyText=True),run(),chain=[],helper=(2,0))
    arrays=[[],[None],['1'],['2','1'],['1','2','3','4'],['1','1.0',{'alias':0},'2','0'],[None,'1'],['1',None],['3','1',None],['1','3',None],['2','1','3',None]]
    rng=random.Random(457)
    arrays += [[str(rng.randrange(-20,21)) for _ in range(n)] for n in [3,4,8,16,31,32,33,64,65,100,511]]
    for length in [4,16,31,32,33,48,64,65,100,127,256]:
        for mode in ['ascending','descending','random']:
            base=[str(i) for i in range(length)]
            if mode=='descending':base.reverse()
            if mode=='random':rng.shuffle(base)
            for null_at in sorted({0,1,length//2,length-1}):
                malformed=base.copy();malformed[null_at]=None;arrays.append(malformed)
    arrays.append([str(i) for i in reversed(range(31))]+[str((1-i%2)*1000+i) for i in range(34)]+[None])
    for values in arrays:add(op('burnNew'),op('burnMedian',values=values),chain=[],median=values)
    add(op('burnNew'),op('burnMedian',values=[],nullValues=True),chain=[])
    for rows in [[],[{}],[dict(txpowdata=leaves[2]['wire'])]]:
        add(op('burnNew'),run(rows=rows),run(),chain=nodes[:1],ram=False)
    for fail,runtime in itertools.product(range(1,25),[False,True]):
        add(op('burnNew'),run(rows=[dict(txpowdata=leaves[2]['wire'])],fail=fail,runtimeFailure=runtime),run(),chain=nodes[:1],ram=False)
    for invalid in [None,'0x','0x00',leaves[2]['wire'][:-2]]:add(op('burnNew'),run(rows=[dict(txpowdata=invalid)]),run(),chain=nodes[:1],ram=False)
    for flag in ['nullPrepared','nullRows']:add(op('burnNew'),run(**{flag:True}),run(),chain=nodes[:1],ram=False)
    add(op('burnNew'),run(),chain=nodes[:1],ram=False,stores=False)
    return cases

def metrics(values,empty_before=False):
    if not values:return dict(txns=0,max=0,med=0,avg=0,min=0)
    with localcontext() as ctx:
        ctx.prec=64;ctx.rounding=ROUND_DOWN
        average=(sum(values)/len(values)).quantize(Decimal('1e-44'))
    return dict(txns=len(values),max=max([Decimal(0)]+values),med=sorted(values,reverse=True)[len(values)//2],avg=average,min=min([Decimal(0 if empty_before else 1000000000)]+values))
def expected(f,length,repeats=1):
    result={}
    refs=[v for v in f['values'][:6] if v is not None]+[f['values'][2]]
    for size in [1,10,50]:
        values=[]
        for index in reversed(range(max(0,length-size),length)):
            own=f['values'][index%len(f['values'])];values+=([own] if own is not None else [])+refs
        result[str(size)+'block']=metrics(values*repeats)
    return result

def validate(request,result,f):
    rows=result['ops'];last=rows[-1]
    def response(row):return json.loads(row['value'],parse_float=Decimal)['response']
    if request.get('metadata'):
        value=last['value'];assert value['name']=='burn' and value['valid']==[] and value['help']=='View Burn metrics' and value['fullhelp'].startswith('\nburn\n'),value
    if 'chainLength' in request:assert response(last)==expected(f,request['chainLength']),(response(last),expected(f,request['chainLength']))
    if 'retainedLength' in request:
        for row,repeats in zip(rows[-3:],[1,2,1]):assert response(row)==expected(f,request['retainedLength'],repeats),(response(row),repeats)
    if 'helper' in request or 'afterEmpty' in request:
        index,counter=request.get('helper',(request.get('afterEmpty'),0));value=f['values'][index]
        wanted={str(size)+'block':metrics([value] if value is not None and (size==50 or counter<10 and size==10 or counter==0 and size==1) else [],'afterEmpty' in request) for size in [1,10,50]}
        assert response(last)==wanted,(response(last),wanted)
    if 'median' in request and None not in request['median']:
        source=[]
        for i,item in enumerate(request['median']):source.append(source[item['alias']] if isinstance(item,dict) else (i,Decimal(item)))
        sorted_values=sorted(source,key=lambda pair:pair[1],reverse=True);value=last['value'];chosen=sorted_values[len(source)//2] if source else (-1,Decimal(0))
        if not source:assert value['emptyIdentity'] is True,value
        assert value['identity']==chosen[0] and Decimal(value['median'])==chosen[1],value
        assert [Decimal(v) for v in value['values']]==[v for _,v in sorted_values],value

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'burn-metrics-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['WalletCommandsOracle.java','SqlDBOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java']]
    listing=build/'burn-metrics-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar','-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=effects=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='burn-metrics-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root]),Runner([str(build/'burn_metrics_runtime'),root])]
        try:
            f=fixtures(runners[0]);cases=corpus(f)
            for i,request in enumerate(cases):
                outputs=[]
                for name,runner in zip(['java','cpp'],runners):
                    try:
                        raw=runner.ask(request);value=project(raw,request);validate(request,value,f);outputs.append(value)
                    except Exception as error:failures.append(dict(case=i,runtime=name,error=repr(error),request=request,stderr=runner.errors(),raw=locals().get('raw')));break
                if failures:break
                if outputs[0]!=outputs[1]:failures.append(dict(case=i,request=request,java=outputs[0],cpp=outputs[1]));break
                completed+=1;operations+=len(request['ops']);effects+=sum(len(x['events']) for x in outputs[0]['ops'])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual burn command, public helpers and runner with real TxPoW arithmetic, tree and RAM/recording SQL. Window boundaries, retained state, median order/identity, malformed inputs and SQL failures. Exact numeric replies and effects; selected VM exception prose normalized. No arbitrary throwing-sort mutation, inherited reference/concurrency, durable SQL or full-node parity certification.')
    (build/'burn-metrics-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
