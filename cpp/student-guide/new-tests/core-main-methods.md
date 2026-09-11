# Lab: four entry points and observable side effects

This lab adds the existing Java main methods to send, Contract, TxPoWTreeNode and
P2PFunctions. It reuses their native implementations. The [driver](../../tools/core_mains_differential.py)
compares the actual source entry points with their native counterparts under null,
empty and unused argument lists.

## Coin order

The send demonstration creates six coins. orderCoins first sorts amounts from
largest to smallest. It then groups by address in the order each address first
appears. The result is DD:10, DD:1, EE:2, EE:0.5, EE:0.2, FF:1. This is not a single
amount sort: grouping moves the one-unit DD coin before the two-unit EE coin.
The test compares all six full JSON objects and independently checks that order.
It does not submit coins or exercise a real wallet.

## A JSON string containing list text

TxPoWTreeNode main formats one, two and three as Java list text, then stores that
text in the transactions field of a JSONObject. The test requires a JSON string
with value `[one, two, three]`. Replacing it with a JSON array would change the
source output even though the data might look similar on screen.

## A real VM trace

Contract main cleans `LET g = 0xFF << -8`, constructs the real contract with tracing
on, and runs it. Before this change, native traceLog wrote directly to std::clog.
Java calls MinimaLogger, which has output and notification side effects. The port
now calls the same native logger used elsewhere. The fixture removes only the
logger's time/heap prefix, then compares every line and the notification count.
The internal accumulated trace remains unchanged. A script without RETURN does
not become successful just because it executes without a parse failure.

## Catching Exception is different from catching Error

P2PFunctions main first calls isNetAvailable, then prints its boolean. A Java URL
handler fixture records open, connect, input and close. The native test reuses the
existing URL callback with the same stage sequence. At each stage it can raise an
IO exception, AssertionError or OutOfMemoryError. Ordinary failures yield NET:false.
Errors escape before any line prints. C++ now explicitly rethrows JavaError before
its ordinary std::exception catch. The complete event sequence and output are checked.

## Results and limits

Normal and ASAN/UBSAN node builds passed. Each build passed 24 comparisons and 87
independent driver assertions. Raw observations are retained in
[validation](../../validation/core-mains-work-in-progress/).
Run `python3 cpp/tools/core_mains_differential.py --build cpp/build` to reproduce.
The sanitizer build uses the corresponding `--build cpp/build-sanitized` argument.

These tests cover the four main methods and the listed trace/error corrections.
They do not certify all VM APIs, all coin ownership behavior or real HTTPS provider
semantics. Existing broader regressions remain separate evidence. Twelve further
entry-point candidates and the unavailable native JNI implementation remain open.
