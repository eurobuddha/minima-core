#!/usr/bin/env python3
"""Real MegaMMR exports, import/resync, and opposite-runtime wallet reopen."""
import argparse, hashlib, json, shutil, struct, subprocess, tempfile
from pathlib import Path
from p2p_functions_differential import ROOT, JAVA
from native_restore_startup import Node, ready, key_state, chain
from native_archive_startup import coins
from peers_checker_differential import Runner, Responder


def coin_proofs(node):
    row = node.ask('coins')
    assert row['status'] and row['response'], row
    proofs = {}
    for coin in row['response']:
        row = node.ask('coinexport coinid:'+coin['coinid'])
        assert row['status'], row
        proofs[coin['coinid']] = row['response']['data']
    return proofs


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT/'build')
    build = parser.parse_args().build.resolve()
    oracle = build/'mega-node-oracle'
    oracle.mkdir(exist_ok=True)
    sources = sorted((JAVA/'src').rglob('*.java')) + [ROOT/'support'/n for n in [
        'ArchiveCommandOracle.java', 'NIOClientOracle.java', 'ArchiveCacheOracle.java']]
    listing = build/'mega-node-sources.txt'
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac', '-cp', str(JAVA/'jar/minima.jar'), '-d', str(oracle), '@'+str(listing)], check=True)
    classpath = f'{oracle}:{JAVA}/jar/minima.jar'
    commands = [['java', '-cp', classpath, 'org.minima.Minima', '-megammr'],
                [str(build/'MinimaCoreC++'), '-megammr']]
    checks, failures, live, runners, exports = [], [], [], [], []
    encodings = {}
    server = None
    processes = 0
    stage = 'baseline'
    try:
        with tempfile.TemporaryDirectory(prefix='mega-node-', dir=build) as scratch:
            root = Path(scratch)
            baseline = root/'baseline'
            backups = []
            original_chain = original_coins = original_keys = original_proofs = None
            for runtime in [0, 1]:
                stage = f'export writer {runtime}'
                folder = baseline if runtime == 0 else root/'native-writer'
                if runtime:
                    shutil.copytree(baseline, folder)
                node = Node(commands[runtime], folder, scratch)
                live.append(node)
                keys = ready(node, 7)
                current_chain, current_coins = chain(node), coins(node)
                proofs = coin_proofs(node)
                assert current_chain['block'] == 1, current_chain
                if runtime == 0:
                    original_chain, original_coins, original_keys, original_proofs = current_chain, current_coins, keys, proofs
                else:
                    assert (current_chain, current_coins, keys, proofs) == (original_chain, original_coins, original_keys, original_proofs)
                backup = root/f'export-{runtime}.megammr'
                row = node.ask(f'megammr action:export file:{backup}')
                assert row['status'] and backup.is_file() and row['response']['ibdtip'] == 1, row
                backups.append(backup)
                exports.append(dict(runtime=['java', 'cpp'][runtime], bytes=backup.stat().st_size,
                                    sha256=hashlib.sha256(backup.read_bytes()).hexdigest(), owned_proofs=len(proofs)))
                node.finish(quit=True)
                live.remove(node)
                processes += 1
            assert backups[0].read_bytes() == backups[1].read_bytes(), 'MegaMMR export bytes differ'

            stage = 'saved chain and proof serving'
            runners = [Runner(['java', '-cp', classpath, 'ArchiveCommandOracle']),
                       Runner([str(build/'archive_command_runtime')])]
            folders = []
            for runtime in [0, 1]:
                folder = root/f'fixture-{runtime}'
                shutil.copytree(baseline/'1.1', folder)
                folders.append(folder)
            def encode(**request):
                values = [r.ask(dict(folder=str(folders[i]), megaEnabled=True, **request)) for i, r in enumerate(runners)]
                assert values[0] == values[1], values
                assert 'wire' in values[0], values
                return values[0]
            peers = '127.0.0.1:19001,127.0.0.2:19002'
            saved = encode(op='mega', task='saved', path=str(backups[0]), peers=peers)
            assert saved['backupIBD'] == saved['currentIBD']
            assert sorted(saved['proofs']) == sorted(original_proofs.values()), 'Served coin proofs differ from actual exports'
            assert saved['weight'] == str(original_chain['weight']), (saved['weight'], original_chain)
            preflight = encode(op='encodeReply', type=16, blocks=[])
            request = encode(op='encodeRequest', start='-1')['wire'][2:].lower()
            encodings = dict(java_cpp_identical=True, actual_owned_proofs=len(saved['proofs']),
                             default_native_callback=True, file_integrity_checked=True,
                             mega_reply_bytes=len(bytes.fromhex(saved['wire'][2:])),
                             ibd_sha256=hashlib.sha256(bytes.fromhex(saved['currentIBD'][2:])).hexdigest())
            for runner in runners:
                runner.close()
            runners = []
            server = Responder()

            for runtime in [0, 1]:
                for mode in ['import', 'resync']:
                    stage = f'{["java", "cpp"][runtime]} {mode}'
                    target = root/f'target-{runtime}-{mode}'
                    shutil.copytree(baseline, target)
                    node = Node(commands[runtime], target, scratch)
                    live.append(node)
                    assert ready(node, 100) == {k: 100 for k in original_keys}
                    if mode == 'import':
                        text = f'megammr action:import file:{backups[1-runtime]}'
                    else:
                        server.configure([preflight['wire'], saved['wire']])
                        text = f'megammrsync action:resync host:127.0.0.1:{server.port}'
                    row = node.ask(text, timeout=300)
                    assert row['status'], row
                    assert row['response']['message'] == ('MegaMMR import finished.. please restart' if mode == 'import' else 'MegaMMR sync fininshed.. please restart'), row
                    if mode == 'resync':
                        assert row['response']['coins'] == len(original_proofs), row
                    node.finish()
                    live.remove(node)
                    processes += 1
                    if mode == 'resync':
                        trace = server.trace()
                        assert trace == [request, saved['request'][2:].lower()], trace
                        raw = (target/'1.1/databases/p2p.db').read_bytes()
                        size = struct.unpack('>i', raw[:4])[0]
                        assert size == len(raw)-4
                        assert json.loads(raw[4:])['peers'] == [dict(host='127.0.0.1', port=19001), dict(host='127.0.0.2', port=19002)]

                    node = Node(commands[1-runtime], target, scratch)
                    live.append(node)
                    expected = {k: 100 for k in original_keys}
                    assert key_state(node) == expected
                    assert chain(node) == original_chain and coins(node) == original_coins
                    assert coin_proofs(node) == original_proofs
                    public = sorted(expected)[0]
                    signed = node.ask(f'sign publickey:{public} data:0x010203')
                    assert signed['status'], signed
                    signature = signed['response']
                    verified = node.ask(f'verify publickey:{public} data:0x010203 signature:{signature}')
                    assert verified['status'] and verified['response'] == 'Signature valid', verified
                    rejected = node.ask(f'verify publickey:{public} data:0x010204 signature:{signature}')
                    assert not rejected['status'] and rejected['message'] == 'Signature NOT valid', rejected
                    assert key_state(node) == {**expected, public: 101}
                    node.finish(quit=True)
                    live.remove(node)
                    processes += 1
                    checks.append(dict(runtime=['java', 'cpp'][runtime], mode=mode, writer_and_reopen=['java', 'cpp'][1-runtime],
                                       keys=len(expected), retained_uses=100, chain_coins_proofs_preserved=True,
                                       real_worker_shutdown=True, signature_valid=True, altered_data_rejected=True,
                                       signing_use_increment=1, exact_request_frames=mode == 'resync',
                                       peers_saved_after_shutdown=mode == 'resync'))
                    print('Verified MegaMMR workflow: '+json.dumps(checks[-1]), flush=True)
    except Exception as error:
        failures.append(dict(stage=stage, error=repr(error),
                             nodes=[dict(stdout=n.output[-10000:], stderr=n.errors()) for n in live]))
    finally:
        for node in live:
            node.cancel()
        if server:
            server.close()
        for runner in runners:
            try:
                runner.close()
            except Exception as error:
                failures.append(dict(close_error=repr(error)))
    report = dict(passed=not failures, node_processes=processes, exports=exports, encodings=encodings, checks=checks, failures=failures,
                  scope='Actual pinned Java/native solo nodes with MegaMMR and real H2. Identical exports, saved-data default proof-serving callback and file integrity helper; import and loopback resync in both runtimes, exact frames and saved peers, opposite-runtime chain/coin/proof/key reopen and signing. Genesis only, existing-wallet branch: no pruned-coin recovery, new-height advancement, cascades, full inbound network serving, phrase/backup resync, delayed timeouts or full parity claim.')
    (build/'native-mega-startup-results.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report, indent=2))
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
