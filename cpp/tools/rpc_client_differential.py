#!/usr/bin/env python3
"""Actual pinned RPC client against controlled HTTP/TLS loopback peers."""
import argparse,base64,itertools,json,ssl,subprocess,threading
from pathlib import Path
from http.server import BaseHTTPRequestHandler,ThreadingHTTPServer
from urllib.parse import urlsplit,parse_qs
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

class Handler(BaseHTTPRequestHandler):
    protocol_version='HTTP/1.1'
    def setup(self):super().setup();self.connection.settimeout(3)
    def log_message(self,*args):pass
    def handle_request(self):
        body=self.rfile.read(int(self.headers.get('Content-Length','0')))
        headers={k.lower():v for k,v in self.headers.items()}
        with self.server.lock:self.server.requests.append(dict(method=self.command,path=self.path,headers=headers,body=body.hex()))
        parsed=urlsplit(self.path);query=parse_qs(parsed.query,keep_blank_values=True);path=parsed.path;status=200;location=None
        content=base64.b64decode(query.get('body',['YWxwaGENCmJldGEKZ2FtbWENZGVsdGE='])[0])
        if path=='/drop':self.close_connection=True;return
        if path=='/loop':status=302;location='/loop'
        elif path=='/proxy' and hasattr(self.server,'proxy_url'):status=305;location=self.server.proxy_url
        elif path.startswith('/status/'):status=int(path.rsplit('/',1)[-1])
        elif path.startswith('/redirect/'):
            status=int(path.rsplit('/',1)[-1]);location=query.get('to',['/status/200'])[0]
        if status in [204,304]:content=b''
        self.send_response(status)
        if location is not None:self.send_header('Location',location)
        chunked=path=='/chunked';truncated=path=='/truncated';self.close_connection=True
        if chunked:self.send_header('Transfer-Encoding','chunked')
        else:self.send_header('Content-Length',str(len(content)+(4 if truncated else 0)))
        self.send_header('Connection','close');self.end_headers()
        try:
            if chunked:
                for value in [content[:1],content[1:7],content[7:]]:
                    if value:self.wfile.write(f'{len(value):X}\r\n'.encode()+value+b'\r\n')
                self.wfile.write(b'0\r\n\r\n')
            else:self.wfile.write(content)
        except (BrokenPipeError,ConnectionResetError):pass
    do_GET=do_PUT=do_POST=handle_request
class Server:
    def __init__(self,tls=None,handler=Handler):
        self.server=ThreadingHTTPServer(('127.0.0.1',0),handler);self.server.daemon_threads=True;self.server.lock=threading.Lock();self.server.requests=[]
        if tls:self.server.socket=tls.wrap_socket(self.server.socket,server_side=True)
        self.url=('https' if tls else 'http')+f'://127.0.0.1:{self.server.server_port}'
        self.thread=threading.Thread(target=self.server.serve_forever,daemon=True);self.thread.start()
    def trace(self):
        with self.server.lock:result=self.server.requests[:];self.server.requests.clear();return result
    def close(self):self.server.shutdown();self.server.server_close();self.thread.join()

def cases(server,other,tls,ca):
    result=[]
    def add(method,path='/status/200',**kw):result.append(dict(method=method,url=server.url+path,user='alice',password='secret',data='a=π&b=😄\r\nnext\x00end',**kw))
    for method,status in itertools.product(['get','basic','put','post','postType','auth'],[200,201,204,206,300,304,308,400,401,403,404,407,410,500]):add(method,f'/status/{status}',type='application/json',token='dGVzdA==')
    from urllib.parse import quote
    for method,status,destination in itertools.product(['get','basic','put','post','postType','auth'],[300,301,302,303,304,306,307,308],['/status/200','/status/404',other.url+'/status/200',tls.url+'/status/200']):add(method,f'/redirect/{status}?to='+quote(destination,safe=''),type='application/json',token='dGVzdA==')
    for method,path in itertools.product(['get','basic','put','post','auth'],['/loop','/drop','/chunked','/truncated']):add(method,path,token='rawtoken')
    for body in [b'',b'\r\n',b'\xef\xbb\xbftext',b'\x00a\x00',b'\xc0\x80\xed\xa0\x80\xf0\x9f\x98\x84',b'\xe2\x82',b'a\rb\nc\r\nd',bytes(range(256))]:
        for method in ['get','put','post']:add(method,'/status/200?body='+quote(base64.b64encode(body).decode(),safe=''))
    for agent in [None,'','Minima/1.0','αé😄','ok\nbad','fold\n next']:
        for method in ['get','postType']:add(method,agent=agent,type='text/plain')
    for user,password in itertools.product([None,'','alice','é:😄'],[None,'','pass','😄:π']):result.append(dict(method='basic',url=server.url+'/status/200',user=user,password=password))
    for token in [None,'','abc','π😄','x\ny']:add('auth',token=token)
    for method,url in itertools.product(['get','basic','getHTTPS','getSSL','basicSSL','put','post','postType','postHTTPS','auth'],[None,'','not a URL','ftp://127.0.0.1/a',server.url+'/status/200']):result.append(dict(method=method,url=url,user=None,password=None,data=None,type=None))
    for method in ['get','getHTTPS','put','post','postHTTPS','basicSSL','getSSL']:
        result.append(dict(method=method,url=tls.url+'/status/200',data='secure',user='alice',password='secret',ca=str(ca) if method=='basicSSL' else None))
    for host in ['127.0.0.1','localhost']:
        result.append(dict(method='basicSSL',url=tls.url.replace('127.0.0.1',host)+'/chunked',user='π',password='😄',ca=str(ca)))
    for method in ['get','basic','post','put','auth']:
        for authority in ['ignored:secret@','ignored@','a%20b:p%40ss@']:
            result.append(dict(method=method,url=server.url.replace('://','://'+authority)+'/status/200',user='explicit',password='password',token='explicit',data='body'))
        for path in ['/a/../status/200','/status/200?x=%00&y=π','/status/200#fragment','/status/200?','/status/200 space']:
            result.append(dict(method=method,url=server.url+path,user='explicit',password='password',token='explicit',data='body'))
        for path in ['/proxy','/status/101','/status/200\x00ignored','/status/200\rignored','/status/200\nignored']:
            result.append(dict(method=method,url=server.url+path,user='explicit',password='password',token='explicit',data='body'))
    return result

