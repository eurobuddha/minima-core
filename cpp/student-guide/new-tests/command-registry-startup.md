# Command initialization, registry parity and a live peer

## What failed before main

CommandRunner used a global C++ vector containing every command prototype. Its
construction created burn, whose default window reads MiniNumber::ZERO. The
MiniNumber object lived in another translation unit and was not initialized yet.
The sanitized executable reported a null reference and the ordinary node crashed
before main. Compiling the program and testing isolated methods had not established
that the linked executable could start.

The fix reuses MinimaFunction::allFunctions: a function-local static owns the vector.
C++ initializes it once at first access, including when several threads arrive
together. The accessor returns a reference, so replacing an element still affects
lookup. Callers that save the registry copy the vector and restore its elements.
This changes the C++ spelling to ALL_COMMANDS(), not the source command behavior.
Arbitrary concurrent mutation is not made safe by initialization synchronization.

## Check the actual source registry

Pinned CommandRunner.java contains 116 entries, including benchmark, jnitest and
jniminingtest. An earlier audit checked Command.java and incorrectly said the three
were unregistered. The port now constructs every prototype in the source order.
These are actual caller implementations; their unavailable external JNI library
remains an explicit limitation.

The new Java/native probe starts eight threads at a barrier before first access.
Every thread must obtain the same registry. The driver extracts the 116 names from
the pinned source, compares every position and all metadata, and checks fresh
factories and empty parameter objects. It changes the first prototype and verifies
that lookup observes the change, then restores it. Configuring RPC port 19005 before
first access checks that constructor metadata sees runtime configuration. Actual
JNI command dispatch must raise the same missing-library error in both runtimes.
There are 480 independent checks per build; successful JNI algorithms are not tested.

The existing help suite retains all 761 cases and adds six detailed-help cases.
It now requires all 116 names and passes 767 comparisons / 2,579 checks per build.
Its initial failure is saved: it still expected three commands to be missing.
Updating that assertion strengthened coverage rather than hiding missing behavior.
Both full builds and the affected runner, error, burn and entry tests pass.

## Connection is different from synchronization

The first isolated node connected to a local Java peer, received a normal IBD and
reached block 2,226,689. Java and C++ returned identical full JSON for block
2,226,688, and the native node reopened the saved chain. The user then identified
that Java peer as old and supplied tip 2,309,561. Agreement with an old peer was
not evidence of current synchronization; that claim was corrected.

At the user's request, the isolated native node was stopped gracefully. Its data
was preserved by rename and fresh data was created. Initial connection to
eurobuddha.com:9001 delivered 1,053 blocks, about 5.0 MB, with a cascade ending at
2,308,514. The native node reached 2,309,569, beyond the reported tip. This fresh
start used normal IBD, not MegaMMR resync. Current observations are saved with
[checkpoint evidence](../../validation/command-registry-startup-work-in-progress/progress.json).
No wallet/private-key data is included in the Git checkpoint.

A tip height alone is insufficient for complete consensus parity. Compare hashes,
observe subsequent blocks, verify persistence, and audit rejection behavior and
remaining API gaps. One mapped file, jnifunctions.cpp, is still absent; the other
456 paths include one entirely inactive Java source. None is promoted to complete
parity by this exercise.

Exercise: why can a command's individual tests pass while adding its prototype to
a globally initialized registry crashes the whole executable? Why do two nodes
agreeing on a block not prove that either has the latest chain?

The follow-up peer ping reported block 2,309,573 and hash
`0x000000788D1C288566FCB61FE9C49C593B104418978FE7B83D66FA32F3C8C6E8`.
Native status matched both fields, proving acceptance of four subsequent blocks
after the first observation. This is a recorded agreement with the designated
peer at that moment; the chain continues moving.
