# MinimaCoreC++

A runnable native MinimaCore console for Apple Silicon macOS, developed against
[pinned Java source](../upstream-study) at
`5564b9d78978efcaf23679190d05253961fc6e69` (1.1.2.8).

Start with the [team and student handover](delivery/START_HERE.md). It links the
executable instructions, build/test instructions, presentation, lessons and current
validation. The [existing port21001 preview](PREVIEW.md) remains a separate frozen
snapshot with its own data.

## Current scope

[The map](SOURCE_MAP.md) assigns all457 Java files unique C++ paths.456 counterparts
exist:455 active partial implementations and one inactive-source preservation.
`jnifunctions.cpp` remains absent because the original external native implementation
is unavailable. Full class/API parity remains unverified. The finite missing-main
candidate audit is now exhausted; that is not a full API certificate.

The native node starts, connects to the designated peer and saves its chain. The
latest packaged fresh start matched the peer; the bounded reopen check missed its
150-second target, while a later diagnostic restart caught up and followed new
blocks. Read the [actual validation results](delivery/VALIDATION_STATUS.md) and
[known gaps](delivery/KNOWN_GAPS.md) before drawing a broader conclusion.

The node uses native OpenSSL/ICU and the proven H2/MySQL dependency compiled ahead
of time from Java. It launches no JVM at runtime. This does not mean every dependency
was rewritten in C++.

## Build, study and review

- [Build and test instructions](delivery/BUILD_AND_TEST.md)
- [All457 file lessons](student-guide/INDEX.md) and [study route](student-guide/how-to-study-this-port.md)
- [Offline presentation](delivery/presentation.html) and [speaker notes](delivery/PRESENTATION.md)
- [Current limitations](delivery/KNOWN_GAPS.md) and [continuation](CONTINUATION.md)
- [Historical porting checkpoints](PORTING_HISTORY.md) and [review history](REVIEW.md)

Source mapping checks verify inventory and hashes. `check_full_port` deliberately
fails while complete parity is unverified. Historical reports remain traceable and
are not silently relabelled as final-candidate passes. Local commits and verified
recovery bundles are retained; nothing has been pushed or sent to the team.
