# Lab: assembling a node status report

Written before the new command tests. Read the pinned status.java body in source
order. It combines already tested database, tree, cascade, decimal arithmetic,
file traversal, formatting and network components; the wrapper must not replace
those algorithms with canned answers.

A supplied clean key requests garbage collection even when its value is false,
null or an object. This happens before parsing debug and complete. Collection is
only a hint in Java. The native runtime exposes a callback with a no-op default:
C++ ownership releases objects without a tracing JVM collector. Test native hint
invocation before parameter failures independently; do not claim JVM collection
or equal heap sizes. Validate native controlled heap values exactly and validate
the actual Java heap figure's format and bounds before excluding it from equality.

The complete option reads the tip early, calculates MAX_VAL / difficulty with
seven significant decimal digits and HALF_EVEN rounding, clamps the saved hash
rate to 10,000, then applies MiniNumber division, low-power multiplication and
ceiling. Test normal and low mode, clamping, rounding, missing tips and zero
or extremely small difficulty. A missing tip has defaults only on the ordinary
path; complete can fail before them. Speed uses median-time blocks, including
zero height difference and zero elapsed time. Preserve integer versus string
JSON fields and the sparse empty-chain object.

Use the existing Main fixture, actual database child objects, real synthetic tree
nodes, controlled SQL rows and real temporary files. Load the wallet with a
synthetic seed row, never a user's wallet. Check file sizes, named depth-limited
reports, chain and cascade weights, SQL counters, debug-log order and optional
network composition. Exercise direct parameters and registered runner validation,
fresh command metadata, repeated calls after state changes and the standalone
static main output. No workers or public network connections are needed.

Uptime is sampled separately in Java and C++. Validate the full formatted value
against timestamps surrounding each operation before normalizing that field.
Pin UTC for Date.toString comparisons. JVM/native enhanced exception prose is
already normalized by the shared driver; compare exception classes and stable
messages. These checks do not certify durable SQL, full-node startup, real mining
or exhaustive concurrent singleton replacement.

## Observed results

Both full builds pass without observed compiler warnings. All 187 programs and
634 operations match Java in normal and ASAN/UBSAN builds. Initial expected
values were corrected for plain integer device text and tied median timestamps;
production arithmetic was not changed to match those mistakes. Nonzero cascade
weights, null cascade receivers, malformed stored hash rates and padding without
truncation have explicit coverage.

[Corpus](../../tools/node_status_commands_corpus.py) ·
[Saved evidence](../../validation/checkpoint-362/checkpoint.json)

Broader affected regression validation is still in progress.
