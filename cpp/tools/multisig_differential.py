#!/usr/bin/env python3
"""Pinned multisig workflows with actual nested commands, files, proofs and wallet."""
import argparse,copy,itertools,json,re,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def corpus(f,t,key,native,root):
    row=dict(size=2,depth=2,uses=0,maxuses=4,modifier='0x00',privatekey=key['privatekey'],publickey=key['publickey'],script=f['contract'],address=f['address'],simple=1,defaultaddress=1,track=1,phrase='fixture seed',seed='0xAA')
    cases=[]
    def op(kind='direct',**kw):return dict(op=kind,rows=[row],probe=['0x01'],caller='0x00',traceChildren=True)|kw
    def cmd(name,action,params=None,**kw):return op(name=name,params=dict(action=action)|(params or {}),**kw)
    def setup(fixture=f):return [op('load'),op('chain',history='8',nodes=fixture['nodes'])]
    def add(*steps,**kw):cases.append(dict(offline=True,consolidation=True,ops=list(steps),**kw))
    create=dict(amount='1',required='1',publickeys=[key['publickey']])
    spend=dict(coinid=f['ids'][2],amount='1',address='0x02',file='lecture.txn')
    for name in ['multisig','multisigread']:
        add(op('meta',name=name),metadata=name)
        for action in ['','bad','CREATE','create','getkey','list','spend','sign','post','view']:
            add(*setup(),cmd(name,action))
        for value in [None,True,1,{},[]]:add(*setup(),op(name=name,params={'action':value}))
        add(op(name=name,params={}))
        add(*setup(),cmd(name,'getkey'),expectKey=key['publickey'])
        add(*setup(),cmd(name,'list'),expectList=3)
        add(*setup(),cmd(name,'list',dict(id='classroom')),expectList=6)
        add(*setup(),cmd(name,'list',dict(id='missing')),expectList=0)
        add(op('chain',history='8',nodes=[]),cmd(name,'list'),expectList=0)
        for fixture in [f,t]:
            for amount in ['-1','0','0.1','1','2','3','4','100.5']:
                add(*setup(fixture),cmd(name,'spend',spend|dict(coinid=fixture['ids'][0],amount=amount)))
        add(*setup(),cmd(name,'spend',spend),expectSpend=True)
        add(*setup(),cmd(name,'spend',spend|dict(id='classroom')),expectSpend=True)
        for params in [dict(id='missing'),dict(id=None),dict(coinid=f['ids'][1]),dict(address=f['address']),dict(address=f['address'].lower()),dict(address=f['mxaddress']),dict(address='MxBAD'),dict(file=str(Path(root)/'offline/nested/lecture.txn')),dict(file='a b.txn'),dict(file='x;txnlist'),dict(file='nul\0name')]:
            add(*setup(),cmd(name,'spend',spend|params))
        for field,value in itertools.product(['id','coinid','amount','address','file'],[None,True,1,{},[],'','bad']):add(*setup(),cmd(name,'spend',spend|{field:value}))
        default=spend.copy();del default['file'];add(*setup(),cmd(name,'spend',default))
        for action in ['sign','view','post']:
            for file in ['missing.txn','dir','nul\0file','bad.txn']:
                add(*setup(),op('fileDirectory',file='dir'),op('filePut',file='bad.txn',data='0x0000'),cmd(name,action,dict(file=file)))
        add(*setup(),cmd(name,'spend',spend),cmd(name,'view',dict(file='lecture.txn')),expectView=True)
        add(*setup(),cmd(name,'spend',spend),cmd(name,'post',dict(file='lecture.txn')))
        for text in [name+' invalid:true',name+' action:list',name+' action:spend coinid:'+f['ids'][2]+' amount:1 address:0x02 file:runner.txn',name+' action:create amount:1 required:1 publickeys:["'+key['publickey']+'"]']:
            for caller in ['0x00','0x1234']:add(*setup(),op('run',text=text,caller=caller,recordSend=True))
    for required in ['-1','0','0.5','1','1.9','2','4294967296','4294967297','2147483648']:
        add(*setup(),cmd('multisig','create',create|dict(required=required,id='classroom'),recordSend=True))
    for root in ['0x21','0x'+'Z'*64,'0X'+'A'*64,'0x'+'🐼'*32,'',key['publickey']]:
        add(*setup(),cmd('multisig','create',create|dict(root=root),recordSend=True))
    for publickeys in [[],[None],[True],[1],[{}],[''],['0x'+'Z'*64],['0x'+'🐼'*32],[key['publickey']]*3]:
        add(*setup(),cmd('multisig','create',create|dict(publickeys=publickeys),recordSend=True))
    for field,value in itertools.product(['id','root','amount','required','publickeys','tokenid','password'],[None,True,1,{},[],'','bad']):add(*setup(),cmd('multisig','create',create|{field:value},recordSend=True))
    for result in [{},{'status':None},{'status':True},{'status':True,'response':{'funded':True}},{'status':False},{'status':False,'message':12,'error':'ignored'},{'status':False,'error':'rejected'},{'status':'true'}]:add(*setup(),cmd('multisig','create',create,recordSend=True,sendResult=result))
    add(*setup(),cmd('multisig','create',create,recordSend=True,sendThrow=True))
    # Real Java/C++ child failures retain partially built custom rows exactly.
    for fail in range(1,25):add(*setup(),cmd('multisig','spend',spend,fail=fail))
    for amount in ['1','3']:
        add(*setup(),cmd('multisig','spend',spend|dict(amount=amount)),cmd('multisig','sign',dict(file='lecture.txn')),cmd('multisigread','view',dict(file='signed_lecture.txn')),expectSigned=True)
    for params in [dict(password=None),dict(password=True),dict(password='unused'),dict(file='missing.txn')]:add(*setup(),cmd('multisig','spend',spend),cmd('multisig','sign',dict(file='lecture.txn')|params))
    add(*setup(),cmd('multisig','spend',spend),cmd('multisig','sign',dict(file='lecture.txn')),op('verifyTxnFile',file='signed_lecture.txn'),expectVerified=True)
    add(*setup(),op('mining'),cmd('multisig','spend',spend),cmd('multisig','sign',dict(file='lecture.txn')),cmd('multisigread','post',dict(file='signed_lecture.txn')),op('verifyMined'),expectPosted=True)
    for password in ['fixture pass','wrong','',None,True]:
        locked=row|{'phrase':'','seed':'0x00'}
        add(*setup(),cmd('multisig','spend',spend),op('vaultLock',password='fixture pass',rows=[locked]),cmd('multisig','sign',dict(file='lecture.txn',password=password)),vault=True)
    funding=row|dict(script='RETURN TRUE',address='0x01')
    for amount in ['1','9','99']:
        add(op('load',rows=[funding]),op('chain',history='8',nodes=native['nodes']),op('mining'),cmd('multisig','create',create|dict(amount=amount),rows=[funding]))
    return cases

