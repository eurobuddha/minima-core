# MASTstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/MASTstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java) | [src/org/minima/kissvm/statements/commands/MASTstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/MASTstatement.cpp) |

## What this file does

Execute the witness script selected by its address/proof lookup.

## Follow the logic

1. Evaluate the supplied expression and cast it to HexValue, then locate the corresponding script in the Contract witness. Reject a missing script before parsing.
2. Tokenize the selected script and parse it at the current Contract stack depth.
3. Run the resulting StatementBlock in the same Contract, so variable and return-state changes remain visible to the caller.
4. Rethrow ExecutionException unchanged; wrap other caught parsing/execution exceptions as ExecutionException. Failures before the try block are not wrapped by this catch.

## How the translation is approached

C++ reuses ScriptTokenizer and StatementParser and uses dynamic_cast for the source type cast. Exact generic class-cast/runtime diagnostic parity remains outside the current normalized suite.

## What the tests tell us

KissVMOracle installs actual ScriptProof objects for RETURN TRUE and LET x=42, then emits their real addresses for MAST. The driver also tests a missing address and a numeric parameter. The harness compares shared state, success, instruction/stack counts and applicable traces.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java), [StatementParser](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java), [ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java), [ScriptTokenizer](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [ScriptProof](../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java).

Method navigation (declarations with bodies found in this file): [MASTstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java#L24), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java#L29), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java#L69).

## Check your understanding

Why would evaluating the nested script in a new independent Contract change the result?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `5e41c5bdb9d527a176f8ddf7f4a83f4af762d411d1093ef9d6d19e98d82d2f49`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
