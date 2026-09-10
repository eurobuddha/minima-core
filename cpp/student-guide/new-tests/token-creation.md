# Lab: wallet token creation

Written before the new tests. Translate the pinned tokencreate command using the
existing wallet, number, token, proof, signing, generator and miner components.
The normal supply is floored; default decimal places are eight. Colored Minima
is 10^-44 times 10^decimals times floored supply. Limits constrain supply and
decimals only when enabled, and the source does not reject negative burn here.

Use Java-created coins/MMR and actual keys, then compare exact replies, TxPoW
wire, SQL calls, proof/signature data and miner queue effects. At miner entry,
reuse the established fixture that fixes generated PRNG bytes, retaining their
length and all other fields. Independently assert supply, scale, recipient token,
change, state placement and input-output difference. Verify both transaction
signatures and the optional token metadata signature over the first output ID.

Vary raw amount/decimals/script types and order competing errors. Names supplied
as JSON remain shared: web validation and signing mutate that object even when a
later step fails. Compare those mutations. Test empty/missing chain, immature,
mining and mempool coins, no available funds, truncated history, missing scripts,
SQL failures and key exhaustion. Synchronous false/throw/post failures use the
existing miner boundary; successful synchronous/asynchronous cases run real code.

A passing fixture is finite evidence. It does not establish durable SQL or
network acceptance of minted transactions, nor complete native-node integration.

## Observed results

Both configurations pass 335 programs / 1,349 operations / 26,719 SQL observations.
Both full builds and the registry, mining and custom-token regressions pass.
The [saved reports](../../validation/token-creation-work-in-progress/progress.json)
retain the initial compile correction and reservation-fixture mismatch.

The source preserves its unusual behavior: negative burn is not rejected here,
JSON metadata changes survive late failures, and async success means queued.
Token creator signatures verify over the future first output coin ID, while
transaction signatures verify over the completed transaction ID.
