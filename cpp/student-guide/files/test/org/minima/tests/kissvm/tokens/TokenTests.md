# TokenTests — Recognizing script tokens and their categories

Java source: [test/org/minima/tests/kissvm/tokens/TokenTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Construct tokens for commands, registered functions, operators, literals, and variables. Compare token type, original text, and type-name text. Exercise invalid constructors, classification helpers, and tokenization of larger strings. These are lexical tokens in a programming language, separate from cryptocurrency tokens.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L21) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 42 |
| [`testInvalidConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L136) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 3 |
| [`testCheckers`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L147) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 74 |
| [`testTokenize`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L236) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 98 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 24](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L24):

```java
assertEquals(ScriptToken.TOKEN_COMMAND, t.getTokenType());
```

[Source line 25](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/TokenTests.java#L25):

```java
assertEquals(s, t.getToken());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this checkpoint records successful execution of unchanged Java tests and native counterparts.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.tokens.ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java)
- [org.minima.kissvm.tokens.ScriptTokenizer](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptTokenizer.java)
- [org.minima.objects.base.MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)

## C++ translation and validation

[Native counterpart](../../../../../../../../test/org/minima/tests/kissvm/tokens/TokenTests.cpp) retains every active original method and 699
assertions per runtime. Unchanged Java and native tests pass in normal and
sanitizer builds. All function/command/operator loops, original explicit lists and random sizes 4/8/16/32/64 remain active. The executable is KissTokenTests to avoid the objects/TokenTests target collision.

[Expression/token lab](../../../../../../../new-tests/original-expression-token-tests.md)
explains types, evaluation, exact spelling and source coverage limits. Twelve
supplemental comparisons/twenty-four independent checks pass across the batch;
the expanded base API and full primitive regressions also pass in both builds.
[Evidence](../../../../../../../../validation/original-expression-token-tests-work-in-progress/progress.json).
Full production API, mutable-registry/alias behavior and node parity remain unverified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `f8968bc7571e2a16e4f8b5ac817366d67ab0bd0775eb43e5de1a67fd1f819163`.
