# Reuse and review

Inspected pinned source main bodies for send, Contract, TxPoWTreeNode and
P2PFunctions. Read send.orderCoins source/native, Coin construction and JSON,
Contract native constructor/run/cleanScript/traceLog with corresponding source
traceLog, P2PFunctions source/native availability and complete callback fixture,
JSONObject interface and existing utility/object-main fixtures/drivers.
Existing wallet_commands fixtures cover orderCoins and mutation. Existing KISS
and P2P suites cover the surrounding production paths.

Sibling search at /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src found
an older Contract wrapper, but no matching source entry points, orderCoins or
isNetAvailable implementation. Current runtime provides the needed source-compatible
helpers and fixtures, so it is reused without replacing the VM or network stack.

Reviewed the added entry points, logger/error changes, both fixture bodies and
complete driver. The source main output is compared exactly after removing only
logger runtime prefix. The URL callback is an existing transport boundary, not a
substitute for production availability logic. Error cases also assert no partial
NET prefix. Contract notifications are checked against every source log line.
No unresolved blocking issue in these changed paths after both comparisons pass.
Full class/runtime parity remains unverified.
