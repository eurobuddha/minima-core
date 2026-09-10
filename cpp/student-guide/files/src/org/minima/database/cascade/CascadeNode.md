# CascadeNode — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/cascade/CascadeNode.java](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java) | [src/org/minima/database/cascade/CascadeNode.cpp](../../../../../../../src/org/minima/database/cascade/CascadeNode.cpp) |

## What this file does

A compact chain node that retains a copied TxPoW header, its cascade level and a link to the next older retained node.

## Follow the logic

1. Deep-copy the supplied TxPoW, then clear its body. Start current level at zero and record the supplied TxPoW’s superlevel.
2. Parent links connect retained nodes but are not serialized within one CascadeNode. The surrounding Cascade reconstructs the list.
3. Compute current weight as the TxPoW weight multiplied by 2^currentLevel using DECIMAL32 rounding. This uses seven significant digits with HALF_EVEN behavior, distinct from MiniNumber’s 64-digit DOWN context.
4. Serialize the copied TxPoW followed by current level encoded as MiniNumber. Reading restores those values and derives superlevel from the decoded TxPoW; it does not explicitly clear a body supplied on the wire.
5. toString includes current/superlevel, TxPoW ID, block number, decimal weight and local date.

## How the translation is approached

The native node stores its copied TxPoW by value and parent by shared_ptr. MMREntryNumber’s decimal adapter represents unbounded weight; multiplyDecimal32 reproduces the seven-digit rounding. Exponentiation uses repeated squaring rather than floating-point powers.

## What the tests tell us

The node fixture varies twelve block inputs, difficulty divisors 1/2/3/7 and levels -1/0/1/7/23/31/64, comparing bytes, weight unscaled value/scale/text and display text. Derived read cases also check superlevel, body presence and trailing-byte count. Dedicated weight vectors target HALF_EVEN ties and large values. TZ is fixed to UTC. See [Java oracle](../../../../../../../support/CascadeOracle.java), [native harness](../../../../../../../support/cascade.cpp) and [differential driver](../../../../../../../tools/cascade_differential.py). The inspected existing report contains 2,892 matching cases with zero failures across this suite. Generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [CascadeNode](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L32), [CascadeNode](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L34), [getTxPoW](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L46), [getLevel](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L50), [setLevel](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L54), [getSuperLevel](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L58), [setParent](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L62), [getParent](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L66), [getCurrentWeight](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L70), [toString](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L78), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L85), [readDataStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L91), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java#L97).

## Check your understanding

Why is multiplying a double weight by pow(2,level) insufficient for a 1:1 port? What does body presence after readnode tell us that the normal constructor alone does not?

## Limits and traceability

Parent identity, exact negative-level errors and every date/locale are outside these observations. Deep-copy failure behavior follows TxPoW and still needs full parity review. Port status remains partial and parity not_verified.

Pinned Java SHA-256: `db3af7a3eed8ac5406db56c6beccbdad5517d1f84bc686b757c377e55c7f6332`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
