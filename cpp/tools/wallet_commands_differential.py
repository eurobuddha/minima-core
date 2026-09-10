#!/usr/bin/env python3
"""Actual script/sign commands through actual Wallet, with recording JDBC only."""
import argparse,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from sql_db_differential import normalize

def corpus():
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(operation='run',**kw):return dict(op=operation,rows=[row],probe=['0x01','0x00'],**kw)
    def command(text,**kw):
        operation=op(text=text,caller=kw.pop('caller','0x00'));operation.update(kw);return operation
    def add(*ops):cases.append(dict(ops=[op('load'),*ops]))
    for name in ['scripts','newscript','removescript','sign']:add(op('meta',name=name))
    examples=['scripts','scripts address:0x01','scripts address:0x0001','scripts address:0xnothex','newscript script:"RETURN TRUE" trackall:true','newscript script:"return  true" trackall:false clean:true','removescript address:0x01','sign data:0xFF publickey:0x01']
    for text,fail,runtime in itertools.product(examples,range(32),[False,True]):
        add(command(text,fail=fail,runtimeFailure=runtime),command('scripts'))
    for text in examples:
        for flag in ['nullRows']:
            add(command(text,**{flag:True}))
        for rows in [[],[row,row],[{**row,'simple':0,'track':0}]]:add(command(text,rows=rows))
        for caller in ['0x00','0x1234']:
            add(command(text+';scripts',caller=caller),command(text+' bad:value;scripts',caller=caller))
    for clean,track,script in itertools.product(['true','false','TRUE'],['true','false','TRUE'],['RETURN TRUE',' return    true ','RETURN 0xabcdef EQ 0xABCDEF','RETURN [hello  world] EQ [hello world]']):
        add(command(f'newscript script:"{script}" trackall:{track} clean:{clean}'),command('scripts'))
    for name,params in [('newscript',{}),('newscript',{'script':'RETURN TRUE'}),('newscript',{'script':'','trackall':'true'}),('newscript',{'script':'RETURN TRUE','trackall':''}),('scripts',{'address':''}),('removescript',{}),('sign',{}),('sign',{'data':'0xFF'}),('sign',{'data':'bad','publickey':'bad'})]:
        add(op('direct',name=name,params=params))
    for address in ['Mx','MxINVALID','mxabc','0xabc','abc',' 0x01 ']:
        for name in ['scripts','removescript']:add(op('direct',name=name,params={'address':address}))
    for uses in [0,1,3,4]:
        add(command('sign data:0xFF publickey:0x01',rows=[{**row,'uses':uses}]))
    add(op('locked'),command('sign data:0xFF publickey:0x01'))
    for text in examples:cases.append(dict(ops=[command(text)]))
    for script in ['RETURN [unterminated','RETURN \"unterminated','RETURN TRUE /* incomplete']:
        add(op('direct',name='newscript',params={'script':script,'trackall':'true','clean':'true'}))
    for name,field,params in [('newscript','script',{'script':'RETURN TRUE','trackall':'true'}),('newscript','trackall',{'script':'RETURN TRUE','trackall':'true'}),('scripts','address',{}),('removescript','address',{}),('sign','data',{'publickey':'0x01'}),('sign','publickey',{'data':'0xFF'})]:
        for value in [None,True,1,{},[]]:add(op('direct',name=name,params={**params,field:value}))
    for log,script in itertools.product([False,True],['RETURN TRUE',' return  true ','RETURN \"unterminated','RETURN [hello] EQ [hello]']):add(op('clean',log=log,script=script))
    return cases