def project(raw,request):
    ids=[];filenames=[]
    for row in raw['ops']:
        for call in row.get('childCalls',[]):
            if call['name'] in ['txncreate','txnimport']:
                uid=call['params']['id'];assert re.fullmatch(r'0x[0-9A-F]{64}',uid),uid
                if uid not in ids:ids.append(uid)
            if call['name']=='txnexport':
                file=call['params'].get('file','')
                if re.fullmatch(r'multispend_\d+\.txn',file) and file not in filenames:filenames.append(file)
    def visit(v,key=''):
        if isinstance(v,list):return [visit(x,key) for x in v]
        if isinstance(v,dict):return {visit(k):visit(x,k) for k,x in v.items()}
        if not isinstance(v,str):return v
        for i,uid in enumerate(ids):
            # A TxnRow begins with a four-byte MiniString length then its ASCII ID.
            # Replace only that complete field, leaving every transaction/witness byte intact.
            marker='00000042'+uid.encode().hex().upper()
            if v.startswith('0x') and re.fullmatch(r'0x[0-9A-F]*',v):
                v=v.replace(marker,'00000042'+('0x'+f'{i:064X}').encode().hex().upper())
            v=v.replace(uid,f'<ID:{i}>')
        for i,file in enumerate(filenames):v=v.replace(file,f'multispend_<TIME:{i}>.txn')
        if key in ['exception','error','logs']:
            v=re.sub(r'java.lang.(NullPointerException|ClassCastException):.*',r'java.lang.\1',v)
        return v
    value=visit(copy.deepcopy(raw))
    if request.get('vault'):
        from wallet_commands_differential import vault_normalize
        value=vault_normalize(value)
    return value

