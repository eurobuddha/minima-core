# ScriptValueTests — String values used to carry script text

Java source: [test/org/minima/tests/kissvm/values/ScriptValueTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ScriptValueTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Create values from bracketed text and direct StringValue constructors. Check preservation of text and the order of concatenation. The historical test filename says ScriptValue, while the imported implementation is StringValue.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ScriptValueTests.java#L12) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 10 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 19](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ScriptValueTests.java#L19):

```java
assertEquals("should be equal ", "HELLO WORLD", Value.getValue("[HELLO WORLD]").toString());
```

[Source line 20](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/ScriptValueTests.java#L20):

```java
assertEquals("should be equal ", "[]", sv2.toString());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.values.StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)

## C++ translation and validation

The mapped counterpart now exists at
[test/org/minima/tests/kissvm/values/ScriptValueTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/values/ScriptValueTests.cpp).
It preserves all ten active Java assertions in the same order. An assertion
failure throws and makes the executable fail. Production StringValue and Value
implementations are reused; this change adds the missing mapped test.

The distinction between the first two checks is deliberate. `Value.getValue`
parses a bracketed script literal and removes its outer brackets, producing
`HELLO WORLD`. Calling the StringValue constructor directly with `[]` stores
those two characters literally. A port that stripped brackets in both places
would fail the second assertion. The concatenation checks run in both orders,
so accidentally reversing operands cannot pass. Finally, each constructor must
report the same script-value type, regardless of its text.

[ScriptValueOriginalTests.java](../../../../../../../../support/ScriptValueOriginalTests.java)
invokes the unchanged pinned Java test method with the repository's bundled
JUnit assertions. The native executable then runs its corresponding assertions.
[script_value_original_tests.py](../../../../../../../../tools/script_value_original_tests.py)
compiles the original Java production sources and test, checks both process exit
statuses, and saves a report. This follows the repository's existing
ParamConfigurerOriginalTests approach and needs no new downloaded dependency.

After a successful build, run:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^(ScriptValueTests|script_value_original_tests)$'
```

**Latest verification:** the original Java method and native mapped assertions
pass in normal and ASAN/UBSAN builds (comparison suites: 2.02s and 1.91s). The
reports record one original method and ten assertions per runtime. The method contains
ten assertions, not ten independent fixtures. Passing it does not establish all
StringValue size limits, Unicode boundaries or error paths.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `34a36b148019510cbfada7909df82f968134f2fc211263d446afdde37ecf4da8`.
