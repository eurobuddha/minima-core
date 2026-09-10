#!/usr/bin/env python3
"""Actual RPC command enable/restart/disable and credentials over loopback."""
import argparse,base64,json,socket,subprocess,time
from pathlib import Path
from urllib.parse import quote_plus
from peers_checker_differential import Runner
from cmd_handler_differential import transact
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'

def exercise(runner,crlf):
    observations=[];port=0;requests=0;pauses=[]
    def bridge(op,pending=False,pause=False,**kw):
        nonlocal port
        result=runner.ask(dict(op=op,**kw));elapsed=result.pop('elapsed');assert elapsed>=0,result
        if pause:assert elapsed>=990,(kw,elapsed);pauses.append(elapsed)
        bound=result['state']['bound'];assert isinstance(bound,int) and 0<=bound<=65535,result
        if op=='wait':assert bound>0,result;port=bound
        result['state']['bound']='<starting>' if pending else '<bound>' if bound else 0
        observations.append(dict(op=op,result=result));return result
    def request(command,credentials=None,status=200,pause=False,success=True):
        nonlocal requests
        auth='' if credentials is None else 'Authorization: Basic '+base64.b64encode(credentials.encode()).decode()+'\r\n'
        wire=('GET /'+quote_plus(command)+' HTTP/1.1\r\n'+auth+'\r\n').encode()
        before=time.monotonic();response=transact(port,dict(wire=wire));elapsed=(time.monotonic()-before)*1000
        if pause:assert elapsed>=990,(command,elapsed);pauses.append(elapsed)
        assert response.startswith(f'HTTP/1.1 {status} '.encode()),(command,response)
        if status==200:
            separator=b'\r\n\r\n' if b'\r\n\r\n' in response else b'\n\n'
            reply=json.loads(response.split(separator,1)[1]);assert reply['status']==success,(command,reply)
        observations.append(dict(op='http',command=command,response=response.decode()));requests+=1
    def closed():
        try:
            with socket.create_connection(('127.0.0.1',port),timeout=.5):pass
        except OSError:return
        raise AssertionError('Stopped RPC port still accepts connections')
    bridge('configure',crlf=crlf);bridge('command',text='rpc')
    bridge('command',text='rpc enable:true password:first',pending=True,pause=True);bridge('wait')
    request('hash data:live',status=401);request('hash data:live','minima:wrong',401)
    request('hash data:live','minima:first')
    bridge('client',path=quote_plus('hash data:client'),user='minima',password='first')
    request('rpc action:adduser username:alice password:alpha mode:read','minima:first')
    request('hash data:alice','alice:alpha');request('hash data:alice','alice:wrong',401)
    # Source CMDHandler does not propagate the stored read mode to CommandRunner.
    request('rpc action:adduser username:bob password:beta mode:write','alice:alpha')
    request('rpc action:listusers','alice:alpha')
    request('rpc action:removeuser username:alice','minima:first')
    request('hash data:alice','alice:alpha',401);request('hash data:bob','bob:beta')
    request('rpc enable:true password:second','minima:first',pause=True);bridge('wait')
    request('hash data:rotated','minima:first',401);request('hash data:rotated','minima:second')
    bridge('client',path=quote_plus('hash data:retained-user'),user='bob',password='beta')
    bridge('replaceDB');request('hash data:old-db-user','bob:beta',401)
    request('rpc action:adduser username:bob password:beta mode:write','minima:second')
    request('rpc enable:true','minima:second',pause=True);bridge('wait')
    request('hash data:header-still-required',status=401);request('hash data:extra-user-disabled','bob:beta',401)
    request('rpc action:removeuser username:bob','minima:anything')
    request('hash data:open')
    request('rpc enable:false',pause=True);assert bridge('state')['state']['bound']==0;closed()
    bridge('command',text='rpc enable:true password:third',pending=True,pause=True);bridge('wait')
    request('rpc enable:true password:""','minima:third',pause=True,success=False)
    failed=bridge('state')['state'];assert failed['bound']==0 and failed['enabled'] and failed['authenticate'] and failed['password']=='third',failed;closed()
    bridge('command',text='rpc enable:true',pending=True,pause=True);bridge('wait');request('hash data:restarted')
    request('rpc enable:false password:final',pause=True);final=bridge('state')['state'];assert final==dict(bound=0,enabled=False,authenticate=True,password='final',users=[]),final;closed()
    return observations,dict(requests=requests,pauses=len(pauses),minimum_pause_ms=min(pauses),maximum_pause_ms=max(pauses))

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve();oracle=build/'rpc-live-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/MainNetworkOracle.java',ROOT/'support/RPCLiveOracle.java']
    listing=build/'rpc-live-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','RPCLiveOracle'],[str(build/'rpc_live')]];failures=[];measurements=[]
    for crlf in [False,True]:
        results=[]
        for command in commands:
            runner=Runner(command)
            try:output,timing=exercise(runner,crlf);results.append(output);measurements.append(dict(crlf=crlf,runtime='java' if command[0]=='java' else 'cpp',**timing))
            except Exception as error:failures.append(dict(crlf=crlf,runtime=command[0],error=repr(error),stderr=runner.errors()));break
            finally:runner.close()
        if failures:break
        if results[0]!=results[1]:failures.append(dict(crlf=crlf,java=results[0],cpp=results[1]))
    report=dict(passed=not failures,lifecycles=2,observations_per_lifecycle=len(results[0]) if results else 0,measurements=measurements,failures=failures,scope='Actual rpc command through manual Main and real NetworkManager/HTTPServer/CMDHandler/UserDB/Authorizer: enable, users, password rotation, header behavior, same-handler restart/disable, failure after stop, and actual RPCClient requests. Exact HTTP bytes except validated Date; ephemeral bound ports and async startup state validated separately before normalization. Real one-second pauses. Full node workers, durable SQL/preferences, TLS and exhaustive concurrency remain outside this fixture.')
    (build/'rpc-live-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:1]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
