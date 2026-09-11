#!/usr/bin/env python3
"""Real old-backup restore/sync with loopback IBD and cross-runtime reopen."""
import argparse, hashlib, json, shutil, subprocess, tempfile, time
from pathlib import Path
from p2p_functions_differential import ROOT, JAVA
from native_restore_startup import Node, ready, key_state, chain
from native_archive_startup import coins
from peers_checker_differential import Runner, Responder


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT/'build')
    build = parser.parse_args().build.resolve()
    oracle = build/'restoresync-remote-oracle'
    oracle.mkdir(exist_ok=True)
    sources = sorted((JAVA/'src').rglob('*.java')) + [ROOT/'support'/n for n in [
        'ArchiveCommandOracle.java', 'NIOClientOracle.java', 'ArchiveCacheOracle.java']]
    listing = build/'restoresync-remote-sources.txt'
    listing.write_text('\n'.join('"'+str(p)+'"' for p in sources)+'\n')
    subprocess.run(['javac', '-cp', str(JAVA/'jar/minima.jar'), '-d', str(oracle), '@'+str(listing)], check=True)
    classpath = f'{oracle}:{JAVA}/jar/minima.jar'
    commands = [['java', '-cp', classpath, 'org.minima.Minima'], [str(build/'MinimaCoreC++')]]
    checks, failures, live, runners, encodings = [], [], [], [], {}
    server = None
    processes = 0
    stage = 'baseline'
    try:
        with tempfile.TemporaryDirectory(prefix='restoresync-remote-', dir=build) as scratch:
            root = Path(scratch)
            baseline = root/'baseline'
            node = Node(commands[0], baseline, scratch)
            live.append(node)
            original_keys = ready(node, 7)
            original_chain, original_coins = chain(node), coins(node)
            assert original_chain['block'] == 1, original_chain
            node.finish(quit=True)
            live.remove(node)
            processes += 1

            stage = 'retime saved genesis and encode IBD'
            runners = [Runner(['java', '-cp', classpath, 'ArchiveCommandOracle']),
                       Runner([str(build/'archive_command_runtime')])]
            folders, retimed = [], []
            for runtime, runner in enumerate(runners):
                folder = root/f'retimed-{runtime}'
                shutil.copytree(baseline, folder)
                folders.append(folder)
                row = runner.ask(dict(op='retimeGenesis', folder=str(folder/'1.1'), time='0'))
                assert row.get('bodyUnchanged') and row.get('mmrUnchanged'), row
                assert row['oldID'] == original_chain['hash'] and row['id'] != row['oldID'], row
                retimed.append(row)
            assert retimed[0] == retimed[1], 'Retimed tree/block encodings differ'
            expected_chain = {**original_chain, 'hash': retimed[0]['id']}
            def encode(**request):
                values = [r.ask(dict(folder=str(folders[i]/'1.1'), **request)) for i, r in enumerate(runners)]
                assert values[0] == values[1], values
                assert 'wire' in values[0], values
                return values[0]
            empty = encode(op='encodeReply', type=16, blocks=[])
            nonempty = encode(op='encodeReply', type=16, blocks=[retimed[0]['block']])
            assert empty['ibd'] != nonempty['ibd']
            requests = {n: encode(op='encodeRequest', start=str(n))['wire'][2:].lower() for n in [-1, 1, 2]}
            encodings = dict(tree_sha256=hashlib.sha256(bytes.fromhex(retimed[0]['tree'][2:])).hexdigest(),
                             block_sha256=hashlib.sha256(bytes.fromhex(retimed[0]['block'][2:])).hexdigest(),
                             nonempty_ibd_bytes=len(bytes.fromhex(nonempty['ibd'][2:])),
                             java_cpp_identical=True, body_unchanged=True, mmr_unchanged=True)
            for runner in runners:
                runner.close()
            runners = []

            backups = []
            for runtime, folder in enumerate(folders):
                stage = f'backup writer {runtime}'
                node = Node(commands[runtime], folder, scratch)
                live.append(node)
                assert key_state(node) == original_keys
                assert chain(node) == expected_chain and coins(node) == original_coins
                backup = root/f'backup-{runtime}.bak'
                row = node.ask(f'backup file:{backup} password:fixture-remote')
                assert row['status'] and backup.is_file(), row
                backups.append(backup)
                node.finish(quit=True)
                live.remove(node)
                processes += 1

            server = Responder()
            modes = [
                ('nonempty', [empty['wire'], nonempty['wire'], empty['wire']], [-1, 1, 2], 263),
                ('empty', [empty['wire'], empty['wire']], [-1, 1], 263),
                ('preflight_failed', [None, None, None], [-1, -1, -1], 7),
                ('later_failed', [empty['wire'], nonempty['wire'], None, None, None], [-1, 1, 2, 2, 2], 263),
            ]
            for runtime in [0, 1]:
                for mode, replies, starts, uses in modes:
                    stage = f'{["java", "cpp"][runtime]} {mode}'
                    target = root/f'target-{runtime}-{mode}'
                    shutil.copytree(baseline, target)
                    node = Node(commands[runtime], target, scratch)
                    live.append(node)
                    assert ready(node, 100) == {k: 100 for k in original_keys}
                    server.configure(replies)
                    before = time.monotonic()
                    row = node.ask(f'restoresync file:{backups[1-runtime]} password:fixture-remote host:127.0.0.1:{server.port}', timeout=300)
                    elapsed = time.monotonic() - before
                    assert row['status'] and row['restore']['file'] == str(backups[1-runtime]), row
                    assert row['message'] == 'Restart Minima for restore to take effect!', row
                    node.finish()
                    live.remove(node)
                    processes += 1
                    trace = server.trace()
                    assert trace == [requests[n] for n in starts], dict(expected=starts, trace=trace)
                    if mode.endswith('failed'):
                        assert elapsed >= 20, elapsed
                    assert not (target/'1.1/restore').exists()

                    node = Node(commands[1-runtime], target, scratch)
                    live.append(node)
                    expected = {k: uses for k in original_keys}
                    assert key_state(node) == expected
                    assert chain(node) == expected_chain and coins(node) == original_coins
                    public = sorted(expected)[0]
                    signed = node.ask(f'sign publickey:{public} data:0x010203')
                    assert signed['status'], signed
                    signature = signed['response']
                    verified = node.ask(f'verify publickey:{public} data:0x010203 signature:{signature}')
                    assert verified['status'] and verified['response'] == 'Signature valid', verified
                    rejected = node.ask(f'verify publickey:{public} data:0x010204 signature:{signature}')
                    assert not rejected['status'] and rejected['message'] == 'Signature NOT valid', rejected
                    assert key_state(node) == {**expected, public: uses+1}
                    node.finish(quit=True)
                    live.remove(node)
                    processes += 1
                    checks.append(dict(restorer=['java', 'cpp'][runtime], writer_and_reopen=['java', 'cpp'][1-runtime],
                                       mode=mode, request_starts=starts, exact_request_frames=True,
                                       seconds=round(elapsed, 3), keys=len(expected), restored_uses=uses,
                                       chain_and_coins_preserved=True, graceful_shutdown=True,
                                       signature_valid=True, altered_data_rejected=True, signing_use_increment=1))
                    print('Verified remote restore: '+json.dumps(checks[-1]), flush=True)
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
    report = dict(passed=not failures, node_processes=processes, encodings=encodings, checks=checks, failures=failures,
                  scope='Actual pinned Java/native nodes and H2; deliberately retimed valid solo genesis, unchanged body/MMR, identical codecs. Opposite-runtime backups and reopen; real loopback preflight, nonempty retained genesis IBD, empty archive, failed preflight/later batch with real ten-second retries, exact request frames, key-use effects and signing. No new-height advancement, cascades, multiple nonempty batches, memory resets, delayed read/connect timeouts or full parity claim.')
    (build/'native-restoresync-remote-results.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report, indent=2))
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
