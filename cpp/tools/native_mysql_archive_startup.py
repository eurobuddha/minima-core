#!/usr/bin/env python3
"""Real MySQL archive commands, cross-runtime files, and actual Main resync."""
import argparse,gzip,hashlib,json,os,re,shutil,subprocess,tempfile,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from native_restore_startup import Node,ready,key_state,chain
from native_archive_startup import coins
from peers_checker_differential import Runner
from horst_differential import number,raw

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--fixture',type=Path,required=True);args=ap.parse_args();build=args.build.resolve();state=json.loads(args.fixture.read_text());started=time.monotonic()
    assert state['address'].startswith('127.0.0.1:') and state['database']=='minima_cpp' and state['name'].startswith('minima-cpp-mysql-')
    oracle=build/'mysql-archive-node-oracle';oracle.mkdir(exist_ok=True)
    names=['ArchiveCommandOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java','SqlDBOracle.java','ArchiveManagerOracle.java','ArchiveServerSeed.java']
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in names];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    dependencies=f'{JAVA}/lib/mysql-connector-java-8.0.24.jar:{JAVA}/jar/minima.jar';subprocess.run(['javac','-cp',dependencies,'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{dependencies}';commands=[['java','-cp',cp,'org.minima.Minima'],[str(build/'MinimaCoreC++')]]
    login=' '.join(f'{k}:{v}' for k,v in [('host',state['address']),('database',state['database']),('user',state['user']),('password',state['password'])])
    live=[];runners=[];failures=[];checks=[];artifacts=[];transcripts=[];processes=assertions=0;stage='prepare';runtime='setup'
    def check(ok,label):
        nonlocal assertions
        assertions+=1
        if not ok:raise AssertionError(label)
    def ask(node,text,*,success=True,timeout=300):
        before=time.time();reply=node.ask(text,timeout=timeout);elapsed=time.time()-before
        check(reply['status']==success,dict(command=text,reply=reply));check(not reply.get('pending',False),'command not pending')
        def timings(value):
            if isinstance(value,dict):
                if 'time' in value:
                    match=re.fullmatch(r'0 Years 0 Months 0 Weeks 0 Days 0 Hours 0 Minutes (\d+) Seconds',value['time']);check(bool(match) and int(match[1])<=elapsed+1,'elapsed time format/window')
                for item in value.values():timings(item)
            elif isinstance(value,list):
                for item in value:timings(item)
        timings(reply)
        transcripts.append(dict(runtime=runtime,stage=stage,command=text,reply=reply));return reply
    def mysql(node,params,**kw):return ask(node,'mysql '+params,**kw)
    def sql(node,text):
        reply=ask(node,'mysqlcoins '+login+' action:search query:"'+text+'"')['response'];check(reply['status'],'SQL query succeeds');return reply
    def finish(node,which,quit=True):
        nonlocal processes
        with os.fdopen(os.dup(node.err.fileno()),'r') as errors:
            node.finish(quit=quit);errors.seek(0);stderr=errors.read()
        live.remove(node);processes+=1
        if which==1:check(not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|LeakSanitizer|runtime error:|DEADLYSIGNAL|SUMMARY:',stderr),'complete native stderr free of sanitizer diagnostics')
        checks.append(dict(stage=stage,runtime=['java','cpp'][which],process=processes,graceful_shutdown=True,stderr=stderr))
    def raw_export(node,path,expected,params=''):
        response=mysql(node,'action:rawexport file:'+str(path)+' '+params)['response'];check(path.is_file() and path.stat().st_size>0,'raw file exists');content=path.read_bytes();check(gzip.decompress(content)==expected,'complete decompressed raw archive equals source wire objects');check(response['file']==path.name and response['path']==str(path),'export response points to actual file')
        artifacts.append(dict(runtime=runtime,stage=stage,format='raw',path=str(path),bytes=len(content),sha256=hashlib.sha256(content).hexdigest(),payload_sha256=hashlib.sha256(expected).hexdigest()));return response
    def stored(node,wires,heights,cascade=None):
        rows=sql(node,'SELECT block, HEX(syncdata) AS wire FROM syncblock ORDER BY block')['rows'];check(len(rows)==len(wires),'durable block count')
        for row,wire,height in zip(rows,wires,heights):check(row==dict(block=str(height),wire=wire[2:]),'durable block height and exact serialized bytes')
        cascades=sql(node,'SELECT HEX(fulldata) AS wire FROM cascadedata ORDER BY cascadetip')['rows'];check(cascades==([] if cascade is None else [dict(wire=cascade[2:])]),'durable cascade bytes')
    try:
        with tempfile.TemporaryDirectory(prefix='mysql-archive-node-',dir=build) as scratch:
            root=Path(scratch);baseline=root/'baseline';node=Node(commands[0],baseline,scratch);live.append(node);original_keys=ready(node,7);original_chain=chain(node);original_coins=coins(node);check(original_chain['block']==1,'valid saved genesis baseline');finish(node,0)
            runners=[Runner(['java','-cp',cp,'ArchiveCommandOracle'],response_timeout=120),Runner([str(build/'archive_command_runtime')],response_timeout=120)]
            stage='source archive fixtures';assets={};linked_wires=None;genesis_wires=None;linked_baseline=None
            for which,runner in enumerate(runners):
                assets[which]={}
                for kind in ['genesis','linked']:
                    folder=root/f'{kind}-fixture-{which}';shutil.copytree(baseline/'1.1',folder)
                    q=dict(folder=str(folder),op='seedTree') if kind=='genesis' else dict(folder=str(folder),op='seed',heights=list(range(1,258)),coins=True)
                    seeded=runner.ask(q);check(seeded.get('seeded') is True,seeded)
                    if kind=='genesis':
                        if genesis_wires is None:genesis_wires=seeded['blocks']
                        else:check(seeded['blocks']==genesis_wires,'native/source saved genesis fixture matches')
                    else:
                        if linked_wires is None:linked_wires=seeded['blocks'];linked_baseline=folder
                        else:check(seeded['blocks']==linked_wires,'native/source 257-block fixtures match')
                    for action,suffix in [('export','gzip'),('exportraw','dat')]:
                        output=root/f'{kind}-{which}.{suffix}';reply=runner.ask(dict(folder=str(folder),params=dict(action=action,file=str(output))));check(reply.get('result',{}).get('status') is True,reply);assets[which][kind,suffix]=output
            for kind in ['genesis','linked']:
                for suffix in ['gzip','dat']:check(assets[0][kind,suffix].read_bytes()==assets[1][kind,suffix].read_bytes(),'source/native archive fixture exports identical')
            for runner in runners:runner.close()
            runners=[]
            seeded=subprocess.run(['java','-cp',cp,'ArchiveServerSeed',state['address'],state['database'],state['user'],state['password']],capture_output=True,text=True,timeout=60);check(seeded.returncode==0,seeded.stderr);cascade_fixture=json.loads(seeded.stdout.splitlines()[-1]);cascade=cascade_fixture['cascade'];cascade_wires=cascade_fixture['blocks'];cascade_payload=b'\x01'+raw(cascade)+number(3)+b''.join(map(raw,cascade_wires));cascade_input=root/'cascade-input.dat';cascade_input.write_bytes(gzip.compress(cascade_payload,mtime=0))
            linked_payload=b'\x00'+number(257)+b''.join(map(raw,linked_wires));genesis_payload=b'\x00'+number(1)+raw(genesis_wires[0]);exports={};targets={};observations=[]
            for which in [0,1]:
                runtime=['java','cpp'][which];stage=runtime+' storage';target=root/f'storage-{which}';shutil.copytree(baseline,target);shutil.rmtree(target/'1.1');shutil.copytree(linked_baseline,target/'1.1');targets[which]=target
                node=Node(commands[which],target,scratch);live.append(node);check(key_state(node)==original_keys,'baseline keys retained');mysql(node,login+' action:setlogin');mysql(node,'action:wipe');exports[which]={};summary={}
                summary['update']=mysql(node,'action:update logs:false')['response'];check(summary['update']['mysql']==dict(mysqlstart=2,mysqlend=257,mysqltotal=255),'source empty-MySQL update skips exclusive oldest archive block');stored(node,linked_wires[1:],list(range(2,258)))
                summary['integrity_after_update']=mysql(node,'action:integrity logs:false')['response'];check(summary['integrity_after_update']==dict(start=2,end=257),'integrity traverses multiple live MySQL batches')
                repaired=mysql(node,'action:fixmissing file:'+str(assets[1-which]['linked','dat'])+' logs:false')['response'];check(repaired['missing']==1,'repair adds skipped first block');stored(node,linked_wires,list(range(1,258)))
                sql(node,'DELETE FROM syncblock WHERE block IN (2,256)');repaired=mysql(node,'action:fixmissing file:'+str(assets[1-which]['linked','dat'])+' startfix:1 endfix:2 logs:false')['response'];check(repaired['missing']==2,'source ignores endfix and repairs both missing heights');stored(node,linked_wires,list(range(1,258)))
                summary['info']=mysql(node,'action:info')['response'];check(summary['info']['mysql']==dict(mysqlstart=1,mysqlend=257,mysqltotal=256),'height difference total');check(mysql(node,'action:size')['response']==dict(size=257),'size is actual row count')
                found=mysql(node,'action:findtxpow block:2')['response'];check(found['found'],'find stored block');identifier=sql(node,'SELECT txpowid FROM syncblock WHERE block=2')['rows'][0]['txpowid'];check(mysql(node,'action:findtxpow txpowid:'+identifier)['response']==found,'TxPoW table miss falls back to block ID');check(mysql(node,'action:findtxpow block:99999')['response']==dict(found=False),'missing stored block');summary['found']=found
                history=mysql(node,'action:addresscheck address:0x01 statecheck:0xAABB')['coins'];check([x['block'] for x in history['created']]==list(range(1,258)) and [x['block'] for x in history['spent']]==list(range(1,258)),'address history crosses range boundary in order');summary['history']=history
                ask(node,'mysqlcoins '+login+' action:wipe');coin_reply=ask(node,'mysqlcoins '+login+' action:update maxblocks:257 logs:false')['response'];duration=coin_reply.pop('duration');check(re.fullmatch(r'\d+ seconds',duration) is not None,'coin update duration');check(coin_reply==dict(blocks=257,firstblock=1,lastblock=257,coinsadded=514,message='MySQL CoinsDB Updated..'),'live nonempty coin index update');check(ask(node,'mysqlcoins '+login+' action:info')['response']['total']==514,'all live coin rows persisted');summary['coin_update']=coin_reply
                for maximum in [None,0,-1,1,256]:
                    path=root/f'raw-{which}-{maximum}.dat';limit=257 if maximum is None else maximum;expected=b'\x00'+number(limit)+b''.join(map(raw,linked_wires[:max(1,limit)]));raw_export(node,path,expected,'' if maximum is None else 'maxexport:'+str(maximum))
                    if maximum is None:exports[which]['linked','dat']=path;check(path.read_bytes()==assets[0]['linked','dat'].read_bytes(),'mysql raw exporter matches existing archive exporter bytes')
                h2=root/f'h2-{which}.gzip';mysql(node,'action:h2export file:'+str(h2)+' logs:false');check(h2.is_file() and h2.stat().st_size>0,'real H2 export created');exports[which]['linked','gzip']=h2
                mysql(node,'action:autobackup enable:true');check(mysql(node,'action:info')['response']['autobackup'],'autobackup preference enabled');mysql(node,'action:autobackup enable:false');mysql(node,login+' action:setlogin');check(not mysql(node,'action:info')['response']['autobackup'],'autobackup disabled');check(mysql(node,'host:ignored action:size')['response']==dict(size=257),'partial fresh credentials still use saved login');mysql(node,'host:ignored database:ignored action:size',success=False)
                mysql(node,'action:rawimport file:'+str(cascade_input)+' logs:false');stored(node,cascade_wires,[0,1,2],cascade);cascade_raw=root/f'cascade-{which}.dat';raw_export(node,cascade_raw,cascade_payload);exports[which]['cascade','dat']=cascade_raw
                cascade_h2=root/f'cascade-{which}.gzip';mysql(node,'action:h2export file:'+str(cascade_h2)+' logs:false');check(cascade_h2.is_file(),'cascade H2 export created');exports[which]['cascade','gzip']=cascade_h2
                check(key_state(node)==original_keys,'storage operations do not consume keys');finish(node,which);observations.append(summary);print('Verified '+stage,flush=True)
            check(observations[0]==observations[1],'exact Java/native stable storage replies and complete address histories')
            for which in [0,1]:
                runtime=['java','cpp'][which];stage=runtime+' cross-import';node=Node(commands[which],targets[which],scratch);live.append(node);check(key_state(node)==original_keys,'keys survive storage restart');check(mysql(node,'action:info readonly:true')['response']['logindetails'],'saved login survives real H2 restart')
                for kind in ['linked','cascade']:
                    for suffix in ['dat','gzip']:
                        source=exports[1-which][kind,suffix];action='rawimport' if suffix=='dat' else 'h2import';reply=mysql(node,'action:'+action+' file:'+str(source)+' logs:false')['response'];expected=linked_payload if kind=='linked' else cascade_payload
                        stored(node,linked_wires if kind=='linked' else cascade_wires,list(range(1,258)) if kind=='linked' else [0,1,2],None if kind=='linked' else cascade)
                        if action=='h2import':check(reply['start']==(0 if kind=='linked' else -1) and reply['end']==(257 if kind=='linked' else 2),'H2 import reports exclusive lower start and final height')
                        raw_export(node,root/f'cross-{which}-{kind}-{suffix}.dat',expected)
                        check(not (targets[which]/'1.1/archiverestore').exists(),'temporary H2 restore folder removed')
                mysql(node,'action:clearlogin');mysql(node,'action:size',success=False);mysql(node,login+' action:setlogin');finish(node,which);print('Verified '+stage,flush=True)
            seed_keys=None;clean_phrase=' '.join(['abandon']*23+['art']);messy_phrase='  '+clean_phrase.upper().replace(' ','  ')+'  '
            for mode in ['chain','reset','seed']:
                for which in [0,1]:
                    runtime=['java','cpp'][which];stage=runtime+' '+mode+' resync';target=root/f'resync-{mode}-{which}';shutil.copytree(baseline,target);node=Node(commands[which],target,scratch);live.append(node);check(ready(node,100)=={k:100 for k in original_keys},'resync starts with known key uses');mysql(node,login+' action:setlogin')
                    if mode!='reset':mysql(node,'action:rawimport file:'+str(assets[1-which]['genesis','dat'])+' logs:false');raw_export(node,root/f'before-{mode}-{which}.dat',genesis_payload)
                    params='action:reset file:'+str(assets[1-which]['genesis','dat']) if mode=='reset' else 'action:resync'
                    if mode=='seed':params+=' phrase:"'+messy_phrase+'" keys:8 keyuses:123'
                    result=mysql(node,params,timeout=1200)['response'];synced=result['resync']['response'] if mode=='reset' else result
                    check(synced['start']=='1' and synced['end']=='1','real Main resync reaches saved genesis');check(synced['message']=='Archive sync completed.. shutting down now.. please restart after','source resync completion reply')
                    if mode=='seed':check(synced['phrase']==messy_phrase.strip() and synced['clean']==clean_phrase.upper(),'seed phrase cleaning and reply')
                    if mode=='reset':check(result['import']['status'] and result['resync']['status'],'real nested reset import and resync both succeed')
                    finish(node,which,quit=False)
                    node=Node(commands[1-which],target,scratch);live.append(node);current=key_state(node)
                    if mode=='seed':
                        check(len(current)==8 and set(current.values())=={123},'all replacement keys and use counters restored')
                        if seed_keys is None:seed_keys=current
                        else:check(current==seed_keys,'Java/native replacement seed derives identical public keys')
                        check(set(current)!=set(original_keys),'seed resync replaces original keys')
                    else:check(current=={k:100 for k in original_keys},'chain/reset preserves wallet and uses')
                    check(chain(node)==original_chain and coins(node)==original_coins,'opposite-runtime reopen retains exact chain and coin totals')
                    public=sorted(current)[0];signature=ask(node,'sign publickey:'+public+' data:0x010203')['response'];check(ask(node,'verify publickey:'+public+' data:0x010203 signature:'+signature)['response']=='Signature valid','reopened key signs and verifies');ask(node,'verify publickey:'+public+' data:0x010204 signature:'+signature,success=False);check(key_state(node)=={**current,public:int(current[public])+1},'reopened signing increments only selected key use');finish(node,1-which);print('Verified '+stage,flush=True)
    except Exception as error:failures.append(dict(stage=stage,runtime=runtime,error=repr(error),nodes=[dict(stdout=n.output[-10000:],stderr=n.errors()) for n in live]))
    finally:
        for node in live:node.cancel()
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(cleanup=repr(error)))
    report=dict(passed=not failures,node_processes=processes,independent_assertions=assertions,checks=checks,artifacts=artifacts,transcripts=transcripts,failures=failures,seconds=round(time.monotonic()-started,3),mysql_image=state['image'],scope='Actual pinned Java/native solo noconnect nodes, real H2 and isolated loopback MySQL. Linked 257-block database paging, exclusive update boundary, missing repair/endfix quirk, exact durable block/cascade wires, address histories, nonempty coin index, raw caps and cross-runtime H2/raw imports, saved credentials across restart. Real genesis chain/reset/seed resync with actual Main processors/shutdown, opposite-runtime node reopen and key-use/signature checks. Synthetic 257-block and cascade fixtures are database inputs, not accepted consensus chains. Successful consensus resync here is genesis only; long chains/cascade resync, crash/reconnect/TLS/interruption/concurrency/platform/full-node fidelity remain unverified.')
    (build/'native-mysql-archive-startup-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ['transcripts','artifacts']},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
