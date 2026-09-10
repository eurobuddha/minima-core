#!/usr/bin/env python3
"""Actual coinimport with root invariants and retained effects after failure."""
import argparse,copy,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize
from node_state_corpus import project as node_project

MISSING=object()
def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00',**kw)
def corpus(f):
    cases=[];proof=f['importProof'];coin=f['coin']
    def chain(mode):
        nodes=copy.deepcopy(f['nodes' if mode=='known' else mode]) if mode!='empty' else []
        for node in nodes:node['relevant']=[]
        return op('chain',history='8',nodes=nodes)
    def command(data=proof,track=MISSING,**kw):
        params={'data':data}
        if track is not MISSING:params['track']=track
        return op(name='coinimport',params=params,replyText=True,**kw)
    def add(mode,*steps,**expected):cases.append(dict(coinImport=True,nodeState=True,mode=mode,ops=[chain(mode),*steps],**expected))
    add('known',op('meta',name='coinimport'))
    tracks=[MISSING,'true','false','TRUE','unknown',' true ','',' ',None,False,True,0,{},[]]
    for mode,track in itertools.product(['known','pruned'],tracks):
        add(mode,command(track=track),expectTrack=('early' if mode=='known' else 'yes' if track is MISSING or track in ['true',' true '] else 'error' if not isinstance(track,str) or not track.strip() else 'no'))
    for mode in ['known','pruned']:
        add(mode,command(),command(),**({'expectedError':'Attempting to add relevant coin we already have'} if mode=='known' else {'expectedCoinCopies':2}))
        add(mode,command(track='false'),command(track='false'))
        add(mode,command(track=None),command(track='true'))
        add(mode,command(data=f['futureProof']),**({'expectedSuccess':True} if mode=='known' else {'expectedError':'Invalid MMR Proof'}))
        add(mode,command(data=f['spentImport']),expectedError='Coin is spent. Can only import UNSPENT coins.')
        add(mode,command(data=f['spentproof']),expectedError='Coin is spent. Can only import UNSPENT coins.')
        add(mode,command(data=f['proofs'][1]),**({'expectedError':'Invalid MMR Proof'} if mode=='known' else {'expectedSuccess':True}))
        add(mode,command(data=proof+'ABCD'),expectedSuccess=True)
    add('missingTree',command(),expectedError='TreeNode at Blocktime not found (proof too old): 3')
    add('empty',command(),expectedError='java.lang.NullPointerException')
    # Malformed inputs must fail before mutating the tree.
    for value in [None,False,True,0,{},[],'',' ','0x','0xGG','0X00','0x00']:
        add('pruned',command(data=value),unchanged=True)
    for length in [1,2,3,4,8,16,32,(len(proof)-2)//2-1]:add('pruned',command(data=proof[:2+length*2]),unchanged=True)
    add('pruned',op(name='coinimport',params={},replyText=True),unchanged=True)
    for mode,caller in itertools.product(['known','pruned'],['0x00','0x1234']):
        for text in ['coinimport data:'+proof,'coinimport data:'+proof+' track:false','coinimport data:'+proof+' track:{}','coinimport invalid:true','coinimport','coinimport data:'+proof+';coinimport data:'+proof]:
            step=op('run',text=text,replyText=True);step['caller']=caller;add(mode,step)
    # SQL uses the existing recording driver; returned rows do not persist inserts.
    for mode in ['known','pruned']:
        for fail,runtime in [(0,False)]+list(itertools.product(range(1,13),[False,True])):
            add(mode,op('coinStore'),op('coinSQL',enabled=True),command(rowsBySQL={'SELECT * FROM coins WHERE txpowtreeid=?':[{'coindata':coin}]},fail=fail,runtimeFailure=runtime),sql=True)
    return cases

def project(raw,request):
    result=node_project(raw,normalize(raw),request,tree_sql=('SELECT * FROM coins WHERE txpowtreeid=?','INSERT INTO coins ( txpowtreeid, blockheight, coindata ) VALUES ( ?, ?, ? )'))
    for row in result['ops']:
        if isinstance(row.get('value'),str):
            try:reply=json.loads(row['value'])
            except json.JSONDecodeError:continue
            replies=reply if isinstance(reply,list) else [reply];changed=False
            for item in replies:
                if isinstance(item,dict) and isinstance(item.get('error'),str):
                    error=normalize(item['error']);changed|=error!=item['error'];item['error']=error
            if changed:row['value']=json.dumps(reply,ensure_ascii=False,separators=(',',':'))
    return result

def validate(request,result,f):
    rows=result['ops'];before=rows[0].get('nodeState',[])
    assert 'exception' not in rows[0] and 'nodeStateError' not in rows[0],rows[0]
    roots=[n['root'] for n in before]
    for row in rows[1:]:
        if not request.get('sql'):assert [n['root'] for n in row['nodeState']]==roots,row
    final=rows[-1]
    if 'expectedError' in request:assert request['expectedError'] in final.get('exception',''),(request,final)
    if request.get('expectedSuccess'):assert json.loads(final['value'])['status'],final
    if request.get('unchanged'):assert final['nodeState']==before and 'exception' in final,(request,final)
    if 'expectedCoinCopies' in request:
        node=final['nodeState'][8];assert node['allCoins'].count(f['coin'])==request['expectedCoinCopies'] and node['entries']==['0']*request['expectedCoinCopies'],node
    if 'expectTrack' in request:
        kind=request['expectTrack'];node=final['nodeState'][11 if request['mode']=='known' else 8]
        assert node['allCoins'].count(f['coin'])==1,node
        assert node['entries']==(['0'] if kind in ['early','yes'] else []),node
        assert node['coins']==([f['coin']] if kind in ['early','yes'] else []),node
        if kind=='error':assert 'exception' in final and node['finalized'],final
        else:assert json.loads(final['value'])['response']==f['importJSON'],final
    if request['ops'][-1]['op']=='meta':assert final['value']['name']=='coinimport' and final['value']['valid']==['data','track'],final

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve();oracle=build/'coin-import-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['SqlDBOracle.java','WalletCommandsOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java','CoinImportOracle.java']]
    listing=build/'coin-import-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=0;completed=0
    with tempfile.TemporaryDirectory(prefix='coin-import-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CoinImportOracle',root]),Runner([str(build/'coin_import_runtime'),root])]
        try:
            fixture=runners[0].ask({'kind':'coinImportFixtures'});cases=corpus(fixture)
            for i,request in enumerate(cases):
                output=[]
                for runtime,runner in zip(['java','cpp'],runners):
                    try:
                        actual=project(runner.ask(request),request);validate(request,actual,fixture);output.append(actual)
                    except Exception as error:failures.append(dict(case=i,runtime=runtime,error=repr(error),request=request,stderr=runner.errors()));break
                if failures:break
                if output[0]!=output[1]:failures.append(dict(case=i,request=request,java=output[0],cpp=output[1]));break
                completed+=1;operations+=len(request['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,failures=failures,scope='Actual coinimport registered command, CoinProof/MMR, MinimaDB/tree/search and recording SQL. Full/pruned knowledge, historical restoration, root invariant, duplicate tracking, early ignored track/supplied proof, late parameter errors with retained mutations, malformed wire, runner permissions and SQL failures. Replies/state compared; enhanced NPE/CCE prose and generated SQL tree IDs normalized. No durable SQL, live-node acceptance or exhaustive concurrent tree mutation certification.')
    (build/'coin-import-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
