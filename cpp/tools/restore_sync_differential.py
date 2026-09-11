#!/usr/bin/env python3
"""Restore-sync six-section restoration, age gates and partial effects with real H2."""
import argparse,json,shutil,subprocess,tempfile
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA
from restore_differential import corpus,project,snapshot

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'restoresync-oracle';oracle.mkdir(exist_ok=True);listing=build/'restoresync-sources.txt';sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['RestoreOracle.java','DecryptBackupOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{JAVA}/jar/minima.jar';runners=[Runner(['java','-cp',cp,'RestoreOracle']),Runner([str(build/'restore_runtime')])];crypto=Runner(['java','-cp',cp,'DecryptBackupOracle'])
    failures=[];completed=successes=partial=0;observations=[]
    try:
        with tempfile.TemporaryDirectory(prefix='restoresync-compare-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline';baseline.mkdir();assert runners[0].ask(dict(op='seed',role='target',folder=str(baseline))).get('seeded')
            sources=[]
            for i,runner in enumerate(runners):
                source=root/f'source-{i}';source.mkdir();assert runner.ask(dict(op='seed',role='source',folder=str(source),treeTime='4102444800000')).get('seeded')
                sources.append((source/'source.bak').read_bytes())
            source=root/'source-0';answer=crypto.ask(dict(folder=str(source),params=dict(file='source.bak',output='plain.bin')));assert 'result' in answer,answer;plain=(source/'plain.bin').read_bytes()
            old=root/'old-source';old.mkdir();assert runners[0].ask(dict(op='seed',role='source',folder=str(old),treeTime='0')).get('seeded');oldbackup=(old/'source.bak').read_bytes()
            empty=root/'empty-source';empty.mkdir();assert runners[0].ask(dict(op='seed',role='source',folder=str(empty))).get('seeded');emptybackup=(empty/'source.bak').read_bytes()
            cases=corpus(*sources,plain,crypto)
            for case in cases:
                if case.get('command'):case['command']=case['command'].replace('restore','restoresync',1)
            for value in [None,False,1,{},[],'','bad','0','-1','1','256','1.9','4294967296']:
                cases.append(dict(data=sources[0],params=dict(file='input.bak',keyuses=value),label='keyuses'))
            for host in [None,False,1,{},[],'','bad','localhost:no']:
                cases.append(dict(data=sources[0],params=dict(file='input.bak',host=host),expected=True,label='recent-host-ignored'))
                cases.append(dict(data=oldbackup,params=dict(file='input.bak',host=host),label='old-host'))
            cases.extend([dict(data=oldbackup,params=dict(file='input.bak'),expected=True,label='old-without-host'),dict(data=emptybackup,params=dict(file='input.bak'),label='empty-tree'),dict(data=sources[0],params=dict(file='nul\x00file'),label='nul-filename'),dict(data=sources[0],params=dict(file='input.bak'),syncFail=True,label='reopen-failure')])
            for host in [None,'','bad','localhost:no']:
                for start in [None,'0','-1']:cases.append(dict(data=sources[0],params={},op='performResync',host=host,start=start,label='perform-invalid-host'))
            for command in ['restoresync file:input.bak','restoresync file:input.bak keyuses:10','restoresync file:input.bak host:bad','restoresync file:input.bak shutdown:false']:
                for caller in ['0x00','0x01']:cases.append(dict(data=sources[0],params={},op='runner',command=command,caller=caller,label='runner'))
            for index,case in enumerate(cases):
                values=[];folder=root/'node'
                for runner in runners:
                    if folder.exists():shutil.rmtree(folder)
                    shutil.copytree(baseline,folder);(folder/'input.bak').write_bytes(case['data'])
                    directory=case.get('directory')
                    if directory:
                        path=folder/('directory' if directory=='input' else 'restore' if directory=='restore' else 'restore/wallet.sql' if directory=='wallet.sql' else 'databases/'+directory)
                        if path.is_file():path.unlink()
                        path.mkdir(parents=True,exist_ok=True);(path/'child').write_text('fixture child')
                    request={k:v for k,v in case.items() if k in ['op','params','keys','readyFail','realReady','command','caller','syncFail','host','start']};request.update(folder=str(folder),name='restoresync',realSync=True)
                    value=project(runner.ask(request));assert 'fixtureError' not in value,value
                    values.append(dict(reply=value,files=snapshot(folder)))
                if values[0]!=values[1]:failures.append(dict(case=index,label=case.get('label'),params=case['params'],java=values[0],cpp=values[1]));break
                result=values[0]['reply'];successful='result' in result and case.get('op')!='runner'
                if case.get('expected'):
                    assert successful,result
                    assert result['user']['fixture_marker']=='source' and result['user']['minidapps_uninstalled']==[],result
                    assert result['keys']['rows']==[{'PUBLICKEY':'0x01','USES':'261'},{'PUBLICKEY':'0x02','USES':'356'}],result['keys']
                    rows=result['history']['rows'];assert sum(r['ISRELEVANT']=='1' for r in rows)==6 and all(r['ISRELEVANT']=='1' or r['TXPOWID']==result['tipID'] for r in rows),result['history']
                    assert result['allowSave'] and result['archiveFile'] and 'restore' not in values[0]['files'],result
                    assert result['processorCreated'] and result['walletReplaced'] and result['shutdown'] and not result['published'],result
                if successful:successes+=1
                if not result['allowSave']:partial+=1
                observations.append(dict(case=index,label=case.get('label'),success=successful,allow_save=result['allowSave'],restoring=result['restoring'],temporary_folder='restore' in values[0]['files']))
                completed+=1
            if not failures:assert successes>=8 and partial>=15,(successes,partial)
    except Exception as error:failures.append(dict(case=completed,error=repr(error)))
    finally:
        for runner in [*runners,crypto]:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=completed,successful_restores=successes,save_disabled_cases=partial,observations=observations,failures=failures,scope='Pinned restoresync and native full class: real H2 six-section restore/reopen, old/future/empty trees, source key-use behavior and runner rejection, malformed backup partial effects, invalid performResync hosts, real processor final shutdown with fixture MDS already stopped. EOF/NPE/class-cast diagnostic prose normalized. Successful remote synchronization, exact age-boundary timing, memory reset and full parity require separate evidence.')
    (build/'restoresync-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
