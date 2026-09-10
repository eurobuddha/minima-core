# coinnotify — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/coinnotify.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/coinnotify.cpp)

## Purpose

coinnotify manages addresses whose chain appearance should trigger notification. This command edits or queries the notification set; it does not itself detect coins or send a message.

## Follow the command

1. Read action, then parse address using getAddressParam, normalizing recognized Minima/hex addresses before validating the action branch.
2. For add, call MinimaDB.addCoinNotify and return the address without a found field.
3. For remove or check, call the corresponding database operation and include its found boolean.
4. Unknown actions raise Invalid action after address parsing. There is no saveUserDB call or script creation in the command.

## Translation and dependencies

The native command now uses the actual MinimaDB owner and existing Command parameter/reply helpers, and is registered for dispatch. It reuses MinimaDB’s notification collection and Command address normalization; notification delivery is a separate chain-processing concern.

## Understanding the tests

The tests normalize equivalent Mx/hex addresses, add twice, check, remove twice and compare exact field presence. An invalid action plus malformed address establishes that address parsing fails first. Owner recreation checks loss of the volatile registration. An explicit alias guard requires successful checks and removal across the two address forms.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both native builds and all six selected regression suites pass. The expanded address group passes 134 programs with 276 operations and 3,693 SQL calls in each build. See the [address-command lab](../../../../../../../new-tests/address-commands.md) for inputs, operation order and limitations.

## Limits

The help says registration is required on each startup. This wrapper does not establish persistence or delivery of NOTIFYCOIN; those need separate integration tests.

## Check your understanding

Why does add return no found value while remove and check do?

Pinned Java SHA-256: `bbaf53780d7219c037953ff2453d360616995e856cde3e798f2aa455900ba228`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
