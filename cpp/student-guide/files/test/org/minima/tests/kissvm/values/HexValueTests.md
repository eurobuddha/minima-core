# HexValueTests — Hexadecimal values and their comparison rules

Java source: [test/org/minima/tests/kissvm/values/HexValueTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Construct hex values from the source input forms, check malformed representations, and compare values. Hex text is a representation of bytes; numeric equality and object identity are different possible comparisons.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java#L18) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 11 |
| [`testComaprisons`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java#L51) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 4 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 91](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java#L91):

```java
assertTrue("should be true ", hv1.isEqual(hv2));
```

[Source line 92](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java#L92):

```java
assertTrue("should be true ", hv2.isEqual(hv1));
```

[Source line 23](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/HexValueTests.java#L23):

```java
assertThrows(NumberFormatException.class, () -> { new HexValue(new MiniData("QWER")); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations. The original-method runner below executes them unchanged; the current verification result is recorded separately.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.values.HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)

## C++ translation and validation

The [mapped HexValueTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/values/HexValueTests.cpp)
now preserves all active cases from both Java methods. Its constructor test checks
three NumberFormatException paths and eight value types. Its two comparison loops
run six construction routes each, with two assertions per iteration. Thus the
fifteen assertion call sites execute **35 assertions**.

The first inequality route uses the text `10` as hexadecimal, which is decimal
16; the numeric route uses decimal 10. Both are unequal to 5, so replacing every
route with the same fixture would hide a distinction in the source inputs.
The inequality loop also checks the same direction twice. The native test retains
that repetition instead of silently changing the second assertion. Ordering
comparisons in comments stay inactive.

Source inspection found a real production gap: numeric HexValue construction
raised a native generic argument error for negative or fractional values, while
Java requires NumberFormatException. The native constructor now reuses
JavaNumberFormatException with the source messages. The test catches that precise
native category. Malformed MiniData text already used this typed exception.

[HexValueOriginalTests.java](../../../../../../../../support/HexValueOriginalTests.java)
invokes the two unchanged Java methods with the bundled JUnit assertions. The
existing [original-value driver](../../../../../../../../tools/script_value_original_tests.py)
now accepts a selected value-test class, preserving its ScriptValue default and
using separate compiler/output directories for each class. It checks the original
Java and native processes and saves `hex-value-original-results.json`.

After a successful build, run:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^(HexValueTests|hex_value_original_tests)$'
```

**Current verification:** full normal and ASAN/UBSAN builds pass, and all eight
affected suites pass in both builds (23.81s and 85.21s). The original Java/native
comparison passes two methods and 35 assertions per runtime (1.83s normal; 1.90s
sanitized native). Regression reports have zero failures across 1,451 value,
4,471 VM and 9,244 object comparisons. These are suite totals, not HexValue-only
case counts. The mapped assertions do not establish all size, aliasing or Unicode
behavior.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `c976c22d6378020a480fdbd0ce90bd0b4fcda290edef6db8c07d0a1207951e73`.