def validate(request,result):
    last=result['ops'][-1];value=last.get('value',{})
    if request.get('expectVerified'):assert value['signatures']==[True] and value['consumed']==value['bytes'],value
    if request.get('expectPosted'):assert value and all(value),result
    if 'metadata' in request:assert value['name']==request['metadata'] and value['fresh']['command']==request['metadata'],value
    if 'expectKey' in request:assert value['response']['publickey']==request['expectKey'],value
    if 'expectList' in request:assert len(value['response'])==request['expectList'],value
    if request.get('expectSpend'):
        assert value['status'] and all(x['status'] for x in value['response']),value
        assert len(last['files'])==1 and last['txnflags']==[],last
    if request.get('expectView') or request.get('expectSigned'):
        assert value['status'] and all(x['status'] for x in value['response']),value
        assert last['txnflags']==[],last
        if request.get('expectSigned'):
            imported=value['response'][0]['response'];assert imported['witness']['signatures'],imported

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'multisig-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['WalletCommandsOracle.java','SqlDBOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java']]
    listing=build/'multisig-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',f'{JAVA}/jar/minima.jar','-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=effects=0;cases=[]
    with tempfile.TemporaryDirectory(prefix='multisig-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletCommandsOracle',root]),Runner([str(build/'multisig_runtime'),root])]
        try:
            key=runners[0].ask({'kind':'keyFixtures'})
            # Obtain the actual source hash via a controlled create call (same hashObject/MiniString path).
            request=dict(ops=[dict(op='direct',name='multisig',params=dict(action='create',id='classroom',amount='1',required='1',publickeys=[key['publickey']]),rows=[],probe=[],recordSend=True)])
            digest=runners[0].ask(request)['ops'][0]['value']['response']['id']
            state=[dict(port=p,data=d) for p,d in [(0,digest),(1,'0x21'),(2,'1'),(3,'1'),(4,key['publickey']),(5,key['publickey']),(9,digest)]]
            f=runners[0].ask(dict(kind='multisigFixtures',state=state));t=runners[0].ask(dict(kind='multisigFixtures',state=state,token=True));cases=corpus(f,t,key,runners[0].ask({'kind':'witnessFixtures'}),root)
            for i,request in enumerate(cases):
                output=[]
                for name,runner in zip(['java','cpp'],runners):
                    try:
                        raw=runner.ask(request);value=project(raw,request);validate(request,value);output.append(value)
                    except Exception as error:failures.append(dict(case=i,runtime=name,error=repr(error),request=request,stderr=runner.errors(),raw=locals().get('raw')));break
                if failures:break
                if output[0]!=output[1]:failures.append(dict(case=i,request=request,java=output[0],cpp=output[1]));break
                completed+=1;operations+=len(request['ops']);effects+=sum(len(x['events']) for x in output[0]['ops'])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for name,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=name,error=repr(error)))
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,sql_calls=effects,failures=failures,scope='Actual multisig/multisigread and nested transaction commands, real MMR proofs/wallet/signatures, recursive temporary files, exact wire bytes except validated generated MiniString transaction-ID fields and clock filenames. Recording SQL and fixture chain do not prove durable storage or live node acceptance; full API/reference/concurrency parity unverified.')
    (build/'multisig-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
