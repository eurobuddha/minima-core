#!/usr/bin/env python3
"""Real dual-stack loopback lifecycle against the unmodified pinned NIOServer."""
import argparse,json,os,socket,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
JAVA=ROOT.parent/'upstream-study'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--build',type=Path,default=ROOT/'build');args=parser.parse_args();build=args.build.resolve();oracle=build/'nio-server-oracle';oracle.mkdir(exist_ok=True)
    sources=sorted((JAVA/'src').rglob('*.java'))+[ROOT/'support/NIOClientOracle.java',ROOT/'support/NIOServerOracle.java']
    listing=build/'nio-server-java-sources.txt';listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac','-cp',str(JAVA/'jar/minima.jar'),'-d',str(oracle),'@'+str(listing)],check=True)
    def run(command):
        result=subprocess.run(command,capture_output=True,text=True,timeout=40,env={**os.environ,'UBSAN_OPTIONS':'halt_on_error=1'})
        if result.returncode:raise RuntimeError(f'{command[0]} exited {result.returncode}\n{result.stdout}\n{result.stderr}')
        return [line for line in result.stdout.splitlines() if not line.startswith('Minima @ ')]
    expected=run(['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOServerOracle'])
    actual=run([str(build/'minima_nio_server')])
    checkpoints=['started','incoming4','incoming6','outgoing','frame:0','frame:1','frame:8193','frame:65536','broadcast-filter','explicit-disconnect','outgoing-eof','oversized-frame','shutdown-retains-map','traffic:73750:73760']
    if expected!=checkpoints:raise RuntimeError(f'Incomplete Java lifecycle: {expected}')
    startup=[]
    commands=[['java','-cp',f'{oracle}:{JAVA}/jar/minima.jar','NIOServerOracle'],[str(build/'minima_nio_server')]]
    for mode,port in [("startup",-1),("startup",65536),("pre-shutdown",0)]:
        for command in commands:
            result=subprocess.run(command+[mode,str(port)],capture_output=True,text=True,timeout=10)
            if result.returncode!=0 or 'returned' not in result.stdout.splitlines():raise RuntimeError(f'Invalid port failed: {result}')
        startup.append(f'invalid-port:{port}' if mode=='startup' else 'shutdown-before-run')
    with socket.socket(socket.AF_INET6,socket.SOCK_STREAM) as occupied:
        occupied.setsockopt(socket.IPPROTO_IPV6,socket.IPV6_V6ONLY,0)
        occupied.bind(('::',0));occupied.listen(1);port=occupied.getsockname()[1]
        for command in commands:
            result=subprocess.run(command+['startup',str(port)],capture_output=True,text=True,timeout=10)
            if result.returncode!=0 or 'returned' in result.stdout.splitlines() or 'ALREADY IN USE. SHUTTING DOWN' not in result.stdout+result.stderr:raise RuntimeError(f'Fatal bind differs: {result}')
        startup.append('occupied-port:halt(0)')
    report={'passed':expected==actual,'startup':startup,'java':expected,'cpp':actual,'scope':'Actual NIOServer event loop over IPv4/IPv6 loopback; port zero, incoming/outgoing metadata and identity, framed binary traffic, direct/broadcast send and greeting filter, explicit disconnect, outgoing EOF reconnect, oversized incoming frame, shutdown retained map and traffic totals. Manager collection replaces node startup. No remote network or full-node integration.'}
    (build/'nio-server-differential-results.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return expected!=actual
if __name__=='__main__':raise SystemExit(main())
