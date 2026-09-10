# Lab: confirmed, unconfirmed and sendable balances

Written before the new comparison tests. Read the pinned base/balance.java first.
A wallet balance is a report over coins, not a stored bank-account number. This
command searches unspent coins, groups them by token and classifies each amount.

For a tip at block 11 and a coin created at block 8, depth is 3. With confirmations
set to 3 this coin is confirmed; a coin created at 9 is unconfirmed. Sendable adds
a further requirement: a confirmed coin must have a simple wallet address, and a
non-Minima token must have the exact token script RETURN TRUE. The debug log's
SENDABLE label checks only the address, so it can describe an unconfirmed coin.

With no address parameter, search relevant coins. With a specific address, search
all matching coins and optionally MegaMMR, but only when the node enables it.
Token ID filtering is an exact string comparison; address normalization follows
the existing Command getter. Search order determines the order of token groups.
The Minima group is seeded even when there are no coins, unless tokenid excludes it.

Use actual Coin/Token objects and Java-generated wire inputs, the existing chain
fixture and Wallet cache loaded through recording SQL. Reuse the actual search,
number, token scaling and JSON parser implementations in both languages. Compare
replies, ordered logs, SQL calls, cache membership and unchanged transaction bytes.
Independently calculate selected totals, confirmation boundaries and token order.
Exercise duplicate/spent coins, relevance versus explicit-address search, MegaMMR,
simple versus complex wallet addresses, scaling, raw token ID filters, empty chain,
invalid parameter order and public dispatch (including debug, which the body reads
but the valid-parameter list excludes).

The source always stores token.name as a MiniString object. JSONValue's fallback
writes that object's toString without quotes. Thus a plain name can produce invalid
JSON, whereas a name containing a JSON object produces an embedded object. Capture
the actual serialized command reply as a string inside the fixture envelope; do
not repair invalid JSON or normalize token names. The simple view explicitly
parses the name as JSON, casts it to JSONObject and reads name. Invalid JSON,
non-object JSON and null therefore follow different exception paths.

These tests observe offline reports and supplied SQL rows. They do not establish
durable SQL, a running full node, live wallet balances or exhaustive reference and
concurrency equivalence.

## First discrepancy: arithmetic overflow text

Program 96 reached token scale 44. Java reported the overflowing amount as
60000000000000000000, whereas C++ reported 6.000000000000000000E+19. The shared
MiniNumber(BigInt, scale) constructor checked range before converting a negative
scale to the plain-decimal representation used by Java's BigDecimal constructor.
Its string constructor must retain scientific notation when the input uses it.

Before adding direct probes, read MiniNumber's constructors and JavaBigDecimalText.
Reuse the existing integer coefficient, power-of-ten and MathContext machinery.
Expand only the zero digits that survive the 64-digit precision; this preserves
both small plain-decimal errors and very large rounded scientific errors without
allocating an exponent-sized string. Compare the two constructor routes, positive
and negative overflow, retained fractional zeros and precision boundaries.

## Test review

The initial MegaMMR expectation incorrectly supplied address:"" for an omitted
address. The actual Command getter rejects a supplied blank String. Correct the
fixture to omit the key for the wallet-wide branch; keep the separate blank-value
rejection test. This is a test expectation correction, not a production change.

The shared SQL comparison normally removes runtime exception prose from strings.
For this group, restore serialized replies and coin debug logs from the raw
observations before comparison. A token name deliberately contains an exception
class name, so the generic regex must not erase part of that data. Direct escaped
exceptions still use the existing category-preserving normalization.

All 275 programs pass in both builds (11.57 seconds normal, 37.07 seconds
ASAN/UBSAN): 776 operations and 21,515 SQL calls. This includes 24 direct
number-constructor probes. Broader regression evidence is recorded below.

## Final regression evidence

Both full builds pass without warnings. Thirteen distinct selected checks pass
in each build across phases. Normal: balance 11.57s, eleven-check regression batch
60.64s, separate primitives 1.48s. ASAN/UBSAN: twelve-check batch including balance
193.38s, separate primitives 2.54s. All 24 comparison reports were inspected.

The other passing groups cover 247 consolidation programs; 675 wallet programs;
736 funding programs; 12,289 command cases; 1,265 runner cases; 1,120 runner-error
requests; 9,464 object cases; 3,802 primitive cases; 4,471 KISS VM cases; 189 search
programs containing 6,468 queries; and 99 original NumberValue assertions plus
11 supplemental arithmetic errors. Source mapping confirms 337 counterparts.
This selected set is not all 175 registered CTest entries and does not establish
full node parity.
