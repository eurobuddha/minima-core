# TransactionTest — teaching guide

**Teaching status:** Authored explanation of the pinned demonstration, native translation, and validation in progress.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/TransactionTest.java) · [Native implementation](../../../../../../../src/org/minima/utils/sphincs/TransactionTest.cpp)

## Purpose

TransactionTest is an executable Java experiment demonstrating signature state inside a KISS VM contract. It is not a JUnit test and its printed success must not be mistaken for full signature verification coverage.

## Follow the algorithm

1. Create SPHINCS from a fixed seed and use a hard-coded public key inside the script. Build a sample token, seven input coins with dummy proofs, and two token outputs.
2. Concatenate input IDs and output address/amount/token/keep-state fields using AMOUNT and TOKENID separators. These differ from the SPHINCSUtils helper’s SPHINCS separators. Convert the assembled string to MiniData bytes and sign it.
3. Write the TreeKey signature/root into states 100/101 and four components per HORST tree into ports 5*i through 5*i+3 for all 16 trees.
4. Construct Contract with this transaction/witness, set @TOTIN and @TOTOUT, and run it. The embedded script’s proof loop is counter LT 1, so it validates only the first tree even though all 16 trees are stored.
5. Print contract success, instruction count, and variables. The final if(!success) block has only a commented trace print, so failure does not throw an assertion or set a test failure exit status.

## Native translation and reuse

The C++ counterpart reuses the actual SPHINCS signer, Coin/Token/Transaction,
Witness/CoinProof, and KISS VM. The formatter and 66-state installation were
extracted from the tested SPHINCSUtils implementation into a shared helper.
Different separator arguments preserve the two source formats. The demo calls
only state installation; SPHINCSUtils still performs its additional output-ID and
transaction-ID updates. Contract construction retains its existing logging and
serialization side effects.

The script literal is copied exactly, including its fixed 0x30DE...6822E public
key and one-iteration loop. The pinned signer derives 0xDBCD...050BF from this seed,
so the script's key check rejects before its proof loop. The port does not change
the literal or skip the expensive signing work to obtain a different result.

## Understanding the tests

The [demonstration lab](../../../../../../../student-guide/new-tests/sphincs-transaction-demo.md)
was written before execution. Both builds pass **169 public-method comparisons
and 338 independent assertions**. Cases cover existing coin fixtures, token
scaling, negative/zero/fractional amounts, fractional scale truncation, keep-state,
empty and oversized hex fields, a null coin and null/empty/Unicode logging.
Python independently assembles each expected formatter string.

Both complete builds pass. The shared extraction also passes the existing
**268-case/1,677-assertion transaction replay** in both builds, including partial
state writes and real full-signature verification in both VMs. The actual demo
mains are running with full-size signing. Their final exact stdout, message,
variables and instruction counts are not yet claimed; see
[current evidence](../../../../../../../validation/transaction-test-work-in-progress/progress.json).

## Limits and a useful next test

This demonstration is not an assertion-based source test and cannot establish
full node or signature security. The source's fixed key currently rejects; its
one-iteration loop would still cover only the first tree if that key matched.
The separate SPHINCSUtils tests execute all sixteen proof iterations successfully.
Arbitrary Coin subclasses and unrepresented null Coin internals remain unverified.

## Check your understanding

Why must the port preserve a demonstration's rejection? If its script key matched,
which literal loop bound would leave tree 15 unchecked?

Pinned Java SHA-256: `6b237bbbc9ed5046bb87a039e7b19c8241c724c0746adbfd15a9962658a5185c`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
