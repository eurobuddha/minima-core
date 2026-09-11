# MMR demonstration and printer validation

Added MMR.main, MegaMMR.main, MMR.printinfo and MMR.printmmrtree. Reused the
existing entry storage, integer bytes, sum/hash tree and pruning implementations.
No new mapped paths: 456 counterparts, one JNI path absent, full parity unverified.

Normal and ASAN/UBSAN focused tests pass: 30 Java/native comparisons and 112
assertions each. Complete stdout, including embedded NUL bytes, is retained in
results.json. Cases include both mains with null/empty/unused arguments, MegaMMR
pruning enabled/disabled, empty and multi-leaf printers, stars/negative values,
exact buffer boundary, out-of-range positions, fractional and wrapping indexes,
and rows requiring Java saturated casts and wrapping multiplication.

Printer calls also compare the serialized tree before and after. Helpful-null
exception prose is runtime-specific and is recorded but not equated; its class
and partial stdout are compared. MegaMMR preserves real random coin/address draws,
including discarded addresses. Random distributions and full API parity are not
certified. The six remaining main candidates are listed separately.

See the [student lab](../../student-guide/new-tests/mmr-main-methods.md).
