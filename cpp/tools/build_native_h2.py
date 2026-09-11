#!/usr/bin/env python3
"""Compile the pinned H2 dependency to a native C library; never use JVM fallback."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
H2_SHA256='b80baaf431ce458c926d72ec2b8cc33b120c76b73abe4301278bb2044ddde9f5'
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--graalvm',type=Path,required=True);parser.add_argument('--output',type=Path,default=ROOT/'build/native-h2');args=parser.parse_args()
    toolchain=args.graalvm.resolve();output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
    jar=ROOT.parent/'upstream-study/lib/h2-2.4.240.jar'
    if hashlib.sha256(jar.read_bytes()).hexdigest()!=H2_SHA256:raise ValueError('Pinned H2 artifact changed')
    source=ROOT/'native/h2/NativeH2Bridge.java';classes=output/'classes';classes.mkdir(exist_ok=True)
    version=subprocess.check_output([str(toolchain/'bin/native-image'),'--version'],text=True)
    subprocess.run([str(toolchain/'bin/javac'),'-cp',str(jar),'-d',str(classes),str(source)],check=True)
    if any(not p.name.startswith('NativeH2Bridge') for p in classes.rglob('*.class')):
        raise ValueError('Bridge classes directory contains unrelated classes; choose a fresh output directory')
    command=[str(toolchain/'bin/native-image'),'-J-Xmx8g','--parallelism=4','-O1','--no-fallback','--shared','-cp',f'{classes}:{jar}','-o','libminima_h2']
    subprocess.run(command,cwd=output,check=True)
    libraries=list(output.glob('libminima_h2.*'));library=next(p for p in libraries if p.suffix in ['.dylib','.so','.dll'])
    manifest=dict(h2_version='2.4.240',h2_sha256=H2_SHA256,bridge_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),toolchain=version,command=command,library=str(library),library_sha256=hashlib.sha256(library.read_bytes()).hexdigest(),minima_java_classes_included=False,jvm_fallback=False)
    (output/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n');print(json.dumps(manifest,indent=2))
if __name__=='__main__':main()
