#!/usr/bin/env python3
"""Actual pinned MinimaDB coordination; recording SQL is not a storage engine."""
import argparse,itertools,json,re,shutil,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[],**kw)
def project(output,request):
    assert 'ops' in output,output
    for row,command in zip(output['ops'],request['ops']):
        before=row.pop('before');after=row.pop('after');assert after>=before
        row['lock']=re.sub(r'@[0-9a-f]+\[','@<identity>[',row['lock'])
        for event in row['main']:
            if event[0]=='timer':
                assert before+1000<=event[2]<=after+1000,(event,before,after)
                event[2]='<validated-1000ms-deadline>'
        for event in row['events']:
            if event[0]=='prepared.setLong' and event[2]==5:
                assert before<=event[3]<=after,(event,before,after)
                event[3]='<validated-insert-time>'
        if command['cmd']=='restart':
            for event in row['events']:
                if event[0]=='prepared.setLong':
                    assert event[2]==1 and before+100000<=event[3]<=after+100000,(event,before,after)
                    event[3]='<validated-hard-cleanup-deadline>'
    return normalize(output)

def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    add(op('info'),op('create'),op('reset'),op('cascade'),op('mega',null=True),op('cascade',null=True),op('reset'),op('hold'),op('create'),op('useHeld'))
    for base,name in itertools.product(['','/','///','relative','a//b/','/tmp/a/../b'],[None,'','x','/x','///x///','..','π😄','nul\0file']):add(op('path',base=base,name=name))
    for address in [None,'','0xAA','0xaa','π😄','\0']:
        add(op('check',address=address),op('add',address=address),op('add',address=address),op('check',address=address),op('remove',address=address),op('remove',address=address),op('check',address=address))
    for writes,reads,safe in itertools.product(range(4),range(4),range(4)):
        add(*[op('write',lock=True)]*writes,*[op('read',lock=True)]*reads,*[op('safe')]*safe,*[op('read',lock=False)]*reads,*[op('write',lock=False)]*writes)
    add(op('write',lock=True),op('read',lock=True),op('safe',ignore=True),op('write',lock=False),op('read',lock=False),op('safe',ignore=False))
    for cmd in ['saveState','saveUser','saveP2P','saveSQL','save','refresh','reload','restore','load']:
        add(op(cmd),op('info'))
    for encrypted,debug in itertools.product([False,True],[False,True]):
        add(op('load',encrypted=encrypted,debug=debug),op('holdTxn'),op('newTxn',id='fixture'),op('saveState',mega=True),op('save',compact=True),op('reload',wallet=True,encrypted=encrypted),op('restore'),op('create'),op('saveTxn'))
    for allow,cmd in itertools.product([False,True],['saveState','saveUser','saveP2P','save']):add(op('load'),op('allow',allow=allow),op(cmd,mega=True),op('saveSQL',compact=True))
    for cmd in ['load','reload','restore','refresh','saveSQL','save','restart']:
        for fail,runtime in itertools.product(range(1,151),[False,True]):
            prefix=[] if cmd=='load' else [op('load')]
            add(*prefix,op(cmd,fail=fail,runtimeFailure=runtime,wallet=True,debug=True),op('info'))
    for cmd,flag in itertools.product(['load','reload','restore'],['nullConnection','nullStatement','nullPrepared','nullRows']):add(op(cmd,**{flag:True},rescue='fixture:9001'),op('safe'))
    for mega,back,time,entry in itertools.product([False,True],[-1,0,4,5,6],[0,5],[0,1]):
        add(op('mmrAdd',target='global'),op('mmrTime',target='global',time=time),op('mmrTime',time=5),op('mmrClear',mega=mega),op('mmrGet',entry=entry,back=back),op('create'),op('mmrGet',entry=entry,back=back))
    add(op('mmrAdd',target='global'),op('mmrHold'),op('mmrClearMega'),op('mmrWire',target='held'),op('mmrWire',target='global'),op('mmrSet',target='held',amount=7),op('mmrGet',target='global'),op('mmrGet',target='held'))
    add(op('mmrAdd'),op('mmrChild',mega=True),op('mmrGet'),op('mmrGet',entry=2),op('mega',null=True),op('mmrGet'),op('mmrGet',entry=2))
    for mega,start,height,restore in itertools.product([False,True],[1,3],[0,2],['load','restore']):
        add(op('load'),op('chain',start=start,cascadeHeight=height),op('mmrAdd',target='global'),op('mmrTime',target='global',time=height),op('saveState',mega=mega),op('mmrHold'),op(restore,mega=mega),op('mmrWire',target='held'),op('mmrWire',target='global'))
    for height,empty in itertools.product([0,1,2,3],[False,True]):
        add(op('load'),op('chain',start=3,cascadeHeight=2,emptyCascade=empty),op('mmrAdd',target='global'),op('mmrTime',target='global',time=height),op('saveState',mega=True),op('load',mega=True,rescue='fixture:9001'))
    for name,length in itertools.product(['cascade.db','chaintree.db','megammr.mmr','userprefs.db'],[0,1,2,3,4,5,8,12,24,64,128,256]):
        add(op('load'),op('chain'),op('mmrAdd',target='global'),op('saveState',mega=True),op('truncate',name=name,length=length),op('load',mega=True),op('snapshot'))
    return result

