# tutorial — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tutorial.java) · `src/org/minima/system/commands/base/tutorial.cpp` (absent)

## Purpose

tutorial returns a fixed KISS VM grammar and function-reference string. It neither parses nor executes any of the scripts described in that string.

## Follow the command

1. Construct the common reply, concatenate literal Java strings beginning with a newline and append the grammar productions in their written order.
2. The grammar presents statement alternatives, then relation/logic/arithmetic precedence down to variables, values and parenthesized expressions. The remainder lists globals and function descriptions.
3. Put the entire string under response and return. getFunction creates a fresh tutorial object; no parameters, chain state, filesystem or network services are consulted.
4. The literal is historical documentation: its function list uses SUMINPUT/SUMOUTPUT while later descriptions say SUMINPUTS/SUMOUTPUTS, and some escape/spacing sequences are unusual. A faithful port must preserve the actual output separately from any decision to improve documentation.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the exact pinned literal as data. A raw C++ string or generated escaped literal can preserve bytes, but Java source escape sequences must be evaluated correctly rather than copied as visible backslashes.

## Understanding the tests

No direct native assertion for this absent command is claimed. The appropriate proposed assertion is byte-for-byte equality of the complete response string and command metadata/fresh reply. Separately compare examples and names with actual parser/function behavior to assess documentation accuracy; a literal equality test does not establish grammar correctness.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

This class is documentation output, not the authoritative VM implementation or a VM test suite. No claim is made that every printed production exactly describes accepted scripts.

## Check your understanding

Why are exact output comparison and checking the grammar against the parser two different tests?

Pinned Java SHA-256: `8bf9c613dcf56938e804c70b18283a153a6b1586a13ddffd4704aa16ca578ced`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
