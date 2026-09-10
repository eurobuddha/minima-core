#!/usr/bin/env python3
"""Compare native KISS execution with fresh compilation of all pinned Java KISS files.
Generic Java/native runtime diagnostics are normalized; KISS exceptions and traces
are compared exactly. This suite does not claim full exception-class parity.
"""
import argparse,json,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'kissvm-oracle';oracle.mkdir(exist_ok=True)
    sources=[*(JAVA/'src/org/minima/kissvm').rglob('*.java'),*(JAVA/'src/org/minima/objects').rglob('*.java'),*(JAVA/'src/org/minima/utils/json').rglob('*.java')]
    sources += [JAVA/'src/org/minima'/name for name in ['utils/Streamable.java','utils/Crypto.java','utils/BaseConverter.java','system/params/GeneralParams.java','system/params/GlobalParams.java']]
    compiled=subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/KissVMOracle.java')],capture_output=True,text=True)
    if compiled.returncode:raise RuntimeError(compiled.stderr)
    cases=[]
    def add(mode,script,limit=1024,rich=False):cases.append(f'{mode} 0x{script.encode().hex()} {limit} {int(rich)}')
    literals=['TRUE','FALSE','a','b','t','f','h','s','missing','@BLOCK','@CREATED','@MISSING','0','1','-1','0.25','-3.7','0x00','0xFF','0x00FF','[hello]','[]']
    for left in literals:
        add('expr',left)
        for op in ['NOT','NEG','~']:add('expr',op+' '+left)
        for right in ['TRUE','1','0xFF','[hello]']:
            for op in ['AND','NAND','OR','NOR','XOR','NXOR','EQ','NEQ','LT','LTE','GT','GTE','+','-','*','/','%','&','|','^','<<','>>']:add('expr',left+' '+op+' '+right)
    for size in [0,1,7,8,9,255,256,-256,-257,257,0.5,-0.5]:
        for h in ['0x00','0x01','0x80','0xFF','0x00FF','0x010000']:
            for op in ['<<','>>']:add('expr',f'{h} {op} {size}')
    expressions=['TRUE OR FALSE AND FALSE','1+7%3*2','NOT TRUE EQ FALSE','NEG NEG 5','0xFF | 0x00 & 0x0F','(1+2)*3','-a','-(1)','1 2','()','(',')','1+','NOT','(1 2)','(1','1)','0x','[unclosed','$1','1=1']
    for name in ['ABS','CEIL','FLOOR','INC','DEC','SQRT']:
        for n in ['0','1','-1','0.25','-0.25','a','h','s','TRUE','18446744073709551615']:expressions.append(f'{name}({n})')
        expressions += [f'{name}()',f'{name}(1 2)',name+' 1']
    for name in ['MIN','MAX']:expressions += [f'{name}({args})' for args in ['','1','1 2','1 2 3 -9 8','1 TRUE 3','1 missing']]
    for name in ['POW','SIGDIG']:
        for x in ['-1024','-2','0','0.5','1','2','64','1023','1024','4294967296']:
            for y in ['0','1','0.5','12.3','-2']:expressions.append(f'{name}({x} {y})')
    for expr in expressions:add('expr',expr)
    scripts=['','RETURN TRUE','RETURN FALSE','RETURN 1','ASSERT FALSE RETURN TRUE','ASSERT TRUE RETURN TRUE','ASSERT 0','LET x=1 RETURN x EQ 1','LET x=TRUE RETURN x','LET (1 2)=a+b RETURN TRUE','LET (TRUE)=1','LET ()=1','LET x 1','LET x=','LET','RETURN','IF TRUE THEN RETURN TRUE ELSE RETURN FALSE ENDIF','IF FALSE THEN RETURN FALSE ELSEIF TRUE THEN RETURN TRUE ELSE RETURN FALSE ENDIF','IF 1 THEN RETURN TRUE ENDIF','IF FALSE THEN ENDIF RETURN TRUE','WHILE FALSE DO ENDWHILE RETURN TRUE','LET x=0 WHILE x LT 5 DO LET x=x+1 ENDWHILE RETURN x EQ 5','WHILE TRUE DO ENDWHILE','WHILE 1 DO RETURN TRUE ENDWHILE','WHILE TRUE DO RETURN TRUE ENDWHILE RETURN FALSE','EXEC [RETURN TRUE] RETURN FALSE','EXEC [LET x=1] RETURN x EQ 1','EXEC [EXEC [RETURN TRUE]] RETURN FALSE','EXEC 1','EXEC [RETURN]','MAST 0xFF','MAST 1','RETURN @BLOCK GT @CREATED','RETURN FALSE AND missing','RETURN TRUE OR missing','RETURN TRUE RETURN FALSE','IF TRUE THEN RETURN TRUE ENDIF RETURN FALSE','WHILE FALSE DO LET x=1','IF FALSE THEN LET x=1','ELSE','endif','RETURN TRUE /*x*/']
    for script in scripts:
        for limit in [0,1,2,8,1024]:add('run',script,limit)
        add('clean',script)
    for depth in [1,63,64,65,70]:
        add('expr','('*depth+'1'+')'*depth)
        add('run','IF TRUE THEN '*depth+'RETURN TRUE'+' ENDIF'*depth)
        add('run','EXEC ['*depth+'RETURN TRUE'+']'*depth)
    for script in ['LET K = 1 RETURN k EQ 1','EXIﬆS(x)',' return  true ','LET x=[ a  b ] RETURN TRUE','SHA3 (0xab) 0xCD','(a)(b)','[a] [b]','RETURN ( 1+ 2) EQ 3']:
        add('clean',script)
    # Every built-in gets valid/invalid types and arities; context fixtures also
    # distinguish witness coin IDs from transaction placeholders and token units.
    for name in ['ASCII','UTF8','STRING','BOOL','HEX','NUMBER','LEN','REV','BITCOUNT','SHA2','SHA3','ADDRESS','STATE','PREVSTATE','GET','EXISTS','SIGNEDBY']:
        for arg in ['TRUE','FALSE','0','1','-1','0.5','128','18446744073709551615','0x00','0x80','0xFFFF','0xFFFFFFFFFFFFFFFFFF','[]','[FALSE]','[false]','[0xFF]','[hello]','[🐼]','h','s','missing']:
            add('expr',f'{name}({arg})')
        for args in ['', '1 2']:add('expr',f'{name}({args})')
    for name in ['BITGET','BITSET']:
        for data in ['0x00','0x01','0x80','0x0001','0x0100','0xFFFF','HEX([])']:
            for bit in [-1,0,1,7,8,15,16,4294967296]:
                for flag in (['TRUE','FALSE','1'] if name=='BITSET' else ['']):add('expr',f'{name}({data} {bit} {flag})')
    for size in [-1,0,1,2,4,65536,65537]:add('expr',f'LEN(SETLEN({size} 0xAABBCC))')
    for start in [-1,0,1,2,4,2147483647]:
        for end in [-1,0,1,2,4,-2147483648]:
            add('expr',f'SUBSET({start} {end} 0xAABBCC)');add('expr',f'SUBSTR({start} {end} [a🐼b])')
    for name in ['REPLACE','REPLACEFIRST']:
        for original in ['abcabc','🐼🐼','a.b','']:
            for search in ['a','🐼','.','']:
                for replacement in ['X','\\$0','$0','$00','$01','$1','$','\\','\\X','${no}','']:
                    add('expr',f'{name}([{original}] [{search}] [{replacement}])')
    for expr in ['SUBSTR(1 2 [a🐼b])+SUBSTR(2 3 [a🐼b]) EQ [🐼]','SUBSTR(1 2 [a🐼b]) EQ [?]','LEN(SUBSTR(1 2 [a🐼b]))','HEX(SUBSTR(1 2 [a🐼b]))','CONCAT(0xFF 0x00 0x11)','CONCAT([x] [y])','CONCAT(0xFF)','OVERWRITE(0xAABB 0 0x000000 1 2)','OVERWRITE(0xAABB -1 0x000000 1 2)','OVERWRITE(0xAABB 0 0x000000 1 3)','PROOF(0x11 0 0x22 0 0x00)','CHECKSIG(0x00 0x01 0x00)','CHECKSIG(HEX([]) 0x01 HEX([]))','MULTISIG(0 0x01 missing)','MULTISIG(-1 0x01)','SAMESTATE(0 1)','SAMESTATE(-1 0)','SAMESTATE(1 0)','SAMESTATE(2 2)']:
        add('expr',expr)
    for script in ['LET (1 2)=7 RETURN GET(1 2) EQ 7','LET (1.0 2)=TRUE RETURN EXISTS(1 2)','RETURN EXISTS(1)','LET (1)=7 LET (1)=8 RETURN GET(1) EQ 8','LET returnvalue=42 LET x=FUNCTION([LET returnvalue=$1+$2] 3 4) RETURN x EQ 7','LET x=FUNCTION([LET y=$1] [hello]) RETURN y EQ [hello]','LET x=FUNCTION([LET y=1]) RETURN x','LET x=FUNCTION([RETURN TRUE]) RETURN FALSE','RETURN FUNCTION([LET returnvalue=$1] TRUE)','RETURN FUNCTION([LET returnvalue=$1] [$1])','RETURN FUNCTION([RETURN])']:
        add('run',script)
    for name in ['GETINADDR','GETINAMT','GETINID','GETINTOK','GETOUTADDR','GETOUTAMT','GETOUTTOK','GETOUTKEEPSTATE']:
        for index in ['-1','0','1','2','3','0.5','4294967296','TRUE']:add('expr',f'{name}({index})',rich=True)
    for name in ['SUMINPUTS','SUMOUTPUTS']:
        for token in ['0x00','tok','0x77','0x88']:add('expr',f'{name}({token})',rich=True)
    for name in ['VERIFYIN','VERIFYOUT']:
        for index in [-1,0,1,2,3]:
            for amount in ['0','5','7','12.5','25']:
                for token in ['0x00','tok','0x77']:
                    address='0x2'+str(max(0,index));args=f'{index} {address} {amount} {token}'
                    add('expr',f'{name}({args})',rich=True)
                    if name=='VERIFYOUT':
                        for flag in ['TRUE','FALSE','1','FALSE FALSE']:add('expr',f'{name}({args} {flag})',rich=True)
    for expr in ['@TOTIN','@TOTOUT','@AMOUNT','@COINID','@CREATED','SIGNEDBY(0xCA)','MULTISIG(2 0xCA 0xFE)','MULTISIG(2 0xCA 0xCA)','MULTISIG(1 0xCA missing)','MULTISIG(0 1)']:add('expr',expr,rich=True)
    fixtures=subprocess.run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','KissVMOracle','--fixtures'],capture_output=True,text=True,check=True)
    encoded=[line for line in fixtures.stdout.splitlines() if line.startswith('0x')][-1]
    for script in json.loads(bytes.fromhex(encoded[2:]).decode()):add('run' if script.startswith('MAST ') else 'expr',script,rich=True)
    rng=random.Random(5564)
    for _ in range(200):
        nums=[str(rng.randrange(-50,51)) for _ in range(4)];expr=nums[0]
        for num in nums[1:]:expr+=rng.choice(['+','-','*','%','/'])+num
        add('expr',expr)
    def run(command):
        raw=subprocess.run(command,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=180).stdout.splitlines();rows=[line for line in raw if line.startswith('0x')]
        if len(rows)!=len(cases):raise RuntimeError(f'Output count {len(rows)} != {len(cases)}')
        return [json.loads(bytes.fromhex(line[2:]).decode()) for line in rows]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','KissVMOracle']);actual=run([str(build/'minima_kissvm')]);failures=[{'case':c,'script':bytes.fromhex(c.split()[1][2:]).decode(),'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Expressions, statements, contract context and currently ported builtins; generic runtime diagnostics normalized'};(build/'kissvm-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:12]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