def chain_command_cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    def direct(name,**params):return op('command',name=name,params=params)
    add(*(op('command',mode='meta',name=name) for name in ['block','printmmr','printtree']))
    for count in [0,1,3,8]:
        for fork,cascade in itertools.product([False,True],[False,True]):
            chain=op('chain',count=count,emptyCascade=not cascade)
            if fork:chain['parents']=[-1,0,0,1,1,2,2,6][:count]
            commands=[direct('block'),direct('printmmr')]
            for depth in [-2,-1,0,1,2,3,8,32,100]:
                for enabled in ['true','false','TRUE','other']:
                    commands.append(direct('printtree',depth=str(depth),cascade=enabled))
            commands += [op('command',mode='runner',text=text,caller=caller) for text in ['block; printmmr; printtree cascade:true','block bad:x; printtree','printtree depth:2 cascade:true','printmmr bad:x'] for caller in ['0x00','0x1234']]
            add(chain,op('snapshot'),*commands,op('snapshot'))
    for name in ['block','printmmr','printtree']:
        for params in [{},{'depth':''},{'depth':' '},{'depth':None},{'depth':1},{'depth':{}},{'cascade':None},{'cascade':[]},{'unexpected':'value'}]:
            add(op('chain'),op('command',name=name,params=params))
    for depth in ['+2','0002',' 2 ','٢','２','-0','1.0','1e2','0x10','+','--1','2147483648','-2147483649','2147483647','-2147483648']:
        # Extreme valid depths would cause billions of source parent checks.
        add(op('chain',count=0),direct('printtree',depth=depth,cascade='true'))
    for entries in [0,1,2,3,7,16,33]:
        add(op('chain'),*[op('mmrAdd',target='tip',amount=i+1) for i in range(entries)],op('mmrTime',target='tip',time=123),direct('printmmr'),op('snapshot'))
    for time in ['-9223372036854775808','-12219292800001','-12219292800000','-2208988800000','-1','0','1','1709164800123','1.9','-1.9','1709164800123.75','9223372036854775807','9223372036854775808']:
        add(op('chain'),op('tipTime',time=time),direct('block'),direct('printtree',depth='1'))
    add(op('chain'),op('hold'),op('create'),op('useHeld'),direct('block'),direct('printtree'),direct('printmmr'))
    add(op('chain'),op('cascade',null=True),direct('printtree',cascade='false'),direct('printtree',cascade='true'))
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--group',choices=['database','chain-commands','pulse'],default='database');args=parser.parse_args();build=args.build.resolve();oracle=build/'minima-db-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/NIOClientOracle.java',ROOT/'support/MinimaDBOracle.java',ROOT/'support/ArchiveManagerOracle.java'];listing=build/'minima-db-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=driver_calls=programs=0
    from pulse_runtime_corpus import cases as pulse_cases,validate as validate_pulse
    corpus=pulse_cases() if args.group=='pulse' else chain_command_cases() if args.group=='chain-commands' else cases()
    zones=['UTC','Europe/London','America/New_York','Asia/Tokyo'] if args.group=='chain-commands' else ['UTC']
    with tempfile.TemporaryDirectory(prefix='minima-owner-',dir=build) as root:
        for zone in zones:
            # Runner sets a UTC default; env here deliberately overrides it for
            # both actual runtimes without changing the shared process helper.
            prefix=['env','TZ='+zone]
            runners=[Runner(prefix+['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','MinimaDBOracle',root]),Runner(prefix+[str(build/'minima_db'),root])]
            try:
                for index,request in enumerate(corpus):
                    outputs=[]
                    for runner in runners:
                        # Only this fixture's private TemporaryDirectory is reset.
                        for child in Path(root).iterdir():
                            if child.is_dir():shutil.rmtree(child)
                            else:child.unlink()
                        output=project(runner.ask(request),request);output['files']={str(p.relative_to(root)):p.read_bytes().hex() for p in Path(root).rglob('*') if p.is_file()};outputs.append(output)
                    programs+=1
                    if outputs[0]!=outputs[1]:failures.append(dict(case=index,timezone=zone,request=request,java=outputs[0],cpp=outputs[1]));break
                    if args.group=='pulse':validate_pulse(request,outputs[0])
                    operations+=len(request['ops']);driver_calls+=sum(len(row['events']) for row in outputs[0]['ops'])
            finally:
                for runner in runners:runner.close()
            if failures:break
    report=dict(passed=not failures,programs=programs,operations=operations,driver_calls=driver_calls,failures=failures,scope='Actual pinned MinimaDB constructor/getters/identity, paths, notifications, reentrant locks, lifecycle/recovery/shutdown ordering and state-file bytes using actual child implementations and recording SQL. Nonempty chain/cascade restoration, truncated files, sizes, partial state, global MMR lookup and held reset/load references compared. Main callbacks recorded; timer and SQL timestamps verified. Java diagnostic identities, logger prefixes/stacks and enhanced NPE expressions normalized. Native H2 persistence/encryption, full runtime integration and further lifecycle/concurrency cases remain unverified.')
    if args.group=='chain-commands':
        report.update(timezones=zones,planned_programs=len(corpus)*len(zones))
        report['scope']='Actual block/printmmr/printtree command factories, replies, direct/runner errors, global database lookup, chain forks, bounded depths, populated MMR JSON, cascade printing and block date extremes across UTC, London, New York and Tokyo. Database identities/locks/files/SQL/Main effects compared. Enhanced NPE/class-cast text and logger prefixes/stacks normalized. Nonempty trees with extreme int depths, arbitrary concurrent mutation, native H2 and full node remain unverified.'
    if args.group=='pulse':report['scope']='Actual Pulse.createPulse with global MinimaDB and TxPowTree, retained list sharing/replacement, mutable versions and instance reader prefix/offset behavior. Existing real DB locks/identities/state files observed; recording SQL is not an engine. Null MiniData entries, direct byte-array aliases and adversarial concurrent mutation remain open.'
    (build/('pulse-runtime-results.json' if args.group=='pulse' else 'chain-commands-results.json' if args.group=='chain-commands' else 'minima-db-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
