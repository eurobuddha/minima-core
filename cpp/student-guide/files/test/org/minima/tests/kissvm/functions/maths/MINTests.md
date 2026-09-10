# MINTests — Selecting the smallest numeric argument

Java source: [test/org/minima/tests/kissvm/functions/maths/MINTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Supply several exact numbers, including negative host integer/long boundaries, and compare the selected minimum. Varying signs and magnitudes catches reversed comparison logic.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L29) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L46) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 21 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L178) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 17 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 59](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L59):

```java
assertEquals("0", ((NumberValue) res).toString());
```

[Source line 74](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L74):

```java
assertEquals(Integer.toString(Integer.MIN_VALUE), ((NumberValue) res).toString());
```

[Source line 186](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/MINTests.java#L186):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These source expectations are retained in the native test. Current execution status is recorded below.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.number.MIN](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped native test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/MINTests.cpp) reuses the existing production MIN, ConstantExpression, NumberValue and Contract. Its setup and typed-exception checks follow the existing FLOORTests convention. The [shared driver](../../../../../../../../../tools/script_value_original_tests.py) freshly compiles and runs the three unchanged Java methods through [MINOriginalTests.java](../../../../../../../../../support/MINOriginalTests.java), then runs the native executable.

**Current verification:** both full native builds and all twelve selected regression suites pass: normal in 36.91 seconds and ASAN/UBSAN in 69.38 seconds. The three unchanged Java methods and the native test pass 120,023 assertions each. The independent reports record 60,000 random trials per runtime.

The count of **120,023 successful-path assertions per runtime** comes from the source, not the number of lines in the file:

| Experiment | Executed assertions |
| --- | ---: |
| Fresh function and registry lookup: name and empty parameter list | 4 |
| Two zero values: number type and text | 2 |
| Six loops × 10,000 trials × two result assertions | 120,000 |
| Too few arguments: zero and one | 2 |
| Ordered type pairs except number/number | 15 |

For each integer width, the test places the smallest representable value first, last after twelve samples, and between two groups of twelve. Thus it tests lists of 2, 13 and 25 values. The selected boundary is the expected answer regardless of the randomly generated background values. Putting it in different positions can expose a function that examines only the first two arguments.

The source constructs a fresh unseeded java.util.Random inside every trial. The unchanged Java test retains that behavior. C++ uses a fresh, reproducibly seeded existing JavaRandom48 for each trial and assembles signed 32/64-bit samples from bounded 16-bit draws. Unsigned shifts followed by bit_cast avoid signed overflow. This preserves the input ranges, loop counts, boundary positions and asserted property; it does **not** reproduce Java's random inputs or test equivalence of random-number generators. Fixed native seeds make a native failure repeatable.

Both versions retain one Contract for the entire valid-parameter method and raise its instruction limit to Integer.MAX_VALUE. ConstantExpression consumes an instruction on evaluation: the successful lists account for 800,002 evaluations. A separate ordinary-limit Contract is used for invalid arguments. The native counter requires exactly 120,023 assertions, preventing an accidentally shortened trial loop from reporting success.

The fifteen type cases are the ordered pairs of Boolean, Hex, Number and String except Number/Number. They require ExecutionException; accepting any exception would conceal a cast or indexing bug. The source's empty domain block stays empty. It does not test decimal fractions, equality of object identities, all evaluation side effects or arbitrary inputs beyond host integer ranges.

After the build, run:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^min_original_tests$'
```

The report is cpp/build/min-original-results.json; the sanitizer build has its own report. The Java stdout is an execution summary, not a capture of all randomly sampled inputs.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `0c96d131ef4dfbb39ff3b5a2c15d629c3c119914e7292cdfbf1f8eab0076f236`.
