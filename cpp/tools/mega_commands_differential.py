#!/usr/bin/env python3
"""Source-derived MegaMMR command/state/file comparisons with real H2."""
import argparse,copy,itertools,json,re,shutil,struct,subprocess,tempfile,time
from datetime import datetime,timezone
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner,Responder
from archive_command_differential import project,snapshot

def coin(i,**kw):return dict(id=f'0x{i:02X}',address='0x01',amount=str(i),**kw)
def spec(**kw):return dict(op='mega',task='command',name='megammr',params={},coins=[coin(1),coin(2,state='0xAA')],time='0',tree=[],cascade=0,blocks=[],addresses=[],publickeys=[],megaEnabled=False,**kw)
def corpus():
    rows=[]
    def add(**kw):
        value=spec();value.update(kw);rows.append(value)
    for name in ['megammr','megammrsync']:
        add(name=name,task='meta')
        for enabled in [False,True]:
            for action in [None,'','unknown','info','mydetails','export','integrity','import']:
                if name=='megammrsync' and action in ['export','integrity','import']:continue
                params={} if action is None else dict(action=action)
                add(name=name,megaEnabled=enabled,params=params)
                for key in ['action','file']:
                    for value in [None,False,1,{},[]]:add(name=name,megaEnabled=enabled,params={**params,key:value})
        for command in [name,name+' action:unknown',name+' action:info bad:1',name+' action:mydetails']:
            add(name=name,task='runner',command=command)
    for tree,coins in itertools.product([[],[[]],[[],[]]],[[],[coin(1)],[coin(1),coin(2)]]):
        add(megaEnabled=True,params=dict(action='export',file='output.mega'),tree=tree,coins=coins)
    for file in ['missing','nul\x00file','directory','directory/nonexistent/file','']:
        for action in ['import','integrity','export']:
            if action=='export' and file=='':continue
            add(megaEnabled=True,params=dict(action=action,file=file),obstruct=True)
    for time,blocks,cascade,bad,twice in itertools.product(['0','1','3'],[[],[1],[1,2],[2,3],[1,3]],[0,1],[False,True],[False,True]):
        add(task='check',time=time,blocks=blocks,cascade=cascade,badCoin=bad,twice=twice)
    trees=[[],[[]],[[coin(1)]],[[coin(1)],[coin(1,spent=True)]],[[coin(3,state='0xAA')],[coin(3)]],[[coin(1),coin(1)]]]
    for tree,enabled,addresses,keys in itertools.product(trees,[False,True],[[],['0x01'],['0xAA'],['0xFF']],[[],['0xAA']]):
        add(task='search',tree=tree,megaEnabled=enabled,addresses=addresses,publickeys=keys)
    for tree,addresses,keys in itertools.product([[],[[]],[[coin(1)]]],[None,[None],[]],[None,[None],[]]):
        add(task='search',tree=tree,megaEnabled=True,addresses=addresses,publickeys=keys)
    for peers in [None,'',',,,','127.0.0.1:9001','127.0.0.1:9001,,127.0.0.1:9001,','bad,127.0.0.2:0,127.0.0.1:-1','::1:9001','127.0.0.1:65536']:
        add(task='peers',peers=peers)
    add(task='details')
    add(task='details',walletRows=True)
    add(name='megammrsync',walletRows=True,params=dict(action='mydetails'))
    for tree in [[],[[]],[[],[]]]:add(task='current',tree=tree)
    for host in ['bad','127.0.0.1:no','127.0.0.1:0']:
        add(name='megammrsync',params=dict(action='resync',host=host))
    return rows

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');parser.add_argument('--start-case',type=int,default=0);args=parser.parse_args();build=args.build.resolve()
    oracle=build/'mega-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['ArchiveCommandOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing=build/'mega-command-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ArchiveCommandOracle']),Runner([str(build/'archive_command_runtime')])]
    failures=[];checks=[];network=[];server=None;stage='commands'
    try:
        with tempfile.TemporaryDirectory(prefix='mega-command-',dir=build) as scratch:
            root=Path(scratch);folder=root/'case'
            def run(runtime,case):
                if folder.exists():shutil.rmtree(folder)
                folder.mkdir()
                if case.get('obstruct'):(folder/'directory').mkdir()
                case=copy.deepcopy(case)
                file=case.get('params',{}).get('file')
                if isinstance(file,str) and '/' in file:case['params']['file']=str(folder/file)
                before=time.time();row=runners[runtime].ask(dict(folder=str(folder),**case));after=time.time();files=snapshot(folder)
                # Default export names encode whole seconds. Bound them before normalization.
                for name in list(files):
                    if re.fullmatch(r'megammr_\d{2}_\d{2}_\d{4}_\d{6}\.megammr',name):
                        stamp=datetime.strptime(name,'megammr_%d_%m_%Y_%H%M%S.megammr').replace(tzinfo=timezone.utc).timestamp()
                        assert before-1<=stamp<=after,(name,before,after)
                        files['<default>']=files.pop(name)
                        if isinstance(row.get('result'),dict) and isinstance(row['result'].get('response'),dict):
                            response=row['result']['response']
                            if 'backup' in response:response['backup']=str(folder/'<default>')
                return project(row,60000),files
            for index,case in enumerate(corpus()):
                if index<args.start_case:continue
                stage=f'case {index}: {case["task"]}'
                left=run(0,case);right=run(1,case)
                if left!=right:
                    failure=dict(case=index,request=case,java=left,cpp=right);(build/'mega-command-failure.json').write_text(json.dumps(failure,indent=2)+'\n');raise AssertionError(f'Case {index} differs; see mega-command-failure.json')
                result=left[0]
                if case['task']=='search' and not case['tree']:assert result.get('coins')==[],result
                if case['task']=='check' and 'weight' in result and case['blocks']:
                    assert result['time']==str(case['blocks'][-1]) and result['finalized'],result
                    if case.get('twice'):assert 'Invalid block number.. not incremental' in result.get('exception',''),result
                if case.get('walletRows'):
                    if case['task']=='details':
                        assert result['wire'][2:14]=='000101000102' and result['wire'].endswith('000103000000011100000001220000000100'),result
                    else:
                        details=result['result']['response'];assert details['size']==2 and [r['publickey'] for r in details['details']]==['0x11','0x22'],details
                checks.append(dict(case=index,task=case['task'],exception=result.get('exception')))
            stage='network';server=Responder()
            encoded=[]
            for runtime in [0,1]:
                case=spec();case.update(task='encode',type=23,blocks=[1],peers='127.0.0.1:9001')
                encoded.append(run(runtime,case)[0])
            assert encoded[0]==encoded[1]
            reply=encoded[0]['wire'];ibd=encoded[0]['ibd']
            # MiniData request frame = length + type22 + SyncData version1 + two empty lists.
            payload=bytes([22])+bytes.fromhex('000101000100000100')
            expected_request=(struct.pack('>i',len(payload))+payload).hex()
            for response in [reply,'0x'+reply[2:10]+'63'+reply[12:],reply[:-2],None,'00000000','ffffffff']:
                observations=[]
                for runtime in [0,1]:
                    server.configure([response]);row=runners[runtime].ask(dict(op='request',folder=str(folder),start='0',host='127.0.0.1',port=server.port,megaRequest=True,addresses=[],publickeys=[]));trace=server.trace()
                    assert trace==[expected_request],trace
                    assert row.get('ibd')==(ibd if response in [reply,'0x'+reply[2:10]+'63'+reply[12:]] else None),row
                    observations.append(row)
                assert observations[0]==observations[1],observations
                row=observations[0];assert row['read']==row['write']==0,row
                assert row['traffic']['writes']=={'sendMegaMMRSyncReq':f'{len(payload)} bytes'},row
                raw=bytes.fromhex(response.removeprefix('0x')) if response else b''
                size=struct.unpack('>i',raw[:4])[0] if len(raw)>=4 else -1
                expected_reads={'sendMegaMMRSyncReq':f'{size} bytes'} if size>=0 and len(raw)==size+4 else {}
                assert row['traffic']['reads']==expected_reads,row
                network.append(dict(reply='drop' if response is None else response[:24],success=observations[0]['ibd'] is not None))
            for flag in ['noMain','noNetwork','noNIO','nullData']:
                observed=[]
                for runtime in [0,1]:
                    server.configure([reply]);row=runners[runtime].ask(dict(op='request',folder=str(folder),start='0',host='127.0.0.1',port=server.port,megaRequest=True,addresses=[],publickeys=[],**{flag:True}));trace=server.trace()
                    assert trace==([] if flag=='nullData' else [expected_request]),trace
                    assert row['ibd'] is None and row['traffic']=={'reads':{},'writes':{}},row
                    observed.append(row)
                assert observed[0]==observed[1],observed
                network.append(dict(flag=flag,success=False))
    except Exception as error:failures.append(dict(stage=stage,error=repr(error)))
    finally:
        if server:server.close()
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=len(checks),network_cases=len(network),checks=checks,network=network,failures=failures,scope='Real H2 command metadata/gates/exports, file effects, mutable MegaMMR integrity and synthetic tree coin selection, peer files, default Main MegaMMR callback and loopback request decoding. Synthetic blocks are not live consensus evidence. Live import/resync, nonempty wallet/proof recovery, delayed timeouts and full parity remain unverified.')
    (build/'mega-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ['checks','network']},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
