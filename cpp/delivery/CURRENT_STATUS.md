# Current handover — 12 September 2026

The objective remains the full 457-file 1:1 port and behavioral parity. It is not
complete. This page supersedes older delivery prose; historical evidence stays
intact and must be read with its recorded source identity.

## Installed development app

`/Applications/MinimaCoreC++ Preview.app` is the installed configurable Apple
Silicon/macOS 26+ app. Its application source is `36940c6`; the optimized node
checkpoint is `10ed9e6`, including the cascade cleanup fix. The native binary UUID
is `54ADF25C-F029-37BA-B111-1425F410DF96`. The bundled
`Contents/Resources/app-build.json` records full hashes and source identities;
[the inspected identity](../validation/parity-resumption-20260912/installed-app-identity.json)
is retained with this update.

The app is Developer ID signed; notarization is not complete. The UI uses Electron;
the node is C++. The H2/MySQL dependency is compiled ahead of time from Java and
runs without a JVM. These facts do not establish an entirely C++ dependency stack.

Use Start node and the built-in command field. Stop node cleanly or quit the app
and let its owned node exit before changing builds. Data is outside the app. The
owner's selected folder is `/Users/eurobuddha/minimaCoreC++21001`, on port 21001;
keep it intact. A new user's default folder is under Application Support.

Ordinary mainnet starts omit `-connect`. The optional manual initial peer is blank
by default, applies to one explicitly requested launch, is not saved, and clears
after Start. A manual connection disables automatic P2P for that session, matching
the pinned Java semantics. Older saved peer values are ignored. See the
[application instructions](../macos-app/README.md) and
[student lab](../student-guide/new-tests/app-initial-peer.md).

## Development changes after the installed app

GenerateKey's RSA cipher now supports private encryption/public recovery, including
empty plaintext. The full utility corpus passes 1,492 cases in each of normal,
optimized and ASan/UBSan builds; [evidence and limits](../validation/rsa-cipher-directions/README.md)
are retained. This source correction has not replaced the installed app. It adds
behavior in an existing counterpart and does not change the 456/457 inventory.

The base API audit additionally exposed thirteen source-named public methods and
corrected MiniByte's mutable FALSE semantics. The full API corpus passes 1,853
cases / 22,184 independent assertions in each of the three configurations. The
7,705-case Streamable regression, original base tests, native scalar tests and
actual node builds also passed in each configuration. See
[helper evidence](../validation/base-stream-helpers/README.md). These changes also
remain separate from the installed preview and do not change the counterpart count.

The next base-class audit corrects MiniByte and MiniString virtual dispatch,
including getter evaluation order and the MiniString source copy constructor.
Its regression retains earlier cases and adds 2,328 subclass cases. All 4,181 API
cases / 40,534 independent assertions pass in normal, optimized and sanitizer
builds, along with the affected primitive/Streamable/original-string regressions
and actual node builds. See
[dispatch evidence](../validation/base-virtual-dispatch/README.md) for final results
and [concrete remaining API gaps](../validation/base-virtual-dispatch/next-api-audit.md).
The counterpart inventory remains 456/457; these source changes are not installed
in the preview.

## Publication and recovery

The source branch `MinimaCoreC++` is pushed through `9fa16e1` to the private
`eurobuddha/MinimaCoreCpp` repository. The only published development release in
[the checked release list](../validation/parity-resumption-20260912/github-releases.json)
is `dev-2026-09-12`. Its binaries precede the cascade cleanup fix.
`dev-2026-09-12-cascade-fix` is a proposed release name, not a published release.
A local ZIP of the installed app source `36940c6` is prepared: all 375 regular
payload files were byte-verified against the signed app. Its
[manifest and checksum](../validation/app-notarization-approval-20260912/prepared-app-artifact.json)
identify exactly what is ready for review. Automatic approval review rejected
uploading this app to Apple's notarization service without explicit approval for
that payload. No submission occurred. Notarization, refreshed full-project package,
uploaded-asset verification and replacement publication remain pending. Do not
retry or bypass the rejected upload until explicit approval arrives.

Local commits have verified Git recovery bundles. The old preview/wallet and the
previous app have separate rollback copies. Wallet data is not in source or release
assets. Replacing the installed app does not replace the saved node data.

## Parity and acceptance evidence

| Requirement | Current evidence | Remaining work |
| --- | --- | --- |
| 457 unique Java/C++ mappings | 456 counterparts: 455 partial, one inactive-source preservation | `jnifunctions.cpp` is absent; obtain original native bodies/contracts |
| Behavioral equivalence | All 457 map entries remain `not_verified`; scoped Java/C++ checks exist | Audit remaining APIs, dispatch, mutation, errors and runtime integration; correct mismatches |
| Cascade crash correction | Nine selected suites passed in Debug, optimized and ASan/UBSan; 60 long-chain worker runs | Sustained operation and untested concurrency schedules are not certified |
| Complete sanitizer validation | Earlier broad run ended with three unfinished suites; targeted fixes passed | Full SPHINCS, SPHINCS command and Magic runs resumed separately; no pass claimed until terminal evidence |
| Mainnet operation | Prior successful sync observations coexist with failed connection/reopen attempts | Establish reliable automatic P2P, sync, reopen and long-running operation; validate resync/recovery |
| Student handover | 457 authored file lessons plus change-specific labs | Keep lessons aligned with each correction; structural checks do not certify full parity |
| Shareable replacement | Installed signed app with verified source bytes | Notarize, package, verify and publish replacement artifacts |

The unchanged native JNI implementation remains unavailable in the inspected
sources. Recording callbacks test callers and cannot replace its implementation.
This blocks that file, not independent parity work. Do not add a placeholder to
make the count 457. `check_full_port` must continue failing until its full
acceptance requirements are met.

[Historical broad test results](VALIDATION_STATUS.md),
[cascade regression results](../validation/cascade-release/README.md), and
[application regression results](../validation/cascade-release/app/peer-once/window-results.json)
cover different source revisions and scopes. No single full-current-suite pass is
claimed. The newly inspected sanitizer inventory contains 298 registered tests;
optional live MySQL tests remain separately configured.
