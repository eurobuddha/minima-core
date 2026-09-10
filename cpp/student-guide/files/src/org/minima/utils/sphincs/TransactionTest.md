# TransactionTest — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/TransactionTest.java) · `src/org/minima/utils/sphincs/TransactionTest.cpp` (absent)

## Purpose

TransactionTest is an executable Java experiment demonstrating signature state inside a KISS VM contract. It is not a JUnit test and its printed success must not be mistaken for full signature verification coverage.

## Follow the algorithm

1. Create SPHINCS from a fixed seed and use a hard-coded public key inside the script. Build a sample token, seven input coins with dummy proofs, and two token outputs.
2. Concatenate input IDs and output address/amount/token/keep-state fields using AMOUNT and TOKENID separators. These differ from the SPHINCSUtils helper’s SPHINCS separators. Convert the assembled string to MiniData bytes and sign it.
3. Write the TreeKey signature/root into states 100/101 and four components per HORST tree into ports 5*i through 5*i+3 for all 16 trees.
4. Construct Contract with this transaction/witness, set @TOTIN and @TOTOUT, and run it. The embedded script’s proof loop is counter LT 1, so it validates only the first tree even though all 16 trees are stored.
5. Print contract success, instruction count, and variables. The final if(!success) block has only a commented trace print, so failure does not throw an assertion or set a test failure exit status.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. A faithful mapped demonstration would preserve its hard-coded script and reduced loop; a separate complete verification test is needed rather than silently treating this experiment as the production helper.

## Understanding the tests

The test logic currently consists of setup, contract execution, and diagnostic prints. Proposed native/Java assertions should compare message bytes, all states, instruction results and variables, then mutate tree zero versus tree one: the source demonstration’s single-iteration loop explains why those mutations have different coverage. A separate full-script test must exercise all 16 trees.

- [Separate full helper script](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSUtils.java)
- [Signature construction](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

Why can this demonstration report a successful contract after a proof in tree 15 has been corrupted? Which literal loop bound explains that result?

Pinned Java SHA-256: `6b237bbbc9ed5046bb87a039e7b19c8241c724c0746adbfd15a9962658a5185c`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
