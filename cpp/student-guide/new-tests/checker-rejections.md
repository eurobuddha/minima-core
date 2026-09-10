# Lab: translating transaction rejection behavior

**Status:** the expanded comparison and all 14 affected suites pass in normal
and ASAN/UBSAN builds. This is not a completed Minima node. The pinned Java source is commit
`5564b9d78978efcaf23679190d05253961fc6e69`.

## Learning objectives

After this lab, you should be able to explain why two validators can return the
same Boolean result yet behave differently, how an exception interrupts a sequence
of assignments, and how to compare implementations without writing the expected
answer from the implementation being tested.

## The files and their responsibilities

| File | What students should understand |
| --- | --- |
| [TxPoWChecker.java](../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java) → [TxPoWChecker.cpp](../../src/org/minima/system/brains/TxPoWChecker.cpp) | Coordinates basic transaction checks, proof checks, script execution, signature verification, and block validation. These are separate stages with observable early returns. |
| [Transaction.java](../../../upstream-study/src/org/minima/objects/Transaction.java) → [Transaction.cpp](../../src/org/minima/objects/Transaction.cpp) | Checks amount conservation and allowed amounts, balances each output token against its inputs, then detects repeated input coin IDs. |
| [TxPoW.java](../../../upstream-study/src/org/minima/objects/TxPoW.java) → [TxPoW.hpp](../../src/org/minima/objects/TxPoW.hpp) | Owns the header and optional body. A header-only object cannot supply body fields; its getters must fail at the same point as Java. The mapped translation also has a TxPoW.cpp file. |
| [Signature.java](../../../upstream-study/src/org/minima/objects/keys/Signature.java) → [Signature.hpp](../../src/org/minima/objects/keys/Signature.hpp) | Stores signature proofs. Getting the root key reads the first proof; an empty list is an exceptional case. Signature.cpp implements its serialization. |
| [Contract.java](../../../upstream-study/src/org/minima/kissvm/Contract.java) → [Contract.cpp](../../src/org/minima/kissvm/Contract.cpp) | Sets the VM environment before executing a spending script. A non-Minima token needs its token definition to scale the amount. |
| [CheckerLogsOracle.java](../../support/CheckerLogsOracle.java) | Executes the original Java checker. This is the reference program, or **oracle**, for this lab. |
| [checker_logs.cpp](../../support/checker_logs.cpp) | Executes the native checker on the same serialized objects and records its observable behavior. |
| [checker_logs_differential.py](../../tools/checker_logs_differential.py) | Builds the Java oracle, sends matching requests to both programs, compares results, and writes a failure report. |

## Work through the logic

Imagine a transaction with 10 units coming in and three outputs of 11 units each.
`Transaction.checkValid()` first calculates the input and output totals. Since
33 is greater than 10, it logs the totals and returns false. `TxPoWChecker` then
logs the invalid transaction as JSON and returns false itself. There are **two
log events**, belonging to two different layers.

Now suppose printing the first log throws an exception. Neither the return from
`checkValid()` nor the second log happens. If a translation silently omits the
first log, this case changes even though ordinary invalid transactions still
produce `false`. This is why the test deliberately makes the logger fail.

For token balances, Java first records distinct output token IDs in encounter
order, calculates output totals, stores them in a Hashtable, then enumerates that
table. The native code reuses [JavaHashtable.hpp](../../src/support/JavaHashtable.hpp)
to preserve that order. A C++ sorted set gives a different iteration order and
does not represent Java's collection behavior.

The outer basic checker has a different collection task: it gathers proof coin
IDs from both the ordinary and burn witnesses and constructs a HashSet. Fewer
unique IDs than original IDs means a repeated coin was supplied. When rejecting,
it logs both the unique set and the original list. The test includes collection
sizes around allocation boundaries; these can alter the set's iteration order.
The native implementation reuses
[JavaConcurrentMap.hpp](../../src/support/JavaConcurrentMap.hpp)'s HashMap behavior.

Script checking also mutates cached flags. It marks that a check has started,
tracks whether the script depends on changing block information, and records
success only if every required script succeeds. A token's spending script is
separate from the coin's address script, so their rejection labels differ. An
exception partway through must leave the flags in their source-defined state.

## What the comparison actually checks

The oracle generates 90 source fixtures: 30 transaction variations, each with
three scripts. Nine more fixtures contain repeated coin IDs across both witnesses,
with different collection sizes. One fixture has an invalid input amount.
The C++ program deserializes the Java-produced bytes; it does not independently
invent a similar transaction.

For each request the test records:

1. The Boolean result, or the escaped exception's type and message.
2. The ordered log bodies, including selected complete VM traces.
3. The main and burn transactions' monotonic-check and validity flags.
4. The cached main transaction ID, or the exception from reading it.

It exercises basic, simple, script, MMR, and signature checks separately. It also
passes a null MMR, an empty MMR, an artificially small maximum size, and logger
failures at the first or second observed log. Java runtime exceptions, Error
failures, and out-of-memory failures are separate cases.

The harness removes the logger's time/memory prefix and generated stack-frame
lines. Java-generated null-pointer diagnostic expressions are normalized because
they refer to Java source expressions; the exception category is still compared.
All other captured text and state must match. These exclusions limit the claim:
this lab does not establish identical operating-system diagnostics or stack traces.

## What the first run taught us

The first, smaller corpus had **78 mismatches among 1,185 comparisons**. It found
missing transaction-level logs, native `bad_optional_access` where Java raised a
null-pointer exception, and a native vector-range exception where Java reported
an empty signature list. These were production differences; changing expected
answers to accept them would hide the porting defects.

The subsequent edits restore those source behaviors. The test was also expanded
to cover duplicate-set capacities and nullable MMR forwarding. The rebuilt native program passes the expanded comparison: **1,343 requests
from 100 source fixtures, with zero mismatches**.

## Run the lab

From the repository root, after configuring the documented CMake build:

```sh
cmake --build cpp/build -j 3
ctest --test-dir cpp/build --output-on-failure -R '^checker_logs_differential$'
```

Only run the second command after the build succeeds. Read
`cpp/build/checker-logs-results.json` for the number of comparisons, scope, and
individual Java/C++ disagreements. The fixtures are saved separately in
`cpp/build/checker-logs-fixtures.json` for investigation.

**Current verification:** 1,343 comparisons pass in the normal build (12.83 seconds).
The related timed-checker suite passes 105 comparisons (19.48 seconds), including
empty reconstructed MMR roots and failures while logging that error. The sanitizer
run also passes all 1,343 checker comparisons (31.07 seconds) and 105 timed
comparisons (29.31 seconds). All 14 affected suites pass in each build, including
object, VM, consensus, collection-order, ownership and processor regressions. The
full sanitizer selection takes 492.87 seconds. These results establish the observed
behaviors, not all methods or a working full node.

## Questions for students

1. Why can adding a missing log change the cached transaction ID in an error path?
2. Why should the same Java-produced wire bytes be given to both implementations?
3. Why is catching every C++ `std::exception` insufficient to reproduce Java's
   distinction between Exception and Error?
4. A null MMR reaches a difficulty rejection before any proof is examined. Should
   that call throw immediately or return false? Trace the source order to decide.
5. What additional evidence would be needed to claim that this code runs as part
   of a complete node, rather than under a controlled test receiver?
