# How to study this Java-to-C++ port

This project is a practical lesson in preserving software behavior across
languages. Our reference is the Java Minima repository at commit
`5564b9d78978efcaf23679190d05253961fc6e69`. A commit identifies a particular
snapshot. Without it, a later change to Java could make yesterday's correct
translation appear wrong today.

There are **457 Java source files** in the mapping: **369 production files and
88 test files**. The mapping gives each source one unique `.cpp` destination.
C++ headers contain declarations and sometimes small inline implementations;
they support those mapped `.cpp` files. Shared native helpers are additional
files, not substitutes for missing mapped sources.

At this checkpoint, **294 counterparts exist and 163 are absent**. Existing
counterparts remain classified as partial, with complete parity unverified.
The full native node is not working yet: Main startup and native H2/MVStore are
among the essential missing pieces. Read [SOURCE_MAP.md](../SOURCE_MAP.md) for
the live inventory; these counts can change as the port advances.

## Start with a question, not a translation of each line

Before examining a file, write down its responsibility in one sentence. For
example: “Transaction checks that outputs do not exceed the available inputs.”
Then refine that sentence as you read: the same check must also account for
different tokens, permitted amounts, and duplicate input coin IDs.

Next identify its inputs, output, and state changes. A method that returns
`false` may first log a message, update a cached hash, or mark a script as checked.
Those actions belong to the behavior you are trying to preserve.

Finally, trace one valid example and one failure example through the actual
statements. Ask exactly where the method returns or throws. This prevents a
common mistake: replacing several ordered checks with a convenient expression
that silently changes which operations happen first.

## Why this is more than changing syntax

| Java feature | Translation question |
| --- | --- |
| Object references | Does the C++ code retain the same object or copy its current value? Which callers can observe later changes? |
| Null references | At what operation does null become an error? Rejecting it too early can skip an earlier, valid return path. |
| Exceptions and Errors | Which failures are caught? Which escape? Does logging a failure itself throw before cleanup or assignments? |
| Hash collections | Is only membership observable, or do iteration order, replacement, and resizing affect results or logs? |
| Streams | Are the bytes identical? How much input is consumed before failure? Are earlier mutations retained after a truncated read? |
| Threads | Who owns the worker's inputs after the caller returns? When can the caller observe completion? |
| Numbers and strings | Does arithmetic round the same way? Are character operations defined in bytes, Unicode code points, or UTF-16 units? |

These are questions to answer from the source and tests. A language's ordinary
library type is useful only when its behavior matches the required operation.

## Our evidence: use the original program as an oracle

A **differential test** gives equivalent inputs to the Java implementation and
the native implementation, then compares their observable outputs. In many
fixtures Java creates a transaction or block, serializes it, and supplies those
exact bytes to both programs. This avoids accidentally testing two different
objects that merely look similar.

The comparison may include return values, bytes, JSON, ordered events, changes to
objects, exception types, or resource ownership. The lab should say which of these
it captures. It should also name any fields it excludes, such as a platform's
generated stack frames. Excluding a field narrows the result we can claim.

A controlled receiver is a test object placed at an external boundary. For
example, a logger receiver can record printed lines and deliberately throw on its
second call. The real validator still runs; the receiver makes an unusual failure
reproducible. This does not prove that an entire production node is integrated.

Some tests instead execute mapped Java test scenarios directly in C++. Others
check native worker ownership or memory safety. These forms of evidence serve
different purposes; none should be presented as universal proof.

## Read a test as an experiment

For each test, answer five questions:

1. **Setup:** what objects, bytes, environment, and initial state are supplied?
2. **Stimulus:** which operation or sequence runs, and what failure is injected?
3. **Observation:** what values, mutations, events, or resources are measured?
4. **Expectation:** does it come from the pinned Java program, a source assertion,
   or an independently justified native property?
5. **Limit:** what important behavior does this experiment leave unexamined?

If a test only checks successful round trips, it does not establish behavior for
truncated data. If it compares one method under controlled receivers, it does not
establish startup, persistence, networking, and shutdown of the whole node.

## A useful reading order

Begin with base values and serialization: learn how amounts, byte strings, and
wire fields are represented. Then study coins, transactions, witnesses, and
proofs. Follow those into the script interpreter and transaction checker. Read
the database and block-tree code next to understand where state lives. Finally,
study message processors, networking, commands, and startup to see how the
components are intended to work together.

The per-file pages mirror the Java package hierarchy so that two files with the
same class name still have unambiguous locations. Test pages live under `files/test`;
production pages live under `files/src`. Follow dependencies when a term is
unfamiliar, but return to the original file's responsibility after each detour.

Start with the worked
[checker rejection lab](new-tests/checker-rejections.md). Its first comparison
found concrete mismatches, so it demonstrates investigation and correction,
rather than presenting a successful final run without explaining how we got there.
Then study the [command failure lab](new-tests/command-runner-errors.md) to see
how catch boundaries, logging and batch stopping interact. The [wallet-command lab](new-tests/wallet-commands.md) connects those rules to SQL side effects and stateful signing. The [address-command lab](new-tests/address-commands.md) follows real key creation, volatile registrations and preference-file reloads.

## What completion means

The following statements make increasingly strong claims:

| Evidence | What it establishes |
| --- | --- |
| A destination path exists in the mapping | We know where a translation belongs. |
| A C++ counterpart exists | Some source has been written at that location. |
| It compiles | The compiler accepts the current translation and its dependencies. |
| Named tests pass | The behaviors observed by those specific tests match their expectations. |
| Complete source/API review plus relevant validation | The file's required behavior has substantially stronger parity evidence. |
| A native node runs and passes integration validation | Components operate together in the tested runtime scenarios. |

We do not promote one claim into the next without evidence. In particular, 457
guide pages would mean that the documentation has an entry for every source file;
it would not mean that all 457 files have been implemented or verified.

## Keep an engineering notebook

For a change, record the source you read, the existing helper you reused, the
smallest adaptation required, the first failing example, and the final test
result. Explain the cause of the mismatch before explaining the fix. Preserve
failed examples so the same defect cannot quietly return.

When proposing an additional test, identify a meaningful boundary: no inputs,
one input, a repeated ID, a truncated field, a log failure, or a worker returning
after its caller. Avoid merely copying the implementation into a second function
and asserting that the two functions agree. A good test could disagree with the
implementation for a reason you can explain.
