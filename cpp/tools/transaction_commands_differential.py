#!/usr/bin/env python3
"""Compare offline custom transaction command sequences with pinned Java."""
import argparse,itertools,json,os,random,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'transaction-commands-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/TxnDBOracle.java',ROOT/'support/TransactionCommandsOracle.java',ROOT/'support/SearcherOracle.java',ROOT/'support/ArchiveCacheOracle.java'];listing=build/'transaction-commands-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n');subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    java=['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','TransactionCommandsOracle'];native=[str(build/'minima_transaction_commands')];rng=random.Random(5564)
    def run(command,cases):
        process=subprocess.run(command,input='\n'.join(json.dumps(c,ensure_ascii=True) for c in cases)+'\n',capture_output=True,text=True,timeout=180,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        lines=process.stdout.splitlines()
        if process.returncode or len(lines)!=len(cases):
            (build/'transaction-commands-crash.txt').write_text(process.stderr);raise RuntimeError(f'{command[0]} exit {process.returncode}, lines {len(lines)} expected {len(cases)}')
        return lines
    def cmd(name,**params):return {'name':name,'params':params}
    def sequence(*ops):return {'ops':list(ops)}
    def seed(index=3,id='test'):return {'fixture':index,'id':id}
    names=['txncreate','txndelete','txnlist','txnclear','txnstate','txnexport','txnimport','txnview','txnoutput']
    cases=[sequence({'name':name,'params':{},'meta':True}) for name in names]
    values=[None,True,False,0,1,{},[],'',' ','0','1','test','missing','all','ALL','😀']
    for name in names:
        cases.append(sequence(seed(),cmd(name)))
        for value in values:cases.append(sequence(seed(),cmd(name,id=value),cmd('txnlist')))
    for port in ['0','1','255','256','-1','2147483647','-2147483648','2147483648','4294967296','1.5','1e2',' 1 ','+1','٠','１２','𝟙']:
        for value in ['0','1.5','-1e-45','18446744073709551615','1e80','TRUE','False','0xab','0Xff','0x','0xGG','[hello 🐼]','[1,2]','hello',' ']:cases.append(sequence(seed(),cmd('txnstate',id='test',port=port,value=value),cmd('txnlist',id='test',transactiononly='true')))
    for flags in itertools.product(['true','false','TRUE',None],repeat=3):cases.append(sequence(seed(),cmd('txnclear',id='test',**dict(zip(['scripts','mmr','signatures'],flags))),cmd('txnexport',id='test',showtxn='true')))
    fixtures=json.loads(run(java,[{'kind':'fixtures'}])[0]);folder=build/'transaction-command-fixtures';folder.mkdir(exist_ok=True)
    for index,fixture in enumerate(fixtures):
        data=fixture['row'];path=folder/f'input-{index}.txn';path.write_bytes(bytes.fromhex(data[2:]));output=folder/f'output-{index}.txn'
        for value in [data,data+'FF',data[:-2],data[:12],'0x0000','0x000000000000']:
            for name in ['txnimport','txnview']:
                cases.append(sequence(seed(),cmd(name,data=value,id='test'),cmd('txnexport',id='test'),cmd('txnlist')))
        cases.append(sequence(cmd('txnimport',file=str(path)),cmd('txnexport',id=f'txn-{index}😀',file=str(output)),cmd('txnview',file=str(output)),cmd('txndelete',id='all'),cmd('txnimport',file=str(output),id='new'),cmd('txnexport',id='new',showtxn='true')))
        cases.append(sequence(cmd('txnimport',file=str(path),data='0x00',id='test'),cmd('txnexport',id='test',showtxn='true')))
    for file in [folder/'missing.txn',folder,folder/'input-0.txn']:
        for name in ['txnimport','txnview']:cases.append(sequence(seed(),cmd(name,file=str(file),id='renamed',data=fixtures[0]['row'])))
    for repeat in range(100):
        ops=[seed(i,str(i)) for i in range(4)]
        for step in range(30):
            name=rng.choice(names);params={'id':rng.choice(['0','1','2','3','new','all','😀'])}
            if name=='txnstate':params.update(port=str(rng.randrange(-1,257)),value=rng.choice(['1','-1','TRUE','[hello]','0xFF','bad']))
            if name=='txnlist':params['transactiononly']=rng.choice(['true','false'])
            if name=='txnclear':params.update({key:rng.choice(['true','false']) for key in ['scripts','mmr','signatures']})
            if name=='txnexport':params['showtxn']=rng.choice(['true','false'])
            if name in ['txnimport','txnview']:params['data']=rng.choice(fixtures)['row']
            ops.append(cmd(name,**params))
        cases.append(sequence(*ops))
    for caller in ['0x00','0x1234']:
        for line in ['txncreate id:test;txnstate id:test port:0 value:[hello];txnlist','txncreate id:test;txncreate id:test;txndelete id:all','txncreate id:a;txncreate id:b;txnlist transactiononly:true;txndelete id:all;txnlist','txncreate id:test;txnview id:x data:0x00;txnlist','txnlist id:missing;txncreate id:test','txncreate bad:test;txnlist','txncreate id:test;txnexport id:test showtxn:true']:
            cases.append(sequence({'line':line,'caller':caller}))
    tokenfixtures=json.loads(run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','SearcherOracle'],[{'kind':'fixtures'}])[0])
    for amount in ['0','-1','1','1.2345','18446744073709551615','1e80','1e-44','1e-45','bad',None,True]:
        for address in ['0x01','0x','bad','0Xff','0x'+('11'*32)]:
            cases.append(sequence(seed(),cmd('txnoutput',id='test',amount=amount,address=address),cmd('txnlist')))
    for token,tokenid in zip(tokenfixtures['tokens'],tokenfixtures['tokenids']):
        for amount in ['-1','0','1','12.345','1e-44']:
            for store in ['true','false','TRUE',None]:cases.append(sequence(cmd('tokens',action='import',data=token),seed(),cmd('txnoutput',id='test',amount=amount,address='0x22',tokenid=tokenid,storestate=store),cmd('txnexport',id='test',showtxn='true')))
    for tokenid in ['0x00','0x0000','0xFF','0x99','Mxinvalid','bad',None]:cases.append(sequence(seed(),cmd('txnoutput',id='test',amount='1',address='0x22',tokenid=tokenid),cmd('txnlist')))
    cases.append(sequence({'line':'txncreate id:test;txnoutput id:test amount:1 address:0x11;txnlist transactiononly:true'}))
    expected=run(java,cases);actual=run(native,cases);failures=[{'case':case,'java':j,'cpp':n} for case,j,n in zip(cases,expected,actual) if j!=n]
    result={'cases':len(cases),'operations':sum(len(c['ops']) for c in cases),'passed':len(cases)-len(failures),'failures':failures,'scope':'Nine transaction commands: metadata, create/delete/list, state mutation, witness clearing/monotonic flags, ordered wire snapshots, Java/native file and HEX import/export, invalid/corrupt inputs and dispatcher sequences; generic exceptions normalized'}
    (build/'transaction-commands-differential-results.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({**result,'failures':[{'case':f['case'],'java':f['java'][:400],'cpp':f['cpp'][:400]} for f in failures[:3]]},indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
