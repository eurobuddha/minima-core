#!/usr/bin/env python3
"""Pinned command-line main vs native main; real loopback HTTP and stdin."""
import argparse,itertools,json,os,pty,re,select,subprocess,time
from pathlib import Path
from rpc_client_differential import Handler,Server
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA

class ReplyHandler(Handler):
    def handle_request(self):
        with self.server.lock:
            self.server.requests.append(dict(method=self.command,path=self.path,headers={k.lower():v for k,v in self.headers.items()},body=''))
            status,content=self.server.replies.pop(0) if self.server.replies else (200,b'{"status":true,"response":{"value":"ok"}}')
        self.send_response(status);self.send_header('Content-Length',str(len(content)));self.send_header('Connection','close');self.end_headers();self.close_connection=True
        try:self.wfile.write(content)
        except (BrokenPipeError,ConnectionResetError):pass
    do_GET=handle_request

def normalize_text(text,stack_stream=False):
    # Calendar/allocator prefixes and native-vs-JVM frame symbols differ. Require
    # real frames where an exception was printed, then compare its source text.
    result=[];frames=0
    for line in text.split('\n'):
        logged=line.startswith('Minima @ ')
        line=re.sub(r'^Minima @ .*? \[.*?\] : ', '', line)
        if (logged and line.startswith('     ')) or (stack_stream and line.startswith('\tat ')):
            frames+=1;continue
        for kind in ['java.lang.NullPointerException','java.lang.ClassCastException','java.net.MalformedURLException','java.net.ConnectException']:
            line=re.sub(re.escape(kind)+r':.*',kind,line)
        result.append(line)
    return '\n'.join(result),frames

def normalize(output):
    result=dict(output)
    for key in ['stdout','stderr']:
        result[key],count=normalize_text(result[key],stack_stream=key=='stderr');result[key+'Frames']=bool(count)
    if 'exception' in result:result['exception']=normalize_text(result['exception'])[0]
    return result

def configure(server,replies):
    with server.server.lock:server.server.replies=list(replies)

def corpus(url):
    result=[]
    def add(args,stdin=b'exit\n',replies=(),**extra):result.append((dict(args=args,input=stdin.hex(),**extra),list(replies)))
    for args in [[],None,[None],['-host'],['-username'],['-password'],['-command'],['-sslpubkey'],['-host',None],['-command',None]]:add(args)
    for user,password in itertools.product([None,'','π😄'],[None,'','pw']):add(['-host',url,'-username',user,'-password',password,'-command','help'])
    for command in ['help',' status ','a b+c/d?e=f&g#h%~!*_-.','π😄中文','\ud800','\udc00','\ud800x\udc00','a\x00b','\r\n\t',''.join(chr(i) for i in range(1,128)),'x'*8200]:
        add(['-host',url,'-command',command])
    for body in [b'{}',b'{"a":{"b":{"c":{"d":42}}}}',b'{"a":[1,true,null,{"b":"x\\\"y"}]}',b'{"z":0,"Aa":1,"BB":2}',b'[]',b'null',b'true',b'12',b'"text"',b'',b'{',b'wrong',b'{"a":"\x00"}',b'{"x":"\xed\xa0\x80"}']:
        add(['-host',url,'-command','status'],replies=[(200,body)])
        add(['-host',url],b'status\nexit\n',[(200,body)])
    for status in [201,204,301,401,404,500]:
        add(['-host',url,'-command','status'],replies=[(status,b'{}')])
        add(['-host',url],b'quit\nexit\n',[(status,b'{}')])
    for stdin in [b'exit\n',b'\n\r\n exit \r',b'   \nexit\n',b'quit\nignored\n',b'help\rstatus\r\nexit\n',b'\xc0\x80\xed\xa0\x80\xe2\x82\nexit\n',b'x'*8191+b'\xf0\x9f\x98\x84\nexit\n',b'\t\x00exit\x1f\n']:
        add(['-host',url],stdin)
    for option,value in [('-sslpubkey','dummy'),('-sslpubkey',None),('-password',''),('-password','π😄'),('-username',None)]:add(['-host',url,option,value])
    for readFail,closeFail in itertools.product([False,True],repeat=2):
        add(['-host',url],b'help\nexit\n',readFail=readFail,closeFail=closeFail)
    add(['-host',url+'/base/','-host',url,'-command','ignored','-command','help'])
    add(['-host',url],b'quit\nhelp\nexit\n',[(200,b'{'),(200,b'{}')])
    return result

