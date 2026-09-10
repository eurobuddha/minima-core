# quit — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/quit.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/quit.cpp)

## Purpose

quit requests node shutdown with optional database compaction and returns a shutdown message.

## Follow the command

1. Parse compact with default false through Command.getBooleanParam.
2. Check Main.getInstance for null. If nonnull, call Main.getInstance again and invoke shutdown(compact). The source performs two lookups rather than caching the first result.
3. After the shutdown call returns, add message Shutdown complete and return. A missing Main still produces that message; an escaping shutdown failure prevents it.

## Translation and dependencies

The native body preserves both singleton lookups, parses compact first and calls actual Main.shutdown. A missing Main still produces the source success text. The constructor/help/parameter list and fresh getFunction clone are registered in the real CommandRunner.

The pinned Java body was read in full. The existing native Command, CommandRunner and Main fixture provide the compatible building blocks. The sibling project's signal/shutdown loop uses different owners and cannot substitute for this command's parameter validation, reply or lifecycle effects.

## Understanding the tests

The [lab written before the tests](../../../../../../../new-tests/main-lifecycle-commands.md) explains the observation plan. The [102-program corpus](../../../../../../../../tools/main_lifecycle_commands_corpus.py) executes 366 operations through the real commands and an unstarted Main with real database child objects. Both normal and ASAN/UBSAN comparisons pass. Cases cover metadata/help, fresh instances, typed/null/blank parameters, missing Main, repeated requests, explicit/configured host precedence, runner parameter rejection and administrator checks.

The fixture records replies, exceptions, logs, flags and queue size. It then drains the real queue and compares each message's type and host. An independent assertion requires every successful rescue reply to correspond to exactly one queued MAIN_DO_RESCUE with the same host and order. No queued rescue handler is executed. For quit, actual Main.shutdown runs against partially initialized owners, exposing its own caught errors and repeat guard rather than replacing shutdown with a success callback.

## Limits

The response text is not an independent proof that every worker/database closed. Full successful node termination, durable compaction and concurrent changes between the two Main lookups remain unverified. The counterpart stays partial until the complete runtime/API review is finished.

## Check your understanding

Why can the source return Shutdown complete when Main does not exist?

Pinned Java SHA-256: `83d9fe6fe9557a13a159904199be874efc43c8622b61a3d0be2ee16283df4c2b`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
