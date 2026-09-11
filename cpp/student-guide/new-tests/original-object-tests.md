# Reading the original Address, Coin and Token tests

Three more mapped files now execute the pinned source tests: AddressTests,
CoinTests and TokenTests. The inventory is 421 counterparts and 36 absent files.
The full C++ node and complete production API parity remain unfinished.

## Reuse and translation

The tests call the existing native Address, Coin, Token, MiniData, MiniNumber,
MiniString and stream implementations. Shared pointers preserve the source's
constructed-object non-null checks; Coin exposes its existing nullable handle
check. Byte vectors and offsets express the existing stream API. No production
implementation changes were required.

The inspected sibling Token implementation writes a fixed 32-byte coin ID.
The pinned source and this repository's tested implementation use a length prefix.
Replacing that native implementation with the sibling version would change the
wire format, so these tests reuse the implementation already in this repository.

## What each test really checks

| Class | Methods | Source assertions per runtime | Logic |
| --- | ---: | ---: | --- |
| AddressTests | 3 | 10 | Constructor equality, same-object stream read, five Mx conversion round trips. |
| CoinTests | 3 | 5 | Three constructor fields; non-null original coin after instance and static reads. |
| TokenTests | 2 | 5 | Non-null token, amount, name, scale; non-null original token after static read. |

The original Coin and Token static-reader tests discard the returned object.
Their subsequent assertion examines the original object. Preserve this behavior
when translating the test, but do not describe that assertion as a decoded-content
check. Similarly, commented assertions for token script equality and coin ID
resetting provide no executed coverage.

To add meaningful evidence without rewriting the original assertions, the driver
also compares exact serialized bytes and decoded JSON for one original sample
per class. These six comparisons are reported separately. Printed source
observations are compared in execution order; JSON object key order is normalized.

## Running the unchanged Java assertions

The initial JUnitCore runner could not load Hamcrest, which is absent from the
inspected bundled dependencies. The existing MiniStringOriginalTests runner
already solves this by invoking source methods directly. This batch reuses that
approach and checks the exact set of `@Test` methods, default exception/timeout
options and absence of lifecycle methods before invocation. Each method gets a
fresh instance and still executes the real JUnit assertions. No source assertion
is replaced by a stub, removed or weakened.

Both normal and sanitizer builds pass all eight methods and twenty assertions
per runtime, all original printed observations and all six supplemental
wire/decoded comparisons. [Saved evidence](../../validation/original-object-tests-work-in-progress/progress.json)
retains the initial dependency failure and final reports.

Run `python3 cpp/tools/original_object_tests.py --build cpp/build` from the
repository root. Use `cpp/build-sanitized` for the sanitizer build.

These source tests sample only a small part of each production API. Passing them
does not verify null field mutation, arbitrary stream failure, allocation failure,
concurrency or complete node behavior. Earlier differential suites keep their
own wider evidence; this batch does not repeat all of them or claim new results
for them.

Exercise: imagine the static reader returns a different coin with the wrong
amount. Identify the original assertion that would still pass, then explain how
the separate decoded-JSON comparison detects the mismatch.
