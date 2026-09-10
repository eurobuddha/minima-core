#!/usr/bin/env python3
import argparse,base64,hashlib,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');args=ap.parse_args();build=args.build.resolve();oracle=build/'command-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/CommandOracle.java'];listing=build/'command-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','CommandOracle'];cpp=[str(build/'minima_command')];rng=random.Random(5564)
    def run(cmd,cases):
        try:p=subprocess.run(cmd,input='\n'.join(cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        except subprocess.TimeoutExpired as error:
            completed=len((error.stdout or b'').splitlines());(build/'command-timeout.json').write_text(json.dumps({'process':cmd,'completed':completed,'next':cases[completed:completed+2]},indent=2));raise
        lines=p.stdout.splitlines()
        if p.returncode or len(lines)!=len(cases):
            (build/'command-crash-stderr.txt').write_text(p.stderr);raise RuntimeError(f'Process {p.returncode}, output {len(lines)}, expected {len(cases)}')
        return lines
    cases=[json.dumps({'op':'mx','value':'0x'+rng.randbytes(n).hex()}) for n in [0,1,2,16,32,64]];addresses=[json.loads(v) for v in run(java,cases)]
    values=[None,'',' ',True,False,1,[],{},['foo'],{'foo':'bar'},'true','True','false',' false ','0','-1',' 1.25 ','1e8','1e80','0xff','0Xff','0x','0xGG','Mx','MxINVALID','Mx+','Mx-','Mx0','Mx+1','Mx-1','Mx1+2','Mxinvalid!','Mx１２３','Mx١٢٣','MxＦＡＣＥ','Mx᠑᠒᠓','Mx1o0o','Mx0w0o','\t\n\x00 a \x1f','\u00a0','π 😄','  \\n ']+addresses
    specs={'param':None,'paramdefault':'default','boolean':None,'booleandefault':True,'number':None,'numberdefault':'1.5','data':None,'datadefault':'0x22','address':None,'addressdefault':'fallback','object':None,'objectdefault':{'default':True},'array':None,'meta':None,'reply':None,'alias':None}
    for val in values:
        for present in [True,False]:
            for op,default in specs.items():
                for null_default in ([False,True] if op.endswith('default') and op!='booleandefault' else [False]):
                    cases.append(json.dumps({'op':op,'params':{'value':val} if present else {},'key':'value','default':None if null_default else default,'name':rng.choice([None,'test']),'help':rng.choice([None,'help']),'complete':rng.choice([None,'test value:foo']),'id':rng.choice([None,'0x00'])},ensure_ascii=True))
    for val in values+['0x12','0X12','test hash fixture',{'nested':[1,None,'π 😄']},['a','b']]:
        for kind in [None,'sha2','sha3','SHA3','nope','']:
            params={'data':val}
            if kind is not None:params['type']=kind
            cases.append(json.dumps({'op':'hash','params':params},ensure_ascii=True))
    folder=build/'command-fixtures';folder.mkdir(exist_ok=True)
    for n in [0,1,256,65536]:
        path=folder/f'{n}.dat';path.write_bytes(rng.randbytes(n))
        for kind in ['sha2','sha3']:cases.append(json.dumps({'op':'hash','params':{'file':str(path),'type':kind,'data':'ignored'}}))
    for path in [folder,folder/'absent.dat']:cases.append(json.dumps({'op':'hash','params':{'file':str(path)}}))
    cases.append(json.dumps({'op':'hashmeta','params':{'data':'test'}}))
    for text in [None,'','unknown','π 😄']:
        for error in [None,'','Command not found']:cases.append(json.dumps({'op':'missing','params':{'test':1},'input':text,'error':error}))
    def command(name,params,meta=False):return json.dumps({'op':'commandmeta' if meta else 'command','command':name,'params':params},ensure_ascii=True)
    for name in ['convert','random','checkaddress','maths','mmrcreate','mmrproof','runscript','verify']:cases.append(command(name,{},True))
    for source,inputs in {'string':values,'hex':values+['0xﬀ','0xabcdef','0xffefbfbd','0xc080','0xf0808080'],'mx':values,'base64':values+['a'*n for n in range(12)]}.items():
        for val in inputs:
            for dest in ['hex','mx','string','base64','invalid']:
                cases.append(command('convert',{'from':source,'to':dest,'data':val}))
    for size in range(6):
        for chars in itertools.product('A/=?é',repeat=size):cases.append(command('convert',{'from':'base64','to':'hex','data':''.join(chars)}))
    for size in range(100):
        raw=rng.randbytes(size)
        for source,val in [('hex','0x'+raw.hex()),('base64',base64.b64encode(raw).decode())]:
            for dest in ['hex','mx','string','base64']:cases.append(command('convert',{'from':source,'to':dest,'data':val}))
    for source in ['String','STRING',' HEX ','Base64','INVALID',None,True]:
        for dest in ['HEX','String','BASE64',' Mx ','INVALID',None]:cases.append(command('convert',{'from':source,'to':dest,'data':'ABCD'}))
    for params in [{},{'from':'hex'},{'from':'hex','to':'string'},{'to':'hex','data':'a'}]:cases.append(command('convert',params))
    walletaddresses=['0x'+v*32 for v in ['11','22','33','44']]
    mxwallet=[json.loads(v) for v in run(java,[json.dumps({'op':'mx','value':v}) for v in walletaddresses])]
    for val in values+walletaddresses+mxwallet+['0x'+('1'*63),'0x'+('1'*65),'0X'+('1'*64)]:cases.append(command('checkaddress',{'address':val}))
    cases.append(command('checkaddress',{}))
    for size in ['0','0.5','-0.5','4294967296','-4294967296','-1','-1.5','2147483648','bad',None,True]:
        for kind in ['sha2','sha3','SHA3','bad','',None]:cases.append(command('random',{'size':size,'type':kind}))
    for expression in [None,True,1,{},[],'','1+2 * (3/4)','1+2 * SIGDIG(1 3/4)','[a🐼b]','TRUE','0xff','1/0','SQRT(-1)','1+','@BLOCK','1 LET x = 2','1 LET x = 1/0','1 RETURN TRUE']+[f'{rng.randrange(-99,100)}{rng.choice(["+","-","*","/","%"])}{rng.randrange(-99,100)}' for _ in range(120)]:
        for logs in ['true','false','TRUE',None]:cases.append(command('maths',{'calculate':expression,'logs':logs}))
    cases.append(command('maths',{}))
    for nodes in [None,{},'',[],[None],[True],[1],[{}],[[]],['bad:'],['0xGG'],['a: 1'],['a:1:2']]:cases.append(command('mmrcreate',{'nodes':nodes}))
    fixtures=[command('mmrcreate',{'nodes':[rng.choice(['0x'+rng.randbytes(8).hex(),'text'+str(i),'[🐼]','0XFF','RETURN TRUE'])+rng.choice(['',':0',':1',':-1',':0.00000001',':1e3']) for i in range(n)]}) for n in [1,2,3,4,5,7,8,9,15,16,17,31,32,33,63,64,65]]
    for fixture,response in zip(fixtures,run(java,fixtures)):
        result=json.loads(response)['response'];root=result['root']['data']+':'+result['root']['value']
        for leaf in result['nodes']:
            data=leaf['data']+':'+leaf['value'];proof=leaf['proof']
            for d,p,r in [(data,proof,root),(data+'0',proof,root),(data,proof,root+ '0'),(data,proof[:-2],root),(data,proof+'FF',root),(' '+data+' ',proof,root),(data,proof,root.split(':')[0])]:cases.append(command('mmrproof',{'data':d,'proof':p,'root':r}))
    cases+=fixtures
    for params in [{},{'data':'a'},{'data':None,'proof':'0x','root':'0x'},{'data':'a','proof':'bad','root':'0x'},{'data':'a','proof':'0x','root':'0x'}]:cases.append(command('mmrproof',params))
    for value in [None,'','+','-',' 1','1 ','1e2','1.0','2147483647','2147483648','-2147483648','-2147483649','000000000000000000001','١٢٣','１２３','᠑᠒᠓','𝟙','+１２','-٠',True,{}]+[str(rng.randrange(-2**34,2**34)) for _ in range(200)]:cases.append(json.dumps({'op':'integer','value':value}))
    for script in [None,True,1,{},[],'','RETURN TRUE',' return  true ','RETURN FALSE','RETURN STATE(1) EQ PREVSTATE(1)','RETURN @COINAGE GT 5','RETURN SIGNEDBY(0xFF)','LET x = 1 LET y = x+2 RETURN y EQ 3','RETURN @SCRIPT EQ [RETURN @SCRIPT EQ [x]]','RETURN 1/0','LET x=','WHILE TRUE DO ENDWHILE','IF TRUE THEN RETURN TRUE ENDIF','LET (1 2)=5 RETURN GET(1 2) EQ 5','MAST 0x00']:
        for context in [{},{'state':{'1':'1'},'prevstate':{'1':'1'},'globals':{'@BLOCK':'20','@CREATED':'10'},'signatures':['0xFF']},{'globals':{'@block':'2','@created':'1','@COINAGE':'0','@script':'[override]'}}]:cases.append(command('runscript',{'script':script,**context}))
    cases.append(command('runscript',{}))
    for field in ['state','prevstate','globals','signatures','extrascripts']:
        for value in [None,[],{},True,1,'x',{'1':None},{'1':True},{'0':'1','+0':'2'},{'１２':'1'},{'256':'0'},{'-1':'1'},{'2147483648':'1'},['0x'],['Mxinvalid']]:cases.append(command('runscript',{'script':'RETURN TRUE',field:value}))
    for fixture,response in zip(fixtures,run(java,fixtures)):
        if len(json.loads(fixture)['params']['nodes'])>9:continue
        result=json.loads(response)['response'];root=result['root']['data'];extras={leaf['data']:leaf['proof'] for leaf in result['nodes']}
        cases.append(command('runscript',{'script':'MAST '+root,'extrascripts':extras}))
    mast=json.loads(run(java,[command('mmrcreate',{'nodes':['RETURN TRUE','RETURN FALSE']})])[0])['response']
    for index in [0,1]:cases.append(command('runscript',{'script':'MAST '+mast['root']['data'],'extrascripts':{mast['nodes'][index]['data']:mast['nodes'][index]['proof']}}))
    for fixture in run(java,[json.dumps({'op':'sigfixture','uses':str(use),'data':'0x000102'}) for use in [0,1,3]]):
        params=json.loads(fixture);cases.append(command('verify',params))
        for field in ['data','publickey','signature']:
            for val in [None,'','bad','0x00',params[field]+'FF',params[field][:-2]]:cases.append(command('verify',{**params,field:val}))
    for params in [{},{'data':'0x00'},{'data':'0x00','publickey':'0x00','signature':'0x000100'}]:cases.append(command('verify',params))
    for name in ['debugflag','txnlock']:cases.append(command(name,{},True))
    for active in [None,'true','false','TRUE',' true ',True,False,1,{},[], '']:
        for var in [None,'','new value',' π 😄 ',True,1,{},[]]:
            cases.append(command('debugflag',{'activate':active,'var':var}));cases.append(command('debugflag',{}))
    for params in [{},{'activate':'true'},{'var':'persistent'},{'activate':'false'},{'activate':'true','var':None},{}]:cases.append(command('debugflag',params))
    for params in [{},{'action':'unlock'},{'action':'lock','timeout':'1'},{},{'action':'lock','timeout':'-1'},{'action':'lock','timeout':'100'},{'action':'unlock','unlockdelay':'-1'},{},{'action':'unlock','unlockdelay':'1'},{}]:cases.append(command('txnlock',params))
    for field in ['action','timeout','unlockdelay']:
        for val in [None,'','invalid',{},[],True,1,'1.5','-1','4294967296']:
            cases.append(command('txnlock',{'action':'list',field:val}))
    expected=run(java,cases);actual=run(cpp,cases);failures=[{'case':c,'java':j,'cpp':n} for c,j,n in zip(cases,expected,actual) if j!=n]
    randomcases=[command('random',{'size':str(size),'type':kind}) for size in [1,2,8,15,32,64,256,1024] for kind in ['sha2','sha3'] for _ in range(8)]+[command('random',{}),command('random',{'size':'32.5'}),command('random',{'size':'4294967328'})]
    def validate_random(encoded,case):
        result=json.loads(encoded);params=json.loads(case)['params'];assert result['status'] is True and result['pending'] is False
        response=result['response'];raw=bytes.fromhex(response['random'][2:]);size=params.get('size','32');assert response['size']==size
        assert len(raw)==(int(float(size))&0xffffffff)
        kind=params.get('type','sha3');assert response['type']==kind
        digest=(hashlib.sha256 if kind=='sha2' else hashlib.sha3_256)(raw).hexdigest().upper();assert response['hashed']=='0x'+digest
        if len(raw)>=16:
            number=int.from_bytes(raw,'big');digits=''
            while number:digits='0123456789ABCDEFGHWJKYMNZPQRSTUV'[number%32]+digits;number//=32
            encoded='Mx'+(digits or '0');assert response['keycode']=='-'.join(encoded[i:i+4] for i in [2,7,12,17,22])
        else:assert 'keycode' not in response
    for runtime in [java,cpp]:
        for case,result in zip(randomcases,run(runtime,randomcases)):
            try:validate_random(result,case)
            except Exception as error:failures.append({'case':case,'runtime':runtime[0],'result':result,'error':str(error)})
    cases+=randomcases
    report={'cases':len(cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Command accessors and metadata, hash/missingcmd, convert malformed Base64/HEX/Mx and arbitrary bytes, checkaddress synthetic wallet cache lookups, random sizes/hash/keycode properties; generic exceptions normalized to ERROR' }
    (build/'command-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({**report,'failures':failures[:6]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
