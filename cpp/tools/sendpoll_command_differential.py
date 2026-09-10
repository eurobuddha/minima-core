#!/usr/bin/env python3
"""Compare the pinned command and actual queue/worker bodies without starting a node."""
import argparse,copy,itertools,json,os,random,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def corpus():
    cases=[]
    def direct(text='sendpoll address:0x12 amount:1',params=None):return dict(op='direct',text=text,params=params or {})
    def add(*ops,**flags):cases.append(dict(ops=list(ops),**flags))
    add(dict(op='meta'))
    for action,text in itertools.product(['add','list','remove','unknown','ADD','',None,False,1,{},[]],['sendpoll','sendpoll action:add','SENDPOLL amount:1','sendpollsendpoll','',' sendpoll ',None]):
        for main in [{},{'missing':True},{'nullManager':True}]:add(dict(op='main',**main),direct(text,dict(action=action)))
    add(dict(op='direct'),expectEmpty=True)
    for uid in [None,False,1,{},[],'','0x00','missing']:
        add(direct(),direct(params=dict(action='remove',uid=uid)))
    for text in ['sendpoll','SENDPOLL','sendpoll address:0x12 amount:1','SENDPOLL address:0x12 amount:1','sendpoll action:list','sendpoll action:unknown','sendpoll action:remove','sendpoll invalid:true','sendpoll coinage:0','sendpoll action:add;sendpoll action:list','sendpoll action:list;sendpoll action:unknown']:
        for caller in ['0x00','0x1234']:add(dict(op='run',text=text,caller=caller))
    add(direct('prefix sendpoll suffix sendpoll'),expectRewrite=True)
    add(direct(),dict(op='hold'),direct(),dict(op='direct',removeIndex=0),dict(op='direct',removeIndex=0),direct(params=dict(action='list')),expectSnapshot=True)
    add(direct('hash data:0x12'),direct('not_a_minima_command'),dict(op='hold'),dict(op='poll'),expectPoll=True)
    add(direct('hash data:0x12'),dict(op='stop'),dict(op='poll'),expectStopped=True)
    rng=random.Random(5564)
    for _ in range(100):
        ops=[direct(),dict(op='hold')]
        for index in range(25):
            kind=rng.choice(['add','list','remove','unknown','heldRemove'])
            if kind=='heldRemove':ops.append(dict(op='direct',removeIndex=0))
            else:ops.append(direct(rng.choice(['sendpoll amount:1','sendpoll amount:2','SENDPOLL amount:1','other sendpoll sendpoll']),dict(action=kind,uid='absent')))
        add(*ops)
    return cases

def normalize(output):
    ids={}
    for op in output['ops']:
        for item in op['queue']+op['held']:
            uid=item['uid'];assert re.fullmatch(r'0x[0-9A-F]{32}',uid),item
            ids.setdefault(uid,f'<UID:{len(ids)}>')
        assert len({x['uid'] for x in op['queue']})==len(op['queue']),op
    def visit(value):
        if isinstance(value,list):return [visit(x) for x in value]
        if isinstance(value,dict):return {k:visit(v) for k,v in value.items()}
        if isinstance(value,str):
            if value in ids:return ids[value]
            for exception in ['java.lang.NullPointerException','java.lang.ClassCastException']:
                if value.startswith(exception):return exception
        return value
    return visit(copy.deepcopy(output))

def validate(case,output):
    final=output['ops'][-1]
    if case.get('expectEmpty'):assert final['queue'][0]['command']=='' and final['value']['status'] is True,final
    if case.get('expectRewrite'):assert final['queue'][0]['command']=='prefix send suffix sendpoll',final
    if case.get('expectSnapshot'):
        assert len(final['queue'])==1 and len(final['held'])==1 and final['queue'][0]['uid']!=final['held'][0]['uid'],final
        assert final['value']['response']['commands']==final['queue'] and final['value']['response']['total']==1,final
    if case.get('expectPoll'):
        assert len(final['queue'])==1 and final['queue'][0]['command']=='not_a_minima_command' and len(final['held'])==2,final
        assert final['timers']==[dict(type='SENDPOLL_FUNCTION',delay=30000)],final
    if case.get('expectStopped'):assert len(final['queue'])==1 and len(final['timers'])==1,final

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'sendpoll-command-oracle';oracle.mkdir(exist_ok=True)
    listing=build/'sendpoll-command-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SendPollCommandOracle.java'])+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cases=corpus();payload='\n'.join(json.dumps(c) for c in cases)+'\n'
    def run(command):
        process=subprocess.run(command,input=payload,capture_output=True,text=True,timeout=150,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=process.stdout.splitlines()
        if process.returncode or len(lines)!=len(cases):
            (build/'sendpoll-command-crash.txt').write_text(process.stderr+'\n'+process.stdout);raise RuntimeError(f'exit {process.returncode}, {len(lines)} lines, expected {len(cases)}')
        return [json.loads(line) for line in lines]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SendPollCommandOracle']);actual=run([str(build/'minima_sendpoll_command')]);failures=[]
    for case,j,n in zip(cases,expected,actual):
        validate(case,j);validate(case,n)
        if normalize(j)!=normalize(n):failures.append(dict(case=case,java=j,cpp=n))
    report=dict(programs=len(cases),operations=sum(len(c['ops']) for c in cases),passed=len(cases)-len(failures),failures=failures,scope='Actual sendpoll metadata, command dispatch, rewrite/error order, queue snapshots and removal, successful/failed actual CommandRunner polling, stopped worker scheduling; 16-byte UID shape and identity checked before stable normalization; NPE/CCE prose normalized; background timer delivery and logger output excluded')
    (build/'sendpoll-command-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report|{'failures':failures[:3]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
