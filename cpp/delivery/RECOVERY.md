# Recovering a local checkpoint

The shareable project ZIP is a source snapshot, not a Git repository. Local recovery
bundles are retained separately under `local-backups/`. Each bundle has a JSON
verification record containing its source commit, SHA-256 and recovered file hashes.
These bundles are local copies. The committed source is also published in the
[GitHub repository](GITHUB.md); runtime wallet data is not part of that publication.

Use the exact bundle and expected checksum from its verification record. The
following procedure was exercised against a dedicated recovery directory. Never
run the checkout step over the working project or an existing wallet directory.

```sh
shasum -a 256 /absolute/path/MinimaCoreC++-TIMESTAMP.bundle
git bundle verify /absolute/path/MinimaCoreC++-TIMESTAMP.bundle
git clone --no-checkout /absolute/path/MinimaCoreC++-TIMESTAMP.bundle MinimaCoreCpp-recovered
cd MinimaCoreCpp-recovered
git rev-parse HEAD
git checkout HEAD -- .
git fsck --connectivity-only
git worktree add --detach upstream-study 5564b9d78978efcaf23679190d05253961fc6e69
python3 cpp/tools/source_map.py --check
python3 cpp/student-guide/tools/audit_guides.py
```

Compare `git rev-parse HEAD` with the verification record before restoring the
working tree. The original Java commit is present in the verified bundle, so the
pinned `upstream-study` worktree can be restored without network access. A normal
fresh clone from the original remote instead uses its submodule initialization.

The recovery checks verify bundle integrity, expected commit, pinned source,
changed checkpoint file contents and the restored student links. They do not back
up runtime wallet or chain data. The shareable delivery deliberately excludes those
private databases. Unrelated uncommitted graphify output is preserved separately
locally and is not part of the C++ checkpoint.

See the [build instructions](BUILD_AND_TEST.md) after restoring the source. Keep
AOT database provider metadata beside its native library. The standalone executable
package supplies its own provider for running; rebuilding the provider requires the
documented development toolchain.
