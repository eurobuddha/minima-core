#!/usr/bin/env python3
"""Pinned reset wrapper through the actual runner, plus real H2 restore/reopen."""
import argparse,copy,json,shutil,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA
from restore_differential import snapshot

def corpus():
    cases=[]
    def add(params=None,**kw):cases.append(dict(name='reset',params=dict(archivefile='archive.gz',action='chainsync') if params is None else params,**kw))
    add(op='meta')
    for action in ['chainsync','seedsync','restore']:
        base=dict(archivefile='archive.gz',action=action)
        if action=='seedsync':base['phrase']='fixture phrase'
        if action=='restore':base['file']='input.bak'
        add(base,expected=action)
        for key in ['archivefile','action']+(['phrase','keys','keyuses'] if action=='seedsync' else ['file','password'] if action=='restore' else []):
            for value in [None,False,17,{},[],'','  ','space name','"quoted value"','π😄','a\x00b','a;archive action:import file:second.gz','a" phrase:"b']:
                p=copy.deepcopy(base);p[key]=value;add(p)
            p=copy.deepcopy(base);p.pop(key,None);add(p)
        for reply in [dict(status=False,error='injected failure'),dict(status=True,response={'fixture':'value'}),dict(status=None),dict(status='true'),{},None]:
            add(base,archiveReply=reply)
            if action=='restore':add(base,restoreReply=reply)
        add(base,archiveThrow='fixture child failure')
        add(base,keys=False)
        if action=='restore':
            add(base,restoreThrow='fixture child failure');add(base,syncFail=True)
            add(base,archiveReply=dict(status=False,error='archive failed'),expected='restore_archive_failure')
            for runtime in ['java','cpp']:
                for archive_ok in [True,False]:
                    add(base,realRestore=True,realSync=True,backup=runtime,archiveReply=dict(status=archive_ok,response='archive fixture'),expected='real')
                for password in ['', 'wrong']:
                    add({**base,'password':password},realRestore=True,realSync=True,backup=runtime)
    for action in ['unknown','CHAINSYNC',' restore ','']:
        add(dict(archivefile='archive.gz',action=action))
    for params in [{},{'action':'chainsync'},{'archivefile':'archive.gz'}]:
        add(params);add(params,keys=False)
    add(dict(archivefile='archive.gz',action='seedsync',phrase='fixture phrase',keys='3',keyuses='1024'),expected='seedsync_optional')
    for text in ['reset','reset archivefile:archive.gz action:chainsync','reset archivefile:archive.gz action:seedsync phrase:"fixture phrase" keys:2 keyuses:500','reset archivefile:archive.gz action:restore file:input.bak','reset archivefile:archive.gz action:restore file:input.bak password:wrong','reset archivefile:archive.gz action:chainsync invalid:1','RESET archivefile:archive.gz action:chainsync']:
        add(op='runner',command=text);add(op='runner',command=text,caller='0x01')
    return cases

def project(value):
    result=copy.deepcopy(value)
    if 'exception' in result:
        for name in ['java.lang.NullPointerException','java.lang.ClassCastException']:
            if result['exception'].startswith(name):result['exception']=name
    # Error-restoring embeds a JSON object; compare its structure, not insertion order.
    def visit(x):
        if isinstance(x,dict):return {k:visit(v) for k,v in x.items()}
        if isinstance(x,list):return [visit(v) for v in x]
        if isinstance(x,str) and 'Error restoring.. {' in x:
            prefix,tail=x.split('Error restoring.. ',1)
            try:return {'prefix':prefix+'Error restoring.. ','nested':json.loads(tail)}
            except json.JSONDecodeError:pass
        return x
    return visit(result)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'reset-oracle';oracle.mkdir(exist_ok=True);listing=build/'reset-sources.txt'
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['RestoreOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','RestoreOracle']),Runner([str(build/'restore_runtime')])]
    failures=[];completed=real=0;observations=[]
    try:
        with tempfile.TemporaryDirectory(prefix='reset-compare-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline';baseline.mkdir();assert runners[0].ask(dict(op='seed',role='target',folder=str(baseline))).get('seeded')
            backups={}
            for name,runner in zip(['java','cpp'],runners):
                source=root/('source-'+name);source.mkdir();assert runner.ask(dict(op='seed',role='source',folder=str(source))).get('seeded');backups[name]=(source/'source.bak').read_bytes()
            for i,case in enumerate(corpus()):
                values=[];folder=root/'node'
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    shutil.copytree(baseline,folder);(folder/'input.bak').write_bytes(backups[case.get('backup','java')])
                    request={k:v for k,v in case.items() if k not in ['backup','expected']};request['folder']=str(folder)
                    value=project(runner.ask(request));assert 'fixtureError' not in value,value;values.append(dict(reply=value,files=snapshot(folder)))
                if values[0]!=values[1]:failures.append(dict(case=i,request=case,java=values[0],cpp=values[1]));break
                value=values[0]['reply'];events=value['events'];expected=case.get('expected')
                if expected:
                    assert value['result']['status'] is True,value
                    children=[e for e in events if isinstance(e,dict)]
                    if expected in ['chainsync','seedsync','seedsync_optional']:
                        assert [e['child'] for e in children]==['archive'] and 'readyForSync' not in events,value
                    else:
                        assert [e['child'] for e in children]==['restore','archive'] and events.index('readyForSync')<events.index(children[-1]),value
                        assert children[0]['params']['shutdown']=='false',value
                    if expected=='seedsync_optional':assert children[0]['params']==dict(action='import',file='archive.gz',phrase='fixture phrase',keys='3',keyuses='1024'),value
                    if expected=='restore_archive_failure':assert value['result']['response']['chainsync']['status'] is False,value
                    if expected=='real':
                        assert value['processorCreated'] and value['walletReplaced'] and value['restoring'] and not value['shutdown'],value
                        assert value['keys']['rows']==[{'PUBLICKEY':'0x01','USES':'261'},{'PUBLICKEY':'0x02','USES':'356'}],value
                        assert len(value['history']['rows'])==6 and value['user']['fixture_marker']=='source',value
                        assert value['walletOpen'] and value['historyOpen'] and value['archiveOpen'] and 'restore' not in values[0]['files'],value
                        real+=1
                observations.append(dict(case=i,ready_for_sync='readyForSync' in events,children=[e['child'] for e in events if isinstance(e,dict)],processor_created=value['processorCreated'],exception='exception' in value))
                completed+=1
            if not failures:assert real==4,real
    except Exception as error:failures.append(dict(case=completed,error=repr(error)))
    finally:
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,real_restore_reopen_cases=real,observations=observations,failures=failures,scope='Pinned reset body and actual parser/runner; scoped archive/restore probes for control-flow cases. Four real H2 restores from Java/native backups and actual Main.restoreReadyForSync processor/store reopen. Archive import is recorded, not implemented. Direct NPE/class-cast prose normalized; embedded Error restoring JSON compared structurally. No complete archive/reset or public-network parity claim.')
    (build/'reset-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
