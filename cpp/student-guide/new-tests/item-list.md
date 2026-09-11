# Lab: split, join and mutable ItemList aliases

Written before the next native test. The pinned helper has observable mistakes:
the two-argument constructor stores the input as its separator, and getArray casts
Object[] to String[] and fails even for an empty list. Preserve them. Compare all
constructors, split/addAll overloads, trim, empty fields, nullable strings/lists,
indexed insertion, retained mutable list aliases, clear/reset and raw objects.

Exact separators use UTF-16 substring search. StringTokenizer mode treats separator
code points independently, skips delimiter runs, then trims each token. Use
supplementary characters and isolated surrogates; record UTF-16 code units so the
JSON output encoder cannot hide a difference. Joining calls raw objects' toString;
a retained mutable object can change later output or throw partway through a join.

An empty exact separator does not advance. Observe this safely by passing a test
list whose add throws after four successful appends, in both runtimes. The list
bounds the experiment; the production splitting loop retains source behavior.
This is evidence of repeated non-advancing appends, not a timing-based claim.

Reuse JavaList storage, Java trim and Unicode helpers, MessageObject's shared raw
object adapter, and the existing JSON collection oracle/Runner pattern. Exercise
real API calls and compare every result and retained list snapshot. Finite tests
do not certify every inherited Java List API or arbitrary concurrent mutation.

## Observed results

Normal and ASAN/UBSAN comparisons pass 1,493 programs and 11,690 operations.
Every declared constructor and method overload is called. String observations
use UTF-16 code units, not replacement-prone console text. Constructor separator
state, raw-list aliases, failing getArray, self-add and a mutating/throwing raw
object all match. The limited append-list records four empty strings before its
intentional exception, confirming that an empty exact separator never advances.

The shared JavaList helper adds indexed insertion and virtual append/destruction;
existing storage and fixed-size behavior are retained. JavaStringTrim gains only
a UTF-16 overload. ItemList joining retains a copy of each raw item while its
renderer executes, keeping it alive across callback-driven list reallocation.

See the [mapped lesson](../files/src/org/minima/utils/json/ItemList.md). Full-build
and affected-suite results are retained with this checkpoint. These finite tests
do not certify inherited List APIs, constructor/subclass dispatch, resource
exhaustion, arbitrary native rendering adapters or concurrent list mutation.

Both complete builds and five affected checks pass per build: peer state,
peer concurrency, messages, JSON collections and real node-count traversal.
[Saved reports](../../validation/item-list-work-in-progress) give the exact scope.
