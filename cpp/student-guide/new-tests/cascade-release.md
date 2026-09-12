# Cascade cleanup: why releasing a pointer can overflow a worker stack

## The observed failure

At 23:39 on September 11, the frozen `6c28109` port-21001 preview crashed with
SIGBUS. Its binary UUID matches the supplied crash report. The report records
14,309 stack frames, repeated CascadeNode/shared_ptr destruction and an address
inside a Stack Guard region. Only 89 frames were retained in the report. The
caller chain includes `IBD::~IBD()` inside `NIOMessage::receiveGreeting()`.
The newer optimized executable still used the same CascadeNode ownership code.
See the [sanitized technical evidence](../../validation/cascade-release/crash-evidence.json).

A stack guard is an inaccessible memory region beside a thread's stack. Each
nested function call consumes stack space. Releasing a long linked structure can
therefore crash even when nobody wrote an explicitly recursive function.

## Follow the ownership, not just the loops

A Cascade owns its newest node through a `shared_ptr`. Each node owns its older
parent the same way. Suppose every node has exactly one owner:

```text
IBD -> Cascade -> newest -> older -> older -> ...
```

Destroying the temporary IBD releases the Cascade. Releasing its last tip owner
starts the tip destructor. The implicit parent-member destructor then releases
the next node, whose destructor releases another parent. Thousands of destructors
remain on the stack together. An iterative serialization loop does not prevent
this separate, implicit recursion during cleanup.

Java's garbage collector does not use this C++ shared_ptr destruction path. The
Java implementation remains our oracle for cascade data and algorithms, but a
native lifetime regression is needed to expose this specific translation defect.

## The reused implementation and minimal change

The fix belongs in [CascadeNode.cpp](../../src/org/minima/database/cascade/CascadeNode.cpp).
Cascade and IBD keep their existing strong ownership and wire formats. The test
reuses [FixedTaskExecutor](../../src/support/FixedTaskExecutor.hpp), the production
node constructors and `IBD::createCompleteIBD()` instead of a mock linked-list class.
Existing [cascade](../../tools/cascade_differential.py),
[IBD](../../tools/ibd_differential.py) and
[greeting](../../tools/nio_message_differential.py) comparisons remain in use.

The first destructor moves its parent reference into a local work queue. It
releases one queued reference at a time. If that was the parent's last owner,
the parent's destructor adds its own parent to the active queue and returns.
There is bounded destructor nesting instead of one stack frame per ancestor.
If another owner retains a node, releasing our reference does not start that
node's destructor, and its entire parent chain stays intact.

Only an object whose destruction has already started surrenders its parent.
Checking `use_count() == 1` and then detaching a live parent would be unsafe: another
thread could promote a weak pointer between the check and the edit. A weak parent
link would also be wrong because a tip alone would no longer keep its history alive.

The queue's ordinary case uses one inline shared_ptr slot and no heap allocation.
A vector handles additional pending references when an unusual custom deleter
reenters cleanup for an independent chain. Its allocation failure would terminate
inside noexcept cleanup; the ordinary production chain does not allocate there.
A trivially destructible thread-local pointer identifies the stack-owned queue,
so cleanup can also originate from a thread-local owner's destructor at thread exit.

Declaring a destructor suppresses implicit move generation in C++. The header
therefore explicitly defaults the existing copy/move operations. The regression
checks both shared copies and moved-from parent pointers; a type trait alone could
mistake a copy accepting an rvalue for a real move.

## The regression and what each case proves

[The native fixture](../../support/cascade_release.cpp) builds 30,000 distinct
CascadeNodes. Most cases share an immutable TxPoW payload to keep the lifetime
experiment focused; the complete-IBD case reconstructs every header through the
real wire-copy path. This is a synthetic ownership fixture, not a valid mainnet
consensus chain. Weak witnesses observe every node without retaining it.
Each case runs on a 512 KiB pthread and through the actual FixedTaskExecutor.

| Case | Observation required |
|---|---|
| Unique chain | Dropping the final tip releases every node without crashing. |
| Thread exit | A thread-local final owner can release a long chain during thread teardown. |
| Shared tail | Releasing one branch frees its prefix, preserves the other branch's tail, then frees everything after the last owner leaves. |
| Retained weak owner | A second thread promotes a middle node and retains its complete ancestry while the tip is released. |
| Concurrent weak promotions | Repeated weak locks never observe a live node with a severed or replaced parent. |
| Temporary IBD | Leaving an IBD scope releases its long cascade. |
| Shared IBD | Destroying IBD copies leaves an externally retained Cascade unchanged. |
| Complete IBD | The actual wire deep copy retains all 30,000 nodes under the fixture quota; destroying the temporary copy frees it and leaves the original intact. |
| Replacement and assignment | Parent replacement, copies, moves, move assignment and Cascade replacement release old chains and preserve expected owners. |
| Reentrant deleter | Two independent chains queued by one custom deleter both complete cleanup without losing pending work. |

Running `cascade_release unique-chain` against the unchanged production code
returned `-10` (SIGBUS) immediately after the START line. The fixed regression must
pass the same depth and small-stack condition. Increasing the worker stack or
reducing the chain length would conceal the defect rather than fix ownership.
The longer timeout accommodates real header reconstruction under sanitizers.

## Reproduce and interpret the results

```sh
cmake --build cpp/build --target cascade_release
ctest --test-dir cpp/build -R '^cascade_release$' --output-on-failure
```

Repeat with `cpp/build-optimized` and `cpp/build-sanitized` after configuring those
builds. [The validation record](../../validation/cascade-release/README.md) separates
this native lifetime test from the Java differential suites and node lifecycle checks.
Weak-witness expiry catches retained nodes in the fixture even where LeakSanitizer
is unavailable. ASan/UBSan check their supported memory/undefined-behavior errors;
they are not a race detector or proof of every possible concurrent schedule.

## Questions for students

1. Why can an iterative `getLength()` coexist with recursive destruction?
2. Why must the second branch retain all the shared tail's parent links?
3. What interleaving makes `use_count()` followed by mutation unsafe?
4. Why do we need both weak-witness checks and wire/state comparisons?
5. What changes when a destructor is declared but moves are not explicitly preserved?

This fix does not resolve shared_ptr cycles, make concurrent mutation of the same
node safe, certify full Minima parity, or establish overnight/live-sync stability.
It addresses the demonstrated cascade cleanup stack overflow.
