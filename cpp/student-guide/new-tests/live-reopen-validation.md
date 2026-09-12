# A connected node is not necessarily caught up

A live workflow asks a different question from a unit test. The
[package verifier](../../tools/verify_macos_preview.py) extracts the actual ZIP,
checks every recorded file hash, removes development-library environment overrides,
and launches the shipped console. It compares a fresh peer response with the latest
local status, requiring both block height and block hash to match.

The e82e91f package passed its fresh stage at height2309856. After shutdown and
immediate reopen, it stayed connected but missed the150-second tip target. A later
restart of the same saved data caught up and accepted new blocks through2309869.
Both results are true. Replacing the failed record with the later success would
hide the question the team still needs to answer.

## Read the code before proposing a fix

[TxPoWProcessor](../../src/org/minima/system/brains/TxPoWProcessor.cpp) only validates
an incoming block once it has its parent and all referenced transactions. The pinned
Java body has the same guard. Receiving an ordinary transaction does not itself
turn that invocation into a block-processing pass. The existing
[processor corpus](../../tools/txpow_processor_differential.py) includes out-of-order
blocks and a later missing transaction followed by another block submission.

[NIOMessage](../../src/org/minima/system/network/minima/NIOMessage.cpp) requests
missing transactions from the sending peer. It also throttles a scan of recent chain
nodes. A socket can therefore be busy transferring transactions while the displayed
tip is still behind. These source mechanisms are candidates for investigation;
they are not proof of the cause of this particular timing failure.

The diagnostic log contains real successful block-check results and additions.
A separate pinned-Java control did not complete its fresh stage, so it provides no
successful reopen baseline. The experiment remains inconclusive about whether the
native delay differs from Java. No production algorithm or test time limit was
changed to make the result appear successful.

## Evidence and exercises

Read the [failure, diagnostic and next action](../../validation/delivery-work-in-progress/live-reopen-limit.md)
and the [public status/hash observations](../../validation/delivery-work-in-progress/candidate-e82e91f-diagnostic-observations.json).
The fixture nodes have exited; their temporary wallet data is excluded from delivery.

Which additional timestamps would distinguish network waiting from processor queue
waiting? How would you show that a missing transaction actually delayed a particular
block? Why are equal heights insufficient if hashes differ? Why does a later success
not establish a reliable150-second startup guarantee? Design a comparable Java/C++
experiment before changing a consensus-processing rule.

## A test can also contain a startup bug

The optimized package initially passed fresh startup at height 2309904. On reopen,
the saved tip already matched a valid independent peer response, but the persistent
connection was still starting (`connected = 0`). The verifier asserted that a
connection existed immediately. It should have kept waiting for all readiness
conditions within the same 150-second deadline. The corrected condition retains
both the height/hash comparison and the connection requirement. It also records a
traceback so future failures identify the assertion that failed.

The [captured state](../../validation/delivery-work-in-progress/optimized-readiness-failure-explanation.json)
demonstrates the test bug. The [repeat workflow](../../validation/delivery-work-in-progress/optimized-runtime-corrected.json)
did not connect or receive a chain during its fresh stage and therefore timed out.
It exited cleanly. This repeat does not establish a successful corrected reopen,
and the earlier Debug catch-up failure remains a separate observation. Distinguish
fixing a premature assertion from proving that the application is ready.
