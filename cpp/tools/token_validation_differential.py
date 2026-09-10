#!/usr/bin/env python3
"""Actual tokenvalidate: source signatures, token discovery and loopback HTTP."""
import argparse,copy,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from rpc_client_differential import Server,normalize as rpc_normalize
from sql_db_differential import normalize
from wallet_balance_corpus import preserve_text

def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00')|kw
def command(id,**kw):return op(name='tokenvalidate',params={'tokenid':id},replyText=True,**kw)

def corpus(runner,server):
    f=runner.ask({'kind':'tokenValidationKeys'});cases=[];descriptions=[]
    def add(name,**expected):descriptions.append((name if isinstance(name,str) else json.dumps(name,ensure_ascii=False,separators=(',',':')),expected))
    unsigned={'signature':{'signed':False},'web':{'webvalidate':False}}
    add({},response=unsigned)
    for field,value in itertools.product(['signature','signedby'],[None,True,1,{},[],'bad']):add({field:value},response=unsigned)
    good={'signature':f['signature'],'signedby':f['publickey']}
    sigvalid={'signed':True,'signedby':f['publickey'],'valid':True}
    add(good,response={'signature':sigvalid,'web':{'webvalidate':False}})
    add(good|{'signature':f['wrong']},signatureValid=False)
    add(good|{'signedby':'0x01'},signatureValid=False)
    add(good|{'signature':f['tooMany']},signatureValid=False,logContains='INVALID KEY found with 9 levels')
    for name in ['null','[]','true','17','"token"','plain token','{broken','']:
        add(name,error=True)
    for field,value in itertools.product(['signature','signedby'],[None,True,1,{},[],'','0x','0xGG','0X01',' 0x01 ']):
        add(good|{field:value},**({'signatureValid':False} if field=='signedby' and value in ['', '0x','0X01'] else {'error':True}))
    for value in [f['empty'],'0x00',f['signature'][:20],f['signature'][:-2]]:add(good|{'signature':value},error=True)
    add(good|{'signature':f['signature']+'CAFE'},signatureValid=True)
    add(good|{'signature':'0x0000'},error=True,exceptionContains='java.lang.NumberFormatException',noLogs=True)
    add(good|{'signature':'0x00FF'},error=True,logContains='ERROR reading MiniNumber - input too large or negative -1')
    add(good|{'signedby':'0x01','webvalidate':server.url+'/status/200'},webMode='match',signatureValid=False,webValid=True,requests=1)
    for value in [None,True,1,{},[],42]:add({'webvalidate':value},**({'webValid':False,'requests':0} if value is None else {'error':True,'requests':0}))
    for value in ['', 'bad URL','file:///unused-token-validation','ftp://127.0.0.1/unused']:
        add({'webvalidate':value},webValid=False,requests=0)
    for mode,body in [('match',None),('trim',None),('lines',None),('wrong',b'0xWRONG'),('case',None),('nbsp',None),('empty',b''),('unicode','π😄'.encode())]:
        for signature in [{},good,good|{'signature':f['wrong']}]:
            add(signature|{'webvalidate':server.url+'/status/200'},webMode=mode,body=body.hex() if body is not None else None,webValid=mode in ['match','trim','lines'],requests=1,signatureValid=signature.get('signature')==f['signature'] if signature else None)
    for path,count in [('/status/404',1),('/status/201',1),('/redirect/302?to=/status/200',2),('/chunked',1),('/drop',2)]:
        add(good|{'webvalidate':server.url+path},webMode='match',webValid=path.startswith(('/redirect','/chunked')),requests=count)
    for sig,expected in [(f['signature'][:-2],0),(f['wrong'],1),(f['empty'],0)]:
        add(good|{'signature':sig,'webvalidate':server.url+'/status/200'},webMode='match',requests=expected,**({'error':True} if expected==0 else {'webValid':True,'signatureValid':False}))
    rows=runner.ask(dict(kind='tokenValidationTokens',names=[n for n,_ in descriptions]))
    for (name,expected),token in zip(descriptions,rows):
        request=dict(ops=[op('tokenImports',tokens=[token['wire']]),command(token['id'])],token=token,**expected)
        cases.append(request)
    empty=rows[0]
    cases.append(dict(ops=[op('tokenImports',tokens=[]),op('meta',name='tokenvalidate')],metadata=True))
    for id in ['0x00','0x0000','0X00','0xFF',empty['id'].lower(),empty['id'][2:],None,True,1,{},[],'',' '] :
        cases.append(dict(ops=[op('tokenImports',tokens=[empty['wire']]),command(id)],error=True,requests=0))
    cases.append(dict(ops=[op('tokenImports',tokens=[]),op(name='tokenvalidate',params={},replyText=True)],error=True))
    for id in ['0x00',empty['id']]:cases.append(dict(ops=[op('tokenImports',tokens=[None]),command(id)],error=True))
    # Same token obtained from the actual tree, and repeated imported copies.
    for token in [rows[0],rows[13]]:
        node=copy.deepcopy(f['nodes'][0]);node.update(coins=[token['coin']],relevant=[])
        cases.append(dict(ops=[op('tokenImports',tokens=[]),op('chain',history='8',nodes=[node]),command(token['id'])]))
        cases.append(dict(ops=[op('tokenImports',tokens=[token['wire']]*3),command(token['id'])]))
    for caller,text in itertools.product(['0x00','0x1234'],['tokenvalidate tokenid:'+empty['id'],'tokenvalidate tokenid:0x00','tokenvalidate','tokenvalidate invalid:true','tokenvalidate tokenid:0x00;tokenvalidate tokenid:'+empty['id']]):
        cases.append(dict(ops=[op('tokenImports',tokens=[empty['wire']]),op('run',text=text,caller=caller,replyText=True)]))
    return cases

