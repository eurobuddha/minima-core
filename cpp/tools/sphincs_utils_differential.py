#!/usr/bin/env python3
"""SPHINCS transaction messages, partial state writes and actual KISS VM integration."""
import argparse,hashlib,json,re,subprocess,time
from pathlib import Path
from horst_differential import ROOT,JAVA,hx,raw,number,data
from fors_differential import ForestRunner
from sphincs_differential import proof

def sha(b):return hashlib.sha3_256(b).digest()
def hexnum(n):s=f'{n:X}';return '0x'+('0' if len(s)%2 else '')+s
token=hx(sha(data(data(b'\xff')+data(b'RETURN TRUE')+number(3)+b'\x03\x01\x01'+data(b'Test')+number(0))))
def message(q):
    ins=''.join('0x01' if q.get('eltoo') and i==0 else hexnum(n+1) for i,n in enumerate(q['inputs']))
    outs=''.join('0x001122SPHINCS'+('125' if n%2 else '0.125')+'SPHINCS'+(token if n%2 else '0x00')+('FALSE' if n%2 else 'TRUE') for n in q['outputs'])
    return f"{len(q['inputs'])}SPHINCS{len(q['outputs'])}SPHINCS{ins}COINJOIN{outs}".encode()
def states(j):
    w=j['WOTSsignature']['signatures'];wots=number(len(w))+b''.join(data(raw(s['publickey']))+data(raw(s['signature']))+proof(s['proof']) for s in w)
    result={100:hx(wots),101:j['FORSroot']['data']};f=j['FORSsignature']
    for i in range(16):
        result[i*5]=f['horstroots'][i]['horstroot']['data'];result[i*5+1]=f['horstroots'][i]['horstproof'];result[i*5+2]=f['horstsignature']['values'][i]['sigvalue'];result[i*5+3]=f['horstsignature']['values'][i]['sigproof']
    return result

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,default=ROOT/'build');p.add_argument('--replay',type=Path,help='Replay an existing actual full-signature transaction vector; do not regenerate its forest');args=p.parse_args();build=args.build.resolve();oracle=build/'sphincs-utils-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/s for s in ['ObjectOracle.java','HORSTOracle.java','FORSOracle.java','SPHINCSUtilsOracle.java']]
    listing=oracle/'sources.txt';listing.write_text('\n'.join(map(str,sources))+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-XX:-OmitStackTraceInFastThrow','-Xmx2g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SPHINCSUtilsOracle'],[str(build/'minima_sphincs_utils')]]
    runners=[ForestRunner(c,response_timeout=1800) for c in commands];cases=0;checks=0;failures=[];groups={};transcript=hashlib.sha256();start=time.monotonic()
    vectorpath=ROOT/'validation/sphincs-utils-work-in-progress/sphincs-input-vector.json';vector=json.loads(vectorpath.read_text());assert vector['java']==vector['cpp'];signature=vector['java'];expected=states(vector['json'])
    def check(condition,detail):
        nonlocal checks
        checks+=1
        if not condition:raise AssertionError(detail)
    def compare(group,q):
        nonlocal cases
        values=[r.ask(q) for r in runners];cases+=1;groups[group]=groups.get(group,0)+1;transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
        if values[0]!=values[1]:failures.append(dict(index=cases,group=group,request=q,java=values[0],cpp=values[1]))
        (build/('sphincs-utils-replay-progress.json' if args.replay else 'sphincs-utils-progress.json')).write_text(json.dumps(dict(cases=cases,groups=groups,failures=failures,independent_assertions=checks),indent=2)+'\n');return values
    def setup(q,updates=66,failed=False):
        values=compare('setup-partial' if failed else 'setup-complete',q)
        for v in values:
            actual={s['port']:s['data'] for s in v['transaction']['state']};want={i:'[old]' for i in range(80)} if q.get('oldState') else {}
            if q.get('oldState'):want.update({100:'[old]',101:'[old]',200:'[keep]'})
            want.update(dict(list(expected.items())[:updates]));check(actual==want,('partial state',q,actual,want))
            check(('error' in v)==failed,('failure outcome',q,v))
            if failed:check(v['transaction']['cachedID']=='0x00','cached ID unchanged after failed setup')
            else:
                check(v['transaction']['cachedID']==hx(sha(raw(v['transaction']['wire']))),'ordinary transaction hash')
                ids=[]
                for i,n in enumerate(q['outputs']):
                    if not q['inputs']:ids.append(hexnum(n+1))
                    elif q.get('eltoo') or q['inputs'][0]==0:ids.append('0x00')
                    else:ids.append(hx(sha(data(raw(hexnum(q['inputs'][0]+1)))+number(i))))
                check(v['transaction']['outputIDs']==ids,'precomputed output IDs')
        return values
    try:
        source=(JAVA/'src/org/minima/utils/sphincs/SPHINCSUtils.java').read_text();script=re.search(r'KISSVM_SPHINCS_SCRIPT = "(.*)";',source)[1]
        for pub in ['0x','0x00','0x1234',vector['public'],None]:
            values=compare('script',dict(action='script',public=pub))
            for v in values:
                check(v['constant']==script,'literal source script')
                if pub is not None:check(v['script']==script.replace('#USER_PUBLIC_KEY','' if pub=='0x' else pub),'exact public-key replacement')
        for ins in [[],[0],[1],[7,0],[0,1,2,3,4]]:
            for outs in [[],[0],[1],[3,2,1,0],list(range(8))]:
                for eltoo in [False,True]:
                    q=dict(action='message',inputs=ins,outputs=outs,eltoo=eltoo);values=compare('message',q)
                    for v in values:
                        if ins:check(v.get('message')==hx(message(q)),('independent unhashed message',q,v))
                        else:check(v.get('error',{}).get('exception')=='java.lang.IndexOutOfBoundsException','zero input failure')
        compare('null-transaction',dict(action='message',nullTransaction=True,inputs=[],outputs=[]))
        for ins in [[],[0],[7,0]]:
            for outs in [[],[0,1,2]]:
                for eltoo in [False,True]:
                    for old in [False,True]:setup(dict(action='setup',wire=signature,inputs=ins,outputs=outs,eltoo=eltoo,oldState=old))
        base=dict(action='setup',wire=signature,inputs=[7,0],outputs=[0,1],oldState=True)
        for i in range(16):
            for field,step in [('roots',0),('outerProofs',1),('values',2),('proofs',3),('nullRoot',0),('nullOuterProof',1),('nullValue',2),('nullProof',3),('oversizeOuterProof',1),('oversizeProof',3)]:
                values=setup(dict(base,mutation={field:i}),updates=2+4*i+step,failed=True)
                for v in values:check(v['logCount']==(1 if field.startswith('oversize') else 0),'conversion I/O log count')
        for mask in range(1,8):
            updates=0 if mask&1 else 1 if mask&2 else 2;setup(dict(base,nulls=mask),updates=updates,failed=True)
        setup(dict(base,wire=None),updates=0,failed=True)
        compare('null-setup-transaction',dict(base,nullTransaction=True));compare('null-both',dict(base,nullTransaction=True,wire=None))
        q=dict(action='signSetup',seed='0x0011223344',inputs=[0,1],outputs=[0,1],oldState=True)
        if args.replay:
            saved=json.loads(args.replay.read_text());check(saved['request']==q,'replay vector request');values=[saved['java'],saved['cpp']]
            check(values[0]['signature']==values[1]['signature'],'saved actual writers agree')
            print(json.dumps(dict(progress='replaying actual full-size transaction signatures',cases=cases,failures=len(failures))),flush=True)
        else:
            print(json.dumps(dict(progress='utility comparisons complete; signing actual transaction',cases=cases,failures=len(failures))),flush=True)
            values=compare('full-sign-setup-script',q)
            for v in values:check(v['message']==hx(message(q)),'actual signed transaction message');check(v['contract']['valid'],'actual full KISS VM signature acceptance')
            (build/'sphincs-transaction-vector.json').write_text(json.dumps(dict(request=q,java=values[0],cpp=values[1]),indent=2)+'\n')
        for writer in values:
            q=dict(action='run',wire=writer['signature'],public=writer['public'],inputs=[0,1],outputs=[0,1],oldState=True)
            for change in [{},{'changeInput':True},{'changeOutput':True},{'changeState':100},{'changeState':101},{'changeState':0},{'changeState':78},{'changeState':79},{'changeState':200}]:
                for v in compare('cross-writer-script',dict(q,**change)):check(v['contract']['valid']==(not change or change.get('changeState') in [79,200]),('script mutation',change,v['contract']))
        check(not runners[1].errors(),'native stderr empty')
    finally:
        for runner in runners:runner.close()
    report=dict(performed_full_signing=not bool(args.replay),replayed_vector_sha256=hashlib.sha256(args.replay.read_bytes()).hexdigest() if args.replay else None,cases=cases,passed=cases-len(failures),independent_assertions=checks,groups=groups,failures=failures,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),input_vector_sha256=hashlib.sha256(vectorpath.read_bytes()).hexdigest(),scope='Pinned SPHINCSUtils script, message and partial transaction state, actual full-size signatures and KISS VM acceptance/mutations. Exact stored state, cached transaction IDs, coin IDs, source exception types/log counts. NPE/EOF prose normalized. Raw aliases, null Coin members/list entries, arbitrary subclasses/concurrency and full node parity remain unverified.')
    (build/('sphincs-utils-replay-results.json' if args.replay else 'sphincs-utils-differential-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
