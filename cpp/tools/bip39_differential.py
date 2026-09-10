#!/usr/bin/env python3
import argparse,json,os,random,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'bip39-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/BIP39Oracle.java'];listing=build/'bip39-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','BIP39Oracle'];cpp=[str(build/'minima_bip39')];rng=random.Random(5564)
    def run(cmd,cases):
        p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'});lines=p.stdout.splitlines()
        if p.returncode or len(lines)!=len(cases):
            (build/'bip39-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    words=re.findall(r'"([a-z]+)"',(JAVA/'src/org/minima/utils/BIP39.java').read_text().split('public final static String[] WORD_LIST')[1]);cases=[]
    def add(op,locale='en',**kwargs):cases.append(json.dumps({'op':op,'locale':locale,**kwargs},ensure_ascii=True))
    add('words')
    for word in words:
        for fragment in [word,word[:4].upper(),word[:3],word[:2]]:add('clean',text=fragment)
    for locale in ['en','tr','az','lt','el']:
        for phrase in [None,'','  ','\t\n','air act zoo','ABAND ABIL ABLE','MEAt inve act THEM WAGO ALONE PURI AVER UNFA RESI AMUS SMOK SENS MILL DIAGRAM OBVIO','air\tzoo','air\nzoo','air\u00a0zoo','\x00 air \x1f','İLL ILL ILLNESS','ΣΣΣ invalid','UNKNOWN','about extraSuffix']:
            for op in ['clean','seed']:add(op,locale,text=phrase)
        for _ in range(25):
            phrase='  '.join(rng.choice(words).upper()[:rng.randrange(2,9)] for _ in range(rng.randrange(25)));add('clean',locale,text=phrase)
        for _ in range(5):add('random',locale)
    for values in [None,[],[None],[''],[' ',''],['air','zoo'],['\t air\n','  ZOO  '],['test seed fixture','π 😄\n'],['air','\u00a0','zoo']]+[[rng.choice(['',None,' ',rng.choice(words),'\t\n','π 😄']) for _ in range(rng.randrange(30))] for _ in range(100)]:
        for op in ['list','listseed']:add(op,words=values)
    for replacement in [None,'abcd','act','ßßß','ΣΣΣΣ']:
        for phrase in ['act','abc','abcd','abandon','zoo']:add('clean',replace=[0,replacement],text=phrase)
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Pinned Minima BIP39 word list, every word/prefix, exact SHA3 seed, list trimming/joining, nulls, mutated word array, five locales; random output checks count/membership only'}
    (build/'bip39-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:4]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
