# Transaction — logic, translation and tests

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Transaction.java](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java) | [src/org/minima/objects/Transaction.cpp](../../../../../../src/org/minima/objects/Transaction.cpp) |

## Teacher's worked explanation

A transaction is a proposed change to the set of spendable coins. Its input list
names the coins being spent, its output list describes the new coins, and its
state list carries values that scripts can inspect. A separate link hash connects
a burn transaction to its associated main transaction. Signatures and proofs live
in the witness, so this class cannot establish complete spend authorization by
itself.

### Follow the amount checks

Take an input worth 5 Minima and outputs worth 2 and 3. `sumInputs()` produces 5,
`sumOutputs()` produces 5, and `getBurn()` produces 0. Removing the output worth 3
leaves a burn of 3. Replacing the outputs with 2 and 4 exceeds the available input
amount, so `checkValid()` logs the totals and returns false.

The method follows this order:

1. Reject a transaction with no inputs.
2. Sum all input amounts, then all output amounts. Reject excess total output.
3. Check each input amount and then each output amount against MiniNumber's valid
   Minima range. This is a separate check from conservation.
4. Collect distinct output token IDs. The special token-creation ID is replaced
   by the Minima token ID for this collection step.
5. Calculate output totals for those IDs, store them in a Hashtable, and enumerate
   the table. For each token, its input total must cover its output total.
6. Compare every pair of input coin IDs and reject a repeated ID.

Why check totals twice? A total over all amounts cannot tell whether the input
and output units belong to the same token. Spending token A must not authorize
creating an equal numerical amount of token B. Also observe the source's precise
treatment of token creation: the port retains its algorithm rather than replacing
it with an independently designed balance rule.

The native implementation reuses MiniNumber arithmetic, Coin accessors, Crypto,
and the existing JavaHashtable adapter. The adapter matters because a sorted C++
container would enumerate tokens differently. The pairwise duplicate check is
quadratic in the number of inputs, matching the source algorithm.

### State and hashes are observable state

`addStateVariable` first removes the same port, inserts the new value, and sorts
by port. `removeStateVariable` leaves the list alone when no matching port exists;
when one is found, Java replaces its state-list object and sorts the survivors.
The native shared record now replaces its owned state-list handle at that same
point. Retained old lists stay alive; clearState instead empties the current list.

`getTransactionID()` returns a cached hash. `calculateTransactionID()` recomputes
it from serialization. Surprisingly, `toJSON()` also recomputes it. Therefore,
formatting a rejected transaction for a log can change the cached ID. Read-only
looking presentation code is not necessarily free of side effects.

Serialization writes the input count and coins, output count and coins, state
count and values, and finally the link hash. Counts use MiniNumber's wire format,
not the C++ vector's in-memory representation. Reading replaces all three lists before reading the first count, reconstructs
them in wire order, then recomputes the transaction ID after the complete link hash.

### How the tests support the translation

`TransactionTests` starts with an empty transaction, adds the 5-unit input and
2/3-unit outputs, checks state lookup, serializes, reads back, and verifies the
result. It provides a small, readable introduction to the class.

`object_differential.py` adds amount boundaries, mismatched token IDs, duplicate
inputs, replacement state ports, wire round trips, and truncated byte sequences.
Its latest normal run passes 9,464 comparisons across **many object classes**;
that is a suite total, not 9,464 Transaction-only cases.

`checker_logs_differential.py` compares the actual checker and Transaction calls.
The first run exposed missing Transaction logs. On an invalid amount, Java logs
inside `checkValid()` before the outer checker logs transaction JSON. Injecting a
logger failure proved that omitting the first event changed where execution
stopped. After correction, the expanded suite passes 1,343 comparisons over 100
fixtures. It also observes cached transaction IDs and validation flags.

### What remains unfinished

This file is still partial. Nullable list entries and Java reference aliases are
not completely represented by the value-based native containers. The native
`calculateStateSize()` now logs source failures and lets Error/allocation failures
escape, although its failing child paths remain untested. Passing amount, wire, and checker fixtures does not
establish these remaining behaviors or full-node integration.

**Exercise:** predict which log is last, and whether the cached transaction ID is
recomputed, when the logger throws on the first versus second rejection event.
Trace `checkValid()`, the outer checker, and `toJSON()` in order.


## Open the test evidence

- [TransactionTests.cpp](../../../../../../test/org/minima/objects/TransactionTests.cpp)
- [object_differential.py](../../../../../../tools/object_differential.py)
- [ObjectOracle.java](../../../../../../support/ObjectOracle.java)
- [objects.cpp](../../../../../../support/objects.cpp)
- [checker_logs_differential.py](../../../../../../tools/checker_logs_differential.py)

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Transaction](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L71), [addInput](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L73), [addOutput](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L77), [isEmpty](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L81), [getAllInputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L85), [getAllOutputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L89), [isCheckedMonotonic](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L93), [clearIsMonotonic](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L97), [sumInputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L101), [sumInputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L109), [sumOutputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L119), [sumOutputs](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L127), [checkValid](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L142), [getBurn](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L237), [addStateVariable](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L258), [compare](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L268), [removeStateVariable](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L280), [compare](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L303), [getStateValue](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L315), [stateExists](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L330), [clearState](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L337), [getCompleteState](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L345), [getLinkHash](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L352), [setLinkHash](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L356), [calculateTransactionID](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L363), [getTransactionID](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L370), [calculateCoinID](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L382), [toString](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L387), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L391), [calculateStateSize](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L427), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L456), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Transaction.java#L481).


Pinned Java SHA-256: `e523aa0ae28d69ce6069434964e9c0ea12b6bcb085b328f934bfc24a2831d9cd`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Retained coin objects

The [coin ownership lab](../../../../../new-tests/coin-ownership.md) follows a single coin retained in transaction inputs, outputs and a witness proof. Ordinary native Coin copies now share field storage, while explicit deep copies reconstruct independent records. The new experiment checks mutation visibility, container growth/removal/clear and handle reassignment. Other collection and element identities remain separate fidelity questions; consult the lab for measured results and exclusions.

## Shared caches and Streamable follow-up

The [Transaction lab](../../../../../new-tests/streamable-transaction.md) explains owned lists and hash handles, shared flag accessors, partial reads, ID failure timing, direct nested writers and actual checker/signing callers. It distinguishes verified behavior from the remaining default/child/null APIs.
