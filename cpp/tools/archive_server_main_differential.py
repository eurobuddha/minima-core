#!/usr/bin/env python3
"""Standalone Java/native ArchiveServer CLI and real MySQL archive socket service."""
import argparse,json,os,re,socket,subprocess,tempfile,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Responder
from horst_differential import data,number,raw,hx

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--fixture',type=Path,required=True);args=ap.parse_args();build=args.build.resolve();state=json.loads(args.fixture.read_text());start=time.monotonic()
    assert state['address'].startswith('127.0.0.1:') and state['database']=='minima_cpp' and state['name'].startswith('minima-cpp-mysql-')
    oracle=build/'archive-server-oracle';seed=build/'archive-server-seed';seed.mkdir(exist_ok=True)
    if not (oracle/'org/minima/utils/mysql/ArchiveServer.class').exists():raise RuntimeError('Run archive_server_differential.py first')
    cp=f'{oracle}:{JAVA}/lib/mysql-connector-java-8.0.24.jar:{JAVA}/jar/minima.jar';subprocess.run(['javac','-cp',cp,'-d',str(seed),str(ROOT/'support/ArchiveServerSeed.java')],check=True)
    commands=[['java','-cp',cp,'org.minima.utils.mysql.ArchiveServer'],[str(build/'minima_archive_server')]]
    env={k:v for k,v in os.environ.items() if k not in ['MYSQL_HOST','MYSQL_DB','MYSQL_USER','MYSQL_PASSWORD','ARCHIVE_PORT']};env.update(TZ='UTC',UBSAN_OPTIONS='halt_on_error=1');failures=[];cli=[];servers=[];checks=0
    def check(ok,label):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(label)
    def projected(result):
        lines=[line.split('] : ',1)[-1] for line in result.stdout.splitlines()]
        error=re.search(r'java\.lang\.[A-Za-z]+(?:Exception|Error)(?::[^\n]*)?',result.stderr)
        return dict(status=result.returncode,lines=lines,exception=error.group(0) if error else None)
    try:
        cases=[(['-help'],{}),(['-HELP'],{}),(['unknown','-help'],{}),(['-port','0','-help'],{}),(['-port','１２','-help'],{}),(['-mysqlhost','ignored','-mysqluser','ignored','-mysqlpassword','synthetic','-mysqldb','ignored','-help'],{})]
        for key in ['-port','-mysqlhost','-mysqluser','-mysqlpassword','-mysqldb']:cases.append(([key],{}))
        for value in ['', ' 1','2147483648','-2147483649','1.5']:cases.append((['-port',value,'-help'],{}))
        cases.extend([(['-help'],dict(ARCHIVE_PORT='bad')),(['-port','0','-help'],dict(ARCHIVE_PORT='bad')),(['-help'],dict(ARCHIVE_PORT='8888'))])
        for argv,extra in cases:
            results=[subprocess.run(command+argv,env=env|extra,capture_output=True,text=True,timeout=20) for command in commands]
            values=list(map(projected,results));check(values[0]==values[1],dict(args=argv,environment=extra,java=values[0],cpp=values[1]));check(values[0]['status']==1,'source CLI exit status');check('Starting Archive Server v1.3' in values[0]['lines'],'driver load/start order')
            check(not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|runtime error:',results[1].stderr),'CLI sanitizer diagnostics');cli.append(dict(args=argv,environment=extra,result=values[0]))
        seeded=subprocess.run(['java','-cp',f'{seed}:{cp}','ArchiveServerSeed',state['address'],state['database'],state['user'],state['password']],env=env,capture_output=True,text=True,timeout=30);check(seeded.returncode==0,seeded.stderr);fixtures=json.loads(seeded.stdout.splitlines()[-1])
        for runtime,command in zip(['java','cpp'],commands):
            with socket.socket() as reservation:reservation.bind(('127.0.0.1',0));port=reservation.getsockname()[1]
            settings=dict(MYSQL_HOST=state['address'],MYSQL_DB=state['database'],MYSQL_USER=state['user'],MYSQL_PASSWORD=state['password'],ARCHIVE_PORT='0')
            with tempfile.TemporaryFile(mode='w+t') as output,tempfile.TemporaryFile(mode='w+t') as errors:
                child=subprocess.Popen(command+['-port',str(port)],env=env|settings,stdin=subprocess.PIPE,stdout=output,stderr=errors);clients=[]
                try:
                    deadline=time.monotonic()+20
                    while time.monotonic()<deadline:
                        output.seek(0);logs=output.read()
                        if 'Server started on port : '+str(port) in logs:break
                        if child.poll() is not None:errors.seek(0);raise RuntimeError(logs+errors.read())
                        time.sleep(.02)
                    else:raise TimeoutError('Archive listener startup')
                    check('Cascade found.. tip:2' in logs,'constructor reads cached Java-written cascade')
                    observations=[];sent=time.monotonic()
                    for first,kind in [(-1,255),(0,0),(2,15),(4,16)]:
                        client=socket.create_connection(('127.0.0.1',port),timeout=20);clients.append(client);client.sendall(data(bytes([kind])+number(first)))
                        prefix=Responder.read(client,4);size=int.from_bytes(prefix,'big',signed=True);check(0<=size<1024*1024,'bounded fixture reply');reply=prefix+Responder.read(client,size)
                        blocks=[] if first==-1 else fixtures['blocks'][max(first,0):];expected=data(bytes([16,int(first==0)])+(raw(fixtures['cascade']) if first==0 else b'')+number(len(blocks))+b''.join(raw(block) for block in blocks))
                        check(reply==expected,dict(first=first,expected=hx(expected),reply=hx(reply)));observations.append(dict(first=first,type=kind,reply=hx(reply)))
                    for client in clients:check(client.recv(1)==b'','source delayed socket closure')
                    check(time.monotonic()-sent>=9.99,'actual ten-second delay before close')
                    child.stdin.write(b'unknown\n \n\nquit\n');child.stdin.flush();child.stdin.close();child.wait(timeout=20);check(child.returncode==0,'standalone graceful exit')
                    output.seek(0);logs=output.read();errors.seek(0);stderr=errors.read();check('Unknown command : unknown' in logs and 'Unknown command : \n' in logs and 'Archive Server stopped' in logs,'source console trimming and stop logs')
                    if runtime=='cpp':check(not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|LeakSanitizer|runtime error:|DEADLYSIGNAL|SUMMARY:',stderr),'complete native sanitizer diagnostics')
                    servers.append(dict(runtime=runtime,requests=observations,stdout=logs,stderr=stderr,graceful_exit=True));print('Standalone '+runtime+' archive server passed',flush=True)
                finally:
                    for client in clients:client.close()
                    if child.poll() is None:child.kill();child.wait(timeout=10)
    except Exception as error:failures.append(dict(error=repr(error)))
    report=dict(passed=not failures,cli_programs=len(cli),server_processes=len(servers),independent_assertions=checks,cli=cli,servers=servers,failures=failures,seconds=round(time.monotonic()-start,3),scope='Actual unmodified pinned Java ArchiveServer and native executable, explicit compiled-driver loading, CLI help/error/environment order, Java-written real MySQL blocks/cascade, actual TCP archive replies and ten-second closes, console trim/quit and process shutdown. Dynamic log prefixes/ports and stack frames excluded from CLI comparison; exact exception class/message and status retained. Live fixture is synthetic, localhost-only, tmpfs. No arbitrary SQL faults, GC timing, full subclass/platform/network parity claim.')
    (build/'archive-server-main-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
