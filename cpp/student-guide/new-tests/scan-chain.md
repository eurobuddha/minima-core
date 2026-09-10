# Lab: scan the chain with the source's inclusive counters

Written before the new test. The pinned `scanchain` command is a useful lesson
in reading loop conditions literally. With a tip at height 20, offset zero and
depth zero, the returned block is height 19. Offset zero advances once, and depth
zero visits once. Negative offset advances zero times; negative depth visits no
blocks. Parse offset before depth, after checking that a tip exists.

Reuse the real Command parser, MinimaDB tree, TxPoWDB lookup, transaction JSON
and Java-compatible date formatter. The test reuses the wallet command fixture
and Java-generated TxPoW wires. It must inspect returned heights, numeric JSON
types, relative height differences, exact dates and ordered transaction details.
Include gaps and fractional heights: relative depth is height subtraction, not
the loop index. The two counters use Java's signed 32-bit increment semantics.

The block's own transaction is listed first when either ordinary or burn inputs
or outputs make it a transaction. Referenced TxPoWs follow in list order. Missing
references are omitted; repeated references remain repeated. A found empty TxPoW
still produces details with both transaction flags false. No archive fallback
or wallet relevance filter belongs here.

Exercise RAM hits and recording-SQL hits, misses, malformed returned bytes and
injected SQL failures. Check that negative depth or an exhausted offset performs
no transaction lookups. Test the public detail helper with empty, ordinary,
burn-only, combined, bodyless and null objects. Compare exact reply text and SQL
events; normalize only the existing platform-specific NPE/CCE diagnostics.

This experiment does not establish durable database storage or atomic traversal
under concurrent chain replacement. Keep these limits separate from observed
command behavior and from the full 457-file port goal.

## Fixture findings and observed results

Before the command comparison could start, Java rejected two test inputs. A
string state must use `[text]`; bare text enters the numeric parser. A timestamp
of 2^64 + 1 exceeds MiniNumber's permitted magnitude. The corrected fixture uses
2^64 - 1, which is valid as a MiniNumber and becomes -1 when read as a signed long
for Date formatting. The response retains the original numeric timestamp.
These are fixture corrections, with the initial failures retained; no production
logic was changed to make those inputs acceptable.

Both builds pass 320 programs / 2,027 operations (23.57 seconds normal and
67.47 seconds with ASAN/UBSAN). The test independently checks inclusive bounds,
JSON number types, height differences, dates, transaction order and field
presence, in addition to comparing the Java and native replies and effects.
See [saved evidence](../../validation/scan-chain-work-in-progress/progress.json).
Existing regressions also pass in both builds: history covers 484 programs /
1,489 operations, TxPoW search covers 680 programs / 2,099 operations, and the
command runner covers 1,265 cases. The fixture extension accepts an optional
source timestamp while keeping the old default zero for existing groups.
