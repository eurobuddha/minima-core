# MASTstatementTests — Looking up and executing a witnessed script by hash

Java source: [test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Construct MAST expressions from script hashes and build witness/script fixtures. Execute in a contract context and inspect decision and instruction state, including rejection paths. A hash commits to script bytes; it is not the script text itself.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java#L22) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 2 |
| [`testExecution`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java#L34) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 25 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 27](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java#L27):

```java
assertEquals("MAST ", ms1.toString());
```

[Source line 30](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java#L30):

```java
assertEquals("MAST 0xBC05B9280323A07AE6B2C6FA183F20991D38D01BC91F02C439CB5A33113E106C", ms2.toString());
```

[Source line 103](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.java#L103):

```java
assertThrows(ExecutionException.class, () -> { ms.execute(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.statements.commands.MASTstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/MASTstatement.java)
- [org.minima.kissvm.values.HexValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java)
- [org.minima.objects.ScriptProof](../../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/tests/kissvm/statements/commands/MASTstatementTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `e991b54a39f7cbe7f17253afb51fb891f771430cd7576e7f0502706e7f627a71`.
