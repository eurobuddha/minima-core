# MiniData — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/base/MiniData.java](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java) | [src/org/minima/objects/base/MiniData.cpp](../../../../../../../src/org/minima/objects/base/MiniData.cpp) |

## What this file does

A byte sequence that also supports unsigned numeric interpretation. Its byte identity and its numerical ordering are deliberately different concepts.

## Follow the logic

1. Decode hexadecimal through BaseConverter. A byte-array constructor stores the supplied Java array reference; the minimum-length constructor pads on the left with zero bytes only when needed.
2. The nonnegative BigInteger constructor removes a redundant positive sign byte. Numeric zero is represented by one zero byte, whereas the default constructor is an empty sequence.
3. isEqual requires identical lengths and bytes. Ordering converts to a positive BigInteger, so 0x01 and 0x0001 have equal numeric magnitude despite unequal byte identity.
4. Java lazily caches that BigInteger. getBytes exposes the original mutable array, so later byte mutation can disagree with an already cached numeric view. This is an ownership behavior, not just a performance detail.
5. Shifts operate on the positive numeric magnitude and rebuild bytes; they can discard leading zero padding. Concatenation instead preserves the exact left and right sequences.
6. Normal wire format is a four-byte signed length followed by bytes. The reader enforces the 512 MiB maximum and optional exact expected length. A negative ordinary length reaches Java array allocation; hash reading explicitly rejects negative lengths and limits length to 64.
7. The Streamable convenience converter serializes the object and returns null after logging IOException. Random data uses SecureRandom; the demonstration main prints random and padded values and is not a unit test.

## How the translation is approached

C++ owns a vector and exposes const byte access, so it does not reproduce Java’s externally mutable array alias API. It now retains the lazy BigInteger cache across in-place reads; comparison and shifts use that numeric view. OS randomness comes from /dev/urandom on the current POSIX target. Native SHA-256 and SHA3-256 helpers are extra implementation support; they are not methods of this Java class.

## What the tests tell us

The primitive fixture compares empty, zero, padded, odd-length and invalid hex inputs, plus numeric less-than cases such as 0x01 versus 0x0001. It establishes selected parsing, formatting and normal-wire behavior. It does not directly test Java mutable-array aliasing, all shift distances, random quality, the convenience converter, or the 512 MiB allocation boundary.

The [primitive differential driver](../../../../../../../tools/differential.py) compiles the pinned Java classes and runs [PrimitiveOracle](../../../../../../../support/PrimitiveOracle.java) against [minima_primitives](../../../../../../../support/primitives.cpp). Both report observable text and wire bytes; both flatten exceptions to `ERROR`. The inspected [existing report](../../../../../../../build/differential-results.json) records 3,802/3,802 comparisons with zero failures for the complete primitive corpus. This does not prove exact error classes, all methods, or full-node parity.

## Dependencies and reading map

Direct Minima imports: [BaseConverter](../../../../../../../../upstream-study/src/org/minima/utils/BaseConverter.java), [MiniFormat](../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L49), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L58), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L67), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L74), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L103), [getLength](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L120), [getBytes](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L124), [getDataValue](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L128), [getDataValueDecimal](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L136), [equals](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L141), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L145), [isLess](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L164), [isLessEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L168), [isMore](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L172), [isMoreEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L176), [shiftr](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L180), [shiftl](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L184), [compare](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L188), [concat](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L192), [toString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L209), [to0xString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L213), [to0xString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L217), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L227), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L233), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L244), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L262), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L273), [writeHashToStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L282), [readHashFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L291), [ReadHashFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L303), [WriteToStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L309), [getMiniDataVersion](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L313), [getRandomData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L338), [main](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java#L345).

The [transaction-check lab](../../../../../../../student-guide/new-tests/transaction-check.md) exercises negative hash rates through the generator and this constructor. It exposed an error-message difference; the native constructor now retains the pinned message exactly, including its spelling. The nonnegative conversion algorithm is unchanged.

## Runtime integration update

The pulse runtime corpus exposed an oversized-data diagnostic mismatch. The native reader now uses the pinned text plus the existing MiniFormat.formatSize; it retains the source message’s stale "256 MB" wording while applying the 512 MB limit. The expected-size mismatch message also follows the inspected Java branch. The 152-program pulse corpus passes in both builds; 35 additional direct-reader cases verify expected-size diagnostics, maximum-check precedence, negative lengths and remaining-byte counts. Locale-dependent formatting and mutable limits still need further coverage. See [the runtime lab](../../../../../../new-tests/main-runtime.md).

## Check your understanding

Can two MiniData objects compare neither less nor greater yet fail isEqual? Give a concrete byte example, then explain how shifting by zero can change its byte representation.

## Limits and traceability

Several Java convenience APIs, including getDataValueDecimal, compare, shortened to0xString and the Streamable converter, are not represented as matching methods in the inspected native header. Equality against an arbitrary Java Object also has cast/null behavior absent from the typed C++ interface. The source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `0ee71d3e8e111cf11bf978ef0036915590ba85505100adfe0725546018079987`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).


