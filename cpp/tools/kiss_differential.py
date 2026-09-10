#!/usr/bin/env python3
"""KISS value and tokenizer comparisons against freshly compiled pinned Java."""
import argparse,json,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'kiss-oracle';oracle.mkdir(exist_ok=True)
    sources=[*list((JAVA/'src/org/minima/kissvm/values').glob('*.java')),*list((JAVA/'src/org/minima/kissvm/tokens').glob('*.java')),*list((JAVA/'src/org/minima/kissvm/exceptions').glob('*.java')),*list((JAVA/'src/org/minima/objects/base').glob('*.java'))]
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/KissOracle.java')],check=True,capture_output=True)
    def hx(text):return '0x'+text.encode().hex()
    values=['',' ','[]','[x]','[ RETURN TRUE ]','[a [b] c]','[🐼]','[','x]','0x','0X01','0x00','0xFF','0xxyz','TRUE','FALSE','true','1','01','1.00','-1','-1e3','1e3','.1','-.1','-','+1','@BLOCK','x','000.1','-18446744073709551615','18446744073709551615']
    cases=[]
    for value in values:
        cases += [f'{cmd} {hx(value)}' for cmd in ['value','type','hexnumber','check']]
        cases += [f'verify {hx(value)} {kind}' for kind in [1,2,4,8,3,-1]]
    for a in ['1','TRUE','0x','[]']:
        for b in ['1','FALSE','0x00','[x]']:
            for kind in [1,2,4,8]:cases.append(f'same {hx(a)} {hx(b)} {kind}')
    for a in ['0','1','-1','123.456','18446744073709551615']:
        for b in ['0','1','-1','3','0.000000001']:cases.append(f'number {hx(a)} {hx(b)}')
    for a in ['','hello','[world]','🐼']:
        for b in ['','yes','🐼']:cases.append(f'string {hx(a)} {hx(b)}')
    for kind,counts in [('hex',[0,1,65535,65536,65537]),('string',[0,1,16383,16384,16385])]:
        cases += [f'size {kind} {n}' for n in counts]
    scripts=['','RETURN TRUE','return true','LET x=-1 RETURN x LT 0','[abc[def]ghi]','[unclosed','[🐼]RETURN TRUE','LET abc1=1','LET '+'x'*33+'=1','RETURN( TRUE )','RETURN TRUE/*comment*/','0XAB','0xAB','0x','1e3','1.0','TRUE FALSE NEG NXOR','@BLOCK $1 $value (x)','<< >>','<','>','<=','>=',']','[ ]','RETURN\u00a0TRUE']
    names=['CONCAT','LEN','REV','SUBSET','GET','EXISTS','ADDRESS','BOOL','HEX','NUMBER','STRING','ASCII','UTF8','ABS','CEIL','FLOOR','MAX','MIN','DEC','INC','SIGDIG','POW','SQRT','FUNCTION','SUMINPUTS','SUMOUTPUTS','SETLEN','REPLACE','REPLACEFIRST','SUBSTR','OVERWRITE','SHA2','SHA3','PROOF','BITSET','BITGET','BITCOUNT','SIGNEDBY','MULTISIG','CHECKSIG','GETINADDR','GETINAMT','GETINID','GETINTOK','VERIFYIN','GETOUTADDR','GETOUTAMT','GETOUTTOK','GETOUTKEEPSTATE','VERIFYOUT','STATE','PREVSTATE','SAMESTATE']
    scripts += [' '.join(names),' '.join(names).lower(),'LET IF THEN ELSEIF ELSE ENDIF RETURN ASSERT WHILE DO ENDWHILE EXEC MAST','+ - / * % & | ^ ~ = LT LTE GT GTE EQ NEQ XOR AND OR NXOR NAND NOR NOT NEG']
    scripts += ['ſIGNEDBY(0x00)','EXIﬆS(x)','LET K = 1 RETURN k EQ 1','VERıFYIN(0)','@BLöCK','LET İ = 1','ﬀ(x)','ß','@ı','@K']
    rng=random.Random(5564)
    for _ in range(300):scripts.append(''.join(rng.choice('abcABC0123[]()<>=+-@ $\n\t') for _ in range(rng.randrange(50))))
    for script in scripts:
        cases += [f'tokens {hx(script)} {flag}' for flag in [0,1]]
        cases.append(f'lexical {hx(script)} 17')
    def run(command):
        result=subprocess.run(command,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=90).stdout.splitlines()
        if len(result)!=len(cases):raise RuntimeError(f'Output count {len(result)} != {len(cases)}')
        return [bytes.fromhex(line[2:]).decode() for line in result]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','KissOracle']);actual=run([str(build/'minima_kiss_values')]);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Values and tokens; not full KISS execution'};(build/'kiss-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:15]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
