# HORST: from secret preimages to a verifiable tree

This lab follows the pinned legacy algorithm, not a modern SPHINCS standard.
The four source classes live in `org/minima/utils/sphincs/HORST`.

A seed expands to 65,536 secrets. Secret `i` is SHA3-256 of the **serialized**
seed followed by the serialized `MiniNumber(i)`. Hashing that secret's **raw**
32 bytes gives its public value. A leaf commits to that public value and the
index, with `i` as its sum. Parents commit to both child hashes and their combined
sum. The complete root sum is 0 + 1 + ... + 65,535 = 2,147,450,880.

To sign, hash the raw message and split its 32 bytes into sixteen unsigned
big-endian pairs. Each pair selects a secret and its sixteen-step MMR proof.
The verifier hashes each revealed secret, reconstructs the indexed leaf, and
checks the proof's final hash **and sum**. An altered message, secret, sibling,
direction, or sum should therefore fail. Repeated indices still occupy separate
signature slots; entries beyond the first sixteen are ignored by verification.

## Planned checks, written before the first test

1. Build real 65,536-leaf trees in Java and C++. Compare all-secret and all-public
   digests, root values, selected secrets, proof bytes, and signature bytes.
2. Independently reconstruct secret expansion and the entire tree with Python's
   SHA3-256. This catches a mistake shared by the comparison fixtures.
3. Feed each runtime signatures produced by the other. Check valid signatures,
   changed messages and roots, every signature position, missing entries, null
   entries, extra entries, and malformed proof encodings.
4. Read truncated data into an existing signature. Keep references to its old
   lists: Java replaces both lists before reading the count, then adds a complete
   secret before attempting its proof. Compare the consumed byte count, partial
   new state, detached old lists, and partial output from a failing reserialization.
5. Test the converter separately: it catches I/O errors, logs, and returns partial
   state. It does not catch arbitrary runtime exceptions. Test index arithmetic
   at signed-int boundaries and null arguments without invoking C++ undefined behavior.
6. Run the source demonstration and compare its output. Use sanitizer builds to
   check native memory and arithmetic safety; passing examples do not prove the
   cryptographic construction secure or the entire node equivalent.

## Completed execution

Both complete native builds passed. Both normal and sanitized runs passed all
**1,438 comparisons and 1,299 independent assertions**. HORST took
108.81s and 287.37s respectively. Each build also passed the existing
9,464 object/MMR and 1,364 utility comparisons. The full 239-test suite was not run.

The initial run's 20 differences were bounds-error messages omitted by Java's JIT
after repeated exceptions. Disabling fast-throw diagnostic omission in the test
VM made those messages stable; the original report is retained. Tests compare
exception classes and messages, except NPE/EOF prose and runtime stack frames.
They compare logger counts and the first two logged exception categories.

[Saved evidence](../../validation/horst-work-in-progress/progress.json) includes
both final reports, exact known vectors, build logs and demo output. Full FORS/
SPHINCS integration, arbitrary subclasses, concurrent mutation and raw Java
MiniData byte-array aliases remain open. This lab certifies none of the full
node's unverified parity and does not establish cryptographic security.

## Work through the source demonstration

For seed `0x00112233` and message `0xFFEEDD`, the raw message hash is
`0x16BF37C42B04EC4EE043B7B6964578329E02693F9D13BF20426CF241FA642650`. Its first pair is `0x16BF`, which selects index **5823**. The
serialized index is `0x000216BF`: scale byte, byte-count byte, then signed big-endian
integer bytes. Hash the serialized seed followed by those bytes to obtain secret
`0xA5A36C3C700EAEBBCF5F43E056D2D172ACC96DADE2B0783DBCA0BFFFAC04598C`.

The full root is
`0xBB86260EBB44C65423528AFB6549672A02F353241FE99ABB790484778D440BE7`,
with sum `2147450880`. The demonstration signature occupies **11,530 bytes**.
The [saved vectors](../../validation/horst-work-in-progress/normal/horst-known-vectors.json)
contain the exact Java and C++ output bytes, not just these summary values.

A useful exercise: explain why replacing the serialized index with a four-byte
native integer would change every secret, even if its numerical value were the same.
