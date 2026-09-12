# MinimaCoreC++ team and student handover

This delivery contains a runnable native MinimaCore console, its source, the pinned
Java reference, behavioral test evidence, and one lesson for each of the 457 mapped
Java files. It is a development delivery. Full behavioral parity remains unverified.

The [cascade cleanup replacement](CASCADE_CRASH_FIX.md) addresses the September 11
preview crash. Read it before selecting an executable from the earlier delivery.

## First session

For the current installed preview, open `/Applications/MinimaCoreC++ Preview.app`.
Read [current build and publication status](CURRENT_STATUS.md) before selecting a
GitHub download: the existing published release predates the crash fix.

The app lets you choose the node port, data folder, network and role, then start
and stop the node and enter commands directly. Its default port is 21001. Data is
outside the app, so replacing the app preserves it. Leave the manual initial peer
blank for automatic P2P; an explicitly entered peer applies to one launch only.
The node requires no separate Java or Homebrew installation. On a fresh mainnet
node, allow wallet setup and synchronization to complete before judging readiness.

The older folder-based distributions use `Start MinimaCoreC++.command` with their
adjacent `bin` and `lib` folders. Those archived builds have separate identities;
do not infer their contents from the current branch or installed app.

Useful first commands are `help`, `status`, `network`, `hash data:hello`, and `quit`.
Wait for `Shut down completed OK..` before closing a console session. Initial wallet key
generation and chain synchronization take time. The terminal may show a height
behind a peer while sync is still running. Compare both height and block hash to
a fresh peer response when demonstrating synchronization.

## Reading routes

[Changes in this delivery](CHANGES.md) separate file counts, behavior corrections,
validation and remaining blockers.

- **Team demonstration:** [offline presentation](presentation.html), [editable outline and speaker notes](PRESENTATION.md), and [demo script](DEMO.md).
- **Students:** [guide index](../student-guide/README.md), then [study route](../student-guide/how-to-study-this-port.md).
- **Developers:** [build and test instructions](BUILD_AND_TEST.md), [native database dependency](../native/h2/README.md), and [current continuation](../CONTINUATION.md).
- **Reviewers:** [known gaps](KNOWN_GAPS.md), the [validation report](VALIDATION_STATUS.md) and its linked evidence.

Each source file lesson links the pinned Java file, native counterpart, logic and
relevant tests. The comparison reference is `project/upstream-study`, at the recorded pin. Root
`project/src` and `project/jar` preserve the surrounding repository checkout and
can contain later work; use the pinned tree linked by each lesson.

Both implementations are included in the same source layout so
those relative links keep working after extraction.

## What the native build contains

The node implementation is C++. It uses native OpenSSL and ICU libraries and the
existing H2/MySQL dependency compiled ahead of time from Java using GraalVM. It does
not launch a JVM at runtime. This distinction matters when discussing the request
for a C++ port: a native executable does not imply every dependency was rewritten
in C++. The package includes dependency information and local license notices.

## Evidence and recovery

The final delivery manifest records the exact source commit, pinned Java commit,
and SHA-256 of every payload file. The ZIP has a separate checksum. The packaging
check reads every payload back from the ZIP. Runtime verification additionally
extracts the standalone runtime ZIP into a different path, runs with a system-only
PATH, checks the command console and exercises startup/shutdown/reopen.

[Local recovery instructions](RECOVERY.md) explain how to restore a verified bundle.
Local Git checkpoints and independently verified recovery bundles protect the
working project. They are retained locally and are not a substitute for an off-machine
backup. The source and development release are now available through the
[GitHub repository](GITHUB.md). No separate message or invitation to the team was sent.

## Reporting a defect

Record the source commit, operating system, command or test name, expected result,
actual result, and the smallest reproducible input. Include sanitized logs. Keep
wallet seeds, private keys and wallet databases out of reports. A behavioral mismatch
belongs with its source/native file pair and its student lesson.
