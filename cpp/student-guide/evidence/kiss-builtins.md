# How the 53 KISS built-in lessons were checked

The lessons explain the pinned Java functions and the current native bodies. The test evidence comes from reading the complete [driver](../../tools/kissvm_differential.py), [Java oracle](../../support/KissVMOracle.java) and [native harness](../../support/kissvm.cpp), not from a function merely appearing in a compiled source list.

## One input, two executions

1. The driver compiles the pinned Java KISS and object sources. It supplies the same expression or script, instruction limit and fixture mode to both programs.
2. Each harness constructs a real Contract. The ordinary fixture installs variables such as `a=7`, `b=3`, current state ports `0=17` and `1=[hello]`, and different previous-state values.
3. The richer fixture adds three inputs and outputs. One uses Minima units, one has Token metadata with a scale of two, and one deliberately lacks Token metadata. Witness proof coin IDs differ from the transaction input IDs. This catches a plausible but incorrect GETINID implementation.
4. Java itself generates valid TreeKey signatures and MMR proofs. The driver also supplies changed messages, wrong keys and changed leaf sums. A valid fixture anchors the mutation tests: otherwise both programs could reject every input and appear to agree.
5. The real parser/evaluator runs the expression or script. The programs emit structured observations, and the driver compares the resulting JSON values.

## What is compared

The output includes the expression display, value type and text, serialized decimal bytes for numbers, parse and success flags, error state, instruction count, stack depth, monotonic flag, variables, and trace output. Thus a function returning the right Boolean can still fail because it evaluated arguments in the wrong order or charged a different number of instructions.

The driver uses boundary products: wrong argument counts, wrong value types, negative and extreme indices, size limits, and nested scripts. The individual lessons identify which cases actually exercise their function.

## What is deliberately lost

Generic Java/native runtime errors are normalized to `ERROR`. When such an error occurs, the harness omits its trace. Exact generic exception classes, wording and those omitted traces are therefore outside this suite's evidence. KISS exception text and ordinary execution traces remain compared. A finite collection of programs also cannot establish every public overload, concurrent operation, allocator failure or locale-dependent behavior.

The existing normal [report](../../build/kissvm-differential-results.json), read for this backfill, records **4,471 cases, 4,471 passed, zero failures**. This is a report snapshot, not a new test run or an assertion that every function has independently passed 4,471 dedicated cases.

## Teaching exercise

Change a proposed BITSET implementation so that clearing the highest set bit retains the original byte-array length. Predict which observation changes. Then change it to evaluate its Boolean argument before checking the index; construct a case where the index and Boolean expression both fail. The first mutation concerns the result; the second concerns the order of failures.
