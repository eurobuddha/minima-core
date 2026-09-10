#!/usr/bin/env python3
"""JsonDB/UserDB accessors, aliasing, serialized state and locale-sensitive names."""
import argparse,json,os,random,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'prefs-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/PrefsOracle.java'];listing=build/'prefs-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','PrefsOracle'];cpp=[str(build/'minima_prefs')]
    source=(JAVA/'src/org/minima/database/userprefs/UserDB.java').read_text();methods=re.findall(r'public\s+(\w+(?:<String>)?)\s+(\w+)\(([^)]*)\)',source)
    getters=[[name] for ret,name,args in methods if ret!='void' and not args.strip()];cases=[]
    def add(commands,initial='{}',locale='en'):cases.append(json.dumps({'initial':initial,'locale':locale,'commands':commands},ensure_ascii=True))
    add(getters+[['wire']])
    for fallback in [False,True]:
        for ret,name,params in methods:
            if ret!='void' or not params.strip():continue
            types=[p.strip().split()[0] for p in params.split(',')]
            for sample in range(4):
                values={'String':None if sample==3 else ['','Example 🐼','0xAAbb'][sample],'MiniNumber':['0','-1.5','12345678901234567890','1024'][sample],'MiniData':['0x','0x00','0x1234','0xFFFFFFFF'][sample],'boolean':fallback,'JSONArray':[None,'alice',{'x':1}],'ArrayList<String>':['a',None,'🐼']}
                add([[name,*[values[t] for t in types]],*getters,['wire']])
    commands=[['exists','x'],['getBoolean','x',True],['getNumber','x','7'],['getData','x','0x0F'],['getString','x'],['getString','x','fallback'],['getJSON','x',{'default':1}],['getJSONArray','x'],['alias'],['wire']]
    for initial in ['{}','null','[]','true','1','"hello"','{"x":null}','{"x":false}','{"x":1}','{"x":"1"}','{"x":"0x01"}','{"x":[]}','{"x":{}}','{"x":','invalid','{"x":"🐼"}','{"x":"NaN"}']:add(commands,initial)
    words=['Minima DAPP',' MINI  HUB ','İSTANBUL','Iİıi','I\u0307','I\u0301','J\u0301','ÌÍĨ','ΟΣ','ΟΣΑ','AΣ','AΣ1','AΣ.A','AΣ:A','AΣ\u0301','A Σ','Σ','AΣ🐼A','AΣ\u200dA','ÉCOLE','ẞß','𐐀𐐨','A\tB','A\x00B','']
    rng=random.Random(5564)
    words += [''.join(rng.choice('AΣa.\' :0123\u0307\u0301\u200d🐼IİJ') for _ in range(rng.randrange(1,20))) for _ in range(150)]
    for locale in ['en','tr','az','lt','el']:
        for word in words:add([['addUninstalledMiniDAPP',word],['checkUninstalledMiniDAPP',word],['addUninstalledMiniDAPP',word],['getUninstalledMiniDAPP'],['removeUninstalledMiniDAPP',word],['getUninstalledMiniDAPP']],locale=locale)
    for millis in [-2**63,-2**53-1,-12219292800001,-12219292800000,-62135769600000,-2208988800000,-1,0,1,1000,1700000000000,2**53+1,2**63-1]:cases.append(json.dumps({'locale':'en','date':str(millis)}))
    def run(cmd):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'})
        out=[s for s in p.stdout.splitlines() if not s.startswith('Minima @ ')]
        if len(out)!=len(cases):raise RuntimeError(f'Output {len(out)} expected {len(cases)}\n{p.stdout[:500]}')
        return out
    expected=run(java);actual=run(cpp);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'All declared UserDB accessors, JsonDB type/default/read errors, mutable array aliasing, name normalization and date formatting; file failure and complete stream API parity pending'}
    (build/'prefs-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:8]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
