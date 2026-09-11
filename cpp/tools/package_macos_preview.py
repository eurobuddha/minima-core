#!/usr/bin/env python3
"""Freeze an existing native node and its dylibs into a relocatable local preview."""
import argparse, hashlib, json, re, shutil, subprocess, zipfile
from datetime import datetime, timezone
from pathlib import Path

CPP = Path(__file__).resolve().parents[1]
ROOT = CPP.parent

def run(*args):
    return subprocess.check_output([str(a) for a in args], text=True).strip()

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def dependencies(path):
    return [line.strip().split(' (compatibility version', 1)[0]
            for line in run('otool', '-L', path).splitlines()[1:]]

def system_library(name):
    return name.startswith(('/usr/lib/', '/System/Library/'))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=CPP/'build')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--port', type=int, default=21001)
    args = parser.parse_args()
    if not 1 <= args.port <= 65535:
        parser.error('--port must be between 1 and 65535')
    output = args.output.resolve()
    if output.exists():
        raise SystemExit('Choose a new output folder; existing previews and their data are never overwritten.')
    binary = args.build.resolve()/'MinimaCoreC++'
    head = run('git', '-C', ROOT, 'rev-parse', 'HEAD')
    if run('git', '-C', ROOT, 'status', '--porcelain', '--', 'cpp/src', 'cpp/support/minima_entry.cpp', 'cpp/CMakeLists.txt'):
        raise SystemExit('Checkpoint the node sources before packaging them.')
    file_info = run('file', binary)
    if 'Mach-O 64-bit executable arm64' not in file_info:
        raise SystemExit('This packager is for the native Apple Silicon macOS build.')
    original_binary_hash = sha(binary)
    output.mkdir(parents=True)
    (output/'bin').mkdir(); (output/'lib').mkdir(); (output/'licenses').mkdir()
    copied = {binary: output/'bin/MinimaCoreC++'}
    pending = [binary]
    edges = {}
    source_hashes = {}
    while pending:
        source = pending.pop()
        destination = copied[source]
        source_hashes[str(source)] = sha(source)
        shutil.copy2(source, destination)
        edges[source] = []
        for dep in dependencies(source):
            if system_library(dep):
                continue
            candidate = Path(dep.replace('@loader_path', str(source.parent)))
            if dep.startswith('@rpath') or not candidate.is_absolute() or not candidate.is_file():
                raise RuntimeError(f'Unresolved dependency: {source}: {dep}')
            candidate = candidate.resolve()
            if candidate == source.resolve():  # dylib's own install name
                continue
            edges[source].append((dep, candidate))
            if candidate not in copied:
                target = output/'lib'/candidate.name
                if target in copied.values():
                    raise RuntimeError(f'Duplicate dylib basename: {candidate.name}')
                copied[candidate] = target
                pending.append(candidate)
    if not (output/'lib/libminima_h2.dylib').exists():
        raise RuntimeError('The working native H2 provider must be bundled.')
    for source, destination in copied.items():
        destination.chmod(0o755)
        if destination.suffix == '.dylib':
            run('install_name_tool', '-id', '@loader_path/'+destination.name, destination)
        for dep, candidate in edges[source]:
            relative = '../lib/' if destination.parent.name == 'bin' else ''
            run('install_name_tool', '-change', dep, '@loader_path/'+relative+copied[candidate].name, destination)
        commands = run('otool', '-l', destination)
        for rpath in re.findall(r'cmd LC_RPATH\s+cmdsize \d+\s+path (.*?) \(offset', commands):
            run('install_name_tool', '-delete_rpath', rpath, destination)
        run('codesign', '--force', '--sign', '-', destination)
        run('codesign', '--verify', '--strict', destination)
        for dep in dependencies(destination):
            if not system_library(dep) and not dep.startswith('@loader_path/'):
                raise RuntimeError(f'Non-relocatable dependency remains: {dep}')
            if dep.startswith('@loader_path/') and not Path(dep.replace('@loader_path', str(destination.parent))).is_file():
                raise RuntimeError(f'Missing packaged dependency: {dep}')
    launcher = output/'Start MinimaCoreC++.command'
    launcher.write_text('''#!/bin/zsh
set -eu
umask 077
preview_dir="${0:A:h}"
cd "$preview_dir"
print -- "MinimaCoreC++ — development preview"
print -- "Initial peer: eurobuddha.com:9001 | Default node port: 21001"
print -- "Data: $preview_dir/data"
print -- "Type help for commands; type quit for a clean shutdown."
print -- "First startup generates a new wallet and downloads the chain."
print -- ""
exec "$preview_dir/bin/MinimaCoreC++" \\
  -data "$preview_dir/data" -port 21001 \\
  -connect eurobuddha.com:9001 -host 127.0.0.1 -desktop -nossl "$@"
'''.replace('21001',str(args.port)))
    launcher.chmod(0o755)
    for name in ['LICENSE', 'BOUNCY_CASTLE_LICENSE', 'OPENJDK_TIMSORT_LICENSE', 'REUSED_CODE_LICENSE']:
        shutil.copy2(CPP/name, output/'licenses'/name)
    shutil.copy2(ROOT/'upstream-study/LICENSE', output/'licenses/MINIMA_JAVA_LICENSE')
    shutil.copy2('/opt/homebrew/opt/openssl@3/LICENSE.txt', output/'licenses/OPENSSL_LICENSE.txt')
    shutil.copy2('/opt/homebrew/opt/icu4c@78/share/icu/78.3/LICENSE', output/'licenses/ICU_LICENSE.txt')
    with zipfile.ZipFile(ROOT/'upstream-study/lib/mysql-connector-java-8.0.24.jar') as jar:
        (output/'licenses/MYSQL_CONNECTOR_LICENSE').write_bytes(jar.read('META-INF/LICENSE'))
    provider_source = args.build.resolve()/'native-h2-archive/manifest.json'
    provider = json.loads(provider_source.read_text()) if provider_source.exists() else None
    if provider:
        # Record the dependency/toolchain versions without retaining machine build paths.
        (output/'native-database.json').write_text(json.dumps({k:v for k,v in provider.items() if k not in ['command','library']}, indent=2)+'\n')
    (output/'README.txt').write_text(f'''MinimaCoreC++ — local development preview
Source checkpoint: {head}
Platform: Apple Silicon (arm64), macOS 26 or later. Built and tested on this Mac.

START
Double-click "Start MinimaCoreC++.command" to open the native console in Terminal.
Keep bin/ and lib/ beside the launcher. No Java installation or build tools are
needed to run it. This is a fixed snapshot: further source rebuilds do not replace it.

The launcher uses eurobuddha.com:9001, local node port {args.port}, RPC disabled,
and its own new wallet/chain under data/ inside this folder. The development
node uses a different directory and port 16001. Data is preserved across starts.
The initial key generation and chain download take time. Run one instance of
this preview folder at a time. Type quit and wait for shutdown before closing.

TRY THESE COMMANDS (one line at a time)
help
status
network
ping host:eurobuddha.com:9001
hash data:hello
help command:txpow
quit

This minima-core console does not include the MiniDapp/MDS graphical dashboard.
It is a development preview with 456 of 457 mapped source paths present; complete
behavioral parity is unfinished. The external JNI library remains unavailable.
Use the preview's separate wallet for testing. Do not copy an existing wallet in.

MOVING / KEEPING THIS PREVIEW
Move the whole folder, including data/ after it has been created. The original
ZIP contains no wallet or chain data. The adjacent SHA256 file identifies it.
manifest.json records the source checkpoint and hashes of bundled files.

DEPENDENCIES
Bundled: OpenSSL 3, ICU 78, and the existing H2/MySQL dependency compiled to native
code with GraalVM. The Minima node is C++; the database dependency is compiled
from its proven Java implementation, with no JVM fallback or Java process.
macOS supplies the system libraries. Relevant local notices are under licenses/.
H2 source: https://github.com/h2database/h2database/tree/version-2.4.240
GraalVM source: https://github.com/oracle/graal
This package is for your local testing; it is not a notarized public release.

To report a problem, include the command, actual/expected response and the source
checkpoint above. Keep seed phrases, private keys and wallet databases private.
''')
    manifest = dict(source_head=head, utc=datetime.now(timezone.utc).isoformat(),
                    platform='macOS 26+ arm64', source_binary_sha256=original_binary_hash,
                    source_dependencies_sha256=source_hashes, native_database=bool(provider),
                    package_kind='local development preview', full_parity=False, node_port=args.port,
                    file_sha256={str(p.relative_to(output)):sha(p) for p in sorted(output.rglob('*')) if p.is_file()})
    (output/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    archive = output.with_suffix('.zip')
    if archive.exists():
        raise RuntimeError(f'Archive already exists: {archive}')
    run('ditto', '-c', '-k', '--keepParent', output, archive)
    archive.with_suffix('.zip.sha256').write_text(sha(archive)+'  '+archive.name+'\n')
    print(json.dumps(dict(folder=str(output), launcher=str(launcher), archive=str(archive),
                         source_head=head, bundled_libraries=len(copied)-1,
                         zip_sha256=sha(archive)), indent=2))

if __name__ == '__main__':
    main()
