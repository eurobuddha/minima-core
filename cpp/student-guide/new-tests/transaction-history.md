# Lab: history queries and per-token transaction changes

Written before the new tests. Read search/history.java and TxPoWSqlDB first.
The command offers four actions. size counts relevant rows, or rows newer than a
time when relevant is false. customsize passes a required condition to the existing
SQL method. transactions counts transaction-bearing blocks and listed transaction
IDs while walking the chain. list loads paginated TxPoWs and computes details.

For list, parse max and offset as MiniNumber and then Java int. Read actual SQL
rows through the existing recording driver and actual TxPoW decoder. For each row,
serialize the TxPoW before invoking the public getTxnDetails helper. This order
matters when a malformed row or helper error interrupts the result.

getTxnDetails examines only the main transaction's inputs and outputs. Include a
coin only when its address is tracked by the wallet. State relevance, burn inputs
and simple-address classification are not additional inclusion rules. Add Minima
raw amounts; for another token call Coin.getTokenAmount, which returns the raw
amount if metadata is missing. Accumulate separate totals and report output minus
input for every encountered token. Keep the original Hashtable enumeration and
HashSet traversal order by reusing the native Java collection adapters.

Reuse the existing wallet/SQL and source-generated wire fixtures. Capture exact
serialized replies to make object-key order observable. Independently calculate
input/output/difference amounts, token order cases and chain transaction counts.
Exercise tracked versus untracked addresses, shared token IDs, negative amounts,
missing metadata, null helper inputs, pagination, invalid actions, nullable rows,
SQL/runtime failure positions and the overridable public helper.

For default startmilli, reuse the SQL comparison's clock-window method: record real
clock bounds around the operation, require the bound SQL timestamp to be one day
earlier, and require any returned startmilli to equal that parameter. Only then
normalize this independently varying time. Explicit times compare exactly,
including wrapped long values. Other replies, SQL calls, logs and stored bytes
remain compared except the established runtime-specific exception prose.

Recording JDBC/native driver rows are a test boundary. They do not establish
native durable SQL, a live history database, full Main startup or exhaustive
Java reference, concurrency and platform equivalence.

Review extension, written before its test: Java's public helper receives a mutable
TxPoW. Use a mutable native pointer as well. A fixture override changes its block
height to 99 and records the changed object, while the command reply must still
contain the original height 20 because toJSON runs before the helper. Reuse the
actual setBlockNumber API already used to generate the source wire. This checks
one concrete mutation and serialization-order boundary; it does not establish
all retained JSONObject alias behavior.

Final evidence: both full builds pass without warnings. The 484 history programs,
1,489 operations and 52,194 SQL observations match Java in normal and ASAN/UBSAN
builds (5.46s/10.04s). The earlier 483-program suite passed before the mutable
helper review extension. Initial fixture generation errors (an incorrect Java
method name and absent optional arrays) were corrected in test support; they
were not native production discrepancies.

Nine additional selected checks pass in each build across validation phases:
wallet commands (675 programs), balance (275), coins (287), command getters
(12,289 cases), runner (1,265), runner errors (1,120), HashMap (135), TxPoWSqlDB
(2,592 programs; 8,729 operations; 86,333 SQL calls), and the 339-counterpart map.
The normal nine-check batch passed in 158.81s. The sanitizer batch passed eight
checks in 160.85s but timed out the SQL check at its existing 120-second limit;
a separate run of that same full SQL driver then passed. All 18 final comparison
reports were inspected. These are selected checks, not all 177 CTest entries.

The review corrected the C++ helper's const argument to a mutable pointer, as
Java permits an override to mutate the passed TxPoW. All history tests were rerun
after the final builds. The other regression groups were unchanged by that
signature and history-only fixture adjustment. Native durable SQL, Main startup
and complete node integration remain unfinished.
