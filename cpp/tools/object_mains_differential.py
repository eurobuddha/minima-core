#!/usr/bin/env python3
"""Actual random entry points and fixed-byte source/native formatting comparisons."""
import argparse,json,os,re,subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def base32(value):
    alphabet='0123456789ABCDEFGHWJKYMNZPQRSTUV';out=''
    while value:out=alphabet[value%32]+out;value//=32
    return 'Mx'+(out or '0')
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'object-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/ObjectMainsOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','ObjectMainsOracle'],[str(build/'object_mains')]]
    report=dict(passed=False,assertions=0,exact_comparisons=0,random_main_runs=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def run(command,args):
        p=subprocess.run(command+args,capture_output=True,text=True,timeout=30,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if p.returncode or p.stderr:raise AssertionError((args,p.returncode,p.stdout,p.stderr))
        return p.stdout
    def expected(name,data):
        h='0x'+data.hex().upper();value=int.from_bytes(data,'big')
        if name=='MiniData':return f'ORIG   : 10 {h} {value}\nLENGTH : 16 0x'+('00'*6)+data.hex().upper()+f' {value}\n'
        b=base32(value)
        for start in [2,7,12,17,22]:
            if start+4>len(b):return f'EXCEPTION:java.lang.StringIndexOutOfBoundsException: Range [{start}, {start+4}) out of bounds for length {len(b)}\n'
        return f'Data     : {h}\nComplete : {b}\nPassword : '+ '-'.join(b[i:i+4] for i in [2,7,12,17,22])+'\n'
    try:
        with ThreadPoolExecutor(max_workers=2) as pool:
            def pair(args):
                rows=[f.result() for f in [pool.submit(run,c,args) for c in commands]];report['observations'].append(dict(args=args,java=rows[0],cpp=rows[1]));return rows
            fixed=[]
            # Every first-failing substring boundary, the final success boundary,
            # zero/all-ones, and leading/sign-bit edges of the 128-bit sample.
            for value in [0,1]+[1<<bits for bits in [15,20,40,45,65,70,90,95,110,115,120,125,127]]+[(1<<128)-1]:fixed.append(('Address',value.to_bytes(16,'big')))
            for value in [0,1,127,128,255,256,1<<79,(1<<80)-1]:fixed.append(('MiniData',value.to_bytes(10,'big')))
            for name,data in fixed:
                args=[name,'fixed','0x'+data.hex()];rows=pair(args)
                check(rows[0]==rows[1],(args,rows));check(rows[0]==expected(name,data),(args,rows));report['exact_comparisons']+=1
            for mode in ['null','empty','args']:
                rows=pair(['TxPoWGenerator',mode]);check(rows==['[2, 1, 0]\n']*2,rows);report['exact_comparisons']+=1
                for name in ['Address','MiniData']:
                    rows=pair([name,mode]);report['random_main_runs']+=2
                    for row in rows:
                        if name=='Address':
                            # A random value can validly fail before printing data.
                            if row.startswith('EXCEPTION:'):
                                check(re.fullmatch(r'EXCEPTION:java.lang.StringIndexOutOfBoundsException: Range \[(2|7|12|17|22), (6|11|16|21|26)\) out of bounds for length \d+\n',row) is not None,row)
                                continue
                            match=re.match(r'Data     : 0x([0-9A-F]{32})\n',row)
                        else:match=re.match(r'ORIG   : 10 0x([0-9A-F]{20}) \d+\n',row)
                        check(match is not None,row);check(row==expected(name,bytes.fromhex(match[1])),row)
            for length in [-2147483648,-100,-1,0,1,10,16]:
                rows=pair(['randomLength',str(length)])
                if length<=0:
                    value=f'EXCEPTION:java.lang.NegativeArraySizeException: {length}\n' if length<0 else '0:\n'
                    check(rows==[value]*2,(length,rows));report['exact_comparisons']+=1
                else:
                    for row in rows:check(re.fullmatch(str(length)+r':0x[0-9A-F]{'+str(length*2)+r'}\n',row) is not None,(length,row))
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='Three source main methods. Java fixed runs install a fixture-only SecureRandom provider and verify exactly one draw of the source-requested length; native fixed runs invoke the private production body after the draw. Actual native/Java mains also run with OS randomness and null/empty/ignored arguments; output relationships are independently checked, not equal random values. No RNG-distribution certification, stdout IO-failure certification, or whole-file parity claim.'
        (build/'object-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
