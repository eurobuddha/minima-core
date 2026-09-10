#!/usr/bin/env python3
"""Source scanchain: inclusive bounds, real TxPoWs and recording SQL effects."""
import argparse,copy,itertools,json,subprocess,tempfile
from decimal import Decimal
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
from wallet_balance_corpus import fixture_request,preserve_text

def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00')|kw
def cmd(params=None,**kw):return op(name='scanchain',params=params or {},replyText=True,**kw)
def details(value):
    body=value['body'];out={'txpowid':value['txpowid']}
    for source,flag,target in [('txn','istransaction','transaction'),('burntxn','isburntransaction','burntransaction')]:
        transaction=body[source];nonempty=bool(transaction['inputs'] or transaction['outputs'])
        out[flag]=nonempty
        if nonempty:out[target]=transaction
    return out

def fixtures(runner):
    request=fixture_request();request['groups']=request['groups'][:2];coins=runner.ask(request)
    native=coins['groups'][0]['coins'];token=coins['groups'][1]['coins']
    specs=[{},dict(inputs=native[:1]),dict(burnInputs=native[1:2]),dict(inputs=token[:1],burnOutputs=native[2:3],state='[java.lang.NullPointerException: ordinary state data]')]
    wires=runner.ask(dict(kind='historyFixtures',cases=[dict(block=str(i+1))|v for i,v in enumerate(specs)]))
    leaves=runner.ask(dict(kind='txpowSearchFixtures',wires=wires));ids=[v['id'] for v in leaves]
    times=['0','1','-1','1234.9','-2208988800000','-12219292800000','-12219292800001','1700000000123','9223372036854775807','-9223372036854775808','18446744073709551615','-0.9']
    specs=[]
    for i in range(24):
        height=str(10+2*i)+('.5' if i in [2,5] else '')
        if i==23:height='9007199254740993'
        specs.append(dict(block=height,time=times[i%len(times)],transactions=ids+ids[1:2]+['0xFF'])|[{},dict(inputs=native[:1]),dict(burnInputs=native[1:2]),dict(inputs=token[:1],burnOutputs=native[2:3])][i%4])
    blocks=runner.ask(dict(kind='txpowSearchFixtures',wires=runner.ask(dict(kind='historyFixtures',cases=specs))))
    nodes=[]
    for block in blocks:
        node=copy.deepcopy(coins['nodes'][0]);node.update(pow=block['wire'],coins=[],relevant=[]);nodes.append(node)
    return dict(nodes=nodes,blocks=blocks,leaves=leaves)

def corpus(f):
    cases=[];nodes=f['nodes'];leaves=f['leaves']
    def add(*steps,chain=nodes,stores=True,ram=True,**expect):
        setup=([op('txpowStores')] if stores else [])+[op('chain',history='8',nodes=chain)]
        if ram:setup += [op('txpowRAM',data=x['wire']) for x in leaves]
        cases.append(dict(ops=setup+list(steps),**expect))
    add(op('meta',name='scanchain'),metadata=True)
    offsets=[None,'-2','-1','0','1','2','16','23','24','2147483647','-2147483648','4294967295','4294967296','1.9','-0.9']
    depths=[None,'-2','-1','0','1','16','24','2147483647','-2147483648','4294967296','2.9']
    def integer(value,default):return (int(Decimal(value))+(1<<31))%(1<<32)-(1<<31) if value is not None else default
    for offset,depth in itertools.product(offsets,depths):
        params={k:v for k,v in [('offset',offset),('depth',depth)] if v is not None}
        o,d=integer(offset,0),integer(depth,16);start=23-max(o+1,0)
        selected=list(range(start,max(-1,start-max(d+1,0)),-1)) if start>=0 and d>=0 else []
        add(cmd(params),selected=selected,expectedOffset=o,expectedDepth=d)
    for length in [0,1,2]:
        for params in [{},{'offset':'-1','depth':'0'},{'depth':'bad','offset':None}]:
            add(cmd(params),chain=nodes[:length],**({'expectedError':'NO Blocks yet..'} if length==0 else {}))
    for field,value in itertools.product(['depth','offset'],[None,True,False,0,1,{},[],'',' ','bad','1e-100','1E100',' 2 ','１２','0x01']):
        for empty in [False,True]:add(cmd({field:value}),chain=[] if empty else nodes,**({'expectedError':'NO Blocks yet..'} if empty else {}))
    for params in [dict(offset=None,depth='bad'),dict(offset='bad',depth=None),dict(offset='100',depth='bad')]:add(cmd(params))
    # Direct helper calls bypass the isTransaction guard to exercise empty and invalid bodies.
    for leaf in leaves:
        add(op('scanDetails',data=leaf['wire'],replyText=True),expectedDetail=details(json.loads(leaf['json'])))
        for flag in ['bodyless','nullTransaction','nullBurn']:
            add(op('scanDetails',data=leaf['wire'],**{flag:True}),expectedError='java.lang.NullPointerException')
    add(op('scanDetails',nullTxPoW=True),expectedError='java.lang.NullPointerException')
    # RAM is absent here: each listed reference resolves through the existing SQL boundary.
    for rows in [[],[{}],[dict(txpowdata=leaves[1]['wire'])]]:
        add(cmd(dict(offset='-1',depth='0'),rows=rows),chain=nodes[:1],ram=False)
    for fail,runtime in itertools.product(range(1,21),[False,True]):
        add(cmd(dict(offset='-1',depth='0'),rows=[dict(txpowdata=leaves[1]['wire'])],fail=fail,runtimeFailure=runtime),chain=nodes[:1],ram=False)
    for invalid in [None,'0x','0x00',leaves[1]['wire'][:-2]]:
        add(cmd(dict(offset='-1',depth='0'),rows=[dict(txpowdata=invalid)]),chain=nodes[:1],ram=False)
    for flag in ['nullPrepared','nullRows']:
        add(cmd(dict(offset='-1',depth='0'),**{flag:True}),chain=nodes[:1],ram=False)
    add(cmd(dict(offset='-1',depth='0')),chain=nodes[:1],ram=False,stores=False)
    for params in [dict(depth='-1'),dict(offset='100'),dict(offset='2147483647')]:
        add(cmd(params),noSQL=True,ram=False)
    for caller,text in itertools.product(['0x00','0x1234'],['scanchain','scanchain depth:0 offset:-1','scanchain invalid:true','scanchain depth:{}','scanchain offset:bad;scanchain','scanchain depth:-1;scanchain depth:0']):
        add(op('run',text=text,caller=caller,replyText=True))
    return cases