def normalize(output):
    if 'exception' in output:
        kind=output['exception'].split(':',1)[0]
        if kind in ['java.lang.NullPointerException','java.lang.ClassCastException','java.net.MalformedURLException','java.net.ConnectException','java.net.SocketException','javax.net.ssl.SSLHandshakeException']:
            output={**output,'exception':kind}
    return output
def notify_integration(build,servers):
    origin=servers[0].url
    process=subprocess.run([str(build/'notify_http_worker'),origin],capture_output=True,text=True,timeout=30)
    if process.returncode:raise AssertionError(f'Native HTTP notification worker failed: {process.stderr}')
    logs=json.loads(process.stdout);trace=servers[0].trace()
    assert all(not server.trace() for server in servers[1:]),'Unexpected notification destination'
    expected=[('TEST','/status/200'),('TEST','/status/404'),('TEST','/status/200?last'),
              ('OTHER','/status/404'),('OTHER','/redirect/307?to=/status/200'),('OTHER','/status/200'),('OTHER','/status/200?last')]
    assert len(trace)==len(expected),(trace,expected)
    for request,(event,path) in zip(trace,expected):
        assert request['method']=='POST' and request['path']==path,request
        assert json.loads(bytes.fromhex(request['body']))==dict(event=event,body='π😄\r\n\x00end',listener=True),request
        assert request['headers']['content-type']=='application/x-www-form-urlencoded',request
    assert len(logs)==2,logs
    for log,event in zip(logs,['TEST','OTHER']):
        prefix=f'ERROR webhook : #{origin}/status/404 java.io.IOException: POST request not HTTP_OK resp:404 @ {origin}/status/404 params '
        assert log['notify'] is False and log['text'].startswith(prefix),log
        assert json.loads(log['text'][len(prefix):])==dict(event=event,body='π😄\r\n\x00end',listener=True),log
    return dict(events=2,requests=len(trace),logged_failures=len(logs))
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'rpc-client-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/RPCClientOracle.java'];listing=build/'rpc-client-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    folder=build/'rpc-client-tls';folder.mkdir(exist_ok=True);cert=folder/'cert.pem';key=folder/'key.pem'
    subprocess.run(['openssl','req','-x509','-newkey','rsa:2048','-nodes','-days','2','-keyout',str(key),'-out',str(cert),'-subj','/CN=localhost','-addext','subjectAltName=DNS:localhost,IP:127.0.0.1'],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    context=ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER);context.load_cert_chain(cert,key);servers=[Server(),Server(),Server(context)];servers[0].server.proxy_url=servers[1].url;runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','RPCClientOracle']),Runner([str(build/'minima_rpc_client')])];corpus=cases(*servers,cert);failures=[];requests=0;notification=None
    try:
        for index,request in enumerate(corpus):
            outputs=[];traces=[]
            for runner in runners:
                outputs.append(normalize(runner.ask(request)));traces.append([s.trace() for s in servers])
            if outputs[0]!=outputs[1] or traces[0]!=traces[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1],java_requests=traces[0],cpp_requests=traces[1]));break
            requests+=sum(len(trace) for trace in traces[0])
        if not failures:notification=notify_integration(build,servers)
    finally:
        for runner in runners:runner.close()
        for server in servers:server.close()
    report=dict(passed=not failures,cases=index+1,requests_per_runtime=requests,notification_integration=notification,failures=failures,scope='Actual Java RPCClient and native libcurl HTTP(S): request method/path/header values/body, response text/UTF8/line removal, status errors, same/cross host and protocol redirects, auth stripping and POST method/header reset, TLS rejection and configured CA verification. Native queued notification integration uses the production HTTP default, filtering, listener mutation, redirects and failure logging. Header order and selected VM/provider-generated diagnostic text normalized; custom global JDK properties/authenticator/proxy/cookie/cache and all native SSLContext providers remain unverified.')
    (build/'rpc-client-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
