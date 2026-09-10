# jnifunctions — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/jni/jnifunctions.java) · `src/org/minima/utils/jni/jnifunctions.cpp` (absent)

## Purpose

jnifunctions declares the Java-to-native boundary for demonstration calls and header hashing. Java native declarations specify an API but contain no implementation of the declared function bodies.

## Follow the algorithm

1. loadNativeLib calls System.loadLibrary("native"). Resolving that library depends on the JVM native-library path and platform naming rules; the class does not load it in a static initializer.
2. The empty constructor does no setup. sayHello, sumIntegers, sayHelloToMe, and hashHeader are native declarations whose actual behavior requires the linked implementation.
3. hashHeaderWithDiff accepts serialized test nonce bytes, an attempt limit, target difficulty bytes and header bytes, returning bytes expected by callers to decode as MiniNumber. The comment identifies the test nonce as a temporary demonstration input.
4. The inspected jnitest/jniminingtest callers call sayHello, pass header bytes with nonce data at offset four, decode the returned nonce, recalculate TxPoW ID in Java, and report whether it is below the target. They print/report timings and outcomes rather than asserting equality of every native attempt.

## Translation work still required

The mapped C++ counterpart is absent, and this Java file does not reveal the native library bodies. Existing native hash/TxHeader/MiniNumber primitives are candidate dependencies, but implementing behavior merely from these names would invent an unspecified boundary. JNI ABI compatibility is also different from directly exposing a C++ class.

## Understanding the tests

The two linked Java commands show how a result is consumed. jniminingtest performs a Java nonce-search loop as a comparison and returns separate Java/native fields, but has no equality assertion. Proposed tests require the actual library implementation or a documented contract, fixed headers/targets/limits, exact returned MiniNumber bytes, and independent hash validation; a mock library would test dispatch only.

- [JNI result decoding caller](../../../../../../../../upstream-study/src/org/minima/system/commands/base/jnitest.java)
- [Java/native timing comparison caller](../../../../../../../../upstream-study/src/org/minima/system/commands/base/jniminingtest.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No JNI library execution, native counterpart, or ABI parity is claimed. Null handling, greeting wording, integer overflow, attempt-limit semantics, mutation of input arrays, and failure behavior cannot be determined from declarations alone.

## Check your understanding

Why does returning some nonce that decodes successfully not prove that the native search obeyed its attempt bound?

Pinned Java SHA-256: `2a20a3d98019533799bda8f58db62f934fe5c80e3bc99ab20f405bfee3d5425c`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
