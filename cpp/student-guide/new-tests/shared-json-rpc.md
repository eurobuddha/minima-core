# Lab: shared JSON records and iterator lifetime

Written before implementing the tests. RPC returns the actual users array, adds
records to it, and replaces it when removing records. A retained list reply must
still see additions to its original array, and must keep that original array after
UserDB installs a replacement. Copying every RPC result would lose that behavior.

Java containers store object references. The native JSONValue wrapper can be
copied while its nested shared_ptr still refers to the same object or array.
Use this distinction to remove borrowed references into reallocatable vectors:
copy a value under a mutex, then release the mutex before traversing a child.
Copy-constructing a container creates independent outer storage, as Java's
collection constructors do, while retaining references to the same children.

Read JSONArray's ArrayList inheritance and JSONObject's LinkedHashMap inheritance.
Both serializers use iterators. A structural change after iterator creation can
make next throw ConcurrentModificationException. Replacing an existing map value
does not structurally change its map. ArrayList.hasNext checks the current size;
LinkedHashMap's iterator retains its next entry. Test changes before the first
element, between elements and after the last element, including initially empty
containers. Do not assume every mutation must make hasNext or next fail.

Compare deterministic operation programs against the pinned Java classes. Include
duplicate keys, Unicode keys, copy independence, shared nested identities, append,
remove, null values, exhausted iterators and retained RPC arrays. The existing JSON
parser and preferences suites remain regression checks, not substitutes for this
iterator experiment. Use actual UserDB and Authorizer in a native concurrent test;
race appends/replacements with authorization and serialization. Accept a complete
valid observation or the documented iterator exception, never corrupted JSON or
a dangling reference. Keep synthetic credentials and bound the work.

JsonDB also needs safe ownership when readDataStream replaces its root object.
A reader must retain a root owner for each access. This does not make separate
source operations, or a whole RPC command, one atomic transaction. Concurrent
stress is finite evidence; review the ownership and locking discipline as well.


## Initial finding

The first [direct comparison](../../tools/json_collections_differential.py) ran
67 programs / 606 operations. Iterator transitions and copy/child cases matched;
three indexed reads differed only in exception category. Java reports
IndexOutOfBoundsException, while the earlier native vector exception reached the
boundary as RuntimeException. The production indexed getter now raises the Java
category explicitly. The [initial failure report](../../validation/shared-json-work-in-progress/initial-index-failure.json)
is retained so students can see the evidence that motivated the correction.

Reuse sources: existing JSON container storage/serialization bodies,
JavaConcurrentMap's mutex-protected copy discipline, JsonDB/UserDB and Authorizer,
and the RPC configuration native test's concurrent writer/reader structure.
The ordered vector and checked iterator adapter is necessary because neither
JavaList's unsynchronized storage nor JavaConcurrentMap's hash traversal can
replace ArrayList/LinkedHashMap behavior unchanged.


## Focused validation result

Both full builds compile without observed warnings. The corrected 67 programs /
606 operations now match Java in both builds. The native test executes 1,600
concurrent loop iterations using actual UserDB and Authorizer; each run recorded
1,594 complete parseable serializations and six permitted iterator failures.
It also verifies retained array/root/child ownership after replacement. Those
schedule-dependent counts are observations, not expected constants.

[Native concurrency experiment](../../support/shared_json_native.cpp) ·
[Current evidence](../../validation/shared-json-work-in-progress/progress.json)

The full 204-test suites are running. They exposed a stale MiniByteTests exception
catch, which has been corrected and directly rerun successfully in both builds.
Full regression completion and live RPC command startup are still pending.
