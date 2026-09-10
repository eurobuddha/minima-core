# Lab: wallet consolidation and delegation to send

Written before this test group. Read base/consolidate.java and send.orderCoins.
The source help says smallest first, but the executed ordering sorts descending
by amount and then groups coins by address. Preserve the executed algorithm and
the original help independently; do not silently rewrite either.

The command parses token ID, burn and Boolean flags, then fetches the tip and
checks minimum coinage. Search relevant, simple, unspent coins for this token;
keep sufficiently old coins and apply the existing address grouping. Fewer than
three eligible coins fails before parsing the signature and coin limits.

For each coin, compute its token amount before testing whether a new address
would exceed the signature limit. Add amount and coin count, then check the coin
limit. Thus nonpositive limits have different effects: no signatures can mean
zero selected coins, while a coin limit of zero still permits the first coin.
The command obtains a default wallet address even when the selected total is zero.

It forwards to send with split:2 and the computed total. Dryrun forces debug.
If password exists, log only password:#### before reading its value, then append
the quoted value to the child command. A successful child contributes response;
a failed child contributes message, else error, else the whole reply object.

Reuse the existing wallet/SQL and chain fixtures. Record a substituted send
prototype's parsed parameters and complete command through the real runner;
restore the prototype after the operation. Controlled replies isolate the
delegation logic. Also execute actual send against the existing proof fixtures.
Compare exact logs, SQL operations, parameter ordering, child text, responses
and stored transaction bytes. Independently assert grouping and limits, age
thresholds, password redaction and failed-reply precedence. Recording SQL is a
test boundary and does not establish durable storage or network acceptance.

For real dry-run responses, consolidate copies only send.response and drops the
outer dryrun marker. Apply the existing funding comparison's checked 32-byte
PRNG normalization at this known response location; compare all other fields.

The first run reached program 167 and exposed a shared runner discrepancy:
missing child status produced Java NullPointerException but native
bad_variant_access. CommandRunner checks status after its command-execution catch
block. Reuse CommandResult.status at that exact point, preserving null and wrong
type categories. Add direct recorded-send cases for missing, null, String and
numeric status, separate from the parent consolidation tests.

Review also checks public dispatch: a valid consolidation request reaches the
recorded send, a non-admin caller is rejected before forwarding, and a nested
missing status becomes the outer runner's logged error response. This distinguishes
direct command-body behavior from execution through the public runner.

## Observed results

All247 programs pass in both builds:742 operations and23,185 SQL calls, with
zero mismatches (17.06s normal/34.49s ASAN/UBSAN). Both full builds and all8 selected
checks pass in58.53s/191.87s. The other comparisons cover675 wallet programs,
736 funding programs,659 wallet-wrapper programs,12289 command cases,1265 runner
cases and1120 runner-error requests; source mapping confirms336 counterparts.
These are finite offline observations, not complete node or durable SQL parity.
