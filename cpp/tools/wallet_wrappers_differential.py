#!/usr/bin/env python3
"""Actual wrapper bodies, controlled child failures and offline real-child integration."""
import argparse,copy,itertools,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
BASE={
 'createfrom':dict(fromaddress='0x01',address='0x02',amount='1',script='RETURN TRUE'),
 'constructfrom':dict(coinlist='0x01,, 0x02,',script='RETURN TRUE',toaddress='0x02',toamount='1',changeaddress='0x01',changeamount='2'),
 'postfrom':dict(data='$ROW'),
 'signfrom':dict(data='$ROW',privatekey='0x12',keyuses='0')}
def corpus():
    cases=[]
    def add(name,params=None,**kw):cases.append(dict(name=name,params=BASE[name]|(params or {}),**kw))
    for name,params in BASE.items():
        add(name,meta=True);add(name)
        for helper in ['hash data:0x12','txncreate id:helper;txnlist','not_a_command','']:
            add(name,helper=helper)
        add(name,runner=name+' invalid:true')
        for field in list(params)+(['burn','tokenid'] if name=='createfrom' else ['tokenid'] if name=='constructfrom' else ['id','mmr','mine'] if name=='postfrom' else ['id','post']):
            for value in [None,True,1,{},[],'','bad']:add(name,{field:value})
        for field in params:
            spec=params.copy();del spec[field];cases.append(dict(name=name,params=spec))
        for index in range(1,10):
            add(name,throwAt=index)
            for result in [dict(status=False,error='child rejected'),dict(status=False),dict(status=False,error=1),dict(status=True),dict(status=True,response=None),dict(status=True,response='wrong'),{},dict(status='true'),dict(status=1)]:
                add(name,resultAt=index,result=result)
    for token,burn in itertools.product(['0x00','0x0000','Mx200104JMFSAS','0x01'],['-1','0','0.1']):add('createfrom',dict(tokenid=token,burn=burn))
    for script in ['RETURN [hello]', 'RETURN "broken', 'RETURN TRUE;txnlist', '', 'é\n\t']:
        for name in ['createfrom','constructfrom']:add(name,dict(script=script))
    for coins,change in itertools.product(['',',,,','0x01,,0x02',' , ','\x00 0x01 \x1f','\u00a00x01\u00a0'],['-1','0','0.1']):add('constructfrom',dict(coinlist=coins,changeamount=change))
    for name in ['postfrom','signfrom']:
        for null in [False,True]:
            for data in ['$ROW','0x','0x00','0x000000FF','bad']:add(name,dict(data=data),nullDB=null)
        add(name,existing='imported');add(name,dict(id='collision'),existing='collision')
    for field,value in [('privatekey',None),('keyuses','bad'),('post',True)]:add('signfrom',{field:value},expectRetained=True)
    add('postfrom',dict(mine=True),expectRetained=True)
    add('postfrom',dict(mmr=True),expectNoInsert=True)
    for result in [dict(status=False),dict(status=True,response=None),dict(status=True,response='wrong'),dict(status=True,response={})]:add('signfrom',dict(post='true'),resultAt=1,result=result,expectSwallow=True)
    # Actual child command integration, including ignored proof-builder failures.
    for change in ['-1','0','2']:
        add('constructfrom',dict(coinlist=',,,',changeamount=change),real=True,expectExport=True)
    add('constructfrom',dict(coinlist='0x01'),real=True,expectCleanFailure=True)
    add('createfrom',real=True,expectEmptyExport=True)
    add('postfrom',real=True,expectClean=True)
    add('postfrom',dict(mmr='true'),real=True,expectClean=True)
    add('signfrom',dict(privatekey='0x12',keyuses='0'),real=True,expectSignature=True)
    add('signfrom',dict(privatekey=None),real=True,expectRetained=True)
    for name in ['postfrom','signfrom']:
        for value in [None,True,1,{},[],'','replacement']:
            add(name,dict(data='0x',id=value),expectIDOrder=value is True or value==1)
    add('createfrom',runner='createfrom fromaddress:0x01 address:0x02 amount:1 script:"RETURN TRUE"',recordRunner=True,resultAt=2,result=dict(status=False),expectNullError=True)
    for error in ['before\0after','','é\n\t']:
        add('createfrom',resultAt=2,result=dict(status=False,error=error))
        add('createfrom',runner='createfrom fromaddress:0x01 address:0x02 amount:1 script:"RETURN TRUE"',recordRunner=True,resultAt=2,result=dict(status=False,error=error),expectError=error)
    return cases

def normalize(output):
    ids=[]
    for call in output.get('calls',[]):
        if call['text'].startswith('txncreate id:'):
            uid=call['text'][13:];assert re.fullmatch(r'0x[0-9A-F]{64}',uid),uid
            if uid not in ids:ids.append(uid)
    def visit(value,key=''):
        if isinstance(value,list):return [visit(x,key) for x in value]
        if isinstance(value,dict):return {k:visit(v,k) for k,v in value.items()}
        if isinstance(value,str):
            for i,uid in enumerate(ids):
                if key in ['wire','data'] and value.startswith('0x00000042'):
                    data=bytes.fromhex(value[2:]);length=int.from_bytes(data[:4],'big');assert length==66
                    if data[4:4+length]==uid.encode():value='0x'+(data[:4]+('0x'+f'{i:064X}').encode()+data[4+length:]).hex().upper()
                value=value.replace(uid,f'<ID:{i}>')
            for exception in ['java.lang.NullPointerException','java.lang.ClassCastException']:
                if value.startswith(exception):return exception
                wrapped='org.minima.system.commands.CommandException: '+exception
                if key=='exception' and value.startswith(wrapped+': '):return wrapped
        return value
    return visit(copy.deepcopy(output))

