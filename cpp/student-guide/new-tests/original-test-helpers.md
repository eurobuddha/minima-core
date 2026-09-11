# Original test helpers: factories are executable source too

This batch maps [TestUtils.java](../../../upstream-study/test/org/minima/utils/TestUtils.java)
to [TestUtils.cpp](../../test/org/minima/utils/TestUtils.cpp), and
[QuickClients.java](../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickClients.java)
to [QuickClients.cpp](../../test/org/minima/system/network/p2p/testingutils/QuickClients.cpp).
These are real helper implementations, not empty test executables. Neither source
contains assertions, so the [driver](../../tools/original_helpers_differential.py)
calls their actual methods in [Java](../../support/OriginalHelpersOracle.java)
and [C++](../../support/original_helpers.cpp).

## TestUtils delegates; it does not mine

The short overload supplies block 1 and weight 0. The other overload constructs a
fresh TxPoW using the existing testing constructor. It keeps the requested ID,
block and weight, with no body, a zero parent ID, block=true and transaction=false.
A negative block or weight is permitted in this test constructor. Do not apply
production consensus validation to synthetic test data unless the source does.

The native helper directly reuses that constructor. Tests cover both overloads,
signed 32-bit boundaries, empty and arbitrary IDs, separate returned objects,
and independent test transaction lists. Adding a transaction to one result must
not modify another result. The native string API still cannot represent Java's
null ID; full dependent TxPoW identity and nullable-ID parity remain unverified.

## QuickClients preserves order and default fields

For i from zero to count-1, append i+1 to the UID and host prefixes, then call the
existing NIOClientInfo constructor. Thus count three yields uid1, uid2, uid3 in
that order. The port and incoming flag are copied without new validation; even
values outside normal TCP port ranges are valid inputs to this simple descriptor.

A nonpositive count produces an empty mutable list. Prefixes are never dereferenced
on that path, so null prefixes can succeed when no entries are requested. With a
positive count, Java evaluates UID concatenation before host concatenation. C++
uses two statements to preserve that order. The returned shared list reuses the
existing JSONSequence container and owns each descriptor separately.

The fixture compares every JSON field produced by the real descriptors, then
clears the list through one retained reference. Another list reference sees the
clear; a saved child remains alive with its original fields. This distinguishes
list ownership from child ownership.

## Evidence and limits

Both builds pass 584 comparisons and 5,472 independent assertions, with identical
transcripts and empty runtime stderr. Python calculates expected IDs, fields,
counts and constructor defaults independently. See the
[saved evidence](../../validation/original-helpers-work-in-progress/progress.json).
No production implementation changed in this batch, so validation builds and runs
the new consuming target in both configurations without repeating unrelated suites.

Both mapped files now exist and their active source methods are translated.
Their status remains partial and full parity not_verified: passing these helper
cases does not prove complete TxPoW/NIOClientInfo behavior, null TxPoW IDs,
arbitrary allocation failures or concurrency semantics.

Exercise: why can a null UID prefix succeed with count zero but fail with count
one? Why is checking only list length insufficient to catch a zero-based suffix
or a wrongly shared client object?
