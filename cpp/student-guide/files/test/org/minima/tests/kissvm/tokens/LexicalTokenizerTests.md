# LexicalTokenizerTests — Moving a cursor through a token sequence

Java source: [test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

An empty cursor cannot advance or move backwards. For 26 letter tokens, check the position before each read and the exhausted flag after the last read. Move back six times and check the positions 25 through 20.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java#L18) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 5 |
| [`testGetters`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java#L33) | Read/set fixture state and compare values or enforce the source type checks. | 15 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 23](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java#L23):

```java
assertEquals(0, lt.getCurrentPosition());
```

[Source line 24](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java#L24):

```java
assertEquals(true, lt.checkAllTokensUsed());
```

[Source line 20](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.java#L20):

```java
assertThrows(MinimaParseException.class, () -> { lt.getNextToken(); });
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.exceptions.MinimaParseException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java)
- [org.minima.kissvm.tokens.LexicalTokenizer](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/LexicalTokenizer.java)
- [org.minima.kissvm.tokens.ScriptToken](../../../../../../../../../upstream-study/src/org/minima/kissvm/tokens/ScriptToken.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/tokens/LexicalTokenizerTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test retains these cases and adds an explicit observation of the constructor's stack-depth behavior.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `LexicalTokenizerTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^LexicalTokenizerTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `546b3b64472d08d7187640084634e72dab378ef949de27fd95965bd55a68f1a8`.