## Streaming restore update

The [restore lab](../../../../../../../student-guide/new-tests/restore.md) adds
readFromStream with a reader callback. Both the existing span decoder and this
reader call the same checkedDataLength helper, preserving maximum/expected-size/
negative-length precedence. The callback path loops until each requested byte is
read, treating early EOF as an EOFException. It never reads ahead into the next
section. That property lets restore expose the same partial state as Java.
Negative and oversized frame lengths and truncations at every restore section
exercise this path. Mutable Java byte-array aliases and convenience APIs listed
above remain separate parity gaps.

## Scalar Streamable integration

The native class now implements Streamable. It validates the source length checks,
allocates zero-filled replacement storage, copies available bytes, and throws EOF
if the declared payload remains incomplete. Reads keep the old allocation alive
until they finish, so input may safely refer to the object's own prior bytes.
The source's lazy numeric cache survives byte replacement. Native comparisons and
shifts now use that cache; atomic cache access and explicit copy/move operations
preserve safe read-only sharing. The byte vector still does not expose Java's full
mutable-array reference API. Static decoders retain their existing bounded path;
allocation-failure timing and arbitrary stream backends remain open.

See the [scalar interface lab](../../../../../../new-tests/streamable-scalars.md) for the test logic. Both expanded interface suites pass 2,095 comparisons and 12,220 assertions across 14 types with matching transcripts and empty native stderr. All nine affected regressions pass in both builds. Full class parity remains unverified.


## Separate hash-writer update

The [hash-writer lab](../../../../../../new-tests/hash-writers.md) explains distinct ordinary/hash dispatch, mutable
length limits, partial output and receiver lifetime during field replacement.
Both builds pass 1,848 comparisons and 12,752 independent assertions for the
represented paths. This update does not establish full field identity or class parity.


## TxHeader reference and stream update

The [TxHeader lab](../../../../../../new-tests/txheader.md) explains the shared default objects, retained field and
array identities, run-length partial writes, and interrupted child factories.
Its evidence is scoped to these behaviors; full class and node parity remain open.

## Original base-test checkpoint

The original base-test conversion adds `compare`, `isLessEqual`, `isMoreEqual`, an unbounded decimal view and `to0xString(length)`. Numeric methods retain the existing lazy cache across in-place stream replacement; byte equality still examines the current bytes. The decimal view reuses the existing MMREntryNumber carrier at scale zero, avoiding MiniNumber's monetary range limit. Bounded hex always appends two dots, including when no truncation is needed, and negative lengths raise the Java substring exception.

The extra corpus exposed raw byte differences leaking through BigInt comparison. That shared primitive now returns exactly -1, 0 or 1. Both builds pass 1,108 API comparisons and 18,632 independent checks, including leading zero bytes, large magnitudes, warmed caches, retained decimal snapshots and negative substring bounds. Complete mutable-byte alias/JDK API behavior remains outside this result.

See the [base-test lab](../../../../../../new-tests/original-base-tests.md) for the retained initial failure and complete scope.

## Borrowed byte access for JNI callers

The native getBytes accessor exposes a fixed-size mutable span over the owned bytes.
Like Java, writes deliberately leave the lazy numeric cache unchanged. The owner
must remain alive; stream replacement expires the view. [JNI caller lab](../../../../../../new-tests/jni-command-callers.md)
explains twelve mutation cases that distinguish cached comparisons from current
hexadecimal display. This API addition does not certify general retained-array or
concurrent mutation parity.

## Follow-up: source entry point

The missing main now generates 10 bytes and shows the existing left-padding operation to 16 bytes. Tests compare fixed-byte output and integer invariance, then check real random output. Negative random sizes now throw the source NegativeArraySizeException with the original length.

See the [random entry-point lab](../../../../../../new-tests/object-main-methods.md) for the calculation, fixture boundaries,
and coverage limits. Full class parity remains unverified.
