# MinimaCoreC++

Team and student presentation. Suggested duration: 20 minutes plus questions.

## 1. The project

A native implementation of MinimaCore, developed against a pinned Java source.
The delivery combines an executable, complete source snapshot, test evidence and
457 file lessons. Full behavioral parity remains unverified.

Speaker note: show the delivery source commit and the upstream pin from the manifest.
Do not present counterpart counts as a percentage of behavioral completion.

## 2. The runnable system

The Apple Silicon console joins the Minima network, manages its own saved state,
and exposes the core command interface. The package includes the native libraries.
H2/MySQL is compiled ahead of time from Java. Runtime does not launch a JVM.

Speaker note: use the actual package for the demonstration, not the development binary.
Explain the data folder and why the runtime and source snapshots stay together.

## 3. Source organization

Java packages map to native source paths under `cpp/src/org/minima`. Tests have
corresponding native files. Supporting code models runtime behavior that C++ does
not provide directly, including Java exceptions, number rules and thread behavior.

Speaker note: open one Java/native pair and its lesson. Explain a header versus a
translation unit without implying that the supporting runtime is itself a mapped file.

## 4. Three kinds of evidence

Compilation checks types and linkage. Differential tests compare observable Java
and C++ behavior. Node workflows check startup, synchronization and persistent state.
The release report records each type separately.

Speaker note: show actual completed results. A green build alone cannot establish
consensus correctness or equivalence across every possible input.

## 5. Example: random bytes and padding

MiniData adds six leading zero bytes when padding ten bytes to sixteen. The byte
length changes. The unsigned integer value stays the same. Fixed-input tests compare
complete output. Real-random tests check relationships between printed fields.

Speaker note: distinguish a controlled test input from changing production randomness.

## 6. Example: Java substring behavior

Address's demonstration forms five groups from a Base32 number. It skips characters
between groups. A short encoding raises an exception before output. The C++ port
explicitly retains those ranges and that failure boundary.

Speaker note: ask students why permissive C++ substr would change the result. Point
to the tests for the first failing range and the first successful final range.

## 7. Example: logging and exceptions

Contract traces use MinimaLogger, including its notification behavior. Network
availability catches ordinary failures while allowing Java Error equivalents to
escape. The tests inspect output, callback sequence and failure timing.

Speaker note: small-looking runtime differences can affect callers even when the
normal success return value matches.

## 8. Current limits

456 mapped counterparts exist. One external JNI file is missing. The current missing-main
candidate audit is exhausted; full API/runtime parity remains unverified.
Live startup and reopen remain unreliable in bounded observations.
The executable currently targets macOS 26+ on Apple Silicon.

Speaker note: walk through KNOWN_GAPS.md and the final test report. State failures
or unrun tests directly. This is the basis for deciding what the team can use safely.

## 9. Student work

Each lesson connects source intent, native logic and validation. Students can
reproduce one test, explain its invariant, propose a counterexample and add a
cross-language case. A useful lesson states what its tests do not prove.

Speaker note: suitable first examples are MiniData padding, Address encoding and
send coin ordering. Advanced work covers ownership, interrupts and SQL lifecycle.

## 10. Delivery and next decisions

The delivery manifest identifies the source and hashes every payload. The project
retains verified local recovery bundles. The team can reproduce tests, inspect the
known gaps and assign the remaining source behaviors by file.

Speaker note: close with the actual acceptance decision the evidence supports.
