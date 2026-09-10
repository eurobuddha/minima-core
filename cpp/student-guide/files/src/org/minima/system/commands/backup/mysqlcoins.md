# mysqlcoins — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mysqlcoins.java) · `src/org/minima/system/commands/backup/mysqlcoins.cpp` (absent)

## Purpose

mysqlcoins derives a searchable coin table from archived TxBlocks and exposes metadata, settings and SQL-based searches.

## Follow the command

1. Reject mobile mode. Use saved MySQL credentials whenever autologin is enabled, ignoring supplied overrides; otherwise require all four connection parameters. Parse logs/maxblocks/maxcoins/readonly and initialize MySQL before dispatching any action.
2. info reports total coins, maximum coin block and automatic-backup flag. wipe calls wipeCoinsDB and stores its response under the source spelling respnse. autobackup changes its flag and either stores or clears shared MySQL credentials; this body does not explicitly save UserDB.
3. update starts at the oldest MySQL archive block, or maximum coin block+1 when coin rows already exist. Fetch block ranges. Increment/check maxblocks before processing each block, then insert all input coins as spent with the spending block and all outputs as unspent with creation block set to that block.
4. Count insertion attempts and check maxcoins only after the whole block, using greater-than rather than greater-or-equal. Consequently a batch can exceed the requested coin count and equality permits another block. Advance to last processed block+1 and return counters/duration.
5. search precedence is where, then address, then query. where is appended to SELECT * FROM coins WHERE; address builds address equality OR state substring matching, optional spent filter, ascending creation order and optional limit. query is passed as the complete SQL text. Forward hidetoken to searchCoins.
6. Call mysql.shutdown only after a normally completed action. Exceptions or invalid actions bypass this final call because there is no finally.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the pinned MySQLConnect schema, insertCoin and query conversion. The helper’s readonly flag does not itself make every operation read-only; SQL behavior and database permissions need direct integration tests.

## Understanding the tests

The runner fixture checks mysqlcoins’s classification only. Proposed fixtures record exact credential choice and SQL text, where/address/query precedence, state substring matching, spent filtering and hidetoken. Update tests check input-object mutation, output creation height, maxblocks zero and maxcoins equality/overshoot, retry/resume positions and shutdown after injected failures.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No database was contacted. coinsadded counts calls rather than proving successful distinct stored rows. A restart from maximum recorded coin block depends on how the connector represents partial updates and blocks containing no coins.

## Check your understanding

Why can maxcoins:10 produce more than ten inserted coin records in one invocation?

Pinned Java SHA-256: `0af8db811a7fe9e852ed5e0407c1bfedbc1113a5abde21fa1ca9b8bd97689a1f`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