def project(raw,request):
    result=preserve_text(raw,normalize(raw),request)
    for row,step in zip(result['ops'],request['ops']):
        if step.get('replyText') and 'value' in row:
            parsed=json.loads(row['value']);replies=parsed if isinstance(parsed,list) else [parsed];changed=False
            for reply in replies:
                if not isinstance(reply,dict):continue
                if isinstance(reply.get('error'),str):
                    value=normalize(reply['error']);changed|=value!=reply['error'];reply['error']=value
                web=reply.get('response',{}).get('web',{})
                reason=web.get('reason','');prefix='Could not download web file : '
                if reason.startswith(prefix):
                    value=prefix+rpc_normalize({'exception':reason[len(prefix):]})['exception'];changed|=value!=reason;web['reason']=value
            if changed:row['value']=json.dumps(parsed,ensure_ascii=False,separators=(',',':'))
    return result

def validate(request,result,trace):
    final=result['ops'][-1]
    if 'requests' in request:assert len(trace)==request['requests'],(len(trace),request['requests'])
    for row in trace:
        assert row['method']=='GET' and not row['body'] and row['headers'].get('user-agent')=='Minima/1.0',row
        assert 'authorization' not in row['headers'],row
    if request.get('metadata'):
        assert final['value']['name']=='tokenvalidate' and final['value']['valid']==['tokenid'],final
    if 'exceptionContains' in request:assert request['exceptionContains'] in final.get('exception',''),final
    if request.get('noLogs'):assert final['logs']==[],final['logs']
    if 'logContains' in request:assert any(request['logContains'] in x for x in final['logs']),final['logs']
    if request.get('error'):assert 'exception' in final,final;return
    if 'value' not in final:return
    if not isinstance(final['value'],str):return
    reply=json.loads(final['value'])
    if isinstance(reply,list):return
    response=reply['response'];assert reply['status'] is True,reply
    if 'response' in request:assert response==request['response'],response
    if request.get('signatureValid') is not None:assert response['signature']['valid'] is request['signatureValid'],response
    if 'webValid' in request:assert response['web']['valid'] is request['webValid'],response
    if 'logContains' in request:assert any(request['logContains'] in x for x in final['logs']),final['logs']

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'token-validation-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['SqlDBOracle.java','WalletCommandsOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java','TokenValidationOracle.java']]
    listing=build/'token-validation-java-sources.txt';listing.write_text('\n'.join('"'+str(x)+'"' for x in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=requests=0;cases=[];server=Server()
    with tempfile.TemporaryDirectory(prefix='token-validation-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TokenValidationOracle',root]),Runner([str(build/'token_validation_runtime'),root])]
        try:
            cases=corpus(runners[0],server)
            for i,request in enumerate(cases):
                token=request.get('token',{}).get('id','');mode=request.get('webMode','match');body=token.encode()
                if request.get('body') is not None:body=bytes.fromhex(request['body'])
                elif mode=='trim':body=b'\x00\t '+body+b' \x1f'
                elif mode=='lines':body=body[:8]+b'\r\n'+body[8:]+b'\n'
                elif mode=='case':body=token.lower().encode()
                elif mode=='nbsp':body=b'\xc2\xa0'+body+b'\xc2\xa0'
                server.server.body_override=body;output=[];traces=[]
                for runtime,runner in zip(['java','cpp'],runners):
                    try:
                        value=project(runner.ask(request),request);trace=server.trace();validate(request,value,trace);output.append(value);traces.append(trace)
                    except Exception as error:failures.append(dict(case=i,runtime=runtime,error=repr(error),request=request,stderr=runner.errors()));break
                if failures:break
                if output[0]!=output[1] or traces[0]!=traces[1]:failures.append(dict(case=i,request=request,java=output[0],cpp=output[1],java_requests=traces[0],cpp_requests=traces[1]));break
                completed+=1;operations+=len(request['ops']);requests+=len(traces[0])
        except Exception as error:failures.append(dict(phase='setup',error=repr(error)))
        finally:
            for runtime,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
            server.close()
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,http_requests_per_runtime=requests,failures=failures,scope='Actual tokenvalidate, source-generated signatures/tokens, TreeKey, imported/tree discovery and real RPCClient against loopback HTTP. Exact replies/log bodies and request traces except existing enhanced NPE/CCE and selected URL/transport-provider diagnostics. No durable storage, public web traffic, arbitrary TLS/provider/platform or full-node parity certification.')
    (build/'token-validation-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
