# getaddress — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/getaddress.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/getaddress.cpp)

## Purpose

getaddress retrieves an existing default address from the wallet. The active body contains legacy parameters whose advertised valid-parameter list is empty.

## Follow the command

1. Obtain the wallet.
2. If createall exists in the raw parameters, execute an empty branch: its former address-creation and response code is commented out. The command returns only the common envelope.
3. Otherwise read type with default single, but do not use it; call wallet.getDefaultAddress and return the ScriptRow JSON.
4. The public-key consistency check is also commented out and does not run. getValidParams returns an empty list, so direct runCommand behavior and normal runner parameter validation must be examined separately.

## Translation and dependencies

The native command now uses the actual MinimaDB owner and existing Command parameter/reply helpers, and is registered for dispatch. It reuses Wallet.getDefaultAddress and ScriptRow JSON. Do not turn this into newaddress or reinstate commented key checks during a 1:1 translation.

## Understanding the tests

The tests compare the selected row, raw createall presence even when its value is false or null, and unused type parsing. They separately check normal runner rejection of these undeclared options. Empty, single and identical duplicate SQL rows make the random selection observable without depending on a shared random seed.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both native builds and all six selected regression suites pass. The expanded address group passes 134 programs with 276 operations and 3,693 SQL calls in each build. See the [address-command lab](../../../../../../../new-tests/address-commands.md) for inputs, operation order and limitations.

## Limits

The source help’s key-use guidance is not an assertion in this wrapper. No command-level validation of the returned public key occurs in the active code.

## Check your understanding

Why can a raw createall:false parameter suppress the response just as createall:true does?

Pinned Java SHA-256: `8df301da56a701478606e4d621701cae1b647d55ea8e6104a769a505b78c89d0`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
