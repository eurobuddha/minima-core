#!/usr/bin/env python3
import argparse,base64,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'network-utilities-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NetworkUtilityOracle.java'];listing=build/'network-utilities-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NetworkUtilityOracle'];cpp=[str(build/'minima_network_utilities')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});lines=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if p.returncode or len(lines)!=len(cases):
            (build/'network-utilities-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[]
    def add(**kw):cases.append(json.dumps(kw,ensure_ascii=True))
    for n in range(6):
        for chars in itertools.product('Az/=!',repeat=n):add(type='base64',data=''.join(chars))
    for _ in range(300):
        raw=rng.randbytes(rng.randrange(100));encoded=base64.b64encode(raw).decode();add(type='base64',data=encoded);add(type='base64',data=encoded.rstrip('='))
    users=[{'username':'alice','password':'test-password','mode':'read'},{'username':'bob','password':'test-password:two','mode':'write'},{'username':'π','password':'😄','mode':'read'}]
    headers=[None,'','Basic ','basic bWluaW1hOg==','Bearer nope','Basic bWluaW1hOg===']
    for credentials in ['minima:test-password','minima:wrong','minima:',':','alice:test-password','alice:wrong','bob:test-password:two','π:😄','no-colon','  minima:test-password  ','minima:test-password\x00']:
        encoded=base64.b64encode(credentials.encode()).decode()
        headers.extend(['Basic '+encoded,'Authorization: Basic '+encoded, 'junk Basic '+encoded.rstrip('='),'Basic '+encoded+' '])
    for authenticate in [False,True]:
        for header in headers:
            for records in [None,[],users,users+[{'username':'alice','password':'test-password','mode':'write'}],users+[None],[{'username':'alice','password':'test-password'}],[{'username':'alice','password':'test-password','mode':42}],[{}],{},[1]]:
                add(type='auth',header=header,authenticate=authenticate,style='basic',password='test-password',users=records)
    for style in ['BASIC','','digest']:
        for header in headers[:10]:add(type='auth',header=header,authenticate=True,style=style,password='test-password',users=users)
    keys=[None,'','A','B','Aa','BB','π','😄','Σ','sendPingMessage']+[''.join('Aa' if i&(1<<b) else 'BB' for b in range(7)) for i in range(128)]
    for _ in range(100):
        ops=[{'op':'reset','amount':rng.randint(-99999,99999)}]
        for i in range(60):ops.append({'op':rng.choice(['read','write','readkey','writekey','readkey','writekey']), 'amount':rng.choice([-(1<<63),(1<<63)-1,0,-1,1,1023,1024,1048576,rng.randint(-99999,999999999)]),'key':rng.choice(keys)})
        add(type='traffic',ops=ops)
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Authorizer basic header parsing, default/additional users, duplicates and malformed data; JDK basic Base64 alphabet/padding; NIOTraffic signed counters, UTF-16 Hashtable ordering and formatted breakdown. No RPC server or wallet commands executed.'}
    (build/'network-utilities-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[{k:v[:500] for k,v in f.items()} for f in failures[:4]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