def address_corpus():
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey='0x'+'12'*32,publickey='0x01',script='RETURN TRUE',address='0x01',simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(operation='run',**kw):
        result=dict(op=operation,rows=[row],probe=['0x01','0x00'],caller='0x00');result.update(kw);return result
    def direct(name,**params):return op('direct',name=name,params=params)
    def run(text,**kw):return op(text=text,**kw)
    def add(*ops,load=False):cases.append(dict(ops=([op('load')] if load else [])+list(ops)))
    for name in ['newaddress','getaddress','coinnotify','slavenode']:
        add(op('meta',name=name));add(run(name+' bad:value'))
    # Real default TreeKey generation is intentionally exercised; never substitute a tiny key.
    for fail,runtime in [(0,False),(1,False),(9,False),(10,False),(17,False),(18,False),(1,True),(18,True)]:
        add(run('newaddress',fail=fail,runtimeFailure=runtime),load=True)
        if fail==0:cases[-1]['expectNewAddress']=True
    add(op('locked'),run('newaddress'),load=True)
    add(run('newaddress'))
    add(run('newaddress',caller='0x1234'),load=True)
    for fail,runtime in itertools.product(range(11),[False,True]):add(run('getaddress',fail=fail,runtimeFailure=runtime),load=True)
    for rows in [[],[row],[row,row]]:add(run('getaddress',rows=rows),load=True)
    add(run('getaddress',nullRows=True),load=True);add(run('getaddress'))
    for value in [None,True,False,'','true','FALSE',1,{},[]]:
        add(direct('getaddress',createall=value),load=True)
        add(direct('getaddress',type=value),load=True)
    for text in ['getaddress createall:true','getaddress type:single','getaddress type:all']:
        add(run(text),load=True)
    add(run('getaddress',caller='0x1234'),load=True)
    for address in ['0x01','0x0001','0xabc','abc','Mx','MxINVALID',' 0x01 ']:
        add(*(direct('coinnotify',action=a,address=address) for a in ['check','add','add','check','remove','remove','check']))
    for name,params,field in [('coinnotify',{'action':'add','address':'0x01'},'action'),('coinnotify',{'action':'add','address':'0x01'},'address'),('slavenode',{'enable':'true','host':'fixture:9001'},'enable'),('slavenode',{'enable':'true','host':'fixture:9001'},'host')]:
        for value in [None,True,False,'','TRUE','unknown',1,{},[]]:add(direct(name,**{**params,field:value}))
    add(direct('coinnotify'));add(direct('coinnotify',action='add'));add(direct('coinnotify',address='0x01'))
    add(run('coinnotify action:add address:0x01',caller='0x1234'),run('coinnotify action:check address:0x01'),op('restart'),run('coinnotify action:check address:0x01'))
    for value in ['true','false','TRUE','FALSE','anything']:
        add(direct('slavenode',enable=value,host='fixture:9001'),run('slavenode'),op('restart'),op('reloadPrefs'),run('slavenode'))
    add(run('slavenode'))
    add(run('slavenode enable:true host:fixture:9001',caller='0x1234'),run('slavenode enable:false'),run('slavenode'))
    add(run('slavenode enable:true host:fixture:9001'),op('allowSave',allow=False),run('slavenode enable:false'),op('reloadPrefs'),run('slavenode'))
    add(op('allowSave',allow=False),run('slavenode enable:true host:fixture:9001'),op('allowSave',allow=True),run('slavenode'),op('restart'),op('reloadPrefs'),run('slavenode'))
    add(direct('slavenode',host='ignored:9001'));add(direct('slavenode',enable='true'))
    for value in [None,True,1,{},[]]:add(direct('slavenode',enable='false',host=value))
    # Java-produced Mx representation of 0x01, retained from the existing Wallet oracle.
    for mx in ['Mx200104JMFSAS','mx200104jmfsas']:
        add(direct('coinnotify',action='add',address='0x01'),direct('coinnotify',action='check',address=mx),direct('coinnotify',action='remove',address=mx),direct('coinnotify',action='check',address='0x01'))
        cases[-1]['expectNotifyAlias']=True
    add(direct('coinnotify',action='unknown',address='MxINVALID'))
    return cases

