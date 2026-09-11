# tutorial — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/tutorial.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/tutorial.cpp)

## Purpose

tutorial returns a fixed KISS VM grammar and function-reference string. It neither parses nor executes any of the scripts described in that string.

## Follow the command

1. Construct the common reply, concatenate literal Java strings beginning with a newline and append the grammar productions in their written order.
2. The grammar presents statement alternatives, then relation/logic/arithmetic precedence down to variables, values and parenthesized expressions. The remainder lists globals and function descriptions.
3. Put the entire string under response and return. getFunction creates a fresh tutorial object; no parameters, chain state, filesystem or network services are consulted.
4. The literal is historical documentation: its function list uses SUMINPUT/SUMOUTPUT while later descriptions say SUMINPUTS/SUMOUTPUTS, and some escape/spacing sequences are unusual. A faithful port must preserve the actual output separately from any decision to improve documentation.

## Translation and dependencies

The native counterpart reuses Command and preserves this older tutorial separately as minima::commands::base::tutorial. It retains its full-help override and all 6,733 payload bytes. The registered root tutorial has different text and is a different native class; replacing one with the other would change behavior.

## Understanding the tests

Both differential suites pass 71 cases and 428 independent assertions. It compares complete text, metadata, reply shape, the public WP constant, fresh factories, ignored direct-call parameters, parameter aliases, ordinary/non-admin callers, invalid runner parameters and the registered tutorial variant. Python independently decodes the pinned literal expressions and records exact lengths and SHA-256 hashes. Both full builds pass; the normal comparison took 2.618 seconds and the sanitizer comparison took 4.137 seconds, with identical transcript hashes and empty native stderr. A literal equality test does not establish grammar correctness.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The test plan was written before execution. See the [text-preservation lab](../../../../../../../../student-guide/new-tests/static-command-text.md) for method and current results.

## Limits

This class is documentation output, not the authoritative VM implementation or a VM test suite. No claim is made that every printed production exactly describes accepted scripts.

## Check your understanding

Why are exact output comparison and checking the grammar against the parser two different tests?

Pinned Java SHA-256: `8bf9c613dcf56938e804c70b18283a153a6b1586a13ddffd4704aa16ca578ced`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
