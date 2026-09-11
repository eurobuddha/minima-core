#!/usr/bin/env python3
"""Pinned ArchiveServer with actual codecs/SQL bodies, socket I/O and real waits."""
import argparse,hashlib,json,re,struct,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
from p2p_functions_differential import ROOT,JAVA
from sql_db_differential import normalize
from horst_differential import number,data,raw,hx

def project(value):
    value=normalize(value);value.pop('milliseconds',None)
    # Stream adapters expose different EOF prose, not different exception classes.
    def clean(v):
        if isinstance(v,list):return [clean(x) for x in v]
        if isinstance(v,dict):return {k:clean(x) for k,x in v.items()}
        if isinstance(v,str):return re.sub(r'java.io.EOFException(?::.*)?$', 'java.io.EOFException',v)
        return v
    return clean(value)

def corpus(fixtures):
    cases=[]
    def add(op,**values):cases.append(dict(op=op,rows=[],columns=[])|values)
    for last in [0,9223372036854775807,-9223372036854775808]:add('clean',last=last)
    add('constructor')
    for fail in range(1,25):
        for runtime in [False,True]:add('constructor',fail=fail,runtimeFailure=runtime)
    for first in ['0',None]:add('retry',first=first)
    for fail in range(1,5):
        for runtime in [False,True]:add('retry',first='7',fail=fail,runtimeFailure=runtime)
    for frame in ['', '00','000000','FFFFFFFF','20000001','00000000','000000010F','000000020F00','000000030F0000','000000030F0021','000000030F00FF']:
        add('handler',frame=frame)
    add('handler',frame='',nullSocket=True)
    add('handler',frame='',closed=True)
    add('handler',frame=hx(data(bytes([15])+number(0))),nullRange=True)
    for first,kind,cascade,rows in [(-1,0,True,[]),(0,255,True,[]),(2,15,False,[dict(syncdata=fixtures['blocks'][1]),dict(syncdata=fixtures['blocks'][2])]),(-2,16,False,[])]:
        expected=data(bytes([16,int(cascade and first==0)])+(raw(fixtures['cascades'][1]) if cascade and first==0 else b'')+number(0 if first==-1 else len(rows))+b''.join(raw(r['syncdata']) for r in ([] if first==-1 else rows)))
        add('handler',frame=hx(data(bytes([kind])+number(first)+b'ignored-trailing-bytes')),cascade=cascade,rows=rows,expected=hx(expected),minimum_ms=10000)
    # A decoded null entry is retained; IBD serialization must reject it.
    add('handler',frame=hx(data(bytes([15])+number(1))),rows=[dict(syncdata='0x00')])
    return cases

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--clean-audit',action='store_true');args=ap.parse_args();build=args.build.resolve();oracle=build/'archive-server-oracle';oracle.mkdir(exist_ok=True);start=time.monotonic()
    names=['SqlDBOracle.java','MySQLConnectOracle.java','ArchiveManagerOracle.java','NIOClientOracle.java','ObjectOracle.java','ArchiveServerOracle.java'];sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in names];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{JAVA}/jar/minima.jar';fixture=Runner(['java','-cp',cp,'MySQLConnectOracle']);fixtures=fixture.ask(dict(locale='en_US',ops=[]));fixture.close()
    runners=[Runner(['java','-cp',cp,'ArchiveServerOracle'],response_timeout=60),Runner([str(build/'archive_server_runtime')],response_timeout=60)];failures=[];cases=checks=0;transcript=hashlib.sha256()
    def check(ok,message):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(message)
    try:
        selected=[dict(op='clean',rows=[],columns=[],last=v) for v in [0,9223372036854775807,-9223372036854775808]]+[dict(op='clean',rows=[],columns=[],last=0,reentrant=True)] if args.clean_audit else corpus(fixtures)
        for index,request in enumerate(selected):
            values=[r.ask(request) for r in runners];cases+=1
            for v in values:
                if 'expected' in request:
                    check(v.get('reply')==request['expected'],'independent complete IBD frame')
                    check(v.get('closed') is True,'successful handler explicitly closes socket')
                    check(v['milliseconds']>=request['minimum_ms']-10,'actual source ten-second sleep')
                if request['op']=='clean':
                    check(v['changed']==(request['last']==0),'clean signed elapsed threshold')
                    if request.get('reentrant'):check(v['logs']==['System Clean..','System Clean..'],'source reentrant monitor permits logger callback')
                if request['op']=='handler' and request.get('frame') in ['', '00','000000','FFFFFFFF','20000001','000000010F','000000020F00','000000030F0000','000000030F0021','000000030F00FF'] and not request.get('nullSocket') and not request.get('closed'):
                    check(v['logs']==[] and not v['closed'] and v['reply']=='','malformed frame quiet return retains socket')
                if request.get('nullRange'):check(v['closed'] and 'System Clean..' not in v['logs'],'null range closes without clean')
            left,right=map(project,values)
            if left!=right:raise AssertionError(dict(case=index,request=request,java=left,cpp=right))
            transcript.update(json.dumps([request,left],sort_keys=True,separators=(',',':')).encode())
            (build/('archive-server-clean-progress.json' if args.clean_audit else 'archive-server-progress.json')).write_text(json.dumps(dict(cases=cases,independent_assertions=checks,failures=[]),indent=2)+'\n')
        check(runners[1].errors()=='','native stderr empty')
    except Exception as error:failures.append(dict(cases=cases,error=repr(error)))
    finally:
        for r in runners:
            try:r.close()
            except Exception as error:failures.append(dict(cleanup=repr(error)))
    report=dict(passed=not failures,cases=cases,independent_assertions=checks,failures=failures,seconds=round(time.monotonic()-start,3),transcript_sha256=transcript.hexdigest(),scope='Actual pinned ArchiveServer constructor/handler/retry/clean and native class. Reuses recording SQL and actual codecs; exact events/logs/framed bytes, actual socket I/O, ignored type/trailing bytes, cascade/probe/ranges, malformed input/explicit close, real ten-second waits and one-second retry. Java TCP socket pairs and native local socket pairs; constructor listeners actual TCP. NPE/ClassCast/EOF prose normalized and stack frames excluded. No real MySQL standalone main, complete GC/subclass/concurrency/resource/platform parity claim.')
    if args.clean_audit:report.update(clean_audit=True,scope='Four actual Java/native SystemClean programs: signed elapsed overflow/no-clean branches and a logger Main receiver that re-enters the synchronized method. Exact logs, changed timestamp state, native stderr and reentrant completion; no handler/SQL/socket/sleep coverage is claimed by this focused audit.')
    (build/('archive-server-clean-audit-results.json' if args.clean_audit else 'archive-server-results.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
