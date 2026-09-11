#!/usr/bin/env python3
"""Run the four actual crypto mains and cross-decrypt their printed envelopes."""
import argparse,json,os,re,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve()
    oracle=build/'crypto-mains-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/CryptoMainsOracle.java',ROOT/'support/UtilityOracle.java']
    listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    cp=f'{oracle}:{JAVA}/jar/minima.jar';commands=[['java','-cp',cp,'CryptoMainsOracle'],[str(build/'crypto_mains')]]
    utilities=[['java','-cp',cp,'UtilityOracle',str(oracle/'unused')],[str(build/'minima_utility'),str(oracle/'unused')]]
    report=dict(passed=False,main_runs=0,assertions=0,cross_decryptions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def run(cmd,stdin=None):
        p=subprocess.run(cmd,input=stdin,capture_output=True,text=True,timeout=45,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        check(p.returncode==0 and not p.stderr,(cmd,p.returncode,p.stderr));return p.stdout
    def hx(data):return '0x'+data.hex().upper()
    def decode(text):check(re.fullmatch(r'0x[0-9A-F]+',text) is not None,text[:100]);return bytes.fromhex(text[2:])
    def envelope(text):
        data=decode(text);parts=[];offset=0
        for size in [8,16]:
            check(struct.unpack_from('>I',data,offset)[0]==size,'envelope field width');offset+=4
            parts.append(data[offset:offset+size]);offset+=size
        ciphertext=data[offset:];check(len(ciphertext)>0 and len(ciphertext)%16==0,'padded ciphertext');return parts,ciphertext
    try:
        for name in ['EncryptDecrypt','PasswordCrypto','CryptoPackage','SignVerify']:
            for mode in ['null','empty','args']:
                normalized=[]
                for language,cmd in zip(['java','cpp'],commands):
                    output=run(cmd+[name,mode]);lines=output.splitlines();report['main_runs']+=1
                    report['observations'].append(dict(name=name,mode=mode,language=language,stdout=output))
                    if name=='SignVerify':check(output=='128\ntrue\n',output);normalized.append(output)
                    elif name=='CryptoPackage':
                        check(len(lines)==6,lines);check(re.fullmatch(r'Private Key : [0-9]+',lines[1]) is not None,lines)
                        # PKCS#8 length depends on the fresh RSA prime integers; do not
                        # demand identical random keys or hide other output differences.
                        canonical=lines.copy();canonical[1]='Private Key : <random DER length>'
                        check(canonical==['Public Key  : 162','Private Key : <random DER length>','Secret Key  : 128','Data        : 256','Enc Data    : 428','Worked     : true'],lines)
                        normalized.append(canonical)
                    elif name=='PasswordCrypto':
                        check(len(lines)==5,lines);original=decode(lines[0]);encrypted=decode(lines[2]);envelope(lines[2])
                        check(len(original)==32 and lines[3]=='32' and lines[4]==lines[0] and int(lines[1])==len(encrypted),lines)
                        for utility in utilities:
                            value=run(utility,'pwdec '+hx(b'apasswordblabla')+' '+lines[2]+'\n').strip();check(value==lines[0],value);report['cross_decryptions']+=1
                        normalized.append(['32-byte original','envelope length checked','AES/GZIP envelope','32','original recovered'])
                    else:
                        check(len(lines)==6,lines)
                        labels=['Orig : ','Salt : ','IVPa : ','Len  : ','Enc  : ','Len  : ']
                        check(all(line.startswith(label) for line,label in zip(lines,labels)),lines)
                        values=[line[len(label):] for line,label in zip(lines,labels)];original=decode(values[0]);parts,ciphertext=envelope(values[4])
                        check(len(original)==1024 and values[3]=='1024' and values[5]=='1072',values[3:])
                        check(hx(parts[0])==values[1] and hx(parts[1])==values[2],values[1:3])
                        for utility in utilities:
                            secret=run(utility,'derive '+hx(b'hello')+' '+values[1]+'\n').strip()
                            recovered=run(utility,'aesdec '+values[2]+' '+secret+' '+hx(ciphertext)+'\n').strip()
                            check(recovered==values[0],recovered[:100]);report['cross_decryptions']+=1
                        normalized.append(['1024-byte original','8-byte salt','16-byte IV','1024','AES envelope','1072'])
                check(normalized[0]==normalized[1],(name,normalized))
        report['passed']=True
    except Exception as error:report['failure']=repr(error);raise
    finally:
        report['scope']='24 actual main runs with null, empty and unused arguments; complete fixed output and structured random output checks; 24 cross-language/password/raw AES envelope decryptions. CryptoPackage private DER length is recorded but not equated across fresh random keys. Existing utility differential covers RSA/package/signature interoperability. No Java provider registry side-effect parity or provider-failure injection is claimed. Source commented-out decrypt reads remain inactive.'
        (build/'crypto-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
