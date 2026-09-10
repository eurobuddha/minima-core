# Lab: selective logging flags and ordered partial changes

Written before the new logs tests.

logs processes scripts, mining, maxima, blocks, networking, ibd, peerschecker and
finally txpowdb in that fixed order. Omitted options preserve the prior setting.
Present values use Command.getParam: trim Java whitespace, reject blank/null/wrong
types, and enable only the exact string true. Other nonblank strings disable the
setting. A later parsing failure does not undo earlier assignments.

txpowdb writes both LOG_ADD_BLOCKS and LOG_ADD_TRANSACTION, in that order. The reply
reports only LOG_ADD_BLOCKS. Seed different values for those flags to prove that
omitting txpowdb leaves both alone and setting it changes both. Read actual state
after failures; the response alone cannot expose all effects.

Worker threads read several of the seven GeneralParams logging flags. Use
std::atomic<bool> for all seven command-controlled fields, following the existing TxPoWSqlDB pattern. Per-field atomic loads
and stores avoid undefined C++ data races. This does not make the whole command a
transaction: another observer can still see a partially applied group of changes.
The Java source uses plain booleans; exact racy Java scheduling is not certified.
Fixture snapshots need load() to copy values instead of noncopyable atomic objects.

Reuse the existing Main driver and real global owners. Cross all options with
missing, null, wrong-type, blank, padded, ordinary and Unicode values under mixed
initial states. Independently calculate the changed prefix and untouched suffix.
Compare metadata, fresh factories, runner permissions and invalid-parameter gates.
Invoke actual Main.setSyncIBD after setting ibd to observe that a real consumer
reads the flag. No public networking, wallet secrets or persistence is involved.

Run affected parameter, processor, miner, checker, SQL and Main/command tests after
the storage adaptation. Those tests provide consumer evidence; a passing flag
snapshot alone does not establish every subsystem's logging or full-node startup.

## Command-specific result

Both full builds pass without observed compiler warnings. The 327 programs and
982 operations match Java in normal and ASAN/UBSAN builds (4.59s / 7.91s). The
[corpus](../../tools/logs_commands_corpus.py) independently checks all nine flags,
including partial changes and the hidden SQL transaction setting. All 25 affected
consumer regressions pass in both builds (194.27s normal, 457.64s ASAN/UBSAN). All
42 result reports were read without failures and are [saved with the checkpoint](../../validation/checkpoint-361/checkpoint.json).
The parameter suite again passes 719 scenarios and its original source test;
miner/processor workers and existing checker, NIO, SQL and command suites pass.
