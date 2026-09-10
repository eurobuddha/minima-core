#!/usr/bin/env python3
"""Real HTTPServer/CMDHandler responses through the native and pinned command runners."""
import argparse,base64,itertools,json,re,socket,subprocess,time
from pathlib import Path
from urllib.parse import quote_plus
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def corpus():
    cases=[]
    def add(name,wire,**options):cases.append(dict(name=name,wire=wire,**options))
    def request(method,path,headers='',body=b'',eol='\r\n'):
        return (method+' '+path+' HTTP/1.1'+eol+headers+eol).encode()+body
    commands=['','hash data:hello','hash data:0x0011 type:sha2','convert from:string to:hex data:"π 😄"',
        'convert from:hex to:string data:0xCF80','maths calculate:"1+2"','zz_rpc_unknown',
        'hash','hash invalid:x','hash data:x;hash data:y','hash data:x;zz_rpc_unknown;hash data:z',
        'convert from:string to:hex data:{"x":[1,"π"]}']
    for method,eol,command in itertools.product(['GET','get','HEAD','PUT'],['\n','\r\n','\r'],commands):
        add('url-command',request(method,'/'+quote_plus(command)+'/',eol=eol))
    for path in ['/','//','///','/hash+data:x/','/%20hash%20data:x%20/','/%2Fhash+data:x%2F',
        '/hash%2520data:x','/hash+data:%00x','/hash+data:%C0%80','/hash+data:%ED%A0%80',
        '/hash+data:%F0%9F%98%84','/hash+data:%E2%82','/%','/%A','/%GG','/%0g','/%+1','/%-1','/%Ｆ１',
        '/%20','/hash+data:x?hello=1','/hash+data:x#fragment']:
        add('path',request('GET',path))
    for raw in [b'',b'\r\n',b'GET\r\n\r\n',b'GET /hash+data:x',b'\tGET\t/hash+data:x\fHTTP/1.1\r\n\r\n',
        b'GET\x0b/hash+data:x HTTP/1.1\r\n\r\n',b'GET /hash+data:\xff HTTP/1.1\r\n\r\n',
        b'GET /hash+data:x HTTP/1.1\r\n   \t\r\nignored']:
        add('request-line',raw)
    for command,eol,method in itertools.product(commands,['\n','\r\n','\r'],['POST','post']):
        body=command.encode();count=len(command.encode('utf-16-be'))//2
        add('post',request(method,'/ignored',f'Content-Length: {count}{eol}',body,eol))
    for body in [b'hash data:x',b'hash data:\xff',b'hash data:\xe2\x82',b'hash data:\xe0\x80',
        'hash data:π😄'.encode(),b'hash data:x\x00',b'hash data:%41',b'hash data:x\r\ny',b'\n\rhash data:x',b'']:
        count=len(body.decode('utf-8',errors='replace').encode('utf-16-be'))//2
        for length in sorted({0,1,max(0,count-1),count,count+1,count+5}):
            add('body-length',request('POST','/hash+data:fallback',f'Content-Length: {length}\r\n',body))
    for length in ['-1','-2147483648','2147483648','1.0','x','','+0','0000',' 2 ','٢','+٢']:
        add('length-header',request('POST','/',f'Content-Length: {length}\r\n',b'xx'))
    for headers in ['content-length: 11\r\n','CONTENT-LENGTH: 11\r\n','X-Content-Length: 11\r\n',
        'X: Content-Length: 11\r\n','Content-Length: 2\r\nContent-Length: 11\r\n',
        'Content-Length: 11\r\nContent-Length: 2\r\n']:
        add('header-matching',request('POST','/',headers,b'hash data:x'))
    for credentials in ['minima:test-password','minima:wrong','minima:','alice:test-password','alice:wrong','bob:test-password:two','π:😄','no-colon']:
        auth=base64.b64encode(credentials.encode()).decode()
        for label in ['Authorization:','authorization:','X-Authorization:']:
            add('authentication',request('GET','/hash+data:auth',label+' Basic '+auth+'\r\n'))
    for headers in ['Authorization: Basic bWluaW1hOnRlc3QtcGFzc3dvcmQ=\r\nAuthorization: nope\r\n',
        'Authorization: nope\r\nAuthorization: Basic bWluaW1hOnRlc3QtcGFzc3dvcmQ=\r\n',
        'Authorization: Basic ???\r\n','Authorization: Bearer none\r\n']:
        add('duplicate-auth',request('GET','/hash+data:auth',headers))
    # Force scalar and CRLF splits across TCP writes, including the 8192-byte reader boundary.
    for body in ['hash data:π😄'.encode(),b'hash data:'+b'x'*8190+'😄'.encode(),b'hash data:\xe0\x80x']:
        count=len(body.decode('utf-8',errors='replace').encode('utf-16-be'))//2
        add('fragmented',request('POST','/',f'Content-Length: {count}\r\n',body),fragmented=True)
    return cases

