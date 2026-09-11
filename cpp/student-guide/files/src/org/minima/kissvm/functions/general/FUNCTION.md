# FUNCTION — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/general/FUNCTION.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java) | [src/org/minima/kissvm/functions/general/FUNCTION.cpp](../../../../../../../../src/org/minima/kissvm/functions/general/FUNCTION.cpp) |

## What this file does

Execute a supplied script after textual argument substitution.

## Follow the logic

1. Require at least the script string. Count dollar-delimited nonempty tokens and enforce the source replacement bound.
2. Evaluate extra arguments sequentially and replace $1, $2, … in order. String arguments receive square brackets; repeat the replacement bound check each time.
3. Remove the old returnvalue variable, tokenize/parse the expanded script at the current stack depth and run it in the same Contract.
4. Rethrow execution errors, wrap other caught failures as execution errors, and return returnvalue if the executed script defined it; otherwise return TRUE.

## How the translation is approached

The native implementation reuses the tokenizer, statement parser and REPLACE helper. A small loop reproduces StringTokenizer’s nonempty-token count; this is not a simple count of dollar characters.

## What the tests tell us

The driver tests argument replacement, string-valued parameters, missing returnvalue, shared variable mutation, scripts containing RETURN and malformed nested scripts. Contract state, instruction counts and traces are compared.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [REPLACE](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACE.java), [StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java), [StatementParser](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java), [ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java), [ScriptTokenizer](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [FUNCTION](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java#L22), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java#L27), [isRequiredMinimumParameterNumber](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java#L94), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java#L99), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/FUNCTION.java#L104).

## Check your understanding

Why can replacement of $1 affect a later $2 replacement? Trace a supplied string containing a dollar placeholder.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `bf6595b06c4ab07b5261bb2fe99c207d6b03e749714d0f43501781f463f01ca1`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
