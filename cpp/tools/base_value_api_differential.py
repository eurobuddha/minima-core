#!/usr/bin/env python3
"""Validate the additional native base-value APIs against pinned originals."""
import argparse,hashlib,itertools,json,os,subprocess,time
from pathlib import Path
from peers_checker_differential import Runner
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def canonical(text):
 text=text.removeprefix('0x');text=('0'+text) if len(text)%2 else text
 return '0x'+text.upper() if text else ''
def decimal_record(value):
 from decimal import Decimal
 d=Decimal(value);parts=d.as_tuple();coefficient=int(''.join(map(str,parts.digits)))*(-1 if parts.sign else 1)
 return dict(coefficient=str(coefficient),scale=-parts.exponent,text=str(d.copy_abs()) if coefficient==0 else str(d))
def integer_decimal(value):return dict(coefficient=str(value),scale=0,text=str(value))
def wire(coefficient,scale):
 size=max(1,(coefficient.bit_length()+8)//8);raw=coefficient.to_bytes(size,'big',signed=True)
 while len(raw)>1 and ((raw[0]==0 and raw[1]<128) or (raw[0]==255 and raw[1]>=128)):raw=raw[1:]
 return '0x'+(bytes([scale%256,len(raw)])+raw).hex().upper()
def cases():
 result=[]
 for value,other,replacement,warm,length in itertools.product(['0x','0x00','0x01','0x0001','0xFF','0x'+'FF'*32,'0x'+'12'*128],['0x','0x00','0x01','0xFF'],[None,'0x00','0x02'],[False,True],[-2147483648,-1,0,1,6,10000]):
  q=dict(op='data',value=value,other=other,replacement=replacement,warm=warm,length=length)
  old=int(value.removeprefix('0x') or '0',16);current=value if replacement is None else replacement
  numeric=old if warm else int(current.removeprefix('0x') or '0',16);right=int(other.removeprefix('0x') or '0',16);text=canonical(current)
  expected=dict(before=integer_decimal(old) if warm else None,decimal=integer_decimal(numeric),bytes=text,compare=(numeric>right)-(numeric<right),lessEqual=numeric<=right,moreEqual=numeric>=right,equalBytes=text==canonical(other))
  if length<0:expected['error']='java.lang.StringIndexOutOfBoundsException: Range [0, '+str(length)+') out of bounds for length '+str(len(text))
  else:expected['limited']=text[:length]+'..'
  result.append((q,expected))
 replacements=[None,(100,2),(-12345,4),(10**40,0),(1,-10),(0,127)]
 for op,value,replacement in itertools.product(['number','bigint'],['0','1','-1','2147483649','18446744073709551615','-18446744073709551615','1.00','0.00','1E3','1E-44'],replacements):
  if op=='bigint' and any(c in value for c in '.E'):continue
  before=decimal_record(value);after=before if replacement is None else decimal_record(str(replacement[0])+'E'+str(-replacement[1]))
  q=dict(op=op,value=value,wire=None if replacement is None else wire(*replacement))
  coefficient=int(after['coefficient']);sign=(coefficient>0)-(coefficient<0)
  expected=dict(before=before,decimal=after,wire=wire(coefficient,after['scale']),compareToZero=sign,decimalCompareToZero=sign)
  result.append((q,expected))
 for value in [str(2**64),str(-2**64),'9'*100,'-'+'9'*100]:
  result.append((dict(op='bigint',value=value,wire=None),dict(__error_class__='java.lang.NumberFormatException')))
 # Additional immutable decimal constructor/limit API cases; prior corpus retained.
 from decimal import Decimal,Context,ROUND_DOWN
 context=Context(prec=64,rounding=ROUND_DOWN)
 limit=2**64-1
 result.append((dict(op='limits'),dict(maximum=integer_decimal(limit),minimum=integer_decimal(-limit),maximumNumber=integer_decimal(limit))))
 for coefficient,scale in itertools.product([0,1,-1,12300,-12300,limit,-limit,limit+1,-limit-1,int('1234567890'*7),-int('1234567890'*7)],[-3,0,4,44,70]):
  q=dict(op='decimal',value=str(coefficient),scale=scale,wire=None)
  # Java passes the decimal's plain spelling through its 64-digit DOWN context.
  value=context.create_decimal(format(Decimal(str(coefficient)+'E'+str(-scale)),'f'))
  if value.as_tuple().exponent < -44:value=context.quantize(value,Decimal('1E-44'))
  if value.copy_abs()>Decimal(limit):expected=dict(__error_class__='java.lang.NumberFormatException')
  else:
   record=decimal_record(str(value));number=int(record['coefficient']);sign=(number>0)-(number<0)
   expected=dict(before=record,decimal=record,wire=wire(number,record['scale']),compareToZero=sign,decimalCompareToZero=sign)
  result.append((q,expected))
 return result

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();started=time.monotonic();oracle=build/'base-value-api-oracle';oracle.mkdir(exist_ok=True)
 sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/BaseValueAPIOracle.java'];listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
 corpus=cases();runners=[];failures=[];count=checks=0;transcript=hashlib.sha256()
 try:
  runners.append(Runner(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','BaseValueAPIOracle']));runners.append(Runner([str(build/'base_value_api')]))
  for index,(q,expected) in enumerate(corpus):
   values=[runner.ask(q) for runner in runners];count+=1
   if values[0]!=values[1]:failures.append(dict(case=index,request=q,java=values[0],cpp=values[1]))
   for runtime,value in enumerate(values):
    for key,answer in expected.items():
     checks+=1
     actual=value.get('error','').split(':',1)[0] if key=='__error_class__' else value.get(key)
     if actual!=answer:failures.append(dict(case=index,runtime=runtime,key=key,expected=answer,actual=actual))
    if 'error' not in expected and '__error_class__' not in expected:
     checks+=1
     if 'error' in value:failures.append(dict(case=index,runtime=runtime,unexpected_error=value['error']))
   transcript.update(json.dumps([q,*values],sort_keys=True,separators=(',',':')).encode())
 except Exception as error:failures.append(dict(phase='execution',case=count,error=repr(error)))
 finally:
  for runtime,runner in enumerate(runners):
   with os.fdopen(os.dup(runner.err.fileno()),'r') as errors:
    try:runner.close()
    except Exception as error:failures.append(dict(phase='cleanup',runtime=runtime,error=repr(error)))
    errors.seek(0);stderr=errors.read()
    if stderr:failures.append(dict(phase='stderr',runtime=runtime,stderr=stderr))
 if count!=len(corpus):failures.append(dict(phase='accounting',executed=count,planned=len(corpus)))
 report=dict(passed=not failures,cases=count,planned_cases=len(corpus),independent_assertions=checks,seconds=round(time.monotonic()-started,3),transcript_sha256=transcript.hexdigest(),failures=failures,scope='New MiniData compare/inclusive comparison, unbounded decimal view and bounded hex methods; MiniNumber BigInt and decimal-carrier constructors, exact min/max constants, MAXIMUM and raw decimal view. Decimal constructor cases retain coefficient/scale, plain-string conversion, 64-digit rounding, 44-place truncation, numeric range checks and immutable snapshots. Cached numeric values across byte replacement, retained immutable decimal snapshots, zero padding, values beyond monetary limits, exact negative substring errors, decimal coefficient/scale/text and wire-read values outside constructor limits. All cases have independent expected fields plus direct Java/C++ comparison. No full base/JDK API or node parity claim.')
 (build/'base-value-api-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:5]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
