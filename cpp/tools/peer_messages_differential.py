#!/usr/bin/env python3
import argparse,ipaddress,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'peer-messages-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/PeerMessagesOracle.java'];listing=build/'peer-messages-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    rng=random.Random(5564);cases=[]
    hosts=[None,'','localhost','LOCALHOST','127.0.0.1','127.1','127.000.000.001','2130706433','0','4294967295','4294967296','1.2.65535','1.16777215','256.1.1.1','0x7f000001','0377.0.0.1','127.1.','invalid host','bad\x00host','::','::1','[::1]','::ffff:127.0.0.1','::127.0.0.1','2001:db8::1','fe80::1%1','fe80::1%01','fe80::1%0','fe80::1%2147483647','fe80::1%2147483648','::1%lo0','fe80::1%lo0','::1%missing_if','::1%','[127.0.0.1]','[::1','::ffff:127.0.0.1%1']
    for host,port in itertools.product(hosts,[-1,0,1,65535,65536]):cases.append({'kind':'address','address':{'host':host,'port':port}})
    for _ in range(250):
        host=str(ipaddress.ip_address(rng.getrandbits(rng.choice([32,128]))));cases.append({'kind':'address','address':{'host':host,'port':rng.randrange(65536)}})
    for a,b in [('127.1','127.0.0.1'),('::ffff:127.0.0.1','127.1'),('::1%0','::1%1'),('::1','[::1]'),('LOCALHOST','localhost'),('Bad Host','bad host'),('İ host','i host'),('𐐀 host','𐐨 host')]:
        for unresolved in [False,True]:cases.append({'kind':'address','address':{'host':a,'port':9001,'unresolved':unresolved},'other':{'host':b,'port':9001,'unresolved':unresolved}})
    values=[None,True,False,'','1',1,0,-1,2147483647,-2147483648,2147483648,-2147483649,9223372036854775807,1.0,1.5,[],{}]
    for value in values:cases.append({'kind':'int','object':{'value':value}})
    cases += [{'kind':'int','object':{}},{'kind':'int','object':None}]
    for value in [None,[],[None],['127.1:1'],['127.1'],['127.1:'],[':1'],['::1:2'],['127.1:1:ignored'],['127.1:１２'],['127.1:2147483648'],['127.1:1',None]]:cases.append({'kind':'io','mode':'strings','value':value})
    for host in hosts:
        for port in values:cases.append({'kind':'io','mode':'objects','value':[{'host':host,'port':port}]})
    for value in [None,[],[None],[{}],[{'host':'127.1'}],[{'host':'127.1','port':1},None]]:cases.append({'kind':'io','mode':'objects','value':value})
    peers=[{'host':h,'port':p} for h,p in [('127.1',9001),('127.0.0.1',9001),('::1',9002),('0.0.0.0',0),('invalid host',1)]]
    for _ in range(150):
        ops=[{'op':'target','value':rng.choice(peers)},{'op':'uid','value':'first'}]
        for i in range(20):
            contents={key:rng.choice(options) for key,options in {'secret':['0x01','0xFF',None,'invalid',''], 'swap_target_host':['127.1','::1','invalid host',None], 'swap_target_port':values}.items() if rng.choice([False,True])}
            ops.append({'op':'read','value':{'do_swap':contents}})
        ops += [{'op':'secret','value':None},{'op':'target','value':None},{'op':'new','value':{'do_swap':{'secret':'0x1234','swap_target_host':'::1','swap_target_port':2}}}]
        cases.append({'kind':'swap','ops':ops})
    for ctor in ['default','target','path']:
        for _ in range(60):
            path=rng.choices(peers,k=4);ops=[]
            for i in range(25):
                action=rng.choice(['read','path','hop','append','external','previous','secret','target'])
                if action=='read':
                    fields={'secret':['0x01','0xFF',None,'invalid',''],'walkInLinks':values,'isJoiningWalk':values,'isClientWalk':values,'availableClientSlots':values,'isReturning':values,'pathTaken':[None,[],path,[None],[{}]]}
                    value={'walk_links':{key:rng.choice(options) for key,options in fields.items() if rng.choice([False,True])}}
                elif action=='path':value=rng.choice([None,[],path,path+[None]])
                elif action in ['hop','append','external','previous']:value=rng.choice(peers+[None])
                elif action=='secret':value=rng.choice([None,'0x','0x01','bad'])
                else:value=rng.choice([None,'uid','changed',1])
                ops.append({'op':action,'value':value})
            ops.append({'op':'new','value':{'walk_links':{'secret':'0x1234','walkInLinks':True,'pathTaken':[]}}})
            cases.append({'kind':'walk','ctor':ctor,'path':path,'ops':ops})
    def run(command):
        process=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',capture_output=True,text=True,timeout=240,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=process.stdout.splitlines()
        if process.returncode or len(lines)!=len(cases):
            (build/'peer-messages-crash.txt').write_text(process.stderr);raise RuntimeError(f'Process exit {process.returncode}, lines {len(lines)}, expected {len(cases)}')
        return lines
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','PeerMessagesOracle']);actual=run([str(build/'minima_peer_messages')]);failures=[{'case':case,'java':json.loads(j),'cpp':json.loads(n)} for case,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Peer address numeric forms, IPv6 scopes, unresolved names, hashes/equality, IO conversions/safeReadInt, swap and walk JSON, sequential failure mutation and shared path lists; generic exception classes normalized, DNS cache/custom JDK network properties not covered'}
    (build/'peer-messages-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:3]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
