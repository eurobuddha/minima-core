# Lab: connecting Main to the native runtime

Written before the new tests. The pinned Main.java owns startup, background
messages, child workers and shutdown. Its constructor depends on existing native
MinimaDB, NotifyManager, TxPoWProcessor, TxPoWMiner, NetworkManager and SendPollManager.
Use the pinned 1,379-line source: the root checkout has later behavior.

The first integration gap is Pulse.createPulse. It takes the global database read
lock, obtains the current tree's pulse-list reference, catches Exception from that
lookup, unlocks, and returns. Lock acquisition is outside the catch. Errors are
not Exceptions, and the source does not use finally around the unlock. Preserve
these distinctions rather than inventing a stronger lock guarantee.

Java assigns a new pulse list whenever TxPowTree recalculates. Existing pulses
and callers that retained the old list still see that old list. While a pulse and
the tree share the current list, adding/removing an ID through either reference
is visible through both. After recalculation they diverge. Native shared ownership
can express these identities while retaining the current reference getters.

Compare Java and C++ using actual MinimaDB and TxPowTree bodies, existing chain
fixtures, and recorded SQL. Observe list identity, list mutations, tree replacement,
current-global versus retained-database lookup, and versioned wire output. Exercise
readDataStream on an existing pulse: it replaces its list before reading the
version; a failed read can leave an empty list or a decoded prefix. Old retained
lists must survive both successful and failed reads. The sibling Pulse header
uses a one-byte count and a 60-ID cap, so it cannot replace the pinned wire codec.

For Main itself, compare source message branch order, timer scheduling before or
after work, owner lookups, notification envelopes, startup flags, genesis insertion,
shutdown ordering and partial failures. Listener notification overloads have
different catch behavior. Native worker ownership requires factories after shared
construction and joins during cleanup. Recording SQL does not prove durable local
storage or live MySQL, and isolated message tests do not prove a working full node.

## Observed pulse results

All 152 programs and 946 operations match Java in normal and sanitizer builds
(10.18/25.01 seconds). A source review identified the old in-place tree-list clear;
new identity assertions verify replacement. The first comparison also exposed a
MiniData oversized-length message mismatch; that error now uses the existing
MiniFormat formatter and exact pinned wording. No server or real wallet was used.

The fault receiver forwards successful calls to real database locks and records
three boundaries. These cases distinguish the source's catch scope:

| Injected failure | Tree lookup reached? | Unlock attempted? | Read lock held afterward? |
|---|---|---|---|
| Acquisition throws | No | No | No |
| Tree lookup throws RuntimeException | Yes | Yes | No |
| Tree lookup throws Error | Yes | No | Yes |
| Unlock throws before forwarding | Yes | Yes | Yes |

Each fixture then restores the global database and explicitly releases remaining
fixture locks. This cleanup happens after observing the source result. Null tree
results and clearing the global singleton during lookup are tested separately.

The pulse prerequisite phase ended before Main was implemented. The following
phase records the subsequent Main implementation and its separate test evidence.

The final corpus includes 35 direct MiniData reads for maximum-size precedence,
expected-size mismatch, negative lengths, zero-length values and truncated data.
Oversized cases provide only headers and fail before allocation. Eight affected
regressions pass in both builds (63.66/184.70 seconds): object codecs, tree logic,
utilities, NIO client, MinimaDB behavior, database locks, ownership and chain
commands. This is a scoped regression run, not a rerun of all 190 CTest entries.

## Main implementation phase

The next corpus calls the original Main bodies with unstarted workers and actual
MinimaDB children. It compares flags, mine-mode timing, nullable startup errors,
command dispatch, listener overloads and retained JSON data, message gating and
timer order. Missing child owners deliberately exercise partial-startup failures.
The public MySQL configuration helper is compared separately through recording
connections; its full command remains unported. Wrong message boxes and poisoned
Boolean preferences must preserve Java's cast failures.

A native ownership test will interrupt startup at the first SQL connection using
an explicit throwing halt boundary. It checks that the worker releases its owner
and that nested native worker names restore correctly. This fixture creates only
a private temporary directory; it must not reach networking or key generation.
These tests do not yet establish successful full-node startup or shutdown.

The native cleanup check also fails library loading before timer creation. This
ensures cleanup does not assume that a timer exists and mask the original failure
or retain the Main worker. The ordinary SQL-failure case follows afterward.

The review extension enables the single-TxPoW MySQL backup branch with synthetic
preferences and the existing recording connection. Observe initialization, wire
binding, shutdown and injected SQL failures. Supply null, Boolean, integer and
Unicode credential values to distinguish missing/default values from wrong-type
casts. Java evaluates credentials left to right; C++ must make that order explicit.
Native owner replacement must release retired objects outside the owner mutex,
because destructors and joined workers can call back into the same registry.


## Observed Main results

All 336 programs and 897 operations match Java in both builds. Normal comparison
took 8.45 seconds; the sanitizer comparison took 5.11 seconds. The
native ownership test also passes in both builds, including a reentrant listener
destructor and failures before timer creation and at the first SQL connection.

The first corpus found wrong Boolean cast classification in JsonDB. Extending
coverage to enabled TxPoW backup found the corresponding string getter issue.
Both now use explicit Java-compatible type errors. Message unboxing distinguishes
null from a wrong box. A fixture-only failure was also corrected: installing a
second JDBC recording driver deregistered the first, so the Java fixture now
installs one synthetic driver accepting the shared jdbc prefix.

The compiler and sanitizer checks cover this implementation increment. Full
startup, successful live shutdown, native SQL/MySQL persistence, enabled command
backups and complete MegaMMR integration remain unverified. The Main counterpart
is therefore labelled implemented_partial, with full parity not_verified.
