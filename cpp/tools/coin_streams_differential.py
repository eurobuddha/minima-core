#!/usr/bin/env python3
"""Source coin/proof stream mutation, retained owners and actual converter logging."""
import argparse,itertools,json,random,subprocess
from pathlib import Path
from p2p_functions_differential import ROOT,JAVA
from peers_checker_differential import Runner

def corpus(fixtures):
    out=[]
    def add(action,wire='0x',**kw):
        request=dict(action=action,wire=wire,after=False,failAt=0,kind='runtime');request.update(kw);out.append(request)
    add('mutate',after=True)
    for family in ['coin','proof','mmr']:
        name={'coin':'Coin','proof':'Proof','mmr':'MMR'}[family]
        for wire in fixtures[family]:
            raw=bytes.fromhex(wire[2:])
            for end in range(len(raw)+1):
                prefix='0x'+raw[:end].hex()
                add('read'+name,prefix,after=True)
                add('convert'+name,prefix)
            add('read'+name,wire+'FEED',after=True);add('convert'+name,wire+'FEED')
            rng=random.Random(5564+len(raw))
            for _ in range(12):
                changed=bytearray(raw);changed[rng.randrange(len(raw))]^=1
                add('read'+name,'0x'+changed.hex(),after=True);add('convert'+name,'0x'+changed.hex())
        for failAt,kind in itertools.product([0,1],['runtime','error']):
            add('convert'+name,'0x',failAt=failAt,kind=kind)
    # Explicit malformed hash lengths and MiniNumber lengths, including runtime errors.
    raw=bytes.fromhex(fixtures['coin'][0][2:]);off=0
    for _ in range(2):off+=4+int.from_bytes(raw[off:off+4],'big')
    bad=[n.to_bytes(4,'big',signed=True)+raw[4:] for n in [-1,65]]
    bad += [raw[:off]+v+raw[off+2:] for v in [b'\0\0',b'\0\x21',b'\0\xff']]
    for wire in bad:
        for action in ['readCoin','convertCoin','readProof','convertProof']:
            for failAt in [0,1]:add(action,'0x'+wire.hex(),failAt=failAt)
    for action,badHash,nullState,failAt,kind in itertools.product(['copy','rename','floating'],[False,True],[False,True],[0,1],['runtime','error']):
        add(action,badHash=badHash,nullState=nullState,failAt=failAt,kind=kind)
    return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'coin-streams-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ObjectOracle.java',ROOT/'support/CoinStreamsOracle.java'];listing=build/'coin-streams-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    runners=[Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CoinStreamsOracle']),Runner([str(build/'minima_coin_streams')])];failures=[]
    try:
        fixtures=runners[0].ask(dict(action='fixtures'));cases=corpus(fixtures)
        for index,request in enumerate(cases):
            try:outputs=[r.ask(request) for r in runners]
            except Exception:
                (build/'coin-streams-crash.json').write_text(json.dumps(dict(case=index,request=request,stderr=[r.errors() for r in runners]),indent=2)+'\n');raise
            assert all(k in outputs[0] for k in ['result','logs','consumed','beforeMutation','afterMutation'])
            if outputs[0]!=outputs[1]:failures.append(dict(case=index,request=request,java=outputs[0],cpp=outputs[1]))
    finally:
        for runner in runners:runner.close()
    report=dict(passed=not failures,programs=len(cases),failures=failures,scope='Actual Coin/CoinProof/MMRProof readers, every fixture prefix, malformed/trailing bytes, consumed offsets, retained coin/proof/state owners, deepCopy/rename/floating-search failure and actual logger body/failure effects. Stack frames, logger prefixes and Java NPE expressions excluded; nested proof-chunk/MMRData/value-field identity and full node remain incomplete.')
    (build/'coin-streams-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':[dict(case=f['case'],request=f['request'],java_result=f['java']['result'],cpp_result=f['cpp']['result']) for f in failures[:8]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
