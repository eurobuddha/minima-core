#!/usr/bin/env python3
"""Actual incentivecash, preference files and loopback PUT, compared to pinned Java."""
import argparse,itertools,json,subprocess,tempfile
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner
from rpc_client_differential import Server,Handler

ORIGIN='https://incentivecash.minima.global'
class IncentiveHandler(Handler):
    def send_response(self,code,message=None):
        # Reuse the existing request recorder, body writer and connection lifecycle.
        super().send_response(getattr(self.server,'status_override',code) if code==200 else code,message)

def op(kind='direct',**kw):return dict(op=kind,rows=[],probe=[],caller='0x00')|kw
def command(params=None,**kw):return op(name='incentivecash',params={} if params is None else params,**kw)
def corpus(server):
    cases=[]
    def add(*steps,body=b'{"rewards":12,"daily":[1,2],"message":"teaching"}',status=200,**expect):
        cases.append(dict(incentive=True,ops=[dict(route=server.url,**step) for step in steps],body=body.hex(),status=status,**expect))
    add(op('meta',name='incentivecash'),requests=0,metadata=True)
    add(command(),requests=0,uid='')
    add(command({'uid':''}),requests=0,uid='',saved=False)
    for uid in ['student-01','00d11b34-7b47-45f3-775c-a37cbe4c9ff3','abc/def','a?x=1','a#fragment','a&b=c','a%20b','../relative','a+b','a:b','a@b','quote"test','π😄',' space ','a\tvalue','a\rvalue','a\nvalue','a\x00value','a\\value']:
        add(command({'uid':uid}),op('incentiveLoad'),uid=uid.strip(' \t\r\n'),saved=True)
    for value in [None,True,False,0,17,[],{},[1],{'uid':'x'}]:
        add(command({'uid':value}),requests=0,uid='')
        add(op('incentivePrefs',values={'uid':value}),command(),requests=0)
        add(op('incentivePrefs',values={'uid':value}),command({'uid':'replaced'}),op('incentiveLoad'),requests=1,uid='replaced',saved=True)
    for body in [b'{}',b'null',b'[]',b'[{}]',b'true',b'false',b'12',b'1.25',b'"text"',b'',b' ',b'{broken',b'{} trailing',b'{"a":1,"a":2}',b'{"nested":{"a":[true,null,1.25]}}',b'\r\n{\r\n"x":\r1\n}\r\n',b'{"x":"a\r\nb"}',b'{"x":"\xff"}',b'\xef\xbb\xbf{}',b'{"x":"\xe2\x82"}',b'{"x":"\x00"}',b'{"exception":"java.lang.ClassCastException: literal user text"}']:
        add(command({'uid':'response-student'}),op('incentiveLoad'),body=body,requests=1,uid='response-student',saved=True)
    for status in [201,204,206,300,304,308,400,401,403,404,410,500,503]:
        add(command({'uid':'status-student'}),op('incentiveLoad'),body=b'',status=status,requests=1,uid='status-student',saved=True)
    add(command({'uid':'failure-student'},httpFail=True),op('incentiveLoad'),requests=0,uid='failure-student',saved=True)
    add(command({'uid':'first'}),command(),command({'uid':'second'}),command({'uid':''}),command(),op('incentiveLoad'),requests=4,uid='second',saved=True)
    for fail,save in itertools.product([False,True],[False,True]):
        add(command({'uid':'old'}),op('allowSave',allow=save),command({'uid':'new'},httpFail=fail),op('incentiveLoad'),requests=1 if fail else 2,uid='new',savedUID='new' if save else 'old')
    add(op('allowSave',allow=False),command({'uid':'unsaved'}),requests=1,uid='unsaved',saved=False)
    add(op('incentiveDirectory'),command({'uid':'blocked-save'}),requests=1,uid='blocked-save',saved=False,saveFailure=True)
    for caller,text in itertools.product(['0x00','0x1234'],['incentivecash','incentivecash uid:runner-student','incentivecash uid:','incentivecash invalid:true','incentivecash uid:{"bad":true}','incentivecash uid:runner-student;incentivecash','incentivecash uid:runner-student;incentivecash uid:"";incentivecash']):
        add(op('run',text=text,caller=caller))
    return cases

def exception(text):
    kind=text.split(':',1)[0]
    return kind if kind in ['java.lang.NullPointerException','java.lang.ClassCastException','java.net.MalformedURLException'] else text

