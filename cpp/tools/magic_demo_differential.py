#!/usr/bin/env python3
"""Run the complete Magic.main with durable output and an explicit runtime budget.

Uses the original demo checks from magic_differential.py. This separate replay
allows a saved checkpoint executable to finish without repeating the field corpus.
"""
import argparse,hashlib,json,os,subprocess,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];JAVA=ROOT.parent/'upstream-study'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,default=ROOT/'build');ap.add_argument('--binary',type=Path);ap.add_argument('--native-source-commit',required=True);ap.add_argument('--timeout',type=int,default=7200);args=ap.parse_args();build=args.build.resolve();binary=(args.binary or build/'minima_magic').resolve()
    if args.timeout<=0:ap.error('--timeout must be positive')
    oracle=build/'magic-demo-oracle';oracle.mkdir(exist_ok=True);sources=sorted((JAVA/'src').rglob('*.java'));listing=oracle/'sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    started=time.monotonic();results={};failures=[];checks=0;outputs=[]
    common=dict(native_source_commit=args.native_source_commit,native_binary=str(binary),native_binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),java_pin=subprocess.check_output(['git','-C',str(JAVA),'rev-parse','HEAD'],text=True).strip(),iterations=86400,expected_lines=1729,timeout_per_runtime=args.timeout)
    progress=build/'magic-demo-progress.json'
    for name,command in [('java',['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','org.minima.objects.Magic']),('cpp',[str(binary),'--demo'])]:
        stdout=build/f'magic-demo-replay-{name}.txt';stderr=build/f'magic-demo-replay-{name}.stderr';now=time.monotonic();code=None;timed_out=False
        progress.write_text(json.dumps(dict(**common,active_runtime=name,stdout=str(stdout),stderr=str(stderr),completed=results),indent=2)+'\n')
        with stdout.open('wb') as out,stderr.open('wb') as err:
            try:code=subprocess.run(command,stdout=out,stderr=err,timeout=args.timeout,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'}).returncode
            except subprocess.TimeoutExpired:timed_out=True
        content=stdout.read_bytes();errors=stderr.read_bytes();outputs.append(content);checks+=1
        results[name]=dict(exit=code,timed_out=timed_out,seconds=round(time.monotonic()-now,3),bytes=len(content),lines=len(content.splitlines()),sha256=hashlib.sha256(content).hexdigest(),stderr=errors.decode(errors='replace'))
        if code!=0 or timed_out or errors or len(content.splitlines())!=1729 or not content.startswith(b'Start:1024 Desired:3000\n1) 1024.0625\n'):failures.append(dict(runtime=name,details=results[name]))
    checks+=1
    if outputs[0]!=outputs[1]:failures.append(dict(phase='exact_stdout_comparison'))
    report=dict(**common,passed=not failures,independent_assertions=checks,seconds=round(time.monotonic()-started,3),runtimes=results,failures=failures,scope='Full unmodified 86400-update Magic.main executed in Java and the identified native checkpoint binary. Exact stdout, 1729 lines, independently derived first update, successful exits and empty stderr. Does not replace field/API tests or establish full node parity.')
    (build/'magic-demo-replay-results.json').write_text(json.dumps(report,indent=2)+'\n');progress.write_text(json.dumps(dict(**common,active_runtime=None,completed=results,passed=not failures),indent=2)+'\n');print(json.dumps(report,indent=2));return bool(failures)
if __name__=='__main__':raise SystemExit(main())
