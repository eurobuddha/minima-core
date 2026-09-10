#!/usr/bin/env python3
"""Actual ping command, Main ownership, nullable replies and isolated wire peer."""
import argparse,itertools,json,struct,subprocess,tempfile
from pathlib import Path
from main_runtime_differential import ROOT,JAVA,op,project
from peers_checker_differential import Runner
from nio_manager_transport_differential import ping,execute

def cases():
    result=[]
    def add(*ops):result.append(dict(ops=list(ops)))
    add(op('meta',name='ping'))
    values=[None,False,True,0,1,{},[], '', ' ', '\t\r', 'host', 'π 🐼', '127.0.0.1', ':', 'host:', 'host:abc', 'host:1:2', '[::1]:9001', 'host:2147483648', 'host:-2147483649', 'host:1.0', 'host:1e3', 'host:１２', 'host:١٢', 'host:𝟙', 'host:+12', 'host: -12 ', 'host:\u00a012', 'host:2147483647', 'host:-2147483648']
    # Any parseable port in this group sees missing Main/network before transport.
    for value,owner in itertools.product(values,['noMain','noNetwork']):
        add(op('clear' if owner=='noMain' else 'info'),op('direct',name='ping',params={'host':value}))
    for owner in ['main','noMain','noNetwork','noNIO']:
        prefix=[op('network',nullNetwork=owner=='noNetwork',nullNIO=owner=='noNIO'),op('clear' if owner=='noMain' else 'info')]
        for params in [{},{'host':'host'},{'host':'  host  '},{'host':'127.0.0.1:-1'},{'host':'127.0.0.1:65536'}]:
            add(*prefix,op('direct',name='ping',params=params),op('networkSnapshot'))
    for text,caller in itertools.product(['ping','ping host:host','ping host:host;hash data:0xAB','ping host:host:bad','ping host:{}','ping bad:true','ping host:127.0.0.1:-1;hash data:0xAB'],['0x00','0x1234']):
        add(op('network'),op('run',text=text,caller=caller))
    return result

def normalized(raw,request):
    value=project(raw,request)
    for row in value['ops']:
        # Socket and wire exception prose is platform-specific. Keep log count/order.
        row['logs']=['Error sending Single Ping message : <diagnostic>' if x.startswith('Error sending Single Ping message : ') else x for x in row['logs']]
    return value

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'ping-command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support'/x for x in ['MainRuntimeOracle.java','MainNetworkOracle.java','SqlDBOracle.java','NIOClientOracle.java','ArchiveManagerOracle.java','NIOManagerTransportOracle.java']]
    listing=build/'ping-command-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar'];failures=[];corpus=cases();checks=[]
    with tempfile.TemporaryDirectory(prefix='ping-command-',dir=build) as root:
        commands=[java+['MainRuntimeOracle',root],[str(build/'minima_main_runtime'),root]]
        runners=[Runner(command) for command in commands]
        try:
            for index,request in enumerate(corpus):
                outputs=[normalized(r.ask(request),request) for r in runners]
                if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]));break
                for step,row in zip(request['ops'],outputs[0]['ops']):
                    if step['cmd']=='direct' and step['params'].get('host') in ('host','  host  '):assert row['value'] is None and not row['logs'],row
                    if step['cmd']=='meta':assert row['value']['fresh'] and row['value']['valid']==['host'],row
        finally:
            for runner in runners:runner.close()
        if not failures:
            wire=bytes.fromhex(execute(java+['NIOManagerTransportOracle'],[dict(mode='create',type=12,kind='greeting')])[0]['result'][2:])[1:]
            version_end=4+struct.unpack('>i',wire[:4])[0];extra_end=version_end+4+struct.unpack('>i',wire[version_end:version_end+4])[0]
            def greeting(extra):
                raw=extra.encode();return wire[:version_end]+struct.pack('>i',len(raw))+raw+wire[extra_end:]
            def frame(body):return struct.pack('>i',len(body))+body
            fixtures=[('normal',frame(b'\x0c'+wire),False,{'welcome':'fixture'}),('fragmented',frame(b'\x0c'+wire),True,{'welcome':'fixture'}),('null-extra',frame(b'\x0c'+greeting('null')),False,None),('nested-extra',frame(b'\x0c'+greeting('{"x":[1,true,null,"π 🐼"]}')),False,{'x':[1,True,None,'π 🐼']}),('ignored-type',frame(b'\xff'+wire),False,{'welcome':'fixture'}),('trailing',frame(b'\x0c'+wire+b'ignored'),False,{'welcome':'fixture'}),('empty-frame',frame(b''),False,'invalid'),('type-only',frame(b'\x0c'),False,'invalid'),('truncated',struct.pack('>i',100)+b'\x0c',False,'invalid')]
            for name,reply,fragment,extra in fixtures:
                outputs=[]
                for command in commands:
                    request=None;port=None
                    def make_request(bound):
                        nonlocal request,port
                        port=bound;request=dict(ops=[op('network'),op('direct',name='ping',params={'host':f' 127.0.0.1 : {bound} '}),op('networkSnapshot')]);return request
                    raw=ping(command,reply,fragment=fragment,request=make_request)
                    output=normalized(raw,request);row=output['ops'][1];response=row['value']['response']
                    assert response['host']=='127.0.0.1' and response['port']==port,response
                    assert response['valid']==(extra!='invalid'),response
                    if extra!='invalid':assert response['extradata']==extra and isinstance(response['version'],str),response
                    else:assert 'extradata' not in response and 'version' not in response and len(row['logs'])==1,row
                    row['value']['params']['host']='<validated loopback endpoint>';response['port']='<validated port>'
                    outputs.append(output)
                if outputs[0]!=outputs[1]:failures.append(dict(case=name,java=outputs[0],cpp=outputs[1]));break
                checks.append(name)
    report=dict(passed=not failures,programs=index+1,total_programs=len(corpus),loopback_cases=checks,failures=failures,scope='Actual ping, CommandRunner/Main and NIO transport. Exact metadata, reply shape, parameter errors, null/cast categories, traffic, ping request bytes and log count/order; socket/wire error prose and enhanced NPE/CCE prose excluded. Isolated loopback, unstarted Main owners, no full-node startup or exhaustive parity certification.')
    (build/'ping-command-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:1]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