def transact(port,case):
    result=b''
    with socket.create_connection(('127.0.0.1',port),timeout=5) as client:
        data=case['wire']
        if case.get('fragmented'):
            # Small requests split at every byte; larger requests split around their tail.
            parts=[data[i:i+1] for i in range(len(data))] if len(data)<200 else [data[:-8]]+[data[i:i+1] for i in range(len(data)-8,len(data))]
            for part in parts:client.sendall(part);time.sleep(.001)
        else:client.sendall(data)
        client.shutdown(socket.SHUT_WR)
        while True:
            try:chunk=client.recv(65536)
            except ConnectionResetError:break # Closing with unread request bytes may reset the TCP stream.
            if not chunk:break
            result+=chunk
    dates=re.findall(rb'(?m)^Date: ([^\r\n]*)\r?$',result)
    if result.startswith(b'HTTP/1.1 200'):
        assert len(dates)==1 and re.fullmatch(rb'[A-Z][a-z]{2} [A-Z][a-z]{2} [ 0-9][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} UTC [0-9]{4}',dates[0]),dates
        separator=b'\r\n\r\n' if b'\r\n' in result.split(b'\n',1)[0]+b'\n' else b'\n\n'
        header,body=result.split(separator,1);length=int(re.search(rb'Content-length: ([0-9]+)',header)[1]);assert len(body)==length+1 and body.endswith(b'\n')
    return re.sub(rb'(?m)^Date: [^\r\n]*',b'Date: <clock>',result)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'cmd-handler-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/HTTPServerOracle.java'];listing=build/'cmd-handler-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-Duser.timezone=UTC','-cp',f'{oracle}:{JAVA}/jar/minima.jar','HTTPServerOracle'];native=[str(build/'minima_http_server')]
    users=[dict(username='alice',password='test-password',mode='read'),dict(username='bob',password='test-password:two',mode='write'),dict(username='π',password='😄',mode='read')]
    configurations=[dict(authenticate=False,crlf=False,users=[],style='basic'),dict(authenticate=False,crlf=True,users=[],style='basic'),dict(authenticate=True,crlf=False,users=users,style='basic'),dict(authenticate=True,crlf=True,users=users,style='basic'),dict(authenticate=False,crlf=True,users=users,style='basic'),dict(authenticate=True,crlf=False,users=users,style='digest')]
    cases=corpus();tested=0;outcomes=set();failures=[]
    for program,configuration in enumerate(configurations):
        runners=[Runner(java),Runner(native)]
        try:
            ready=[r.ask(dict(op='create',port=0,auto=True,rpc=True,**configuration)) for r in runners];assert all(isinstance(row,dict) and row['bound'] for row in ready),ready
            for index,case in enumerate(cases):
                # Fragment delivery can be cut short by expected early authentication rejection.
                current={**case,'fragmented':case.get('fragmented',False) and not configuration['authenticate'] and not configuration['users']}
                responses=[transact(row['bound'],current) for row in ready];tested+=1;outcomes.add(responses[0].split(b'\n',1)[0].strip().decode())
                if responses[0]!=responses[1]:
                    failures.append(dict(program=program,case=index,name=case['name'],request=case['wire'].hex(),java=responses[0].decode(errors='replace'),cpp=responses[1].decode(errors='replace')))
                    if len(failures)>=10:break
            statuses=[r.ask(dict(op='status',completed=index+1)) for r in runners]
            if not all(isinstance(row,dict) for row in statuses):
                (build/'cmd-handler-fixture-failure.json').write_text(json.dumps(dict(program=program,case=index,statuses=statuses,failures=failures,stderr=[r.errors() for r in runners]),indent=2)+'\n')
                raise AssertionError(f'Handler status failure; see {build}/cmd-handler-fixture-failure.json')
            # Authorizer and command logger bindings are separately tracked gaps. Here
            # compare handler log count/order by failure site, plus exact short-read logs.
            def handler_logs(row):
                output=[]
                for message in row['logs']:
                    if message.startswith('CMDHANDLER : Read wrong amount'):output.append(message)
                    elif message.startswith('ERROR CMDHANDLER :'):output.append('command-exception')
                    elif message.startswith('CMDHANDLER :'):
                        output.append('authentication-failure' if 'Invalid Authentication at RPC' in message else 'request-exception')
                return output
            if handler_logs(statuses[0])!=handler_logs(statuses[1]):failures.append(dict(program=program,java_logs=handler_logs(statuses[0]),cpp_logs=handler_logs(statuses[1])))
            for runner in runners:assert runner.ask(dict(op='shutdown'))['finished']
        finally:
            for runner in runners:runner.close()
        if failures:break
    report=dict(passed=not failures,configurations=program+1,requests=tested,outcomes=sorted(outcomes),failures=failures,scope='Actual pinned/native HTTPServer and CMDHandler over real TCP, actual CommandRunner, URL decoding, malformed UTF-8, UTF-16 POST lengths, truncated/zero-padded bodies, CR/LF/CRLF lines, case-sensitive and duplicate headers, basic auth and configured users, exact response bytes/lengths except wall-clock Date value, fragmented multibyte input and reader boundary. Handler log failure sites/order and exact short-read logs compared; complete exception text, Authorizer/CommandRunner logger binding, unpaired-surrogate command parameters, quit command/Main lifecycle and arbitrary concurrent configuration mutation remain under review.')
    (build/'cmd-handler-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:2]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
