# DUMMYFUNCTION — A controllable function used to test function infrastructure

Java source: [test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Give the function the name DUMMYFUNCTION, return TRUE from runFunction, create a fresh instance from getNewFunction, and declare at least one required parameter. The simple result isolates argument-list behavior from complex function logic.

This is a test fixture helper rather than a standalone test suite.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`DUMMYFUNCTION`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java#L11) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`runFunction`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java#L16) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`getNewFunction`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java#L21) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`isRequiredMinimumParameterNumber`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java#L26) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |
| [`requiredParams`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.java#L31) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

There are no active assertions in this helper. Its outputs become the inputs to other tests; correctness must be assessed through callers or a dedicated helper test.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java)
- [org.minima.kissvm.exceptions.ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java)
- [org.minima.kissvm.functions.MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java)
- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)
- [org.minima.kissvm.values.Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java)

## C++ translation and validation

The [mapped C++ helper](../../../../../../../../test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.cpp) and [header](../../../../../../../../test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.hpp) subclass the existing native MinimaFunction. Construction sets the source name. `runFunction` returns a newly allocated TRUE value; it does not evaluate parameters or check the advertised minimum. `getNewFunction` returns a fresh helper with an empty parameter list. `requiredParams` is 1 and `isRequiredMinimumParameterNumber` is true; an external caller must invoke parameter-count validation for that metadata to take effect.

The helper is linked only into MinimaFunctionTests. It is deliberately absent from the production registry. The [consuming test lesson](MinimaFunctionTests.md) explains why lookup by DUMMYFUNCTION must fail even though direct construction succeeds. The original test observes its name, empty initial list, and parameter storage/retrieval. It does not invoke the helper's runFunction or minimum-count validation; those methods' presence is not additional executed coverage.

Both full builds passed. The consuming MinimaFunctionTests and unchanged Java methods pass in normal and sanitizer builds. All nine selected suites pass in 10.55 seconds and 10.83 seconds respectively. This file has no standalone CTest case or original assertions of its own.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `331fff9691201b9d0cbbcf98e8e72ce0f1e52392b5b721fe6c0ab92b72a8898b`.
