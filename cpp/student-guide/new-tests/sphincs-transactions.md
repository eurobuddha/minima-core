# Turning a SPHINCS signature into transaction state

This lab follows the [signer lab](sphincs-signer.md). SPHINCSUtils has two different
jobs. Its calculateTransactionID method returns an **unhashed message**, assembled
from input IDs and output address, scaled token amount, token ID and keep-state
flag. The method name does not mean that it returns Transaction's cached hash.
Its setupTransaction method writes 66 signature state variables, recomputes output
coin IDs, then asks Transaction to calculate its ordinary serialized-data hash.

Before execution, the checks are:

- Compare the exact script constant and public-key replacement, including null
  and empty public data. Build message bytes independently from coin fixtures.
- Vary input/output counts, coin ordering, scaled tokens, output state flags and
  ELTOO input IDs. Zero inputs fail while zero outputs are allowed by the message
  builder. The script itself still asks for output zero; test that distinction.
- Compare every state variable and serialized transaction after setup. Existing
  ports 100/101 and 0–78 except every fifth gap are replaced; unrelated ports stay.
- Shorten and null each of the four nested FORS lists at each position, and inject
  an oversized proof hash. Observe the exact partial state and cached transaction
  ID. Serialization I/O failures log first, then the caller dereferences null.
- Sign an actual transaction message with the full native and Java signers. Feed
  both signatures through both setup implementations and the actual KISS VM script.
  Provide witness coin proofs because GETINID reads their IDs. Compare success,
  operation count, variables and failure after changing committed transaction data.

The existing CoinFixture/ObjectOracle coin builder, signature readers, codecs,
Runner and Contract APIs are reused. The wire-to-MiniData helper is extracted
unchanged from FORSSignature so its exception handling stays shared. Results will
be added after execution. Full node and unrepresented Java reference behavior
remain outside any claim made by this lab.

## Results and the error found by the script test

All 250 helper cases matched in both initial runs. The normal run also produced
identical full-size signatures for an actual two-input/two-output transaction.
Both actual VMs accepted the complete sixteen-tree script in **937 operations**.

Changing state 100 to malformed signature bytes exposed a production difference:
Java's signature converter returned null and TreeKey dereferencing it produced a
NullPointerException. Native CHECKSIG reported a generic exception instead. The
native TreeKey now accepts the nullable signature at this boundary, CHECKSIG
reuses that path, and Contract preserves the Java exception type and observed
pinned-JDK message. The original report's two failures remain saved.

After the correction, **268 replay cases and 1,677 independent assertions** pass
in each build, with identical transcript hashes. The replay includes all helper
cases and both actual writers' signatures through both VMs, including altered
input IDs, output keep-state, each selected signature region and unused ports.
It reuses the saved genuine signatures; it does not regenerate their forest.
Normal replay took 8.382s, sanitized replay 28.486s. Both complete builds and
object (9,464), VM (4,471), envelope (1,449) and log (3) regressions pass.
The original sanitizer full-signing process remains active; its final report is
not yet claimed. [Current evidence](../../validation/sphincs-utils-work-in-progress/progress.json).
