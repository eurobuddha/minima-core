# ABSTests — Taking an absolute value

Java source: [test/org/minima/tests/kissvm/functions/maths/ABSTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Exercise zero, positive, and negative exact numbers. The sign changes while magnitude is retained. Wrong-type and wrong-arity cases ensure the mathematical operation is not applied to arbitrary values.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L28) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testValidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L45) | Build function arguments as expression nodes, run the function in a contract context, and compare results; inspect any explicitly expected exceptions here too. | 21 |
| [`testInvalidParams`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L130) | Vary arguments and exercise rejection paths. The asserted exception class is part of the expectation, not an incidental implementation detail. | 5 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 56](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L56):

```java
assertEquals("0", ((NumberValue) res).toString());
```

[Source line 67](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L67):

```java
assertEquals("1", ((NumberValue) res).toString());
```

[Source line 138](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/maths/ABSTests.java#L138):

```java
assertThrows(ExecutionException.class, () -> { Value res = mf.runFunction(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. The native counterpart preserves these source expectations; its typed exception checks do not accept an arbitrary failure.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.functions.number.ABS](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/ABS.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.kissvm.values.NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The [mapped C++ test](../../../../../../../../../test/org/minima/tests/kissvm/functions/maths/ABSTests.cpp) preserves **23 assertions executed on the success path**: four constructor/lookup checks, fourteen valid-result checks and five expected-exception checks. The navigation table counts `fail()` sites in catch blocks as well; those are failure guards, not extra successful observations.

Constructor checks verify a fresh ABS instance and a registry-created instance both have name ABS and zero supplied parameters. The valid cases pass ConstantExpression nodes containing 0, -1, 1, -32768, 32767, Long.MIN_VALUE and Long.MAX_VALUE into the actual function. Each checks the NUMBER type tag and the expected decimal string. In particular, the magnitude of Long.MIN_VALUE is `9223372036854775808`, which cannot fit in a signed 64-bit integer. The existing MiniNumber implementation handles that magnitude; the mapped test does not call a machine-integer absolute-value operation.

The invalid cases separately try zero arguments, two numeric arguments, a Boolean, a HexValue and a StringValue. Each must throw ExecutionException. The source ABS body first checks exact argument count, then asks Contract for a numeric parameter, then delegates to MiniNumber.abs. That ordering explains why different invalid fixtures reach different checks. The test does not assert every resulting message or instruction counter.

The [original Java runner](../../../../../../../../../support/ABSOriginalTests.java) calls all three unchanged source methods. Native code reuses the existing ABS, Contract, ConstantExpression and value classes; no production function was rewritten. The native loop groups the seven repeated source blocks while retaining their input order and shared valid-test Contract. The invalid method uses its own fresh Contract, as in Java. Both full builds passed. All three unchanged Java methods and 23 native assertions pass in normal and sanitizer builds. All nine selected suites pass in 10.55 seconds and 10.83 seconds respectively.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `bc89c3d146c4366a7701e373da0b20abe6feb6684fec20cd84edca5cfe35ab6c`.
