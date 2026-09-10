# BooleanValue — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/values/BooleanValue.java](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java) | [src/org/minima/kissvm/values/BooleanValue.cpp](../../../../../../../src/org/minima/kissvm/values/BooleanValue.cpp) |

## What this file does

Represent a KISS Boolean as its own typed value rather than an implicit numeric truth value.

## Follow the logic

1. Construct the object with one Boolean field. The static TRUE and FALSE objects are reusable values.
2. Return type tag VALUE_BOOLEAN; print exactly TRUE or FALSE in uppercase.
3. isFalse negates the field, while isEqual compares two Boolean fields.

## How the translation is approached

C++ uses an explicit BooleanValue subclass with the same type tag and two static objects. Value.getValue exposes stable shared references to the static objects through a non-owning deleter; raw object-identity parity beyond that path is not established.

## What the tests tell us

The value/type/verify/same cases include TRUE, FALSE, lowercase true and mixed Boolean/number/string types. The execution suite additionally tests RETURN/IF/ASSERT requiring Boolean rather than automatically coercing number 1.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Method navigation (declarations with bodies found in this file): [BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L13), [isTrue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L20), [isFalse](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L24), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L29), [getValueType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L34), [isEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java#L38).

## Check your understanding

Why does RETURN 1 fail when RETURN TRUE succeeds?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `9caab8c8b190c3673d9f66a2dfac42635c9f5825c4d8a09f79a85f6d4555bf84`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
