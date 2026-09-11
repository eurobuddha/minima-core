#!/usr/bin/env python3
"""Original two-start/two-stop NIOServer main and its four interrupt boundaries."""
import argparse,fcntl,json,os,queue,socket,subprocess,tempfile,threading,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
MAIN=['Started.. waiting..','Stopping..','Started again.. waiting..','Stopping..']
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'nio-server-main-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOServerMainOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOServerMainOracle'],[str(build/'nio_server_main')]]
    report=dict(passed=False,comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def listening(pid):
        p=subprocess.run(['lsof','-nP','-a','-p',str(pid),'-iTCP:9002','-sTCP:LISTEN','-Fn'],capture_output=True,text=True,timeout=3)
        if p.returncode not in [0,1]:raise RuntimeError(p.stderr)
        return any(line.startswith('n') for line in p.stdout.splitlines())
    def run(command,stage,occupied=False):
        started=time.monotonic();p=subprocess.Popen(command+[str(stage)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        messages=queue.Queue();rows=[];caught=False;port=None
        def read():
            for line in p.stdout:messages.put(line)
            messages.put(None)
        reader=threading.Thread(target=read,daemon=True);reader.start()
        try:
            while True:
                line=messages.get(timeout=20)
                if line is None:break
                row=json.loads(line);rows.append(row)
                if row['event']=='caught':
                    caught=True;deadline=time.monotonic()+5;expected=stage in [1,3]
                    while True:
                        port=listening(p.pid)
                        if port==expected:break
                        if time.monotonic()>deadline:raise AssertionError(('listener after interruption',stage,port,expected))
                        time.sleep(.02)
                    # Let the real hello worker publish its source cleanup failure.
                    expected_workers=1 if stage<=2 else 2
                    while sum(r['event']=='uncaught' for r in rows)<expected_workers:
                        follow=messages.get(timeout=5)
                        if follow is None:raise AssertionError('child exited before worker evidence')
                        rows.append(json.loads(follow))
                    p.stdin.write('\n');p.stdin.flush()
            code=p.wait(timeout=5);stderr=p.stderr.read();check(not stderr,('unexpected stderr',stderr))
            logs=[r['value'] for r in rows if r['event']=='log'];ordered=[s for s in logs if s in MAIN]
            workers=[r['value'] for r in rows if r['event']=='uncaught'];worker_logs=[s for s in logs if 'NIO_SERVERSTARTED' in s]
            result=dict(code=code,main=ordered,worker_logs=worker_logs,workers=workers,caught=[r['value'] for r in rows if r['event']=='caught'],hooks=[r['value'] for r in rows if r['event']=='hook'],listener_after_catch=port)
            elapsed=time.monotonic()-started
            check(not any(r['event']=='returned' for r in rows),rows)
            if occupied:
                check(code==0 and not caught and result['hooks']==[],result);check(any('ALREADY IN USE. SHUTTING DOWN' in s for s in logs),logs)
            else:
                n=stage if stage else 4;worker_count=1 if stage in [1,2] else 2
                check(ordered==MAIN[:n],result);check(len(worker_logs)==worker_count,result);check(workers==['hello:java.lang.NullPointerException']*worker_count,result)
                check(result['caught']==(['java.lang.InterruptedException:false'] if stage else []),result);check(code==(7 if stage else 0),result);check(result['hooks']==([] if stage else ['exit']),result)
                check(elapsed>=(stage-1)*2 if stage else elapsed>=8,('source sleeps shortened',stage,elapsed))
            return result,dict(rows=rows,seconds=elapsed)
        finally:
            if p.poll() is None:p.kill();p.wait()
            for stream in [p.stdin,p.stdout,p.stderr]:stream.close()
            reader.join(timeout=1)
    try:
        # Both builds share the source's fixed demonstration port. Serialize only
        # this fixture across processes; never stop an existing listener.
        with open(Path(tempfile.gettempdir())/'minimacorecpp-nio-main-9002.lock','w') as lock:
            fcntl.flock(lock,fcntl.LOCK_EX)
            with socket.socket(socket.AF_INET6,socket.SOCK_STREAM) as probe:
                probe.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0);probe.bind(('::',9002))
            for stage in range(5):
                results=[run(command,stage) for command in commands];check(results[0][0]==results[1][0],results);report['comparisons']+=1;report['observations'].append(dict(stage=stage,java=results[0],cpp=results[1]))
            with socket.socket(socket.AF_INET6,socket.SOCK_STREAM) as occupied:
                occupied.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0);occupied.bind(('::',9002));occupied.listen(1)
                results=[run(command,0,True) for command in commands];report['observations'].append(dict(occupied=True,java=results[0],cpp=results[1]));report['comparisons']+=1
            report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Actual original NIOServer.main on port 9002 with four unchanged 2000-ms waits, two real server starts/stops, default missing-DB hello worker failures, successful System.exit shutdown hook, each sleep interrupted from actual logger boundary, surviving/closed listener observed without connecting, and occupied-port Runtime.halt bypassing hook. Null unused arguments supplied. Fixtures halt after interrupted caller evidence to clean up only their own child. No user database or existing node touched; all broader server/thread API parity unverified.'
        (build/'nio-server-main-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
