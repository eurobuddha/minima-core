# Next source work: TxHeader

The complete pinned TxHeader.java and native TxHeader.hpp/.cpp were read, as were
sibling tools/minima-core-cpp/src/objects/TxHeader.hpp/.cpp. Finish the full relevant
caller and test review before editing. Reuse the current bounds-checked scalar
codecs, date formatter and Magic integration; sibling code has unchecked pointer
reads, different empty/hash defaults and a fixed 32-element array. It silently
clips oversized RLE runs instead of reproducing Java's array writes/errors.

Keep the earlier source notes in
../streamable-txbody-work-in-progress/next-header-magic-notes.md. Further details:

- Distinguish GlobalParams.MINIMA_CASCADE_LEVELS from the actual retained array's
  length. The while condition uses the global limit, but the inner loop writes
  the entire run into the actual array. An oversized run can succeed when a
  caller supplied a sufficiently long array, including slots beyond the global
  limit. A short array receives all in-bounds assignments before failure.
- Source initializes separate empty MiniData objects for each superparent slot.
  After a read, every slot in one decoded run references the SAME decoded object.
- Non-final MAIN_NET and TEST_NET references can be replaced. Existing headers
  retain their original chain object; mutating that object differs from replacing
  the static reference. Other default aliases involve Crypto.MAX_HASH,
  MiniNumber.ZERO and MiniData.ZERO_TXPOWID, whose global identity work is unfinished.
- JSON/serialization treat a null previous run marker specially. Leading null
  elements can increment the count while leaving that marker null; do not invent
  a blanket prevalidation that changes the eventual source behavior.
- Magic.ReadFromStream finishes its entire factory before mMagic is replaced.
  A truncated nested Magic must not replace the retained old Magic in the header.
- Conversion constructs its ByteArrayInputStream before the try block and catches
  only IOException. Runtime/null exceptions must escape; mapped IO logs and
  returns null. Reuse the current typed exception/logging helper after inspection.
- Before subclass hash-writer tests, compare MiniData.writeHashToStream against
  native serialiseHash: the latter currently returns serialise(), whose inherited
  Streamable bridge can dispatch a writeDataStream override. Audit this separately
  rather than assuming ordinary and hash writer override behavior is identical.

Magic's own field operations are now owned; UserDB/ConsensusServices still return
MiniNumber values at their native convenience boundaries. Complete propagation of
source reference identity through those services and TxHeader/TxPoW is not proved
by direct Magic tests. Keep these obligations within the full port objective.
