# MinimaCoreC++ team and student handover

This delivery contains a runnable native MinimaCore console, its source, the pinned
Java reference, behavioral test evidence, and one lesson for each of the 457 mapped
Java files. It is a development delivery. Full behavioral parity remains unverified.

## First session

Open the runtime folder and double-click `Start MinimaCoreC++.command`. Keep its
`bin` and `lib` folders beside the launcher. It runs on Apple Silicon, macOS 26 or
later, without a separate Java or Homebrew installation. The default node port is
21001 and the initial peer is eurobuddha.com:9001. RPC is disabled. The launcher
creates a separate `data` directory. An existing node using port 21001 must either
finish cleanly first or the new launcher must receive a different `-port` argument.

In Terminal, a separate instance can use:

```sh
./Start\ MinimaCoreC++.command -port 22001
```

Useful first commands are `help`, `status`, `network`, `hash data:hello`, and `quit`.
Wait for `Shut down completed OK..` before closing Terminal. Initial wallet key
generation and chain synchronization take time. The terminal may show a height
behind a peer while sync is still running. Compare both height and block hash to
a fresh peer response when demonstrating synchronization.

## Reading routes

- **Team demonstration:** [offline presentation](presentation.html), [editable outline and speaker notes](PRESENTATION.md), and [demo script](DEMO.md).
- **Students:** [guide index](../student-guide/README.md), then [study route](../student-guide/how-to-study-this-port.md).
- **Developers:** [build and test instructions](BUILD_AND_TEST.md), [native database dependency](../native/h2/README.md), and [current continuation](../CONTINUATION.md).
- **Reviewers:** [known gaps](KNOWN_GAPS.md), the validation directory, and the delivery test report when the final run completes.

Each source file lesson links the pinned Java file, native counterpart, logic and
relevant tests. Both implementations are included in the same source layout so
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

Local Git checkpoints and independently verified recovery bundles protect the
working project. They are retained locally and are not a substitute for an off-machine
backup. No push or message to the team is performed as part of this delivery.

## Reporting a defect

Record the source commit, operating system, command or test name, expected result,
actual result, and the smallest reproducible input. Include sanitized logs. Keep
wallet seeds, private keys and wallet databases out of reports. A behavioral mismatch
belongs with its source/native file pair and its student lesson.
