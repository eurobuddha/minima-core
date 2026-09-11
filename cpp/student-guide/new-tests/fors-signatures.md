# FORS: a forest of authenticated secret values

This lab extends the [HORST lab](horst-signatures.md). The pinned FORS constructor
derives sixteen seeds with Crypto.hashAllObjects(seed, MiniNumber(i)), builds a
complete 65,536-leaf HORST tree for each, then places those sixteen root hashes
in another indexed MMR. There are **1,048,576 inner leaves**. The outer root sum
is **120**, while each inner root sum is **2,147,450,880**.

The message hash selects one secret per tree. A signature supplies sixteen
secrets, their inner proofs, the sixteen inner roots, and sixteen outer proofs.
The verifier checks an outer proof before looking at its corresponding secret.
That order matters: a bad outer root can return false before a missing inner
value would otherwise throw.

## Checks planned before execution

- Build the actual full-size forest in Java and C++. Independently rebuild all
  sixteen inner roots and the outer root in Python with SHA3-256.
- Sign fixed, empty, longer, and repeated-index messages. Compare exact bytes and
  JSON; feed each writer's signatures to both verifiers. Reconstruct every inner
  and outer proof independently.
- Alter each of sixteen slots, roots, sums, proof bytes, missing/null lists, and
  combinations that reveal outer-before-inner failure order.
- Truncate both a small synthetic signature and a full signature. Compare consumed
  input, partial output, exceptions, list lengths and alias identity. If the inner
  signature read fails, the old outer lists must still be present. If the outer
  root finishes reading but its proof fails, the new lists have different lengths.
- Encode unequal inner and outer counts: Java reads them independently but writes
  the inner count twice. Do not normalize the lists early.
- Exercise the unused root serialization call in toJSON: an oversized root hash
  logs an I/O failure but its null conversion result is ignored, so JSON can still
  succeed. An oversized proof hash instead leads to a subsequent null-pointer error.
- Run the actual source main and compare its output, then regress the shared MMR
  writer through existing object and HORST tests in both native builds.

Results will be added after execution. Full SPHINCS integration and full node
parity remain unverified. These comparisons do not establish cryptographic security.
