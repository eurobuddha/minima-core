#!/usr/bin/env python3
"""Actual manager discovery through the existing controlled TCP responder."""
import argparse,json,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA,op,address
from p2p_manager_differential import process,canonical
from peers_checker_differential import Runner,Responder

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'p2p-discovery-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/name for name in ['NIOClientOracle.java','P2PFunctionsOracle.java','PeersCheckerOracle.java','P2PManagerOracle.java']];listing=build/'p2p-discovery-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','P2PManagerOracle']);native=Runner([str(build/'minima_p2p_manager')]);server=Responder()
    try:
        def encode(extra,kind=12):return java.ask(dict(encode=True,version='1.1.2.8',extra=extra,type=kind))
        one=encode({'peers-list':['8.8.8.8:9001']});missing=encode({})
        specs=[('empty',encode({'peers-list':[]})),('one',one),('multiple',encode({'peers-list':['8.8.8.8:9001','8.8.4.4:9001','8.8.8.8:9001']})),('missing',missing),('null',encode({'peers-list':None})),('wrong-type',encode({'peers-list':17})),('missing-port',encode({'peers-list':['bad']})),('invalid-port',encode({'peers-list':['8.8.8.8:65536']})),('partial-list',encode({'peers-list':['8.8.8.8:9001','bad']})),('wrong-element',encode({'peers-list':[17]})),('empty-host',encode({'peers-list':[':9001']})),('ignored-type',encode({'peers-list':['8.8.8.8:9001']},99))]
        corpus=[]
        for name,reply in specs:
            for kind in ['P2P_INIT','P2P_LOOP']:
                corpus.append((name+'-'+kind,dict(discovery=True,defaults=[address('127.0.0.1',server.port)],ops=[process(kind),op('have'),op('status',full=True)]),[reply]*3))
        for flag in ['discovery-off','no-connect','stopped','no-defaults']:
            request=dict(discovery=flag!='discovery-off',noconnect=flag=='no-connect',defaults=[] if flag=='no-defaults' else [address('127.0.0.1',server.port)],ops=([op('shutdown')] if flag=='stopped' else [])+[process('P2P_INIT'),op('have')])
            corpus.append((flag,request,[one]*3))
        corpus.append(('missing-then-success',dict(discovery=True,defaults=[address('127.0.0.1',server.port)],ops=[process('P2P_INIT')]),[missing,one]))
        # Retain actual five-second pauses in both implementations. A dropped
        # response followed by success proves retry; three drops prove the cap.
        corpus.append(('drop-then-success',dict(discovery=True,defaults=[address('127.0.0.1',server.port)],ops=[process('P2P_INIT')]),[None,one]))
        corpus.append(('three-drops',dict(discovery=True,defaults=[address('127.0.0.1',server.port)],ops=[process('P2P_INIT')]),[None]*3))
        pings=0;counts={};operations=0
        for i,(name,request,replies) in enumerate(corpus):
            server.configure(replies);left=java.ask(request);java_trace=server.trace();server.configure(replies);right=native.ask(request);native_trace=server.trace()
            if canonical(left)!=canonical(right) or java_trace!=native_trace or not isinstance(left,list) or len(left)!=len(request['ops']):
                (build/'p2p-discovery-failure.json').write_text(json.dumps(dict(case=i,name=name,request=request,java=left,cpp=right,java_trace=java_trace,cpp_trace=native_trace),indent=2)+'\n');raise AssertionError(f'Case {i} ({name}); see {build}/p2p-discovery-failure.json')
            count=len(java_trace);counts[name]=count;pings+=count;operations+=len(request['ops'])
            if name.startswith(('one-','multiple-','ignored-type-')) or name in ['missing-then-success','drop-then-success']:
                assert left[0]['result']=='OK' and left[0]['state']['known'] and left[0]['verified'] and not left[0]['state']['discovery'],name
            if name.startswith(('empty-','one-','multiple-','ignored-type-','empty-host-')):assert count==1,(name,count)
            if name.startswith(('missing-P2P','null-P2P')) or name=='three-drops':assert count==3 and not left[0]['state']['discovery'],(name,count)
            if name in ['discovery-off','no-connect','stopped','no-defaults']:assert count==0,(name,count)
        assert counts['drop-then-success']==counts['missing-then-success']==2
        report=dict(passed=True,programs=len(corpus),operations=operations,tcp_pings=pings,attempts=counts,scope='Actual pinned Java and native P2PManager discovery, real NIOManager TCP ping, exact request bytes and traffic, known/verified peers, malformed replies, stopped/no-connect/discovery guards, three-attempt cap and real five-second failed-ping pauses. Native full Main/NetworkManager integration remains pending.')
        (build/'p2p-discovery-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
    finally:server.close();java.close();native.close()
if __name__=='__main__':main()
