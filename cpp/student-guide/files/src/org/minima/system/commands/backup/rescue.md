# rescue — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/rescue.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/rescue.cpp)

## Purpose

rescue resolves a rescue host and queues an asynchronous request to Main.

## Follow the command

1. Prefer an explicitly supplied host; otherwise use GeneralParams.RESCUE_MEGAMMR_NODE when nonempty. Explicit blank text is rejected by Command rather than falling back.
2. If the resulting host is empty, throw No valid rescue / megammr node specified.
3. Create MAIN_DO_RESCUE, add host and post it to Main. Return host and Rescue attempt started without awaiting connection or synchronization.

## Translation and dependencies

The native body reuses Command parameter validation, GeneralParams, Message and actual Main.PostMessage. It queues MAIN_DO_RESCUE with the selected host; it does not implement a second synchronization algorithm. An explicit host is trimmed by Command, while the configured fallback is copied as stored. The command is registered with the source permission classification.

The pinned Java body was read in full. The existing native Command, CommandRunner and Main fixture provide the compatible building blocks. The sibling project's signal/shutdown loop uses different owners and cannot substitute for this command's parameter validation, reply or lifecycle effects.

## Understanding the tests

The [lab written before the tests](../../../../../../../new-tests/main-lifecycle-commands.md) explains the observation plan. The [102-program corpus](../../../../../../../../tools/main_lifecycle_commands_corpus.py) executes 366 operations through the real commands and an unstarted Main with real database child objects. Both normal and ASAN/UBSAN comparisons pass. Cases cover metadata/help, fresh instances, typed/null/blank parameters, missing Main, repeated requests, explicit/configured host precedence, runner parameter rejection and administrator checks.

The fixture records replies, exceptions, logs, flags and queue size. It then drains the real queue and compares each message's type and host. An independent assertion requires every successful rescue reply to correspond to exactly one queued MAIN_DO_RESCUE with the same host and order. No queued rescue handler is executed. For quit, actual Main.shutdown runs against partially initialized owners, exposing its own caught errors and repeat guard rather than replacing shutdown with a success callback.

## Limits

No host was contacted. This body does not parse or validate host syntax or report a completed rescue. Complete MegaMMR synchronization remains missing. Arbitrary concurrent owner changes and Java null assignments to the native nonnullable global configuration string remain outside these checks; the counterpart stays partial.

## Check your understanding

Why should a command unit test inspect the posted message rather than wait for chain synchronization?

Pinned Java SHA-256: `007221e071ab6c65c71c56ee383455dbd00bbebd101a82b449092c393ab770b4`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
