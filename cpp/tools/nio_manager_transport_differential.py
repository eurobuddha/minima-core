#!/usr/bin/env python3
"""Pinned NIOManager serialization/send accounting and real single-peer ping."""
import argparse,json,os,random,socket,struct,subprocess,threading,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'

def execute(command,requests):
    result=subprocess.run(command,input=''.join(json.dumps(q)+'\n' for q in requests),capture_output=True,text=True,timeout=30,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
    if result.returncode:raise RuntimeError(f'{command}: {result.returncode}\n{result.stdout}\n{result.stderr}')
    rows=[json.loads(line) for line in result.stdout.splitlines() if not line.startswith('Minima @ ')]
    if len(rows)!=len(requests):raise AssertionError(f'Expected {len(requests)} responses: {result}')
    return rows

def receive(sock,count):
    result=b''
    while len(result)<count:
        part=sock.recv(count-len(result))
        if not part:raise EOFError('Incomplete ping request')
        result+=part
    return result

def ping(command,reply,fragment=False,delay=False,request=None):
    errors=[];received=[]
    with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as listener:
        listener.bind(('127.0.0.1',0));listener.listen(1);listener.settimeout(15)
        def peer():
            try:
                with listener.accept()[0] as sock:
                    sock.settimeout(15)
                    size=struct.unpack('>i',receive(sock,4))[0]
                    if size<0 or size>1024:raise AssertionError(f'Unexpected ping length {size}')
                    received.append(receive(sock,size))
                    if delay:
                        # SO_TIMEOUT applies to each blocking read; no reply for 10s.
                        time.sleep(10.25)
                    elif fragment:
                        for start in range(0,len(reply),3):sock.sendall(reply[start:start+3]);time.sleep(.001)
                    else:sock.sendall(reply)
            except BaseException as error:errors.append(error)
        worker=threading.Thread(target=peer);worker.start()
        try:
            port=listener.getsockname()[1]
            query=request(port) if request else {'mode':'ping','host':'127.0.0.1','port':port}
            result=execute(command,[query])[0]
        finally:worker.join(16)
        if worker.is_alive():raise AssertionError('Peer did not stop')
        if errors:raise errors[0]
        # Exact actual SINGLE_PING + ZERO_TXPOWID envelope (no extra type payload).
        if received!=[bytes([11])+struct.pack('>i',1)+bytes(1)]:raise AssertionError(f'Ping request differs: {received}')
        return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'nio-manager-transport-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NIOManagerTransportOracle.java']
    listing=build/'nio-manager-transport-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOManagerTransportOracle'];native=[str(build/'minima_nio_manager_transport')]
    rng=random.Random(6703);requests=[]
    for type_ in range(256):
        requests.append({'mode':'create','type':type_,'kind':'data','value':'0x'+rng.randbytes(type_%35).hex()})
        requests.append({'mode':'send','type':type_,'kind':'data','value':'0x'+rng.randbytes(type_%35).hex(),'uid':'' if type_%2 else 'missing'})
    for kind,value in [('string',''),('string','\x00 hello £ 雪 😀'),('greeting',None),('data','0x'),('data','0x'+'ff'*65536),('null',None),('failure',None),('oom',None)]:
        for type_ in [0,11,12,255]:
            requests.append({'mode':'create','type':type_,'kind':kind,'value':value})
            for uid in [None,'','missing']:requests.append({'mode':'send','type':type_,'kind':kind,'value':value,'uid':uid})
    expected=execute(java,requests);actual=execute(native,requests)
    for i,(left,right) in enumerate(zip(expected,actual)):
        if left!=right:raise AssertionError(f'Case {i}: {requests[i]}\nJava: {left}\nC++: {right}')
    encoded=execute(java,[{'mode':'create','type':12,'kind':'greeting'}])[0]['result']
    greeting=bytes.fromhex(encoded[2:])[1:]
    def frame(body):return struct.pack('>i',len(body))+body
    cases=[]
    for type_ in [0,11,12,255]:cases.append((f'valid-type-{type_}',frame(bytes([type_])+greeting),False,False))
    cases.extend([
        ('fragmented',frame(b'\x0c'+greeting),True,False),
        ('trailing-bytes',frame(b'\x0c'+greeting+b'ignored'),False,False),
        ('empty-frame',frame(b''),False,False),
        ('type-only',frame(b'\x0c'),False,False),
        ('negative-length',struct.pack('>i',-1),False,False),
        ('oversize-length',struct.pack('>i',512*1024*1024+1),False,False),
        ('truncated-header',b'\x00\x00',False,False),
        ('truncated-frame',struct.pack('>i',100)+b'\x0c',False,False),
        ('read-timeout',b'',False,True),
    ])
    for end in [1,4,12,len(greeting)-1]:cases.append((f'truncated-greeting-{end}',frame(b'\x0c'+greeting[:end]),False,False))
    checks=[]
    for name,reply,fragment,delay in cases:
        left=ping(java,reply,fragment,delay);right=ping(native,reply,fragment,delay)
        if left!=right:raise AssertionError(f'{name}\nJava: {left}\nC++: {right}')
        if name.startswith('valid') and left['result'] is None:raise AssertionError('Valid ping failed in both implementations')
        checks.append(name)
    report={'passed':True,'serialization_and_send_cases':len(requests),'ping_cases':checks,'scope':'Actual pinned NIOManager methods; exact payload bytes, all 256 type names, null/failing serializer timing, allocation failure conversion, empty server single/broadcast accounting, loopback ping framing, response type acceptance, truncation, length limits and read timeout. Singleton fixture only; full native node runtime and NIOMessage dispatch remain pending.'}
    (build/'nio-manager-transport-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
