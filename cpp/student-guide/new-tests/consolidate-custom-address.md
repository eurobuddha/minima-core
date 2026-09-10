# Lab: consolidating coins from a custom address

Written before the consolidatefrom test group. Read the pinned Java class,
including its separate public createConsolidate helper.

The main command creates a temporary row before parsing mine or reading the
chain tip. Search all unspent coins for the requested address and token, sort
stably by descending Minima amount, and keep coins at least three blocks old
whose IDs are absent from the mempool. The mempool check runs only after age
passes. No valid coins raises an exception without deleting the temporary row.

The selection counter starts at one and checks its limit after adding a coin.
Predict the selected count for maxcoins -1, 0, 1, 2 and 50. A positive Minima burn
must be strictly less than the selected total; otherwise deletion precedes the
error. The main path creates ten equal outputs using fixed-precision division,
then adds script, proofs, signature and posting commands. It deletes the row and
returns posting.response without checking posting.status.

The public helper preserves supplied coin order, performs no chain search or
age/mempool filtering, applies positive burn regardless of token, and creates
one output. It always posts with mine:true. Its response is explicitly cast to
JSONObject after deletion; null and a wrongly typed response differ.

Reuse the existing wrapper fixture and real tree/coin/RAM database objects.
Supply chain coins and mempool witness coins as fixture inputs, then execute
the actual source and native wrappers. Record exact child text and row bytes.
Compare tied amounts, age boundaries, spent coins, token amounts, negative and
wrapped limits, every child failure position, malformed parameters, burn limits,
nullable helper inputs and return types. Independent assertions check selection
order, output count/amount, and cleanup. Only generated IDs and known runtime
exception prose may be normalized. Controlled children establish orchestration;
they do not establish network acceptance or complete native Main integration.

Review extension: supply 55 coins to distinguish the default 50-coin cutoff from
an unlimited loop, compare an explicit 51 limit, and show that the public helper
uses all 55. Divide a total of 10^-44 by ten and predict truncation to zero in
each output. These are separate from network validity. Keep the helper virtual,
matching Java's overridable public method.

## Verified results

The first 327 programs and 3,603 child calls passed both builds. After review,
the expanded 331 programs and 3,815 child calls also pass (12.81s normal,
22.38s ASAN/UBSAN). Both full builds pass without warnings. All eight selected
checks pass per build in 22.25s/65.92s: consolidation, custom-address wrappers,
previous wallet wrappers, raw wrappers, command getters, runner, runner errors
and source mapping. All fourteen comparison reports were inspected. The shared
groups retain 492, 659 and 656 programs respectively; command, runner and error
comparisons retain 12,289, 1,265 and 1,120 cases. This is not all 173 CTest entries
or full node parity. The fixture adds coins through real chain/MegaMMR/RAM APIs;
selected real input/output/script children expose the stored transaction bytes.
