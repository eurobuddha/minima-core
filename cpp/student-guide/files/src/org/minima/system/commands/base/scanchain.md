# scanchain — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/scanchain.java) · `src/org/minima/system/commands/base/scanchain.cpp` (absent)

## Purpose

scanchain walks recent parent blocks and returns transaction details. Its inclusive counters make the actual defaults different from a simple take-depth interpretation.

## Follow the command

1. Require a tip, parse offset default zero and depth default sixteen as integers, then move to the parent while counter <= offset. Default offset zero therefore skips the current tip; a negative offset skips nothing.
2. If offset exhausts the chain, return the requested depth/offset and an empty blocks array.
3. Remember the selected start height and visit nodes while counter <= depth, producing up to depth+1 blocks. Each entry contains its height, relative depth, timestamp, Date string and TxPoW ID.
4. Include the block TxPoW itself when it is a transaction. Then look up every referenced transaction ID and include each found object; missing IDs are skipped, and this branch has no extra isTransaction filter or deduplication.
5. For each object report whether its ordinary and burn transactions are nonempty, adding each transaction JSON only when its corresponding flag is true. Return the assembled blocks.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse parent traversal, TxPoWDB lookup and transaction JSON. Preserve MiniNumber-valued fields and date formatting; replacing inclusive loops with conventional exclusive limits changes output.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed cases use offset 0 and -1, depth 0 and -1, offsets beyond the root, missing/duplicate referenced IDs and a block that is itself a transaction. Assert exact returned heights and transaction field presence; control timezone when comparing Date text.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

This body does not lock the chain into one snapshot or fall back to the archive. A requested depth is not the number of returned blocks.

## Check your understanding

With tip height 20, offset 0 and depth 0, which one block should appear?

Pinned Java SHA-256: `23fd87d24f3cfcf3c54435a679d5128511abdaeeba099db0a279c21ceef5854a`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
