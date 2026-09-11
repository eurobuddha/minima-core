#!/usr/bin/env python3
"""Actual TxBody ID lists and TxPoWTreeNode consumer mutation schedules."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
CME='java.util.ConcurrentModificationException';END='java.util.NoSuchElementException'
NPE='java.lang.NullPointerException';INDEX='java.lang.IndexOutOfBoundsException'
def op(name,**kw):return dict(op=name,**kw)
def cases():
    result=[]
    def add(q,expected=None):result.append((q,expected))
    samples=[[],['0x01'],['0x01','0x02'],['0x01',None,'0x03'],['0x01','0x01','0x02'],[None],['0x01','0x02','0x03','0x04']]
    for initial in samples:
        mutations=[op('add',value='0x99'),op('add',value=None),op('clear'),op('remove',value='0x01'),op('remove',value=None),op('remove',value='0xFF'),op('replace'),op('release')]
        for index,value in itertools.product([-1,0,len(initial)-1,len(initial),2147483647],[None,'0x99']):mutations.append(op('set',index=index,value=value))
        for consumed,mutation in itertools.product(range(len(initial)+2),mutations):
            steps=[op('iter')]+[op('next')]*consumed+[mutation,op('has'),op('next'),op('has'),op('next'),op('sameList')]
            if mutation['op']!='release':steps.append(op('serial'))
            add(dict(kind='collection',initial=initial,ops=steps))
        for index in [-1,0,len(initial)-1,len(initial),2147483647]:add(dict(kind='collection',initial=initial,ops=[op('get',index=index)]))
    explicit=[
        ([],[op('iter'),op('clear'),op('has'),op('next')],['ok','ok',False,CME]),
        ([],[op('iter'),op('has'),op('next')],['ok',False,END]),
        (['0x01','0x02'],[op('iter'),op('set',index=1,value='0x99'),op('next'),op('next'),op('next'),op('serial')],['ok','0x02','0x01','0x99',END,['0x01','0x99']]),
        (['0x01'],[op('iter'),op('release'),op('has'),op('next'),op('has'),op('next')],['ok','ok',True,'0x01',False,END]),
        (['0x01'],[op('retain',index=0),op('addHeld'),op('iter'),op('next'),op('mutate'),op('next'),op('held'),op('serial')],['ok','ok','ok','0x01','ok','0x99','0x99',['0x99','0x99']]),
        (['0x01'],[op('retain',index=0),op('iter'),op('set',index=0,value='0xAA'),op('held'),op('next')],['ok','ok','0x01','0x01','0xAA']),
    ]
    for initial,steps,expected in explicit:add(dict(kind='collection',initial=initial,ops=steps),expected)
    actions=[dict(action=a) for a in ['none','add','clear','replace','clearOwner']]
    actions += [dict(action='set',index=i,value=v) for i,v in itertools.product([-1,0,1,9],[None,'0x99'])]
    actions += [dict(action='remove',value=v) for v in [None,'0x01','0x02','0xFF']]
    actions += [dict(action='mutate',index=i) for i in [0,1,9]]
    for initial in samples:
        for trigger,stop,action in itertools.product(range(1,len(initial)+2),[False,True],actions):add(dict(kind='consumer',initial=initial,trigger=trigger,stop=stop,**action))
    # Hand-derived boundary cases also verify each runtime independently. The EE
    # insertion before a second call distinguishes cached success from failure.
    def boundary(initial,action,first,calls,held,second,again,body=None,trigger=1,stop=False,**kw):
        current=held if body is None else body
        expected=dict(first=first,calls=calls,heldList=held,bodyList=current,hasBody=True,beforeSecond=current+['0xEE'],second=second,again=again)
        add(dict(kind='consumer',initial=initial,action=action,trigger=trigger,stop=stop,**kw),expected)
    boundary(['0x01','0x02'],'add',CME,['0x01'],['0x01','0x02','0x99'],False,['0x01'])
    boundary(['0x01','0x02'],'clear',CME,['0x01'],[],False,['0xEE'])
    boundary(['0x01','0x02'],'remove',True,['0x01'],['0x01'],True,[],value='0x02')
    boundary(['0x01','0x02'],'remove',True,['0x01'],['0x02'],True,[],value='0x01')
    boundary(['0x01','0x02'],'set',NPE,['0x01'],['0x01',None],False,['0x01'],index=1,value=None)
    boundary(['0x01','0x02'],'set',True,['0x01','0x99'],['0x01','0x99'],True,[],index=1,value='0x99')
    boundary(['0x01','0x02'],'set',INDEX,['0x01'],['0x01','0x02'],False,['0x01'],index=9,value='0x99')
    boundary(['0x01','0x02'],'add',False,['0x01'],['0x01','0x02','0x99'],False,['0x01'],stop=True)
    boundary(['0x01','0x02'],'replace',True,['0x01','0x02'],['0x01','0x02'],True,[],body=['0xAA'])
    boundary([],'none',True,[],[],True,[])
    boundary([None],'none',NPE,[],[None],NPE,[])
    boundary(['0x01'],'remove',CME,['0x01'],[],False,['0xEE'],value='0x01')
    boundary(['0x01','0x02'],'add',CME,['0x01','0x02'],['0x01','0x02','0x99'],False,['0x01'],trigger=2)
    boundary(['0x01','0x02'],'add',True,['0x01','0x02'],['0x01','0x02'],True,[],trigger=3)
    boundary(['0x01','0x02'],'clear',False,['0x01'],[],False,['0xEE'],stop=True)
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic()
    oracle=build/'txbody-ids-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/TxBodyIDsOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    corpus=cases();runners=[];failures=[];executed=assertions=0;transcript=hashlib.sha256()
    try:
        runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TxBodyIDsOracle']))
        runners.append(Runner([str(build/'txbody_ids')]))
        for index,(request,expected) in enumerate(corpus):
            outputs=[runner.ask(request) for runner in runners];executed+=1
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]))
            if expected is not None:
                for runtime,value in enumerate(outputs):
                    assertions+=1
                    if value!=expected:failures.append(dict(case=index,runtime=runtime,request=request,expected=expected,actual=value))
            transcript.update(json.dumps([request,*outputs],sort_keys=True,separators=(',',':')).encode())
    except Exception as error:failures.append(dict(phase='execution',case=executed,error=repr(error)))
    finally:
        for runtime,runner in enumerate(runners):
            with os.fdopen(os.dup(runner.err.fileno()),'r') as errors:
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
                errors.seek(0);stderr=errors.read()
                if stderr:failures.append(dict(phase='stderr',runtime=runtime,stderr=stderr))
    if executed!=len(corpus):failures.append(dict(phase='accounting',executed=executed,planned=len(corpus)))
    report=dict(passed=not failures,cases=executed,planned_cases=len(corpus),independent_assertions=assertions,collection_programs=sum(q['kind']=='collection' for q,e in corpus),consumer_programs=sum(q['kind']=='consumer' for q,e in corpus),seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual TxPoW body ID lists: nullable and duplicate entries, get/set/clear/add/remove-by-value, held MiniData mutation, retained iterator after list replacement and body release, iterator hasNext/next before/between/after mutations. Actual TxPoWTreeNode.checkFullTxns with controlled TxPoWDB exists callback: structural and nonstructural changes, early false return, null entry order, retained original list, and cached-success behavior after an extra ID is added. Hand-derived expectations independently check 21 boundary programs in each runtime. Not a complete ArrayList inherited API or multithreaded schedule certification.')
    (build/'txbody-ids-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