def project(raw,request):
    value=preserve_text(raw,normalize(raw),request)
    for row,operation in zip(value['ops'],request['ops']):
        if operation.get('replyText') and isinstance(row.get('value'),str):
            reply=json.loads(row['value']);items=reply if isinstance(reply,list) else [reply];changed=False
            for item in items:
                if isinstance(item,dict) and isinstance(item.get('error'),str):
                    error=normalize(item['error']);changed|=error!=item['error'];item['error']=error
            if changed:row['value']=json.dumps(reply,ensure_ascii=False,separators=(',',':'))
    return value

def validate(request,result,f):
    row=result['ops'][-1]
    if request.get('metadata'):
        m=row['value'];assert m['name']=='scanchain' and m['valid']==['depth','offset'] and m['fullhelp']==m['help'],m
    if 'expectedError' in request:assert request['expectedError'] in row.get('exception',''),row
    if request.get('noSQL'):assert row['events']==[],row['events']
    if 'expectedDetail' in request:assert json.loads(row['value'])==request['expectedDetail'],row
    if 'selected' not in request:return
    response=json.loads(row['value'],parse_float=Decimal)['response'];blocks=response['blocks'];selected=request['selected']
    assert response['depth']==request['expectedDepth'] and response['offset']==request['expectedOffset'],response
    assert len(blocks)==len(selected),(len(blocks),selected)
    start=Decimal(json.loads(f['blocks'][selected[0]]['json'])['header']['block']) if selected else None
    leafdetails=[details(json.loads(x['json'],parse_float=Decimal)) for x in f['leaves']]
    for block,i in zip(blocks,selected):
        source=json.loads(f['blocks'][i]['json'],parse_float=Decimal);header=source['header'];height=Decimal(header['block'])
        assert isinstance(block['block'],(int,Decimal)) and not isinstance(block['block'],bool),block
        assert block['block']==height and block['depth']==start-height and block['timemilli']==Decimal(header['timemilli']),block
        assert block['date']==header['date'] and block['txpowid']==source['txpowid'],block
        own=details(source);expected=([own] if own['istransaction'] or own['isburntransaction'] else [])+leafdetails+leafdetails[1:2]
        assert block['transactions']==expected,(i,block['transactions'],expected)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve();oracle=build/'scan-chain-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['SqlDBOracle.java','WalletCommandsOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java']]
    listing=build/'scan-chain-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=completed=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='scan-chain-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root]),Runner([str(build/'scan_chain_runtime'),root])]
        try:
            f=fixtures(runners[0]);cases=corpus(f)
            for i,request in enumerate(cases):
                output=[]
                for runtime,runner in zip(['java','cpp'],runners):
                    try:
                        value=project(runner.ask(request),request);validate(request,value,f);output.append(value)
                    except Exception as error:failures.append(dict(case=i,runtime=runtime,error=repr(error),request=request,stderr=runner.errors()));break
                if failures:break
                if output[0]!=output[1]:failures.append(dict(case=i,request=request,java=output[0],cpp=output[1]));break
                completed+=1;operations+=len(request['ops'])
        except Exception as error:
            failures.append(dict(phase='fixture or comparison setup',error=repr(error)))
        finally:
            for runtime,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,failures=failures,scope='Actual scanchain and detail helper, CommandRunner, tree and TxPoWDB RAM/recording-SQL. Inclusive bounds, wrapped integers, fractional/gapped/large heights, extreme dates, duplicate/missing/empty references, ordinary/burn transaction JSON, invalid parameters/receivers/SQL bytes and injected SQL failures. Exact reply text and effects; existing NPE/CCE diagnostic normalization. No durable SQL, atomic concurrent traversal or full-node parity claim.')
    (build/'scan-chain-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
