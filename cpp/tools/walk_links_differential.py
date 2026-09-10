#!/usr/bin/env python3
"""Pinned peer walk routing, path mutation and swap generation."""
import argparse,itertools,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,execute,op,client,address
from swap_links_differential import canonical

def cases():
    rows=[];own=address('8.8.4.4');a=address('8.8.8.1');b=address('8.8.8.2')
    def links(n):return [dict(uid=f'c{i}',address=address(f'8.8.8.{i+1}')) for i in range(n)]
    def clients(n):return [client(f'c{i}',f'8.8.8.{i+1}') for i in range(n)]
    def walk(path,**kw):return dict(path=path,secret='0x01',**kw)
    def add(group,ops,**kw):rows.append((group,dict(swap=True,level=2,my=own,walk=walk([]),ops=ops)|kw))
    for entries,excluded in itertools.product([None,[],[dict(uid='c0',address=None)],links(2)],[None,[],[None],[a],[a,b],[b,b]]):
        add('filter',[op('w_filter',map=entries,list=excluded)])
    for name,entries in itertools.product(['in','out','not','none'],[None,[],[dict(uid='c0',address=None)],[dict(uid='c0',address=b)]]):
        maps={k:[dict(uid='c0',address=a)] for k in ['in','out','not','none']};maps[name]=entries
        add('address',[op('w_address',uid=uid) for uid in ['c0','absent',None]],maps=maps)
    for n,length,kind,clientwalk,maximum in itertools.product([0,1,2,5],[0,7,8,9],['w_in','w_out'],[False,True],[-2147483648,5]):
        path=[address('1.1.1.1',9001+i) for i in range(length)]
        add('routing',[op(kind,info=client('c0','8.8.8.1'),target=1)]*2,clients=clients(n),maps={'in':links(n),'out':links(n),'not':links(2)},walk=walk(path,client=clientwalk),maxNone=maximum,seed=5564)
    for n,target,seed,path in itertools.product([0,1,2,3,8],[-1,0,1,5],[0,1,5564],[None,[],[a]]):
        add('generate',[op('w_generate',target=target)]*4,maps={'in':links(n)},walk=walk(path),seed=seed)
    for path,target,allow,mode in itertools.product([None,[],[a],[own],[a,b,a],[a,address('127.0.0.1'),b]],[0,1,5],[False,True],['normal','pending','missingManager','noMain']):
        cfg={mode:True} if mode in ['missingManager','noMain'] else {}
        cs=[client('p','8.8.8.1',connected=False)] if mode=='pending' else []
        add('returned',[op('w_return',target=target)]*2,walk=walk(path),allow=allow,clients=cs,**cfg)
    for n,maximum,slots,target in itertools.product([0,1,3,6],[-1,0,1,2,5],[-1,0,1,3,100],[None,a,own]):
        add('load',[op('w_load',address=target,slots=slots),op('w_load_return')],maps={'not':links(n)},maxNone=maximum,walk=walk([target],slots=slots))
    for destination,mode,path in itertools.product([None,a,b,own],['normal','missingManager','noMain'],[None,[],[a,own],[own,a,own],[None,a]]):
        cfg={mode:True} if mode!='normal' else {}
        add('next-response',[op('w_next',address=destination),op('w_response')],walk=walk(path),maps={'in':links(2)},clients=clients(2),**cfg)
    for nullstate,nullwalk,fromnull in itertools.product([False,True],repeat=3):
        add('null-receivers',[op('w_in',info=None if fromnull else client('c0','8.8.8.1')),op('w_out',info=None if fromnull else client('c0','8.8.8.1')),op('w_return'),op('w_generate',target=-1),op('w_load_return')],nullState=nullstate,walk=None if nullwalk else walk([a]),maps={'in':links(2),'out':links(2)},clients=clients(2))
    for name,entries in itertools.product(['in','out','not','none'],[None,[dict(uid='c0',address=None)]]):
        maps={k:links(2) for k in ['in','out','not','none']};maps[name]=entries
        add('map-failures',[op('w_in',info=client('c0','8.8.8.1')),op('w_out',info=client('c0','8.8.8.1')),op('w_response')],maps=maps,clients=clients(2),walk=walk([]))
    for my,path in itertools.product([None,own],[None,[],[a],[None],[a,own]]):add('own-null',[op('w_return'),op('w_load_return')],my=my,walk=walk(path),allow=True)
    for allow in [False,True]:
        add('null-after-rejection',[op('w_return')]*2,walk=walk([a,None,address('127.0.0.1')]),allow=allow)
    for secret in [None,'0x','0x0102']:
        add('walk-secret',[op('w_in',info=client('c0','8.8.8.1')),op('w_out',info=client('c0','8.8.8.1')),op('w_generate',target=0)],walk=dict(path=[a],secret=secret),maps={'in':links(2),'out':links(2)},clients=clients(2))
    return rows

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'walk-links-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/P2PFunctionsOracle.java'];listing=build/'walk-links-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();requests=[r for _,r in corpus];expected=execute(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2PFunctionsOracle'],requests);actual=execute([str(build/'minima_p2p_functions')],requests)
    outcomes=set();types=set();bodies=set();returned=set();paths_changed=0
    for i,((group,request),left,right) in enumerate(zip(corpus,expected,actual)):
        if canonical(left)!=canonical(right) or not isinstance(left,list) or len(left)!=len(request['ops']):
            (build/'walk-links-failure.json').write_text(json.dumps(dict(case=i,group=group,request=request,java=left,cpp=right),indent=2)+'\n');raise AssertionError(f'Case {i} ({group}); see {build}/walk-links-failure.json')
        for row in left:
            value=row['result'];outcomes.add(type(value).__name__)
            if value=='ERROR':outcomes.add('ERROR')
            if row['walk']:
                returned.add(row['walk']['returning'])
                if request.get('walk') and row['walk']['path'] is not None and len(row['walk']['path'])!=len(request['walk']['path'] or []):paths_changed+=1
            types.update(msg['type'] for msg in row['posted'])
            for message in value if isinstance(value,list) else [value]:
                if isinstance(message,dict) and 'json' in message:
                    types.add(message['type']);bodies.update(message['json'])
    assert {'walk_links','do_swap'}<=bodies and {'P2P_SEND_MSG','NIO_CONNECT'}<=types,(bodies,types)
    assert {'NoneType','list','dict','ERROR'}<=outcomes and returned=={True,False} and paths_changed>0,(outcomes,returned,paths_changed)
    # The pinned generator deliberately excludes the final key; one key must fail.
    for (group,request),program in zip(corpus,expected):
        if group=='generate' and request['ops'][0]['target']==0:
            n=len(request['maps']['in'])
            if n==1:assert all(row['result']=='ERROR' for row in program)
            if n>1:
                for row in program:
                    if isinstance(row['result'],dict):assert row['result']['uid']!=row['state']['in'][-1]['uid']
    report=dict(passed=True,programs=len(corpus),operations=sum(len(r['ops']) for _,r in corpus),changed_path_observations=paths_changed,groups={g:sum(name==g for name,_ in corpus) for g in sorted({g for g,_ in corpus})},scope='Actual pinned WalkLinksFuncs methods; native peer maps, message codecs and NIOManager connect effects. Exact walk/path snapshots, logs through Main notification fixture, signed slots, separate seeded Java Random streams. Generated load-swap secret bytes canonicalized with length/equality retained. Full logger/Main/P2PManager runtime, typed exception text, concurrent mutation and resource limits remain unverified.')
    (build/'walk-links-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
