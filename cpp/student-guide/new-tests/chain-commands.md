# Chain inspection commands — wrappers still have contracts

This lab ports `block`, `printmmr` and `printtree`. Their bodies are short because existing database and rendering classes do most of the work. A faithful wrapper still has to preserve lookup order, errors, JSON types and whitespace.

Read the [block lesson](../files/src/org/minima/system/commands/base/block.md), [printmmr lesson](../files/src/org/minima/system/commands/base/printmmr.md), [printtree lesson](../files/src/org/minima/system/commands/base/printtree.md), [Java experiment](../../support/MinimaDBOracle.java), [native experiment](../../support/minima_db.cpp) and [comparison driver](../../tools/minima_db_differential.py).

## Reuse the implementations that own the behavior

The new commands reuse the current native MinimaDB singleton, TxPowTree, Cascade and MMR implementations. `block` uses the existing Java date helper; `printtree` uses the existing Java integer parser. Their help strings and factory behavior come from the pinned Java commands. No separate tree, MMR serializer or date formatter is introduced.

The sibling tree and cascade implementations under `/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/chain/` were inspected. Their weight representation and diagnostic formatting differ from this pin. The current native renderers already have source differential tests, so they are the compatible building blocks.

## Begin with an empty chain

| Command | Source behavior with an initialized database and no tip |
|---|---|
| block | Throws CommandException with `NO Blocks yet..`. |
| printmmr | Attempts to use the absent tip and throws a null-pointer exception. |
| printtree | Receives an empty string from the tree renderer and returns `"chain":"\n"`. |

These differences are deliberate observations of the source. Giving all three commands one common friendly error would change their contract. Through CommandRunner, a CommandException becomes a failed reply; a generic exception also passes through the actual logger. The harness captures those logger bodies while excluding dates, memory prefixes, stack frames and detailed Java null-expression text.

## Follow each successful path

`block` reads the current database, its tree and its selected tip. It does not choose a heavier branch or search an archive. Its response contains block number, hash, millisecond timestamp and a human-readable date. Block and millisecond fields are **strings**, even when they contain only digits. Date formatting converts the MiniNumber to a signed Java long first; the printed date also depends on the runtime's default time zone.

`printmmr` follows the same database/tree/tip path, obtains that node's MMR, then returns its existing JSON representation. The MMR determines entry ordering, maxima, root and numeric field types. The wrapper must not sort the entries or infer a list of all historical coins. The fixtures add synthetic MMR records to test representation; they do not establish those records as authenticated transaction outputs.

`printtree` parses a trimmed depth string as a Java integer, with default 32. It compares the cascade parameter exactly with `"true"`; `"TRUE"` does not enable it. When enabled, cascade rendering happens before tree rendering. Both strings receive an additional leading newline, and the cascade key is absent when disabled. Fractional text is rejected by integer parsing, even though other commands accept MiniNumber values.

## Test the wrapper and its caller separately

Direct command calls populate the parameter object and invoke `runCommand()`. Runner calls additionally perform parameter validation and caller checks. `block` and `printmmr` inherit empty valid-parameter lists: an unused parameter can be ignored by a direct call but rejected before execution through the runner. `printtree` explicitly permits depth and cascade.

Metadata comparisons include exact help, full help, valid parameters and a factory-created reply. Before asking for that fresh reply, the fixture adds a marker parameter to the original command. The marker must not leak into the new instance.

## Make state selection observable

The fixture uses the actual MinimaDB and child classes. It creates linear trees and forks, then calls the existing tree recalculation to choose a tip. A second experiment retains an old populated database, replaces the global singleton with an empty database, and continues observing the old owner. Commands must still read the **new global database**, because their Java bodies call MinimaDB.getDB(). Retaining a convenient fixture pointer would return the wrong chain.

Surrounding snapshots compare owner identities, locks, file bytes, SQL calls and recorded Main effects. This helps detect unintended writes by inspection commands. The SQL driver records requests; it is not a native H2 engine. The Java Main receiver is allocated for recording external effects without starting its constructor or workers. Thus the experiment exercises real command/database code, but it is not a full node startup test.

