# Lab: a command that queues another command

Written before the new tests. Source: [sendpoll.java](../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java).

Imagine a university print queue. Adding a job records an intention; it does not
prove that the printer has finished. Here `sendpoll` records command text in
`SendPollManager`. The worker tries each queued command and removes it only when
its returned `status` is true. The existing manager supplies locking, copied
snapshots, random identifiers, and retry processing. We reuse that implementation.

Follow the command in source order: obtain the action (default `add`), obtain the
manager through Main, and then choose add, list, or remove. An unknown action
returns an empty response. This order matters: even an unknown action accesses
Main. A null manager is only dereferenced in a branch that uses it.

Adding replaces the first case-sensitive occurrence of `sendpoll` with `send`.
Only the exact resulting text `send` is rejected. This is string rewriting, not
validation of a complete transaction. Leading spaces, uppercase spelling, and a
second occurrence help distinguish the source algorithm from a tempting rewrite.

The test will run identical JSON operation sequences against compiled Java and
C++. Both use the actual command and queue classes. The Java fixture bypasses the
thread-starting constructor so no node or wallet starts. The C++ fixture uses its
existing dormant-worker constructor. Explicit poll operations call the actual
worker body and real command runner; timer publication is recorded, not delivered.
The existing worker tests separately exercise the live timer/thread machinery.

After each operation, compare its return value or exception and the entire queue.
Keep an old list snapshot, add/remove more commands, then check that the snapshot
is unchanged. Random UIDs must have 16-byte hexadecimal form; replace them with
stable encounter-order labels only for cross-language comparison. The same UID
must keep the same label, and distinct jobs must retain distinct identifiers.
Compare exception types; JVM-generated null-dereference prose is platform specific. The existing native
parameter reader also uses generic cast prose instead of Java class/loader details;
cast exception types are compared, but that wording remains a parity gap.

Boundary cases include missing action/UID, null parameters, unknown actions,
missing Main, a null manager, mixed-case command text, empty text, duplicate jobs,
removing an absent UID, and real runner dispatch. A successful hash job should
leave the queue after polling; a failed command should remain. These tests show
observable command/queue behavior. They do not establish full-node or durable
wallet parity, nor prove that every concurrent interleaving is correct.

## Evidence

Both full C++ builds pass. The new comparison passes 367 programs and 3,216
operations: 6.75 seconds normal and 8.54 seconds with ASAN/UBSAN. The initial
comparison exposed 88 cast-message wording differences, all in the existing
Command parameter reader; exception types match. That wording is now explicitly
excluded, and remains an API parity limitation. Worker/runner regressions are
recorded in the current project review.
