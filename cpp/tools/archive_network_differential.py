#!/usr/bin/env python3
"""Archive request framing, traffic and unshortened retries on loopback."""
import argparse,json,shutil,struct,subprocess,tempfile,time
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner,Responder

def frame(payload):return (struct.pack('>i',len(payload))+payload).hex()

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');build=parser.parse_args().build.resolve()
    oracle=build/'archive-network-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/n for n in ['ArchiveCommandOracle.java','NIOClientOracle.java','ArchiveCacheOracle.java']]
    listing=build/'archive-network-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ArchiveCommandOracle'],response_timeout=90),Runner([str(build/'archive_command_runtime')],response_timeout=90)]
    server=Responder();checks=[];failures=[];stage='encodings'
    try:
        with tempfile.TemporaryDirectory(prefix='archive-network-',dir=build) as scratch:
            root=Path(scratch);folders=[]
            for runtime,runner in enumerate(runners):
                folder=root/str(runtime);folder.mkdir();folders.append(folder)
                row=runner.ask(dict(op='seed',folder=str(folder),heights=[]));assert row.get('seeded'),row
            def ask(runtime,**request):return runners[runtime].ask(dict(folder=str(folders[runtime]),**request))
            encoded=[ask(i,op='encodeReply',type=99) for i in [0,1]];assert encoded[0]==encoded[1],encoded
            valid=encoded[0]['wire'];ibd=encoded[0]['ibd'];raw=bytes.fromhex(valid[2:]);payload=raw[4:]
            starts=['-1','0','1','1.25','4294967296','12345678901234567890'];requests={}
            for start in starts:
                wires=[ask(i,op='encodeRequest',start=start) for i in [0,1]];assert wires[0]==wires[1],wires;requests[start]=wires[0]['wire'][2:].lower()
            cases=[]
            def add(name,replies,success,attempts=1,start='0',expected_connections=None,read_bytes=0,**flags):
                connections=len(replies) if expected_connections is None else expected_connections
                cases.append(dict(name=name,replies=replies,success=success,attempts=attempts,start=start,connections=connections,read_bytes=read_bytes,flags=flags))
            for start in starts:
                for type in [0,99,255]:add('height/type',[frame(bytes([type])+payload[1:])],True,start=start,read_bytes=len(payload))
            add('trailing bytes',[frame(payload+b'\x01\x02')],True,read_bytes=len(payload)+2)
            for reply in [None,'00','000000','ffffffff','7fffffff',frame(b''),frame(b'\x00'),valid[2:-2]]:
                size=len(bytes.fromhex(reply)) if reply else 0
                complete=size>=4 and struct.unpack('>i',bytes.fromhex(reply[:8]))[0]==size-4
                add('malformed',[reply],False,read_bytes=size-4 if complete else 0)
            for flag in ['noMain','noNetwork','noNIO']:add(flag,[valid],False,read_bytes=0,**{flag:True})
            for attempts in [-1,0]:add('no attempts',[],False,attempts=attempts)
            add('null start',[],False,start=None,expected_connections=0)
            add('default attempts first success',[valid],True,attempts=None,read_bytes=len(payload))
            add('retry succeeds',[None,valid],True,attempts=2,read_bytes=len(payload))
            add('retry exhausted',[None,None],False,attempts=2)
            add('default third attempt',[None,None,valid],True,attempts=None,read_bytes=len(payload))
            for index,case in enumerate(cases):
                stage=f'{index}: {case["name"]}';values=[];durations=[]
                for runtime in [0,1]:
                    server.configure(case['replies']);request=dict(op='request',start=case['start'],host='127.0.0.1',port=server.port,**case['flags'])
                    if case['attempts'] is not None:request['attempts']=case['attempts']
                    before=time.monotonic();row=ask(runtime,**request);duration=time.monotonic()-before;trace=server.trace();durations.append(round(duration,3))
                    assert 'fixtureError' not in row and 'exception' not in row,row
                    assert len(trace)==case['connections'],dict(case=case,trace=trace,row=row)
                    if trace:assert trace==[requests[case['start']]]*case['connections'],trace
                    assert row['ibd']==(ibd if case['success'] else None),row
                    assert row['read']==0 and row['write']==0,row
                    written=0 if case['flags'] else sum(len(bytes.fromhex(t))-4 for t in trace)
                    expected={'reads':{'sendArchiveReq':f'{case["read_bytes"]} bytes'} if case['read_bytes'] or case['name']=='malformed' and case['replies']==[frame(b'')] else {},'writes':{'sendArchiveReq':f'{written} bytes'} if written else {}}
                    assert row['traffic']==expected,dict(row=row,expected=expected)
                    retries=max(0,case['connections']-1);assert 10*retries<=duration<10*retries+30,dict(duration=duration,case=case)
                    values.append(dict(row=row,trace=trace))
                assert values[0]==values[1],values
                checks.append(dict(case=index,name=case['name'],requests=case['connections'],success=case['success'],seconds=durations))
                if case['connections']>1:print('Verified retry: '+json.dumps(checks[-1]),flush=True)
    except Exception as error:failures.append(dict(stage=stage,error=repr(error)))
    finally:
        server.close()
        for runner in runners:
            try:runner.close()
            except Exception as error:failures.append(dict(close_error=repr(error)))
    report=dict(passed=not failures,cases=len(checks),checks=checks,failures=failures,scope='Real loopback sendArchiveReq in pinned Java/native archive: exact request frames, ignored response type, decoded empty IBD, trailing/malformed frames, missing runtime receivers, payload-only traffic, zero attempts and actual 10-second retries including default third attempt. OS/JVM exception log prose, delayed read/connect timeouts, cascaded/nonempty IBD over this helper and full parity remain unverified.')
    (build/'archive-network-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)

if __name__=='__main__':raise SystemExit(main())
