# SUBSETTests — Extracting a byte interval

Java source: [test/org/minima/tests/kissvm/functions/base/SUBSETTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Select the interval starting at 0 and ending at 2 from 0x01234567 and require 0x0123. Also inspect full-span and invalid-range cases. Endpoint order and bounds must be validated before indexing native memory.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L30) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L47) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 12 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L137) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 19 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 60](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L60):

```java
assertEquals("0x0123", ((HexValue) res).toString());
```

[Source line 73](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L73):

```java
assertEquals("0x01234567", ((HexValue) res).toString());
```

[Source line 84](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/base/SUBSETTests.java#L84):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; completed execution results are recorded below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.hex.SUBSET](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)
- [org.minima.utils.MinimaLogger](../../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/base/SUBSETTests.cpp)
reuses the actual native function, registry, Contract and ConstantExpression, with
the existing CONCAT/LEN assertion adapters. The fixture preserves both successful hex slices, all six expected exceptions inside the valid method, and all nineteen exceptions inside the invalid method. The repeated out-of-range case remains repeated, and string slicing remains rejected.

Both complete builds and all four registered GET/SUBSET tests pass in each build.
The Java wrapper calls three unchanged original methods; the native counterpart
executes the same 33 assertions. Native stderr is empty and UBSAN halts on errors.
Source call-site counts above include fail() handlers that a passing run never
executes. See the [test lab](../../../../../../../../../student-guide/new-tests/get-and-subset-original-tests.md)
for the assertion breakdown and the limits of this evidence. No production function changed.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `fed68bfd05f82f3f826578bc79388087e2bd087d08267d7bba26531d16eeda95`.
