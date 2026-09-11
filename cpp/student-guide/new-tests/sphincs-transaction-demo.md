# Porting the transaction demonstration faithfully

TransactionTest is in the production source tree despite its name. Its public
formatter uses AMOUNT and TOKENID separators. Its main signs a transaction with
seven inputs and two outputs, writes all sixteen trees' signature state, then
runs a script containing a fixed public key and a one-iteration proof loop.
The fixed key differs from the key derived by the current pinned signer. A faithful
translation preserves the demonstration's actual result, including rejection.

We reuse the tested signer, transaction, token, witness and KISS VM. The output
formatter and state installation are extracted from the current SPHINCSUtils
implementation. Separator parameters preserve the two different formats. State
installation does not recompute output IDs; only SPHINCSUtils performs that step.

Before execution, the test plan is to compare:

- The public formatter across existing coin fixtures, scaled tokens, keep-state
  flags, empty byte values and null input; independently assemble expected strings.
- Null, empty and ordinary logging through null-safe output capture.
- Both actual source mains, using full-size signatures and comparing exact stdout,
  message text/bytes, variables, instruction count and observed contract outcome.
- Existing transaction replay tests in both builds after the shared extraction,
  including partial writes, cached IDs, untouched gap state, and both actual VMs.

The main remains a demonstration, not a successful-payment example. Matching it
does not establish security or full node parity. Results will be added after the
actual executions finish.

## Results so far

Both builds passed all **169 public-method comparisons and 338 independent
assertions**. The normal run also completed both actual full-size source mains:
stdout matched exactly, including message bytes and variables, with **221
operations and contract rejection**. Four further independent demo checks bring
the normal total to 342 assertions. The whole normal run took 286.371 seconds.
Both main processes exited zero and native stderr was empty; the driver explicitly
checked the contract result instead of treating a zero exit as acceptance.

Both full builds and the existing 268-case/1,677-assertion transaction replays pass
after extraction. The sanitized actual main is still running; no final sanitizer
demo pass is claimed. [Saved status](../../validation/transaction-test-work-in-progress/progress.json).
