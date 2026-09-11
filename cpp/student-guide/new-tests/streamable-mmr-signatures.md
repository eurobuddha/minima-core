# Reading into existing MMR and signature objects

This stage adds MMRData, MMREntryNumber, MMREntry, MMRProofChunk, SignatureProof and
Signature to the native Streamable interface. Existing scalar decoders supply
field values; new readers assign each field at the same point as the pinned Java
method. A failed later field must not roll back an earlier assignment.

MMRData can receive a new hash while keeping its previous sum if the sum is
truncated. SignatureProof can receive a new public key and signature while retaining
its previous proof. MMREntry clears its empty flag before it reads even the row.
Its flag must therefore be separate from whether its data pointer is null.

MMREntryNumber behaves differently: it constructs the entire decimal before
replacing its number. A truncated unscaled value must leave the previous decimal
unchanged. Empty BigInteger bytes throw NumberFormatException. The test includes
that malformed value as well as every truncation boundary of normal encodings.

Signature replaces its list before reading the count. Someone holding the old
list should still see that old list, while an alias to the Signature itself sees
the replacement. The native code reuses HORSTSignature's shared-data/list pattern,
and the verifier retains a shared list handle while traversing it.

Tests extend the existing common-interface driver and fixtures. They compare
partial output, field updates, cursor position, explicit flags, retained list
identity and successful generic file/hash/network-message calls. Additional
negative cases check an empty MMR entry's written row/entry prefix before its null
data error. Oversized nested hashes also check the precise prefix retained before IOException.

Both complete builds pass. The interface suite passes 1,299 comparisons and 6,290
assertions in each build, with identical transcript hashes and empty native stderr.
All six affected regressions pass, including TreeKey signing and verification.
See the [saved evidence](../../validation/streamable-mmr-signatures-work-in-progress/progress.json).
There are now 11 of 44 explicit source implementers integrated. The remaining 33,
nullable fields/list entries, all constructor/reference APIs and custom streams
still require work. Complete class/API parity is not claimed.

For students: contrast the assignment timing in MMRData and MMREntryNumber. Why
can a failed MMREntry read change isEmpty? Why must a retained list and an alias
to the containing Signature observe different things after replacement?
