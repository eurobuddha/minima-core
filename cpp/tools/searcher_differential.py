#!/usr/bin/env python3
"""Pinned Java search selection, ordering, relevance and lock-state comparison."""
import argparse,copy,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'searcher-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ArchiveCacheOracle.java',ROOT/'support/SearcherOracle.java'];listing=build/'searcher-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SearcherOracle'];native=[str(build/'minima_searcher')];rng=random.Random(5564)
    def run(command,cases):
        process=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=process.stdout.splitlines()
        if process.returncode or len(lines)!=len(cases):
            (build/'searcher-crash.txt').write_text(process.stderr);raise RuntimeError(f'{command[0]} exit {process.returncode}, lines {len(lines)} expected {len(cases)}')
        if any(line=='"ERROR"' for line in lines):
            (build/'searcher-errors.txt').write_text(process.stderr);raise RuntimeError('Searcher fixture/setup failed; see searcher-errors.txt')
        return lines
    fixtures=json.loads(run(java,[{'kind':'fixtures'}])[0]);cases=[]
    def query(**args):return {'op':'coins','start':0,'relevant':False,'cid':False,'id':'0x01','cam':False,'amount':'1','caddr':False,'address':'0x01','ctok':False,'token':'0x00','cstate':False,'state':'[state1]','wild':False,'simple':False,'depth':2147483647,'mega':False,**args}
    def request(count=3):
        nodes=[]
        for i in range(count):
            coins=[rng.choice(fixtures['coins']) for _ in range(12)];nodes.append({'pow':fixtures['pow'][i%8],'txns':rng.choices(fixtures['ids']+['0x99'],k=3),'coins':coins,'relevant':coins[::2]})
        return {'nodes':nodes,'pool':fixtures['pow'][::2],'megacoins':fixtures['coins'][::3],'tokens':fixtures['tokens']+fixtures['tokens'][::2],'addresses':['0x01'],'simple':['0x02'],'keys':['0x03'],'mega':True,'queries':[]}
    for count in [0,1,2,3,8]:
        for mega in [False,True]:
            req=request(count);req['mega']=mega;queries=[]
            for start in [-1,0,count-1]:
                for depth in [-2,-1,0,1,2,2147483647]:
                    for flags in itertools.product([False,True],repeat=3):queries.append(query(start=start,depth=depth,relevant=flags[0],simple=flags[1],mega=flags[2]))
            for flags in itertools.product([False,True],repeat=5):queries.append(query(start=count-1,cid=flags[0],cam=flags[1],caddr=flags[2],ctok=flags[3],cstate=flags[4],mega=mega))
            for op in ['coin','coindefault','nodecoin','floating','block','chain','nodeblock','address','checkaddress','checkrelevant','tokens','token','relevant','relevanttoken']:
                for index in range(4):queries.append(query(op=op,start=count-1,id=rng.choice(fixtures['ids']+['0x01','0x08','0x99']),amount=str(index),token=rng.choice(fixtures['tokenids']+['0x00','0x99']),address='0x0'+str(index+1),mega=mega))
            for node in range(count):
                for op in ['chain','nodeblock']:queries.append(query(op=op,start=count-1,nodeid=node))
            req['queries']=queries;cases.append(req)
    for repeat in range(150):
        count=rng.randrange(6);req=request(count);req['mega']=rng.choice([False,True]);req['megacoins']=rng.choices(fixtures['coins'],k=rng.randrange(20));req['tokens']=rng.choices(fixtures['tokens']+[None],k=rng.randrange(4));req['addresses']=rng.sample(['0x01','0x02','0x03'],rng.randrange(4));req['keys']=rng.sample(['0x01','0x02','0x03'],rng.randrange(4));req['simple']=rng.sample(['0x01','0x02','0x03'],rng.randrange(4))
        for index in range(25):
            q=query(op=rng.choice(['coins','coins','coins','relevant','relevanttoken','floating','coin','coindefault','nodecoin','block','chain','nodeblock','address','checkaddress','checkrelevant','tokens','token']),start=rng.randrange(-1,max(1,count)),id=rng.choice(['0x0'+str(i) for i in range(1,10)]+fixtures['ids']),amount=str(rng.randrange(-1,9)),address=rng.choice(['0x01','0x02','0x03','0x04']),token=rng.choice(fixtures['tokenids']+['0x00','0x99']),state=rng.choice(['','state','[state0]','state1','0x01','x0','[missing]']),depth=rng.choice([-1,0,1,2,100]))
            for flag in ['relevant','cid','cam','caddr','ctok','cstate','wild','simple','mega']:q[flag]=rng.choice([False,True])
            req['queries'].append(q)
        cases.append(req)
    proofdata=json.loads(run(java,[{'kind':'prooffixtures'}])[0])
    for testparams in [False,True]:
        req=request(0);req['nodes']=proofdata['nodes'];req['testparams']=testparams;req['tokens']=[];req['megacoins']=[];req['queries']=[]
        for name in ['tokens','coincheck','coinexport']:req['queries'].append(query(op='command',name=name,params={},meta=True,start=11))
        for coinid in proofdata['ids']+['0x99','bad','Mx','0x','']:
            req['queries'].append(query(op='command',name='coinexport',params={'coinid':coinid},start=11))
        for proof in proofdata['proofs']+[proofdata['spentproof']]:
            for data in [proof,proof+'FF',proof[:-2],proof[:12],proof[:11]+('0' if proof[11]!='0' else '1')+proof[12:]]:req['queries'].append(query(op='command',name='coincheck',params={'data':data},start=11))
        for name in ['coincheck','coinexport']:
            req['queries'].append(query(op='command',name=name,params={},start=11))
        cases.append(req)
    for shape in [0,1,4]:
        req=request(shape);req['queries']=[]
        for action in [None,'export','import','IMPORT','bad','']:
            for tokenid in [None,'0x00','0x99','bad','']+fixtures['tokenids']:
                params={}
                if action is not None:params['action']=action
                if tokenid is not None:params['tokenid']=tokenid
                req['queries'].append(query(op='command',name='tokens',params=params,start=shape-1))
        cases.append(req)
    for token in fixtures['tokens']:
        raw=bytes.fromhex(token[2:]);variants=[token,token+'FF',token[:-2],'0x','0x00','0x0021','0x'+(raw[:20]+bytes.fromhex('0000')+raw[23:]).hex(),'0x'+(raw[:20]+bytes.fromhex('0021')+raw[23:]).hex()]
        for value in variants:
            req=request(0);req['tokens']=[];req['queries']=[query(op='command',name='tokens',params={'action':'import','data':value},start=-1),query(op='command',name='tokens',params={},start=-1)]+[query(op='command',name='tokens',params={'action':'export','tokenid':tokenid},start=-1) for tokenid in fixtures['tokenids']]
            cases.append(req)
    expected=run(java,cases);actual=run(native,cases);failures=[]
    for case,j,n in zip(cases,expected,actual):
        if j!=n:
            ja=json.loads(j);na=json.loads(n);differences=[{'query':q,'java':a,'cpp':b} for q,a,b in zip(case['queries'],ja,na) if a!=b] if isinstance(ja,list) and isinstance(na,list) else [{'java':ja,'cpp':na}]
            failures.append({'case':case,'differences':differences})
    result={'cases':len(cases),'queries':sum(len(c['queries']) for c in cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Filtered coin searches, spent/dedup ordering, depth, MegaMMR fallback and lock counts, simple/relevant selection, block/transaction/address searches with duplicate matches, token imports and lookup, tokens command failure state, coin export/check against a real twelve-block MMR history; native chain token reference identity remains incomplete'}
    (build/'searcher-differential-results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({**result,'failures':[{'differences':f['differences'][:2]} for f in failures[:2]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
