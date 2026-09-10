# healthcheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/healthcheck.java) · [Native C++ body](../../../../../../../../src/org/minima/system/commands/base/healthcheck.cpp)

## Purpose

healthcheck reports tip/root heights and checks whether the cascade tip is immediately before the tree root. The active source does not produce the Maxima fields advertised in help.

## Follow the command

1. Get the chain tip and reject null with No TIP block found.. ?.
2. Get the root, emit tip/root heights as strings, and compute chainlength as tip−root, without adding one.
3. If the cascade has a tip, read its height, set tipcorrect to equality with tree root−1, and include cascade length.
4. If no cascade tip exists, emit the literal string nocacade under cascade. Return the response; there are no proof, parent-hash or Maxima checks in this body.

## Translation and dependencies

The registered native command reuses Command's reply envelope and typed receiver checks, the actual MinimaDB singleton, TxPowTree and Cascade getters, and MiniNumber arithmetic. It preserves the source's repeated database/tree and cascade-tip lookups. Shared pointers retain native object ownership while each read executes; no cached response or substitute chain model is introduced.

## Understanding the tests

The [healthcheck corpus](../../../../../../../../tools/healthcheck_commands_corpus.py) supplies 176 programs and 635 operations through the existing Main comparison driver. Both normal and ASAN/UBSAN builds match the pinned Java implementation. The fixture constructs actual tree nodes and cascade nodes, invokes tree recalculation, and then executes the registered command.

Independent expected values check single-node length zero, gapped and decreasing heights, fractional heights, large allowed heights, a heavier branch, and matching or mismatching cascade boundaries. Empty trees produce the exact missing-tip error. An explicitly null cascade fails only after the tree is available; an empty cascade returns the literal string nocacade. Replacing tree/cascade state between calls checks that the command reads current state. Direct calls must leave SQL events, timers, listeners and Main's queue empty.

Metadata checks include the empty valid-parameter list and fresh command instances. Direct calls ignore unused parameters, while text-runner cases exercise invalid-parameter and permission gates. The full reply is compared, so adding the Maxima fields mentioned only in help would fail.

One initial test mistakenly supplied a height outside MiniNumber's allowed range. Java rejected setup before a tip existed. The corrected corpus retains that rejection as its own test, uses supported heights for arithmetic cases, and asserts setup success in every other case. This was a test expectation correction, not a production-code change. See the [lab](../../../../../../../new-tests/healthcheck-command.md).

## Limits

Height adjacency is a limited consistency check, not chain/cascade cryptographic validation. Synthetic setup uses unstarted Main workers. Concurrent singleton/root replacement and exhaustive failure interleavings are not established. Existing tree and cascade tests cover their deeper algorithms; this command suite establishes the finite wrapper behavior described above.

## Check your understanding

Why does a chain containing just its root report chainlength zero?

Pinned Java SHA-256: `cff60ae34f39db98ac66f3222925f953da734bf85e43dc87234227d194d2df0b`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
