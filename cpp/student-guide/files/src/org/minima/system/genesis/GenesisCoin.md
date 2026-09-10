# GenesisCoin — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/genesis/GenesisCoin.java](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisCoin.java) | [src/org/minima/system/genesis/GenesisCoin.cpp](../../../../../../../src/org/minima/system/genesis/GenesisCoin.cpp) |

## What this file does

Defines the special starting coin used to seed the initial MMR and fund the genesis transaction.

## Follow the logic

1. Use the fixed identifier 0x5350415254414355534C4F5645534D494E494D41.
2. Construct an ordinary Coin locked to Address.TRUE_ADDRESS, with amount MiniNumber.BILLION and the Minima token identifier.
3. Inherit the normal Coin representation and wire behavior. This class adds no separate serialization layer or validation routine.

## How the translation is approached

The C++ class derives from Coin and invokes the matching base constructor with the same constants. Reusing Coin keeps genesis serialization subject to the same field encoding as later coins.

## What the tests tell us

Each genesis fixture outputs the GenesisCoin wire bytes and JSON alongside the genesis MMR and TxPoW. Five destination addresses vary the genesis transaction while this starting coin remains fixed. This verifies the observed constant initialization, not a running network bootstrap.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [Address](../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Token](../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [GenesisCoin](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisCoin.java#L13).

## Check your understanding

Why is the genesis coin’s address different from the destination supplied to GenesisTxPoW? Trace the coin from input to output.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `f04d5802b99ce54aeb17658d69351da33117e2bff62799706dafdc073d28c62c`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