def validate(case,out):
    if case.get('expectRetained'):assert len(out['rows'])==1 and 'exception' in out,out
    if case.get('expectNoInsert'):assert out['rows']==[] and out['calls']==[],out
    if case.get('expectCleanFailure'):assert 'exception' in out and out['rows']==[],out
    if case.get('expectClean'):assert out['rows']==[] and out['value']['status'] is True,out
    if case.get('expectSwallow'):assert out['value']['response']['post'] is True and len(out['calls'])==3,out
    if case.get('expectEmptyExport'):
        assert out['rows']==[] and out['value']['response']['txn']['transaction']['inputs']==[] and out['value']['response']['txn']['transaction']['outputs']==[],out
    if case.get('expectExport'):
        response=out['value']['response'];assert out['rows']==[] and response['data'].startswith('0x00000042'),out
        expected=2 if float(case['params']['changeamount'])>0 else 1
        assert len(response['txn']['transaction']['outputs'])==expected,out
    if case.get('expectIDOrder'):assert out['exception'].startswith('java.lang.ClassCastException'),out
    if 'expectError' in case:assert out['value'][0]['error']==case['expectError'] and out['logs']==[],out
    if case.get('expectNullError'):assert out['value'][0]['status'] is False and out['value'][0]['error'] is None and out['logs']==[],out
    if 'export' in out:
        assert out['export']['wire']==out['value']['response']['data'] and out['export']['consumed']==len(bytes.fromhex(out['export']['wire'][2:])),out
    if case.get('expectSignature'):
        assert out['rows']==[] and out['value']['response']['post'] is False,out
        assert out['export']['signatures']==[True],out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--group',choices=['wrappers','raw','custom','consolidate'],default='wrappers');args=ap.parse_args();build=args.build.resolve();prefix={'raw':'raw-transaction-commands','custom':'custom-address-send-commands','consolidate':'consolidate-address-commands','wrappers':'wallet-wrappers'}[args.group];oracle=build/(prefix+'-oracle');oracle.mkdir(exist_ok=True)
    listing=build/(prefix+'-java-sources.txt');listing.write_text('\n'.join('"'+str(p)+'"' for p in sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/WalletWrappersOracle.java'])+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    if args.group=='raw':
        from raw_transaction_corpus import corpus as raw_corpus,validate as raw_validate
        cases=raw_corpus()
    elif args.group=='custom':
        from custom_address_send_corpus import corpus as custom_corpus,validate as custom_validate
        cases=custom_corpus()
    elif args.group=='consolidate':
        from consolidate_address_corpus import corpus as consolidate_corpus,validate as consolidate_validate
        cases=consolidate_corpus()
    else:cases=corpus()
    payload='\n'.join(json.dumps(case) for case in cases)+'\n'
    def run(command):
        result=subprocess.run(command,input=payload,capture_output=True,text=True,timeout=240,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=result.stdout.splitlines()
        if result.returncode or len(lines)!=len(cases):
            (build/(prefix+'-crash.txt')).write_text(result.stderr+'\n'+result.stdout);raise RuntimeError(f'exit {result.returncode}, {len(lines)} lines, expected {len(cases)}')
        return [json.loads(line) for line in lines]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','WalletWrappersOracle']);actual=run([str(build/'wallet_wrappers')]);failures=[]
    (build/(prefix+'-observations.json')).write_text(json.dumps(dict(cases=cases,java=expected,cpp=actual)))
    for case,j,n in zip(cases,expected,actual):
        try:
            validate(case,j);validate(case,n)
            if args.group=='raw':raw_validate(case,j);raw_validate(case,n)
            if args.group=='custom':custom_validate(case,j);custom_validate(case,n)
            if args.group=='consolidate':consolidate_validate(case,j);consolidate_validate(case,n)
        except (AssertionError,KeyError,TypeError):failures.append(dict(case=case,java=j,cpp=n,assertion=True));continue
        if normalize(j)!=normalize(n):failures.append(dict(case=case,java=j,cpp=n))
    report=dict(programs=len(cases),passed=len(cases)-len(failures),child_calls=sum(len(o['calls']) for o in expected),failures=failures,scope='Actual four inherited wrapper bodies; recorded child text/order and rows; failure injection; real offline child execution and exported TxnRow bytes. Only shape-checked random32byte rowID field and matching text normalized; NPE/CCE prose normalized. Full node and durable SQL are absent.')
    if args.group=='raw':report['scope']='Actual rawfrom/rawtxnfrom inherited bodies, metadata, null/type gates, exact ordered child text and row wire, state conversion, cleanup on throws versus ignored statuses; real child execution and full exported TxnRow roundtrip. Known random32byte IDs alone normalized in text and verified serialized ID fields; NPE/CCE prose normalized. No live node, durable SQL or funded network acceptance.'
    if args.group=='custom':report['scope']='Actual sendfrom/createtokenfrom bodies, metadata, parameter/error order, child failures, real stored output replacement, token JSON and wire, cleanup, and public runner/helper execution. Controlled funding/posting replies are fixture inputs; real-child cases exercise offline failure paths. Only checked random IDs and NPE/CCE wording normalized. No live node or durable SQL.'
    if args.group=='consolidate':report['scope']='Actual consolidatefrom main and public helper bodies, real coin search/age/mempool filtering, supplied helper coin order, child text and stored row bytes, typed/null parameters, failures and cleanup. Selected real input/output/script children. Only verified random IDs and specified runtime exception prose normalized. No live node, durable SQL or network acceptance.'
    (build/(prefix+'-differential-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report|{'failures':failures[:3]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