def vault_normalize(value):
    # Preserve all observations except independent random ciphertext bytes.
    import copy
    value=copy.deepcopy(value)
    for entry in value.get('ops',[]):
        observation=entry.get('vault',{});cipher=observation.get('encrypted','0x00')
        if cipher!='0x00':observation['encrypted']={'bytes':(len(cipher)-2)//2}
        prefs=entry.get('prefs')
        if prefs:
            data=bytes.fromhex(prefs[2:]);stored=json.loads(data[4:].decode('utf-8'));encrypted=stored.get('encrypted_seed','0x00')
            if encrypted!='0x00':entry['prefs']={'wire':(data[:4]+data[4:].replace(encrypted.encode(),b'<CIPHERTEXT>')).hex(),'cipherBytes':(len(encrypted)-2)//2}
    return value

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--group',choices=['scripts','addresses','witness','inputs','check','mining','vault','keys','funding','offline','unsigned','consolidation','balance','coins','history','txpow','node_state','seedrandom'],default='scripts');args=ap.parse_args();build=args.build.resolve();prefix={'scripts':'wallet-commands','addresses':'wallet-address-commands','witness':'wallet-witness-commands','inputs':'transaction-input-commands','check':'transaction-check-command','mining':'transaction-mining-commands','vault':'vault-signing-commands','keys':'wallet-key-commands','funding':'coin-funding-commands','offline':'offline-send-commands','unsigned':'unsigned-send-command','consolidation':'wallet-consolidation-command','balance':'wallet-balance-command','coins':'coin-search-command','history':'transaction-history-command','txpow':'txpow-search-command','node_state':'node-state-commands','seedrandom':'seed-random-command'}[args.group];oracle=build/(prefix+'-oracle');oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SqlDBOracle.java',ROOT/'support/WalletCommandsOracle.java',ROOT/'support/SearcherOracle.java',ROOT/'support/ArchiveCacheOracle.java',ROOT/'support/ConsensusOracle.java'];listing=build/(prefix+'-java-sources.txt');listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];operations=0;effects=0;cases=address_corpus() if args.group=='addresses' else corpus()
    with tempfile.TemporaryDirectory(prefix='minima-wallet-commands-',dir=build) as root:
        # Default 64^3 custom signing is considerably slower under ASAN/UBSAN.
        timeout=180 if args.group in ['vault','keys'] else 30
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root],response_timeout=timeout),Runner([str(build/'wallet_commands'),root],response_timeout=timeout)]
        try:
            if args.group in ['witness','inputs','check','mining']:
                if args.group=='witness':from witness_commands_corpus import corpus as fixture_corpus
                elif args.group=='inputs':from transaction_input_corpus import corpus as fixture_corpus
                elif args.group=='check':from transaction_check_corpus import corpus as fixture_corpus
                else:from transaction_mining_corpus import corpus as fixture_corpus
                fixtures=runners[0].ask({'kind':'checkFixtures' if args.group in ['check','mining'] else 'witnessFixtures'})
                cases=fixture_corpus(fixtures,runners[0].ask({'kind':'witnessFixtures'})) if args.group=='mining' else fixture_corpus(fixtures)
            if args.group=='vault':
                from vault_signing_corpus import corpus as vault_corpus
                cases=vault_corpus(runners[0].ask({'kind':'vaultFixtures'}),runners[0].ask({'kind':'witnessFixtures'}),runners[0].ask({'kind':'checkFixtures'}))
            if args.group=='keys':
                from wallet_key_corpus import corpus as key_corpus
                cases=key_corpus(runners[0].ask({'kind':'keyFixtures'}))
            if args.group=='seedrandom':
                from seed_random_corpus import corpus as seed_corpus,project as seed_project,validate as seed_validate
                cases=seed_corpus()
            if args.group=='node_state':
                from node_state_corpus import fixtures as node_fixtures,corpus as node_corpus,project as node_project,validate as node_validate
                cases=node_corpus(node_fixtures(runners[0]))
            if args.group=='txpow':
                from txpow_search_corpus import fixtures as txpow_fixtures,corpus as txpow_corpus,project as txpow_project,validate as txpow_validate
                cases=txpow_corpus(txpow_fixtures(runners[0]))
            if args.group=='history':
                from transaction_history_corpus import fixture_request as history_fixtures,corpus as history_corpus,project as history_project,validate as history_validate
                from wallet_balance_corpus import fixture_request as balance_fixtures
                fixtures=runners[0].ask(balance_fixtures());cases=history_corpus(fixtures,runners[0].ask(history_fixtures(fixtures)))
            if args.group=='coins':
                from coin_search_corpus import corpus as coins_corpus,validate as coins_validate
                from wallet_balance_corpus import fixture_request as balance_fixtures
                request=balance_fixtures();request['groups']=request['groups'][:2]+[request['groups'][1]|dict(script='RETURN FALSE')]
                cases=coins_corpus(runners[0].ask(request),runners[0].ask({'kind':'witnessFixtures'}))
            if args.group=='balance':
                from wallet_balance_corpus import corpus as balance_corpus,validate as balance_validate,fixture_request as balance_fixtures,preserve_text as balance_preserve_text
                cases=balance_corpus(runners[0].ask(balance_fixtures()))
            if args.group=='consolidation':
                from wallet_consolidation_corpus import corpus as consolidation_corpus,validate as consolidation_validate,normalize as consolidation_normalize
                cases=consolidation_corpus(runners[0].ask({'kind':'consolidationFixtures'}))
            if args.group=='funding':
                from coin_funding_corpus import corpus as funding_corpus,normalize as funding_normalize,validate as funding_validate
                cases=funding_corpus(runners[0].ask({'kind':'witnessFixtures'}),runners[0].ask({'kind':'keyFixtures'}),runners[0].ask({'kind':'fundingFixtures'}))
            if args.group=='offline':
                from offline_send_corpus import corpus as offline_corpus,normalize as offline_normalize,validate as offline_validate
                cases=offline_corpus(runners[0].ask({'kind':'offlineFixtures'}),runners[0].ask({'kind':'keyFixtures'}),root)
            if args.group=='unsigned':
                from unsigned_send_corpus import corpus as unsigned_corpus,normalize as unsigned_normalize,validate as unsigned_validate
                cases=unsigned_corpus(runners[0].ask({'kind':'witnessFixtures'}),runners[0].ask({'kind':'keyFixtures'}),runners[0].ask({'kind':'fundingFixtures'}),root)
            cross_seen=set()
            for index,request in enumerate(cases):
                try:
                    raw=[r.ask(request) for r in runners]
                    outputs=[normalize(value) for value in raw]
                    if args.group=='seedrandom':outputs=[seed_project(original,value,request) for original,value in zip(raw,outputs)]
                    if args.group=='node_state':outputs=[node_project(original,value,request) for original,value in zip(raw,outputs)]
                    if args.group=='txpow':outputs=[txpow_project(original,value,request) for original,value in zip(raw,outputs)]
                    if args.group=='history':outputs=[history_project(original,value,request) for original,value in zip(raw,outputs)]
                    if args.group=='balance':outputs=[balance_preserve_text(original,value,request) for original,value in zip(raw,outputs)]
                    if args.group=='consolidation':outputs=[consolidation_normalize(value,request) for value in outputs]
                    if args.group=='unsigned':outputs=[unsigned_normalize(value) for value in outputs]
                    if args.group=='offline':outputs=[offline_normalize(vault_normalize(value) if request.get('vault') else value) for value in outputs]
                    if args.group=='funding':outputs=[funding_normalize(vault_normalize(value) if request.get('vault') else value) for value in outputs]
                    if args.group=='vault':
                        for result in raw:
                            for entry,operation in zip(result.get('ops',[]),request['ops']):
                                observation=entry.get('vault',{});cipher=observation.get('encrypted','0x00');plain=observation.get('decrypted','DECRYPT_ERROR')
                                if cipher!='0x00' and plain!='DECRYPT_ERROR' and cipher not in cross_seen and len(cross_seen)<8:
                                    cross_seen.add(cipher);password=operation.get('observePassword','fixture pass')
                                    cases.append(dict(vault=True,ops=[dict(op='vaultDecrypt',rows=[],probe=[],data=cipher,password=password)],expectPlaintext=plain))
                        outputs=[vault_normalize(value) for value in outputs]
                except Exception:
                    (build/(prefix+'-crash.json')).write_text(json.dumps(dict(case=index,request=request,processes=[dict(returncode=r.child.poll(),stderr=r.errors()) for r in runners]),indent=2)+'\n')
                    raise
                if outputs[0]!=outputs[1] or 'ops' not in outputs[0]:
                    failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                if args.group=='seedrandom':seed_validate(request,outputs[0])
                if args.group=='node_state':node_validate(request,outputs[0])
                if args.group=='txpow':txpow_validate(request,outputs[0])
                if args.group=='history':history_validate(request,outputs[0])
                if args.group=='coins':coins_validate(request,outputs[0])
                if args.group=='balance':balance_validate(request,outputs[0])
                if args.group=='consolidation':consolidation_validate(request,outputs[0])
                if args.group=='unsigned':unsigned_validate(request,outputs[0])
                if args.group=='offline':offline_validate(request,outputs[0])
                if args.group=='funding':funding_validate(request,outputs[0])
                if 'expectKeyCheck' in request:assert outputs[0]['ops'][-1]['value'] is request['expectKeyCheck'],outputs[0]
                if 'expectKeyError' in request:assert request['expectKeyError'] in outputs[0]['ops'][-1]['exception'],outputs[0]
                if 'expectGeneratedPhrase' in request:
                    entry=outputs[0]['ops'][-1];response=entry['value']['response']
                    assert response['phrase']==request['expectGeneratedPhrase'] and entry['events']==[],entry
                    assert response['modifier']==0 and response['script']=='RETURN SIGNEDBY('+response['publickey']+')',entry
                if request.get('expectKeyCounts'):
                    response=outputs[0]['ops'][-1]['value']['response'];assert [response['correct'],response['wrong']]==request['expectKeyCounts'],outputs[0]
                if 'expectKeyMessage' in request:
                    entry=outputs[0]['ops'][-1];assert entry['value']['response']['message']=='Keys being created..',entry
                    assert entry['mining']['posted']==[dict(type='MAIN_LOAD_ALL_KEYS',keyuses=request['expectKeyMessage'],wire=None)],entry
                if request.get('expectPlaintext') is not None:assert outputs[0]['ops'][-1]['value']==request['expectPlaintext'],outputs[0]
                if request.get('expectUnlocked'):assert outputs[0]['ops'][-1]['value']['response']['locked'] is False,outputs[0]
                if request.get('expectLocked'):assert outputs[0]['ops'][-1]['value']['response']['locked'] is True,outputs[0]
                if request.get('expectReset'):
                    entry=outputs[0]['ops'][-1];assert entry['value']['response']=='All private keys restored!',entry
                    assert entry['vault']['calls']==['restoreReady','setHasShutDown','stopMessageProcessor'] and entry['vault']['files']==[False]*4,entry
                    assert entry['vault']['shutdown'] and entry['vault']['stopped'],entry
                if request.get('expectSignatures') is not None:
                    results=outputs[0]['ops'][-1]['value'];assert results==[True]*request['expectSignatures'],outputs[0]
                if request.get('expectPosted'):
                    entry=outputs[0]['ops'][-1];expected=request['expectPosted'];assert 'value' in entry,entry;response=entry['value']['response']
                    assert response['txnpost'] and response['delete']==expected['deleted'],entry
                    assert len(entry['mining']['pending'])==int(not expected['sync']) and len(entry['mining']['posted'])==int(expected['sync']),entry
                    if expected['deleted']:assert entry['txnflags']==[],entry
                if request.get('expectPostError'):
                    entry=outputs[0]['ops'][-1];assert request['expectPostError'] in entry['exception'],entry
                    assert entry['mining']['calls']==[] and len(entry['txnflags'])==1,entry
                if request.get('expectMining'):
                    expected=request['expectMining'];entries=outputs[0]['ops'];entry=entries[expected['op']];response=entry['value']['response'];mining=entry['mining']
                    assert len(mining['calls'])==1,mining
                    call=mining['calls'][0];assert call['kind']==expected['kind'] and call['post']==expected['post'],call
                    assert call['sameTransaction']==expected.get('shared',True) and call['sameWitness']==expected.get('shared',True),call
                    if expected['kind']=='sync':assert call['limit']==120000 and not call['maxima'],call
                    assert len(mining['pending'])==int(expected['kind']=='async'),mining
                    assert len(mining['posted'])==int(expected['kind']=='sync' and expected['post']),mining
                    if expected.get('deleted'):assert entry['txnflags']==[],entry
                if request.get('expectBurnPost'):
                    body=outputs[0]['ops'][-2]['value']['response']['body']
                    assert body['burntxn']['inputs'] and len(body['burnwitness']['signatures'])==1,body
                    assert body['burntxn']['linkhash']==body['txn']['transactionid'],body
                if request.get('expectValid'):
                    response=outputs[0]['ops'][-1]['value']['response'];assert response['validtransaction'] is True,response
                    assert outputs[0]['ops'][-1]['txnflags']==[[True,True,True]],outputs[0]
                if request.get('expectBadSignature'):
                    response=outputs[0]['ops'][-1]['value']['response'];assert response['allsignaturesvalid'] is False and response['validtransaction'] is True,response
                if request.get('expectShared'):
                    assert outputs[0]['ops'][1]['value']['sameTransaction'] and outputs[0]['ops'][1]['value']['sameWitness'],outputs[0]
                    final=outputs[0]['ops'][-1]['value'];deleted=any(o.get('text','').startswith('txndelete') for o in request['ops'])
                    assert final['sameTransaction'] is not deleted and final['sameWitness'] is not deleted,final
                    for entry in outputs[0]['ops'][2:-1]:assert all(reply['status'] for reply in entry['value']),entry
                    first=outputs[0]['ops'][1]['value']
                    if any(o.get('text','').startswith('txnstate') for o in request['ops']):assert first['transaction']!=final['transaction']
                    if any(o.get('text','').startswith('txnclear') for o in request['ops']):assert first['witness']!=final['witness']
                if request.get('expectBodyIdentity'):
                    for entry in outputs[0]['ops']:
                        value=entry.get('value')
                        if isinstance(value,dict) and 'sameBurn' in value:assert all(value[k] for k in ['sameTransaction','sameWitness','sameBurn','sameBurnWitness']),value
                if request.get('expectBurn'):
                    expected=request['expectBurn'];amount=expected['amount'];last=outputs[0]['ops'][-1]
                    if amount in ['1','3','4','5','7','8']:
                        row=last['value']['row'];trans=row['transaction'];witness=row['witness']
                        count=1 if int(amount)<=4 else 2 if int(amount)<=7 else 3
                        assert len(trans['inputs'])==count and len(witness['mmrproofs'])==count,last
                        assert len(witness['signatures'])==int(expected['sign']),last
                        assert trans['linkhash']=='0xAABB',last
                        assert len(trans['outputs'])==int(amount not in ['4','7','8']),last
                    else:assert 'exception' in last,last
                if request.get('expectPool'):assert outputs[0]['ops'][-2]['value'] is True,outputs[0]
                if request.get('expectWitness') is not None:
                    reply=outputs[0]['ops'][-1]['value'];witness=reply['response']['witness'];expected=request['expectWitness']
                    assert reply['status'] is True and len(witness['scripts'])==expected['scripts'] and len(witness['mmrproofs'])==expected['proofs'],reply
                if request.get('expectNewAddress'):
                    reply=outputs[0]['ops'][-1]['value'][0];response=reply.get('response',{})
                    assert reply.get('status') is True and response.get('simple') is True and response.get('default') is False and response.get('track') is True and response.get('total')==1 and len(response.get('publickey',''))==66,reply
                if request.get('expectNotifyAlias'):
                    values=[x['value']['response'] for x in outputs[0]['ops']]
                    assert [values[i].get('found') for i in [1,2,3]]==[True,True,False],values
                operations+=len(request['ops']);effects+=sum(len(x['events']) for x in outputs[0]['ops'])
        finally:
            for runner in runners:runner.close()
    report=dict(passed=not failures,programs=index+1,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual selected commands, registry/parameter/permission gates, MinimaDB/Wallet/ScriptRow/Address/Contract cleaning and TreeKey signatures. Exact replies, metadata, SQL calls, logs and selected cache membership. Recording SQL only; no native H2 or live wallet. Java-specific NPE/class-cast diagnostics, stack frames and logger prefix excluded. Supplied query rows do not establish durable SQL persistence. Address group also compares real default TreeKey generation, volatile coin notifications and exact UserDB file bytes across save/disable-save/reload/owner-recreation. Default-address randomness compared with zero, one or identical duplicate rows; selection among distinct candidates is not proved. Witness group uses actual chain/transaction/witness objects and Java-produced MMR bytes, compares full custom-DB wire and monotonic flags after every operation. Check group reuses actual consensus fixtures and generator/checker/TreeKey, compares full stored transaction/witness wire, all three cache flags, generated owner identity/lifetime and in-place TxBody reads including aliases, null fields, offsets and partial failures. Fixed fixture times force existing clock clamp; random generated PRNG omitted from ownership snapshots. Mining group runs actual queued/synchronous mining after fixing only the generated 32 PRNG bytes at miner entry. Compares miner arguments, full TxPoW wire, exact replies, Main messages, retained queue owners and reservations, burn signatures, failed conversion logs and late deletion effects. False/throwing miner results and Main post failures are injected separately. Main construction, external network acceptance, full coin/list aliasing and complete node integration remain unverified.')
    if args.group=='unsigned':report['scope']='Actual sendnosign and create/view/sign/verify/post flow using real native chain, MMR, wallet, generator, file and miner components. Complete file bytes compared except the independently measured 32 PRNG bytes; decoded body PRNG and shape-checked clock filenames alone normalized. Exact replies, metadata, SQL, logs, directories, proofs, outputs and signature verification compared. Recording SQL does not prove durable storage; fixture chain and miner queue do not prove live network acceptance. Full node, arbitrary Java reference APIs and all filesystem/platform behavior remain unverified.'
    if args.group=='offline':report['scope']='Actual sendview/sendpost/sendsign, metadata and runner permission/parameter gates. Complete TxPoW file bytes, recursive file/directory observations, SQL calls, logs, verified signatures, retained miner queues and response snapshots are compared. Repeated inputs produce repeated signing calls but duplicate-root witness entries are omitted by actual Witness. Missing/truncated/NUL/directory/path errors and writer append/replace/error ordering are compared. The 512-line demonstration main is compared for null/empty arguments. Generated wall-clock filenames are shape-checked then normalized; actual file bytes remain compared. Vault cases normalize independently random ciphertext using existing vault observations. Java-specific NPE/class-cast diagnostics, stack frames and logger prefixes excluded. Recording SQL rows do not establish durable storage; key-use write counts are observed, not persisted. Synthetic fixtures are not consensus-valid network transactions. Full Main construction, node/network acceptance, stream races, all Java reference APIs and filesystem/platform equivalence remain unverified.'
    if args.group=='funding':report['scope']+=' Funding group compares source selector order, caller-list mutation and shared Coin handles, coin locks and partial failures, real search/proof/scaling/change/signing/submission. Dry-run PRNG bytes alone normalized after checking length32; other fields are compared. JSONObject object-key API and full Main/native SQL integration remain unverified.'
    if args.group=='keys':report['scope']+=' Keys group compares actual list/filter/new/check/genkey/createallkeys behavior and helper seed/private/public consistency. Generated-key cases supply a phrase; random no-phrase genkey branch and full Main key worker execution remain unverified.'
    if args.group=='vault':report['cross_decryption_programs']=len(cross_seen);report['scope']+=' Vault group compares actual seed state, recorded Main lifecycle calls, temporary state-file deletion, signed witnesses, real lock/unlock cryptography and cross-language ciphertext decryption. Only independent ciphertext bytes are normalized; persisted file metadata and other bytes remain compared. Seed rows returned after reopen are explicitly supplied, not proof of durable SQL.'
    if args.group=='consolidation':report['scope']='Actual consolidate, coin search/order, Wallet and CommandRunner. Record forwarded send text/parameters and controlled replies through a scoped prototype; compare real send dry-run integration using existing proof/miner fixtures. Exact SQL, logs, response and stored bytes compared except existing shape-checked PRNG and runtime exception prose. No durable SQL or live node/network acceptance.'
    if args.group=='balance':report['scope']='Actual balance, real coin search/tree/MegaMMR, Wallet cache, MiniNumber/Token scaling and JSON parser. Exact serialized command reply captured as a string, including source malformed JSON; SQL calls, ordered logs, cache and stored transaction bytes compared. Existing runtime-specific exception prose/logger normalization only. Supplied rows and source-generated fixture coins do not prove durable SQL, live balances, full reference API or node integration.'
    if args.group=='coins':report['scope']='Actual coins command, search, wallet cache, MegaMMR, miner reservations, RAM witness membership and send.selectCoins. Source-generated Coin/Token wire and existing chain/MMR fixtures; exact metadata, replies, SQL/log/cache and stored transaction observations except established runtime exception prose. Independent IDs/ages/order/filter/selection assertions. No live Main startup, durable SQL, network acceptance or exhaustive reference/concurrency parity.'
    if args.group=='history':report['scope']='Actual history, SQL methods, wire decoding, Wallet, MiniNumber/Token and Java collection adapters. Exact serialized replies, helper dispatch, SQL calls and chain counts. Default time is bounded against the real clock before normalization; explicit times remain exact. Existing runtime-specific exception prose normalization only. Recording rows do not prove durable SQL, full Main, live history, arbitrary reference APIs or exhaustive concurrency parity.'
    if args.group=='txpow':report['scope']='Actual txpow command and registry, RAM/SQL/archive/on-chain lookups, chain search and source-generated TxPoW/TxBlock wire. Exact serialized replies, IDs/order, confirmations, SQL calls, failures and MySQL forwarding through real runner. Optional rowsBySQL routes supplied recording-driver data. MySQL body uses a scoped recording prototype; no native MySQL, durable SQL, full Main/live chain, exhaustive Java reference or concurrency/platform parity is proved.'
    if args.group=='seedrandom':report['scope']='Actual seedrandom, wallet seed availability/updates, MiniString/MiniData and Crypto. Exact replies and SQL effects, synthetic seeds only, independent SHA3-256 framed-byte expectations. No live wallet, full Main, durable SQL or exhaustive concurrency/platform certification.'
    if args.group=='node_state':report['scope']='Actual mempool, cointrack, magic, RAM map/flags, CoinDB, tree search/relevance, UserDB/JsonDB and TxPoWGenerator. Exact replies, preference/coin/MMR wire and SQL effects. Random tree-ID bindings shape-checked and assigned stable per-program labels; existing runtime diagnostic normalization. Recording SQL does not prove durable storage, full Main/node/network, arbitrary retained collection alias mutation or exhaustive concurrency/platform parity.'
    (build/(prefix+'-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
