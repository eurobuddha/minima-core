# Lab: raw transactions and ordered JSON input

Written before the rawfrom/rawtxnfrom tests.

Both commands assemble a custom transaction by calling existing commands. rawfrom
accepts an input object containing coinid and script, then adds each input and its
script together. rawtxnfrom accepts separate arrays: it adds all input IDs, then
all outputs, then all scripts. That ordering changes what exists when a later
step fails. Neither wrapper checks child status or adds MMR proofs/signatures.

Inputs and outputs are parsed as arrays before txncreate. A null array is allowed
by the getter and fails only when the later loop starts. A wrong JSON type fails
before creation. String fields accept null for concatenation as the text `null`,
but reject non-strings. storestate uses Boolean cast/unboxing: null fails, a JSON
Boolean works, and the string `true` is not a Boolean.

The source JSONObject extends LinkedHashMap. Traverse state keys in insertion
order, parse the integer port before converting its value, then let StateVariable
normalize the data. Java string concatenation differs from JSON encoding for a
string and for a floating-point number. We reuse the exact stateText conversion
already used by send.cpp. Strings keep their contents, doubles use Java formatting,
and other JSON values use their existing representation.

Reuse the previous wrapper oracle. Subclasses record exact child text and row
bytes before each call. Controlled replies and throws test ordering and cleanup;
real children test output/state/script construction and exported TxnRow bytes.
Random IDs are shape-checked and normalized only in known text and the verified
serialized ID field. All other bytes, replies and logs must match Java.

Compare two inputs with different scripts, omitted defaults, every wrong element
and field type, null arrays, duplicate ports, Unicode numeric keys, overflowing
ports, quoted strings, and floating state values. A false child status should not
stop these wrappers. A thrown child exception should stop before later calls,
and can leave the temporary row. Test real exported state/output contents as
independent expectations, not only cross-language equality.

Java equalsIgnoreCase includes Unicode case mappings. The long-s spelling `falſe`
matches FALSE in the pinned source; an ASCII-only lowercase comparison rejects
it. A source/native baseline demonstrated that mismatch in StateVariable. Reuse
the existing JavaStringIgnoreCase helper and test both the generated txnstate
text and real exported state bytes, including ordinary upper/lowercase spellings.

## Evidence

The final raw group passes 656 programs and 3,991 child calls in normal and ASAN/UBSAN builds (4.06s/4.55s). After the Unicode Boolean correction, the object regression passes 9,464 cases per build (25.10s/91.34s). The earlier seven-check batches also pass, including funding and wallet-wrapper regressions. Both full C++ builds pass without compiler warnings. The preserved baseline is `cpp/build/raw-state-unicode-baseline.json`. It shows source acceptance and the previous native numeric-conversion failure for long-s FALSE.

Full native Main, durable SQL and funded network acceptance remain unfinished;
an exported row is not a confirmed transfer.
