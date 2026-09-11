#!/usr/bin/env python3
"""Actual database mains, real H2 files, and both readers for every writer."""
import argparse,copy,json,os,re,subprocess,tempfile,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');build=ap.parse_args().build.resolve();oracle=build/'database-mains-oracle';oracle.mkdir(exist_ok=True)
    listing=oracle/'sources.txt';sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/DatabaseMainsOracle.java'];listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','DatabaseMainsOracle'],[str(build/'database_mains')]]
    report=dict(passed=False,actual_main_runs=0,cross_reads=0,main_comparisons=0,assertions=0,observations=[])
    def check(value,detail):report['assertions']+=1;assert value,detail
    def call(writer,action,name,mode,home):
        p=subprocess.run(commands[writer]+[action,name,mode,str(home)],capture_output=True,text=True,timeout=90,env={**os.environ,'TZ':'UTC','UBSAN_OPTIONS':'halt_on_error=1'});check(p.returncode==0 and not p.stderr,(action,name,writer,p.returncode,p.stderr,p.stdout[:1000]));return json.loads(p.stdout)
    def stable_txpow(value):
        result=copy.deepcopy(value)
        # Each omitted value is independently checked against both readers of
        # the exact saved wire. Time is also bounded by the writer's clock window.
        del result['txpowid'];del result['superblock'];del result['header']['timemilli'];del result['header']['date'];del result['body']['prng'];return result
    try:
        with tempfile.TemporaryDirectory(prefix='database mains π ',dir=build) as folder:
            for name in ['ArchiveManager','TxPoWOnChainDB','TxPoWSqlDB']:
                homes=[Path(folder)/name/str(w) for w in [0,1]];start=int(time.time()*1000)
                for home in homes:home.mkdir(parents=True);(home/'keep.txt').write_text('outside source deletion scope')
                previous=[[],[]]
                for iteration,mode in enumerate(['null','empty','args'],1):
                    projections=[]
                    for writer,home in enumerate(homes):
                        if name=='ArchiveManager':
                            child=home/'testfolder'/'nested';child.mkdir(parents=True,exist_ok=True);(child/'old.txt').write_text('must be wiped')
                        before=int(time.time()*1000);result=call(writer,'main',name,mode,home);after=int(time.time()*1000);report['actual_main_runs']+=1
                        check(result['error']=='none',(name,mode,writer,result))
                        readers=[call(r,'read',name,mode,home) for r in [0,1]];report['cross_reads']+=2;check(readers[0]==readers[1],(name,mode,writer,'cross-reader difference',readers));rows=readers[0]
                        report['observations'].append(dict(name=name,mode=mode,writer=['java','cpp'][writer],clock_window=[before,after],main=result,readers=readers))
                        count=1 if name=='ArchiveManager' else iteration*(3 if name=='TxPoWOnChainDB' else 1);check(len(rows)==count,(name,count,rows))
                        check((home/'keep.txt').read_text()=='outside source deletion scope','main changed adjacent file')
                        file=home/('testfolder/sqlsync.mv.db' if name=='ArchiveManager' else 'testsql.mv.db');check(file.stat().st_size>0,'no durable H2 file')
                        if name=='ArchiveManager':check(not (home/'testfolder/nested/old.txt').exists(),'archive failed to wipe old folder')
                        else:check(rows[:len(previous[writer])]==previous[writer],(name,'prior rows changed'))
                        for row_index,row in enumerate(rows):
                            check(row['id']>0 and (row_index==0 or row['id']>rows[row_index-1]['id']),'positive increasing identity; gaps allowed')
                            check(start<=row['timemilli']<=after,('SQL timestamp outside complete run',row))
                            if name=='TxPoWOnChainDB':
                                number=row_index%3+1;check(row['block']==number and row['blockid']==['0xFF','0xEE','0xDD'][number-1] and row['txpowid']==f'txPowID-0{number}',row)
                            else:
                                txpow=row['txpow'];check(row['wire']==row['wireagain'],(name,'wire roundtrip'));check(re.fullmatch(r'0x[0-9A-F]{64}',txpow['body']['prng']) is not None,'real 32-byte PRNG field')
                                check(start<=int(txpow['header']['timemilli'])<=after,('header timestamp',txpow))
                                check(txpow['header']['superparents'][0]['parent']=='0xFFEEFF',txpow)
                                if name=='ArchiveManager':check(row['block']==1 and txpow['header']['block']=='1' and row['txpowid']==row['before_parent_id'] and row['txpowid']!=txpow['txpowid'],('source calculates ID before changing parent',row))
                                else:check(row['txpowid']==txpow['txpowid'] and row['isrelevant']==1 and row['parentid']=='0xFFEEFF' and row['isblock']==int(txpow['isblock']) and row['istransaction']==int(txpow['istransaction']),row)
                        previous[writer]=rows
                        lines=result['stdout'].splitlines()
                        if name=='TxPoWOnChainDB':
                            check(len(lines)==2,lines);outputs=[json.loads(line) for line in lines];project=[]
                            for number,value in enumerate(outputs,1):
                                expected={k:v for k,v in rows[number-1].items() if k!='id'};expected['found']=True;check(value==expected,(value,expected));del value['timemilli'];project.append(value)
                            projections.append(project)
                        elif name=='TxPoWSqlDB':
                            check(len(lines)==1 and lines[0].startswith('IN :'),lines);printed=json.loads(lines[0][4:]);check(printed==rows[-1]['txpow'],('printed vs saved TxPoW',printed,rows[-1]));check(before<=int(printed['header']['timemilli'])<=after,'new header timestamp');projections.append(stable_txpow(printed))
                        else:
                            loaded=[line for line in lines if line.startswith('Sync Loaded : ')];check(len(loaded)==1,lines);printed=json.loads(loaded[0][14:]);check(printed==rows[0]['txpow'],('loaded output differs from saved bytes',printed,rows[0]));check(before<=int(printed['header']['timemilli'])<=after,'new archive timestamp');
                            check(lines.count('DB Size : 1')==1 and lines.count('Sync Range : 0')==1,lines)
                            projections.append([stable_txpow(printed) if line.startswith('Sync Loaded : ') else line for line in lines])
                    check(projections[0]==projections[1],(name,mode,'main output difference',projections));report['main_comparisons']+=1
        report['passed']=True
        report['scope']='18 real main invocations; null/empty/unused arguments; same-home repeat behavior; 36 full saved-file reads across Java/native H2. Exact cross-reader rows, wire roundtrips, JSON, source pre-parent ID ordering, SQL metadata, bounded timestamps and adjacent file preservation. Main projections omit only independently checked time/date/hash/superblock and 32-byte PRNG fields. No injected database faults, provider shutdown-error/identity-allocation equivalence, arbitrary home values, random distributions, concurrency or full class parity claim.'
    except BaseException as error:
        report['error']=repr(error);raise
    finally:
        (build/'database-mains-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='observations'},indent=2))
if __name__=='__main__':main()
