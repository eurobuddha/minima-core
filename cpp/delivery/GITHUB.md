# GitHub repository and development release

Repository: [eurobuddha/MinimaCoreCpp](https://github.com/eurobuddha/MinimaCoreCpp).
The project name and working branch remain `MinimaCoreC++`. The local remote is
`minimacorecpp`; the existing Java upstream/fork remotes are preserved.

The repository is initially private unless the owner explicitly selects public
visibility. A published private release is available only to people with repository
access. The owner can manage visibility and collaborators in GitHub settings.

## Cascade crash replacement

The replacement app is installed locally and its source fixes are pushed, but
`dev-2026-09-12-cascade-fix` has not been published. The checked release list still
contains only `dev-2026-09-12`, whose binaries have the recursive cleanup defect.
See [current delivery status](CURRENT_STATUS.md) and [the fix](CASCADE_CRASH_FIX.md).

## What is published

The working branch contains the committed project and its history, C++ sources,
Java reference gitlink, tests, validation records, all 457 student lessons and
presentation material. Clone with `--recurse-submodules` to populate the reference.
The [development release](https://github.com/eurobuddha/MinimaCoreCpp/releases/tag/dev-2026-09-12)
provides the already verified full source/runtime ZIP and standalone macOS runtime,
each with its checksum. The full project ZIP includes the pinned Java files directly.

Release tag `dev-2026-09-12` identifies the exact packaged source commit
`aea5d892a623271155dd7f2a2af1d06bdecab9b1`. Later branch commits include the cascade fix and app changes. They do not
retroactively change the artifacts attached to this older release. The release
notes retain all incomplete-parity, sanitizer and live-startup limits.

## Reproducing the publication workflow

1. Commit source, tests, lessons and documentation after relevant checks pass.
2. Verify a local recovery bundle and the release artifact checksums.
3. Push the named branch to `minimacorecpp` without rewriting existing history.
4. Create a new annotated release tag at the packaged source commit and push it.
5. Upload artifacts to a draft development release, verify the uploaded assets,
   then publish it. Never overwrite a previously published release asset or tag.
6. Verify the remote branch/tag and a fresh clone, including pinned Java and lessons.

This workflow reuses the local checkpoint rules and the existing project packagers.
It follows the no-overwrite release pattern in the user's companion-app release
script, without invoking its Android build or unrelated repository operations.
Generated packages, local wallets/databases and uncommitted graphify changes are
not staged into the project. The original source and license notices stay intact.
