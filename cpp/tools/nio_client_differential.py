#!/usr/bin/env python3
"""Actual pinned NIOClient methods with controlled Java/native channels."""
import argparse,copy,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'nio-client-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java'];listing=build/'nio-client-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    def op(name,value=None,field=None):
        result={'op':name,'value':value}
        if field is not None:result['field']=field
        return result
    def add(ops,ctor='connected',host='127.0.0.1',port=9001,**extra):cases.append({'ctor':ctor,'host':host,'port':port,'incoming':rng.choice([False,True]),'ops':ops,**extra})
    def hx(value):return '0x'+value.hex()
    def prefix(n):return (n&0xffffffff).to_bytes(4,'big')
    def frame(payload):return prefix(len(payload))+payload
    dates=[-2**63,-2**53-1,-12219292800001,-12219292800000,-62135769600000,-2208988800000,-1,0,1,1000,1700000000000,2**53+1,2**63-1]+[rng.randrange(-2**63,2**63) for _ in range(60)]
    for date in dates:cases.append({'date':date,'ops':[]})
    for ctor in ['connected','connecting']:
        for host in [None,'','127.1','::1','peer/host']:
            add([op('info',True),op('explicitInfo'),op('infoExtra',{'x':[1,None]}),op('nullInfo'),op('read'),op('write'),op('send','0x01'),op('disconnect')],ctor,host,port=-1)
        add([],ctor,runtime=False)
        add([op("info",True),op("send","0x12"),op("write"),op("feed",hx(frame(b"abc"))),op("read"),op("read"),op("disconnect")],ctor,nullManager=True)
        for _ in range(50):
            operations=[]
            for i in range(35):
                field=rng.choice(['host','port','minimaport','welcome','maxima','mls','valid','sent','attempts','extra']);value=rng.choice([None,'','peer','::1','🐼']) if field in ['host','welcome','maxima','mls'] else rng.choice([True,False]) if field in ['valid','sent'] else rng.choice([None,'abc',1,False,{'x':1},[1,2]]) if field=='extra' else rng.choice([-2147483648,-1,0,1,9001,2147483647])
                operations.append(op('set',value,field));operations.append(rng.choice([op('info',True),op('info',False),op('increment'),op('p2p'),op('maximaDisconnected'),op('infoExtra',value),op('connectedTime',rng.choice(dates))]))
            add(operations,ctor)
    for length in [-2147483648,-1,0,1,8191,8192,8193,268435456,268435457,2147483647]:
        for split in [0,1,2,3,4]:
            wire=prefix(length)+b'abc';add([op('feed',hx(wire[:split])),op('read'),op('feed',hx(wire[split:])),op('read'),op('read'),op('control',True,'eof'),op('read')])
    for n in [0,1,2,3,4,8185,8186,8187,8188,8189,8191,8192,8193,16379,16380,16381,32768]:
        payload=rng.randbytes(n);wire=frame(payload)+frame(b'')+frame(b'ab')
        for rcap in [3,8191,8192]:
            reads=min(len(wire)//rcap+3,18);add([op('feed',hx(wire)),op('control',rcap,'rcap')]+[op('read') for _ in range(reads)])
        for wcap in [0,1,8191,8192]:
            writes=1 if not wcap else min(len(wire)//wcap+3,18);add([op('send',hx(payload)),op('send','0x'),op('send','0x6162'),op('control',wcap,'wcap')]+[op('write') for _ in range(writes)])
    for flag in ['readError','writeError','postError','nullTraffic','interestError','wakeError','cancelError','keyValid']:
        add([op('feed',hx(frame(b'abc')+frame(b'x'))),op('control',True,flag),op('send','0x1234'),op('read'),op('write'),op('control',False,flag),op('read'),op('write'),op('disconnect'),op('send','0x12'),op('read'),op('write')])
    for _ in range(130):
        operations=[]
        for i in range(40):
            action=rng.choice(['feed','read','read','write','write','send','control','clock'])
            if action=='feed':value=hx(frame(rng.randbytes(rng.choice([0,1,2,7,8190,8192]))))
            elif action=='send':value=rng.choice([None,'0x','0x1234',hx(rng.randbytes(rng.choice([4,8190,8192])))])
            elif action=='clock':value=rng.randrange(1600000000000,1800000000000)
            else:value=None
            if action=='control':
                field=rng.choice(['rcap','wcap','eof','readError','writeError','keyValid','interestError','wakeError','postError','nullTraffic']);value=rng.choice([0,1,3,4,8191,8192]) if field in ['rcap','wcap'] else rng.choice([True,False]);operations.append(op(action,value,field))
            else:operations.append(op(action,value))
        add(operations)
    add([op("largeSend",268435457),op("send","0x12"),op("write"),op("write")])
    def run(command,inputs=None,zone="UTC"):
        if inputs is None:inputs=cases
        process=subprocess.run(command,input='\n'.join(json.dumps(case,ensure_ascii=True) for case in inputs)+'\n',capture_output=True,text=True,timeout=300,env={**os.environ,'TZ':zone,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=[line for line in process.stdout.splitlines() if not line.startswith('Minima @ ')]
        if process.returncode or len(lines)!=len(inputs):
            (build/'nio-client-crash.txt').write_text(process.stderr);(build/'nio-client-crash-input.txt').write_text(json.dumps(inputs[min(len(lines),len(inputs)-1)])+'\n');raise RuntimeError(f'Process {process.returncode}, output {len(lines)} expected {len(inputs)}')
        return [json.loads(line) for line in lines]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOClientOracle']);actual=run([str(build/'minima_nio_client')]);failures=[]
    for case,j,n in zip(cases,expected,actual):
        if j!=n:
            mismatch={'case':case,'java':j,'cpp':n};failures.append(mismatch)
    date_cases=[{'date':date,'ops':[]} for date in dates]
    timezone_comparisons=0
    for zone in ['Europe/London','America/New_York','Asia/Tokyo']:
        java_dates=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOClientOracle'],date_cases,zone)
        native_dates=run([str(build/'minima_nio_client')],date_cases,zone)
        timezone_comparisons+=len(date_cases)
        for case,j,n in zip(date_cases,java_dates,native_dates):
            if j!=n:failures.append({'case':{**case,'timezone':zone},'java':j,'cpp':n})
    report={'cases':len(cases)+timezone_comparisons,'operations':sum(len(c['ops']) for c in cases),'passed':len(cases)+timezone_comparisons-len(failures),'failures':failures,'scope':'Actual pinned NIOClient constructors and methods, controlled channel/selection/manager boundaries, complete buffer digests and positions, fragmented headers, partial writes, empty/null/invalid frames, failure mutation, traffic/messages, metadata and NIOClientInfo snapshot/live access. Random UID format and clock update effects checked without identical entropy/timestamps. Date.toString in UTC, London, New York and Tokyo including extremes; custom mutable timezone API and other display-name datasets remain unverified. Generic exception diagnostics normalized; real selector/Main runtime integration pending.'}
    (build/'nio-client-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');brief=[]
    for f in failures[:4]:
        j,n=f['java'],f['cpp'];first=None
        if isinstance(j,dict) and isinstance(n,dict):
            for index,(a,b) in enumerate(zip(j.get('states',[]),n.get('states',[]))):
                if a!=b:first={'index':index,'java':a,'cpp':b};break
        brief.append({'case':{k:v for k,v in f['case'].items() if k!='ops'},'mismatch':first or {'java':j,'cpp':n}})
    print(json.dumps({**report,'failures':brief},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
