#!/usr/bin/env python3
"""Exercise the pinned JSON lexer, DOM parser, and stopping/resuming event parser."""
import argparse,json,random,subprocess,struct,math
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'json-oracle';oracle.mkdir(exist_ok=True)
    sources=list((JAVA/'src/org/minima/utils/json').rglob('*.java'))
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),*map(str,sources),str(ROOT/'support/JSONOracle.java')],check=True,capture_output=True)
    texts=['',' ','{}','[]','null','true','false','TRUE','[1 2,,,3,]','{,"a":::1,"a":2,}','{"a" [true {"b":null}]}','{"a":}','{1:2}','[1:2]','[1','[}','"unterminated','"\\x"','"\\u123"','"\\uGGGG"','"\\uD800"','"\\uDC00"','"\\uD83D\\uDC3C"','{"\\uD800":1,"\\uD801":2}','"🐼" 0','"raw\nline\t"']
    texts += ['0','-0','001','-01','+1','1.','1.e2','.1','1e','1e+2','1E-5','1.25','0.0001','1e7','1e20','-0.0','9223372036854775807','9223372036854775808','-9223372036854775809','1e999','1e-999']
    texts += ['"'+chr(i)+'"' for i in range(128)]
    rng=random.Random(5564)
    texts += ['1e23','5e-324','2.2250738585072014e-308','1.7976931348623157e308']
    for _ in range(500):
        value=struct.unpack('!d',rng.randbytes(8))[0]
        if math.isfinite(value):texts.append(repr(value))
    for _ in range(350):texts.append(''.join(rng.choice('{}[],:"\\0123truefalsenull \t\n') for _ in range(rng.randrange(40))))
    for _ in range(80):texts.append(json.dumps({'first':[rng.randrange(-10000,10000),True,None,'🐼'], 'last':{'nested':['\\', '\n', '']}},ensure_ascii=False))
    cases=[f'{command} {mode} 0x{text.encode().hex()}' for text in texts for command,mode in [('dom',0),('lex',0),('stream',0),('stream',1),('stream',3)]]
    def run(cmd):
        result=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,check=True,timeout=120).stdout.splitlines()
        if len(result)!=len(cases):raise RuntimeError(f'Output count {len(result)} != {len(cases)}')
        return [bytes.fromhex(line[2:]).decode() for line in result]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.utils.json.parser.JSONOracle']);actual=run([str(build/'minima_json')])
    failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures};(build/'json-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:15]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
