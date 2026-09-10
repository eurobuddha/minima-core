# ParamConfigurerTest — Configuration defaults, validation, and precedence

Java source: [test/org/minima/system/params/ParamConfigurerTest.java](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Compare an empty configuration with command-line options, invalid options, environment variables, and a configuration file. The file sets port 999 while the command line sets 8888; the expected result demonstrates command-line precedence. Unrelated environment keys must not change configuration.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`usingNothing`](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L24) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |
| [`usingProgramArgs`](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L38) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 14 |
| [`invalidProgramArgs`](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L80) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`usingEnvVars`](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L96) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 5 |
| [`usingConfFile`](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L121) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 33](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L33):

```java
assertEquals(9001, GeneralParams.MINIMA_PORT);
```

[Source line 34](../../../../../../../../upstream-study/test/org/minima/system/params/ParamConfigurerTest.java#L34):

```java
assertFalse(configurer.isDaemon());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

## C++ translation and validation

The mapped native test is [test/org/minima/system/params/ParamConfigurerTest.cpp](../../../../../../../test/org/minima/system/params/ParamConfigurerTest.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native adaptation resets global parameters before each case and supplies a controlled environment map and configuration file path. Invalid arguments must raise the specific UnknownArgumentException.

[CMake registration](../../../../../../../CMakeLists.txt) registers `ParamConfigurerTest` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^ParamConfigurerTest$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `444bbc31d93e416b2ae16b8aff05cb5be9ec786fff3a70328f5249c945e3f055`.
