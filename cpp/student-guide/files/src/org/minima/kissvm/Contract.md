# Contract — logic, translation and tests

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/Contract.java](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java) | [src/org/minima/kissvm/Contract.cpp](../../../../../../src/org/minima/kissvm/Contract.cpp) |

## Teacher's worked explanation

Contract is the execution context for one KISS script. It keeps the transaction,
witness, previous state, supplied signature keys, variables, global values,
instruction/stack counters, parsed statement block, result flags and trace text.
It does not itself mine a block or decide every transaction-validity rule.

### Separate parsing from execution

Construction stores the supplied context and records introductory trace entries.
It tokenizes the script, parses tokens into a StatementBlock and records whether
parsing succeeded. A parse Exception sets an exception flag and message. Creating
a Contract does not imply that the script executed or returned true.

`run()` first refuses to execute a failed parse. Otherwise it resets stack depth,
runs the statement block, and records instruction count and final result. An
execution Exception records an error and forces success false, with success-set
true. These are distinct states: “has not returned,” “returned false,” and “failed
with an exception” are not interchangeable even when `isSuccess()` is false in
all three cases.

`setRETURNValue` accepts only the first explicit return value. A later ordinary
attempt to set another return value does not overwrite it. An execution failure
can still force failure through the catch path. The mapped test exercises a
successful return followed by an attempt to overwrite it.

### Build the script environment

`setGlobals()` selects a transaction input and sets block number, block time,
creation height, coin age, input index, coin ID, amount, address, token ID, script,
and total input/output counts. For a non-Minima token, it scales the amount using
the token definition. The current fix preserves Java's null-pointer category if
that definition is missing; silently using the unscaled amount would change
script meaning.

`getGlobal()` marks the contract non-monotonic when it reads changing block/time
values (`@BLOCK`, `@BLOCKMILLI`, or `@COINAGE`). The checker uses monotonic status
when deciding whether it may reuse a prior script result. A script that depends
on block age may legitimately need to be checked again later.

Ordinary variables and globals use separate tables. State reads obtain a current
transaction port or search the previous-state list, then parse the stored text
into a VM Value. Missing current and previous ports have distinct error messages.
Typed parameter helpers evaluate an expression, check its Value type, and return
the corresponding typed value or raise an ExecutionException.

Signature membership here means comparing a requested key with the supplied list.
The String-signatures constructor overload in this pinned source ignores that
string and delegates with an empty list. The native overload retains this
behavior; it must not invent a string parser because the argument name suggests
one.

### Counters, representation and reuse

Instructions and stack depth have explicit limits. Reaching the maximum is
allowed; exceeding it throws. The native counter arithmetic uses an explicit
32-bit wrapping helper before comparing the limit, avoiding undefined C++ signed
overflow. Variables use the existing JavaHashtable adapter so enumeration in
JSON and trace text follows the source.

The native implementation also reuses ScriptTokenizer, StatementParser, Values,
Expressions and StatementBlock. `cleanScript()` is a separate formatting helper:
it collapses whitespace, tokenizes in cleaning mode and reconstructs spacing and
hexadecimal casing. It returns the original script when the source parse-error
path is reached. Cleaning does not execute the script.

The source has a demonstration `main()` which cleans and runs a sample negative
bit-shift script with tracing. The mapped native class does not yet reproduce
that demonstration entry point as a separate mapped runtime API.

### How the tests support the translation

`ContractTests` checks initial parse/result/exception flags, exact invalid-token
text, a shared global table, variable storage, first-return behavior and rejection
at instruction 1,025 with a default maximum of 1,024. These are selected adapted
assertions; the native test does not reproduce every fixture in the Java class.

`kissvm_differential.py` executes expressions and scripts through the actual
Java/C++ contexts. It varies types, missing variables, parse errors, branching,
loops, nested EXEC, limits, current/previous state, signatures, token amounts and
all ported built-ins. Its latest normal run passes 4,471 comparisons. Captured
observations include parse/success/error flags, counters, monotonic status,
variables and execution trace. Generic runtime diagnostics are deliberately
normalized to `ERROR`; KISS exception messages are compared more precisely.

`checker_logs_differential.py` supplies serialized consensus fixtures to actual
checker calls. It compares selected complete failure traces and cache state, and
now checks missing token data before execution. That expanded suite passes 1,343
normal-build comparisons across several classes, not Contract alone.

### What remains unfinished

Trace-enabled native console output still uses legacy logging paths, and broad
native catches do not fully preserve Java Exception-versus-Error behavior.
Complete aliasing of previous-state lists, nullable contexts and all mutable
global APIs remain unverified. The present change fixes the missing-token
category; it does not claim to resolve those wider Contract gaps.

**Exercise:** compare `RETURN @CREATED EQ 100` with a script that reads `@COINAGE`.
Which can change monotonic status, and why does that matter to a checker reusing
a prior result at a later block?


## Open the test evidence

- [ContractTests.cpp](../../../../../../test/org/minima/tests/kissvm/ContractTests.cpp)
- [kissvm_differential.py](../../../../../../tools/kissvm_differential.py)
- [KissVMOracle.java](../../../../../../support/KissVMOracle.java)
- [kissvm.cpp](../../../../../../support/kissvm.cpp)
- [checker_logs_differential.py](../../../../../../tools/checker_logs_differential.py)

## Cleaning logger correction

The [wallet-command lab](../../../../../new-tests/wallet-commands.md) found that malformed quoted input returned the same original script in both languages but lost the source log event. cleanScript now calls the actual MinimaLogger for both token tracing and its parse-error message. Eight direct cleaning combinations compare log-enabled and disabled paths in addition to newscript cases. These pass within the 675-program corpus in both builds. Other contract trace and exception boundaries remain separate review items.

## Dependencies and reading map

Direct Minima imports: [ExecutionException](../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaParseException](../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java), [MinimaFunction](../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [StatementBlock](../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java), [StatementParser](../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementParser.java), [ScriptToken](../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java), [ScriptTokenizer](../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java), [BooleanValue](../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Coin](../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Token](../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [Witness](../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Contract](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L119), [Contract](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L123), [Contract](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L127), [Contract](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L131), [setGlobals](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L215), [setGlobalVariable](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L251), [getGlobal](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L256), [setAllGlobalVariables](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L276), [isParseOK](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L280), [isException](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L284), [getException](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L288), [isTrace](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L292), [traceLog](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L296), [getCompleteTraceLog](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L305), [incrementInstructions](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L309), [incrementInstructions](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L313), [getNumberOfInstructions](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L320), [setMaxInstructions](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L324), [resetStackDepth](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L328), [getStackDepth](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L332), [incrementStackDepth](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L336), [decrementStackDepth](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L343), [run](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L347), [setRETURNValue](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L382), [isSuccess](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L389), [isSuccessSet](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L393), [isMonotonic](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L397), [getMiniScript](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L401), [getTransaction](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L405), [getWitness](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L409), [getAllVariables](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L413), [removeVariable](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L442), [existsVariable](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L446), [getVariable](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L450), [setVariable](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L455), [getNumberParam](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L464), [getHexParam](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L472), [getStringParam](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L480), [getBoolParam](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L488), [getState](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L497), [getPrevState](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L509), [traceVariables](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L527), [checkSignature](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L570), [cleanScript](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L588), [cleanScript](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L592), [main](../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java#L714).


Pinned Java SHA-256: `45c4b194ae723e4d5ba56e011c0c203ba7ea0f5d166153c9f66e9b53d702b969`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
