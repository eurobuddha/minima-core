# Lab: checking the chain and cascade boundary

Written before implementing the new command-specific tests.

healthcheck reads the actual MinimaDB tree tip first. If there is no tip, it throws
CommandException with No TIP block found.. ?. Otherwise it obtains the root and
returns both heights as strings. Its chainlength is tip minus root, without adding
one: a single-node chain reports zero. Preserve MiniNumber arithmetic and output
strings even for unusual synthetic heights.

For a nonempty cascade, tipcorrect means only that cascade tip height equals tree
root minus one. cascadelength counts linked cascade nodes. An empty cascade emits
the literal string nocacade instead of an object. The help mentions Maxima, but the
body does not inspect Maxima or output a Maxima section. Do not add that behavior.

Reuse the Main fixture's real MinimaDB, tree-node test constructor, TxPowTree
recalculation and Cascade.addToTip. Seed synthetic height sequences and optional
branches, then invoke the actual registered command. Use matching and mismatching
cascade heights, nonconsecutive heights, empty trees and cascades, and repeated
calls after replacing state. The command should read current state without adding
queued messages, SQL calls or notifications. A null cascade should fail only after
the tree checks; an empty tree should still produce the explicit missing-tip error.

Independently assert exact response values for a root-only tree, a matching
multi-node cascade, a mismatch and a gapped chain. Compare metadata, fresh command
instances, direct parameters versus the runner's empty valid-parameter list, and
admin/non-admin dispatch with pinned Java. Parent hashes are deliberately irrelevant
to this wrapper: height adjacency is not a cryptographic validity certificate.

The fixture leaves Main workers unstarted and does not establish concurrent owner
replacement or full-node persistence. State construction is controlled test setup;
actual chain/cascade getters and arithmetic run in the command. Existing tree and
cascade tests cover their deeper algorithms separately.

## Command-specific results

Both full builds pass without observed compiler warnings. All 176 programs and
635 operations match Java in normal and ASAN/UBSAN builds. The initial out-of-range
height exposed an incorrect setup expectation; the retained rejection case checks
that no tip was created, and every intended successful setup is now asserted.

[Corpus and independent assertions](../../tools/healthcheck_commands_corpus.py) ·
[Shared Main driver](../../tools/main_runtime_differential.py)

All fourteen affected regressions pass in both builds (115.26s normal, 247.61s
ASAN/UBSAN). These include 2,892 cascade cases, 1,027 tree cases, 324 existing chain
command programs, the Main fixture and related command/runner comparisons. All
twenty-six JSON reports were read without failures. [Saved evidence](../../validation/checkpoint-360/checkpoint.json)
records the scope and limitations.
