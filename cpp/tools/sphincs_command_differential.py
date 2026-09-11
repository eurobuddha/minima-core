#!/usr/bin/env python3
"""Pinned sphincs command, real full-size signing, file bytes, and offline child flow."""
import argparse,copy,hashlib,json,shutil,subprocess,tempfile,time
from pathlib import Path
from fors_differential import ForestRunner
from wallet_wrappers_differential import ROOT,JAVA,normalize

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--quick',action='store_true');args=ap.parse_args();build=args.build.resolve()
    oracle=build/'sphincs-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/SPHINCSCommandOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    vectors=json.loads((ROOT/'validation/sphincs-work-in-progress/normal/sphincs-known-vectors.json').read_text());vector=vectors[0]
    suffix='quick' if args.quick else 'full';reportpath=build/f'sphincs-command-{suffix}-results.json';progresspath=build/f'sphincs-command-{suffix}-progress.json'
    runners=[ForestRunner(['java','-XX:-OmitStackTraceInFastThrow','-Xmx4g','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SPHINCSCommandOracle'],response_timeout=3600),ForestRunner([str(build/'minima_sphincs_command')],response_timeout=3600)]
    failures=[];count=checks=0;groups={};transcript=hashlib.sha256();start=time.monotonic()
    with tempfile.TemporaryDirectory(prefix='sphincs-command-',dir=build) as temporary:
        base=Path(temporary)/'files'
        def compare(group,params=None,expected=None,**kw):
            nonlocal count,checks
            request=dict(base=str(base),params=params or {})|kw;values=[]
            for runtime,r in zip(['java','cpp'],runners):
                if base.exists():shutil.rmtree(base)
                base.mkdir()
                out=r.ask(request)
                if 'inspect' in kw:
                    path=base/kw['inspect'];out['file']=path.read_bytes().hex().upper() if path.is_file() else None
                out=normalize(out);values.append(out)
                if expected is not None:
                    expected(out);checks+=1
            count+=1;groups[group]=groups.get(group,0)+1
            # Paths are isolated and random per run; normalize only that test root.
            canonical=json.loads(json.dumps([request,*values]).replace(str(base),'<BASE>'))
            transcript.update(json.dumps(canonical,sort_keys=True,separators=(',',':')).encode())
            if values[0]!=values[1]:failures.append(dict(index=count,group=group,request=request,java=values[0],cpp=values[1]))
            progresspath.write_text(json.dumps(dict(cases=count,groups=groups,independent_assertions=checks,failures=failures),indent=2)+'\n')
            if count%25==0 or group.startswith('full'):print(json.dumps(dict(cases=count,group=group,failures=len(failures))),flush=True)
            return values
        def expect_error(text):
            def check(out):assert out.get('exception')==text and not out['calls'],out
            return check
        def valid(flag):
            def check(out):assert out['value']['response']=={'valid':flag},out
            return check
        try:
            compare('metadata',meta=True)
            compare('missing',expected=expect_error('org.minima.system.commands.CommandException: param not specified : action'))
            for action in ['unknown','Generate','VERIFY',' ',None,True,1,{},[]]:compare('action',dict(action=action))
            for action,required in [('generate',['seed']),('sign',['privatekey','data']),('verify',['publickey','data','signature']),('transaction',['amount','address','tokenid','privatekey'])]:
                good=dict(action=action,seed='seed',privatekey='0x00',data='0x00',publickey='0x00',signature='0x00',amount='1',address='0x02',tokenid='0x00')
                for field in required:
                    q=good.copy();del q[field];compare('required-order',q)
                    for value in [None,True,1,{},[],'','bad']:
                        q=good|{field:value};compare('parameter-types',q)
            for seed in ['xxx','seed','é🐼','  trimmed  ','a\nb','before\0after']:
                def check(out,seed=seed):
                    response=out['value']['response'];hashed=hashlib.sha3_256(seed.strip(' \t\r\n\0').encode()).digest()
                    private=b''.join(hashlib.sha3_256(hashed+tag).digest() for tag in [b'WOTS',b'FORS'])
                    assert response['privatekey']=='0x'+private.hex().upper();assert response['publickey'] in response['script'];assert response['address'].startswith('Mx')
                compare('generate',dict(action='generate',seed=seed),expected=check)
            for n in [0,1,31,32,33,63]:compare('short-private',dict(action='sign',privatekey='0x'+('AB'*n),data='0x00'))
            verify=dict(action='verify',publickey=vector['public'],data=vector['message'],signature=vector['java'])
            compare('verify-known',verify,expected=valid(True));compare('wrong-message',verify|dict(data='0x00'),expected=valid(False));compare('wrong-public',verify|dict(publickey='0x00'),expected=valid(False))
            for signature in ['0x','0x00','0x00000000',vector['java'][:40],vector['java']+'1234']:
                compare('signature-decode',verify|dict(signature=signature))
            compare('file-precedence',verify|dict(file='saved.sig',signature='invalid'),write=vector['java'],path='saved.sig',inspect='saved.sig',expected=valid(True))
            compare('missing-file',verify|dict(file=str(base/'nested'/'missing.sig')))
            compare('malformed-file',verify|dict(file='broken.sig'),write='0x00',path='broken.sig',inspect='broken.sig')
            compare('nul-file-path',verify|dict(file='saved.sig\0suffix'),write=vector['java'],path='saved.sig',inspect='saved.sig')
            for text in ['sphincs invalid:true','sphincs action:unknown','sphincs action:generate seed:xxx','sphincs action:verify publickey:0x00 data:0x00 signature:0x00']:
                compare('registered-runner',runner=text)
            transaction=dict(action='transaction',amount='1',address='0x02',tokenid='0x00',privatekey=vector['private'])
            for reply in [dict(status=False,error='funding rejected'),dict(status=False),dict(status=False,error=1),dict(status=None),dict(status='true')]:
                compare('funding-failure',transaction,inputs=1,fail='txnaddamount',reply=reply)
            def cap(out):
                assert len(out['calls'])==3 and out['rows']==[]
                assert out['exception']=='org.minima.system.commands.CommandException: Input Coin number too great.. MAX 8. Pls Send a smaller amount.'
            compare('input-cap',transaction,inputs=9,expected=cap)
            compare('empty-input',transaction,inputs=0)
            if not args.quick:
                def inline(out):assert out['value']['response']['signature']==vector['java']
                compare('full-inline-sign',dict(action='sign',privatekey=vector['private'],data=vector['message']),expected=inline)
                def filecheck(out):
                    assert out['file']==vector['java'][2:];response=out['value']['response'];assert response['size']==len(out['file'])//2;assert response['signaturefile']==str(base/'saved.sig')
                compare('full-file-sign',dict(action='sign',privatekey=vector['private'],data=vector['message'],file='saved.sig'),write='0xAABBCC',path='saved.sig',inspect='saved.sig',expected=filecheck)
                def testcheck(out):assert out['value']['response']=={'verify':True}
                compare('full-test-action',dict(action='test'),expected=testcheck)
                def posted(out):
                    assert [c['text'].split()[0] for c in out['calls']]==['txncreate','txnaddamount','txnscript','txnmmr','txnpost','txndelete'];assert out['rows']==[]
                    txn=out['calls'][2]['rows'][0]['transaction'];assert len(txn['state'])==66;assert all(not c['storestate'] for c in txn['outputs'])
                compare('full-transaction',transaction,inputs=1,expected=posted)
                # Source ignores txnscript/txnmmr status, but observes txnpost and deletes afterward.
                compare('full-post-rejected',transaction|dict(mine='true'),inputs=8,fail='txnpost',reply=dict(status=False,error='posting rejected'),expected=posted)
            assert not runners[1].errors(),'Native fixture stderr: '+runners[1].errors()
        except Exception as e:failures.append(dict(phase='execution',cases=count,error=repr(e)))
        finally:
            for runtime,r in zip(['java','cpp'],runners):
                try:r.close()
                except Exception as e:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(e)))
    report=dict(passed=not failures,cases=count,groups=groups,independent_assertions=checks,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='Actual registered Java/C++ sphincs, exact replies and metadata, deterministic key derivation, saved valid and malformed signatures, signature-file precedence, and real custom-transaction rows with controlled offline child commands. Full mode additionally executes five genuine full-size signing forests, comparing inline/file signatures and one/eight-input transaction state. Random transaction IDs and JDK NPE/ClassCast prose normalized. Does not certify actual funding, mining or network admission.')
    reportpath.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2),flush=True);return bool(failures)
if __name__=='__main__':raise SystemExit(main())