def subprocess_checks(build,classpath,server):
    commands=[['java','-cp',classpath,'org.minima.utils.MinimaRPCClient'],[str(build/'minima-rpc')]]
    count=0
    for args,stdin in [(['-help'],b''),(['unknown'],b''),(['-command'],b''),(['-host',server.url,'-command',' status π😄 '],b''),(['-host',server.url],b'help\r\nexit\n'),(['-host',server.url],b'quit\nignored\n')]:
        outputs=[];traces=[]
        for command in commands:
            configure(server,[])
            process=subprocess.run(command+args,input=stdin,capture_output=True,timeout=10,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
            outputs.append((process.returncode,normalize_text(process.stdout.decode()),normalize_text(process.stderr.decode(),stack_stream=True)))
            traces.append(server.trace())
        # Number and spelling of frame entries are compiler/VM dependent.
        outputs=[(code,(out[0],bool(out[1])),(err[0],bool(err[1]))) for code,out,err in outputs]
        assert outputs[0]==outputs[1],(args,outputs)
        assert traces[0]==traces[1],(args,traces)
        count+=1
    # Input must be processed before EOF, and the source intentionally keeps
    # running after EOF. Bound and terminate these fixture processes explicitly.
    for command in commands:
        process=subprocess.Popen(command+['-host',server.url],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,bufsize=0)
        try:
            buffer=b'';deadline=time.monotonic()+10
            while b"To exit this app" not in buffer:
                assert time.monotonic()<deadline,'Interactive banner timeout'
                if select.select([process.stdout],[],[],.1)[0]:buffer+=os.read(process.stdout.fileno(),65536)
            process.stdin.write(b'help\n');process.stdin.flush()
            deadline=time.monotonic()+5
            while b'"status"' not in buffer:
                assert time.monotonic()<deadline,'Line blocked until EOF'
                if select.select([process.stdout],[],[],.1)[0]:buffer+=os.read(process.stdout.fileno(),65536)
            process.stdin.close();time.sleep(.15)
            assert process.poll() is None,'Source EOF loop changed'
            assert len(server.trace())==1
        finally:
            process.kill();process.wait(timeout=5);process.stdout.close();process.stderr.close()
    for command in commands:
        master,slave=pty.openpty()
        process=subprocess.Popen(command+['-host',server.url],stdin=slave,stdout=subprocess.PIPE,stderr=subprocess.PIPE,bufsize=0)
        os.close(slave)
        try:
            buffer=b'';deadline=time.monotonic()+10
            while b"To exit this app" not in buffer:
                assert time.monotonic()<deadline,'Terminal banner timeout'
                if select.select([process.stdout],[],[],.1)[0]:buffer+=os.read(process.stdout.fileno(),65536)
            os.write(master,b'\x04');time.sleep(.15)
            assert process.poll() is None,'Terminal EOF exited'
            os.write(master,b'exit\n')
            assert process.wait(timeout=5)==0,'Terminal did not resume after Ctrl-D'
            assert not server.trace(),'Exit unexpectedly sent RPC'
        finally:
            if process.poll() is None:process.kill();process.wait(timeout=5)
            os.close(master);process.stdout.close();process.stderr.close()
    return count+4

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'minima-rpc-cli-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/MinimaRPCClientOracle.java'];listing=build/'minima-rpc-cli-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    classpath=f'{oracle}:{JAVA}/jar/minima.jar';server=Server(handler=ReplyHandler)
    runners=[Runner(['java','-cp',classpath,'MinimaRPCClientOracle']),Runner([str(build/'minima_rpc_cli')])]
    failures=[];requests=0;process_cases=0;cases=corpus(server.url)
    try:
        for index,(request,replies) in enumerate(cases):
            outputs=[];traces=[]
            for runner in runners:
                configure(server,replies);outputs.append(normalize(runner.ask(request)));traces.append(server.trace())
            if outputs[0]!=outputs[1] or traces[0]!=traces[1]:failures.append(dict(index=index,request=request,java=outputs[0],cpp=outputs[1],javaRequests=traces[0],cppRequests=traces[1]));break
            requests+=len(traces[0])
        if not failures:process_cases=subprocess_checks(build,classpath,server)
    finally:
        for runner in runners:runner.close()
        server.close()
    report=dict(passed=not failures,cases=index+1,requests_per_runtime=requests,process_cases=process_cases,failures=failures,scope='Actual pinned main vs native main, real HTTP/auth/UTF8 form encoding, JSON/exception paths, banner and interactive trim/quit/close. Native process stdin decoding and pre-EOF response verified; EOF spin preserved. Heap/date prefixes, native frame symbols/count and VM-generated diagnostics normalized; frames must be present. Full node and full JVM runtime parity not established.')
    (build/'minima-rpc-cli-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='failures'},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
