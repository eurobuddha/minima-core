# Three source entry points and their observable behavior

The [Java wrapper](../../support/RuntimeMainsOracle.java) and
[native wrapper](../../support/runtime_mains.cpp) call the real main methods.
They replace only the logger's timestamp/memory prefix and count notifications
through the existing receiver interface. The [driver](../../tools/runtime_mains_differential.py)
compares null, empty and unused argument arrays.

## Miner

TxPoWMiner.main performs 10,000 old-style hashes, then 1,000,000 old-style hashes,
then 1,000,000 header hashes. The implementation keeps every iteration. The source
has no live printing, so the test checks successful silent completion and records
duration. It does not infer correct internal invocation counts from elapsed time or
claim performance equality. Existing miner/hash helpers have separate functional tests.

## TreeKey

A fixed seed, four keys per level and four levels yield reproducible signature
objects. The same tree signs the same byte twice. Comparing both complete pretty
JSON outputs catches lost key-use state; parsing them also checks that they differ.
Four MinimaLogger calls must reach the notification receiver.

## Cascade and the file logger

The Windows-looking filename is kept literally. On this Unix host it is a relative
filename containing backslashes, so an isolated temporary current directory contains
all test files. The existing CascadeOracle produces shared bytes for empty, valid
and invalid-parent inputs. The driver checks the exact output, notification count,
and unchanged input bytes. A missing file logs its exact path then produces Correct:true
for the still-empty cascade. An invalid parent logs a diagnostic and produces false.

The work corrected the default cascade checker and loadObjectSlow's missing-file
message to use MinimaLogger. The existing helper implementations remain in use.
Malformed file exception logging, other file methods, all provider effects and full
class parity are outside this focused result.

Results: [runtime main evidence](../../validation/runtime-mains-work-in-progress/README.md).
Read the recorded terminal reports before describing a pass.

## Exercise

Why does a missing file leave an empty cascade that passes this checker? Why is that
not evidence that a real chain was loaded? Why must the two signatures use the same
TreeKey object, and why cannot elapsed time prove the exact benchmark loop count?