## Corpus and baseline

The base corpus has **78 programs and 1,002 operations**. It includes metadata; empty, single-node, linear and forked trees; bounded negative/default/positive depths; exact cascade flags; direct and runner failures; populated MMRs with 0, 1, 2, 3, 7, 16 and 33 records; timestamp boundaries; a replaced singleton; and a null cascade owner.

The original fail-fast baseline stops at program zero: Java returns the three commands' metadata while the native registry has no corresponding objects. The retained report is `cpp/build/chain-commands-baseline-results.json`. This is one failed baseline program, not a claim that all 78 were executed against the missing commands.

The initial UTC-only run passes all 78 programs in both builds: **4.16 seconds normal**, **6.92 seconds with sanitizers**. The first four-zone run stopped at London program two: cascade output used `BST` for January 1970, where the actual Java output uses `GMT` at the same local hour. Its report is retained as `cpp/build/chain-commands-zones-baseline-results.json`. A focused call showed that the existing shared Java date helper was already correct. The older independent `strftime` formatter inside TxHeader caused the discrepancy; TxHeader now reuses the shared helper unchanged.

The expanded corpus also adds fractional timestamps and checks tree/header rendering after changing each timestamp. It now has **81 programs per zone**, repeated in UTC, Europe/London, America/New_York and Asia/Tokyo: **324 planned programs and 4,096 operations**. Both actual runtimes receive the same explicit TZ environment. The expanded corpus passes all **324 programs and 4,096 operations** in both builds, with zero differences: normal CTest **9.69 seconds**, sanitizer CTest **20.22 seconds**. The nine normal affected regressions also pass in **50.23 seconds**. The same nine sanitizer regressions pass in **177.75 seconds**. Together with the new command comparison, all **ten selected checks** pass in each build. Both final full builds complete without compiler warning/error lines. The original database suite still passes 2,367 programs, 7,519 operations and 334,711 recorded SQL calls per runtime; all fourteen regression comparison reports were inspected with zero differences. This is a selected subset of 142 registered tests.

```sh
ctest --test-dir cpp/build -R '^chain_commands_differential$' --output-on-failure
ctest --test-dir cpp/build-sanitized -R '^chain_commands_differential$' --output-on-failure
```

Reports use `chain-commands-results.json` in each build directory. The comparison retains literal strings and JSON types; it does not replace arbitrary errors with one generic success marker. Enhanced null/class-cast diagnostic expressions are the stated exception-text normalization.

## A boundary found during source review

TxPowTree's Java loop computes `depth - 1` using signed 32-bit arithmetic. At -2,147,483,648 this wraps to +2,147,483,647. The native code formerly widened before subtraction, producing a negative bound and skipping the loop. The renderer now reproduces Java's arithmetic using an unsigned subtraction and signed bit interpretation, while retaining its traversal algorithm.

A populated Java tree can perform billions of parent checks for this extreme input, including redundant checks after reaching its root. The command tests use extreme valid depths only with an empty tree, where both implementations return before that loop. They validate parsing and empty-tree behavior, not a completed billion-iteration traversal or concurrent changes to parent links. Ordinary populated-tree depths are compared directly.

## Check your understanding

1. Why is `"block":"3"` different from `"block":3`? **Answer:** one is a JSON string, the other a number; clients can distinguish their types.
2. Why not retain the old database pointer in the command? **Answer:** the source resolves the current singleton at execution time.
3. Why compare whitespace literally? **Answer:** these commands return display text as data. Leading newlines and indentation are observable output.
4. Does a printed MMR root prove a coin is spendable? **Answer:** no. Printing state does not validate a transaction or authenticate a proof against consensus.

Complete node startup, native H2/MVStore persistence, arbitrary concurrent mutation and full per-file parity remain unfinished. Mapping status remains partial.