def project(raw,request):
    # Only exception provider prose and the fixture's substituted HTTP origin vary.
    # Preference bytes, user strings and returned service data remain untouched.
    for row,step in zip(raw['ops'],request['ops']):
        if 'exception' in row:row['exception']=exception(row['exception'])
        row['logs']=[exception(line) for line in row['logs']]
        replies=row.get('value',[])
        if isinstance(replies,list):
            for reply in replies:
                if isinstance(reply,dict) and isinstance(reply.get('error'),str):reply['error']=exception(reply['error'])
        printed=[]
        for line in row['printed'].splitlines():
            if line.startswith('PUT request not HTTP_OK resp:'):
                marker=' @ '+step['route']
                if marker in line:line=line.replace(marker,' @ '+ORIGIN,1)
                printed.append(line)
        row['printed']=printed
    return raw

def decode_prefs(value):
    if value is None:return None
    data=bytes.fromhex(value[2:]);length=int.from_bytes(data[:4],'big');assert len(data)==length+4,(length,len(data))
    return json.loads(data[4:].decode())

def validate(request,result,trace):
    rows=result['ops'];final=rows[-1]
    if 'requests' in request:assert len(trace)==request['requests'],(trace,request['requests'])
    for event in trace:
        assert event['method']=='PUT' and event['body']=='',event
        assert event['headers']['user-agent']=='Minima/1.0' and 'authorization' not in event['headers'],event
    for row in rows:
        for call in row['incentiveCalls']:
            assert call['url']==ORIGIN+'/api/ping/'+call['user']['uid']+'?version=1.1.2',call
            assert call['user']==row['user'],row
            assert call['prefs']==row['prefs'],row
    if request.get('metadata'):assert final['value']['valid']==['uid'] and final['value']['name']=='incentivecash',final
    if 'uid' in request:assert final['user'].get('uid','')==request['uid'],final
    if request.get('saved') or 'savedUID' in request:
        saved=decode_prefs(final['prefs']);assert saved['uid']==request.get('savedUID',request.get('uid')),final
        assert final['value']==saved,final
    if request.get('saved') is False:assert final['prefs'] is None,final
    if request.get('saveFailure'):assert final['logs'],final
    # Assert the successful response shape and the no-request empty-ID behavior.
    for row,step in zip(rows,request['ops']):
        if step['op']=='direct' and 'value' in row:
            response=row['value']['response'];assert response['uid']==(row['user'].get('uid') or ''),row
            if not response['uid']:assert response=={'uid':''} and not row['incentiveCalls'],row
            else:
                parsed=json.loads(bytes.fromhex(request['body']).replace(b'\r',b'').replace(b'\n',b'').decode('utf-8',errors='replace'),strict=False)
                assert response['details']==parsed,row

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'incentive-cash-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['SqlDBOracle.java','WalletCommandsOracle.java','SearcherOracle.java','ArchiveCacheOracle.java','ConsensusOracle.java','IncentiveCashOracle.java']]
    listing=build/'incentive-cash-java-sources.txt';listing.write_text('\n'.join('"'+str(x)+'"' for x in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    failures=[];completed=operations=requests=0;server=Server(handler=IncentiveHandler);cases=corpus(server)
    with tempfile.TemporaryDirectory(prefix='incentive-cash-',dir=build) as root:
        runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','IncentiveCashOracle',root]),Runner([str(build/'incentive_cash_runtime'),root])]
        try:
            for index,request in enumerate(cases):
                server.server.body_override=bytes.fromhex(request['body']);server.server.status_override=request['status'];outputs=[];traces=[]
                for runtime,runner in zip(['java','cpp'],runners):
                    try:
                        value=project(runner.ask(request),request);trace=server.trace();validate(request,value,trace);outputs.append(value);traces.append(trace)
                    except Exception as error:failures.append(dict(case=index,runtime=runtime,error=repr(error),request=request,stderr=runner.errors()));break
                if failures:break
                if outputs[0]!=outputs[1] or traces[0]!=traces[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1],java_requests=traces[0],cpp_requests=traces[1]));break
                completed+=1;operations+=len(request['ops']);requests+=len(traces[0])
        finally:
            for runtime,runner in zip(['java','cpp'],runners):
                try:runner.close()
                except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
            server.close()
    report=dict(passed=not failures,programs=completed,total_programs=len(cases),operations=operations,http_requests_per_runtime=requests,failures=failures,scope='Actual incentivecash, UserDB file serialization/reload, MinimaDB save ordering, JSONParser and RPCClient PUT against loopback. Exact saved bytes, recorded original HTTPS URL, service response and request traces. Only enhanced NPE/CCE/malformed-URL provider prose and fixture HTTP origin in PUT diagnostics normalized. Public incentive service, TLS account interaction, durable SQL and full-node/API/reference/concurrency parity not certified.')
    (build/'incentive-cash-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
