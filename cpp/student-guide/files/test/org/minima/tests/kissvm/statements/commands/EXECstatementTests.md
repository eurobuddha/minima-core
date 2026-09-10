# EXECstatementTests — Executing a script held inside a string

Java source: [test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../../source-map.json).

## Purpose and logic

Execute an empty script, RETURN TRUE, RETURN FALSE, and invalid text. Check both the decision and the instruction count. Empty execution consumes one instruction; the two RETURN examples consume three.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java#L21) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 3 |
| [`testExecution`](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java#L37) | Execute prepared objects and inspect outcomes, state changes, instruction counts, or expected exceptions present in this body. | 16 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 27](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java#L27):

```java
assertEquals("EXEC ", es1.toString());
```

[Source line 30](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java#L30):

```java
assertEquals("EXEC RETURN TRUE", es2.toString());
```

[Source line 92](../../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.java#L92):

```java
assertThrows(ExecutionException.class, () -> { es.execute(ctr); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.expressions.ConstantExpression](../../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java)
- [org.minima.kissvm.statements.commands.EXECstatement](../../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/EXECstatement.java)
- [org.minima.kissvm.values.StringValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.objects.Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java)
- [org.minima.objects.Witness](../../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.cpp](../../../../../../../../../test/org/minima/tests/kissvm/statements/commands/EXECstatementTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native adaptation retains these four cases and verifies invalid text raises ExecutionException without setting a success decision.

[CMake registration](../../../../../../../../../CMakeLists.txt) registers `EXECstatementTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^EXECstatementTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `bd2a75484591bda1817af4897075fab7f1237140dd625a4d4fffa92da20b0d48`.
