# mysqlcoins — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mysqlcoins.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/backup/mysqlcoins.cpp)

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

The native class reuses Command, MySQLConnect, UserDB, actual TxBlock/Coin codecs
and MinimaLogger's date formatter. It preserves every action, parameter order,
reply key and explicit shutdown point. Integer increments use Java-width wrapping
without C++ signed-overflow undefined behavior. The optional native memory collection
hint corresponds to the source's periodic System.gc request. The helper's readonly
flag does not itself enforce database permissions.

## Understanding the tests

The actual Java command and native class are now compared through the existing
recording SQL driver and actual MySQLConnect bodies. Both complete builds pass.
Both suites pass 771 cases, 1,007 independent assertions and 24,372 SQL events,
with identical transcript hashes (normal14.523s; sanitizer25.142s). Native stderr
is empty. The tests cover metadata, parameter order/errors, mobile rejection,
auto-login choice, preference updates, all five actions, exact SQL and bindings,
and SQL/runtime failures injected at every event in each baseline action.

Update fixtures deserialize real synthetic block bytes and observe input-spent
and output-creation changes through the actual insertCoin bindings. They preserve
whole-block limits, equality/overshoot behavior and insertion-attempt counts.
Search tests exercise where/address/query precedence, spent/limit and hidetoken.
Durations are checked against each operation's clock window before normalization.

The first normal run found a fixture exception-labeling error. The existing
CommandException adapter from the SPHINCS command fixture fixed it; the failed
report is retained. Production exception behavior did not change.

- [Command test lab](../../../../../../../../student-guide/new-tests/mysql-coins-command.md)
- [Comparison driver](../../../../../../../../tools/mysql_coins_differential.py)
- [Saved evidence](../../../../../../../../validation/mysql-coins-work-in-progress/progress.json)
- [Shared command implementation](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

## Limits

No live database was contacted. The runtime MySQL provider is still missing; this command checkpoint does not claim a working live MySQL backend. Synthetic SQL ranges are bounded using maxblocks and do not prove durable pagination, concurrent updates, or the greater-than-five-second progress-log timing. coinsadded counts calls rather than proving successful distinct stored rows. A restart from maximum recorded coin block depends on how the connector represents partial updates and blocks containing no coins.

## Check your understanding

Why can maxcoins:10 produce more than ten inserted coin records in one invocation?

Pinned Java SHA-256: `0af8db811a7fe9e852ed5e0407c1bfedbc1113a5abde21fa1ca9b8bd97689a1f`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
