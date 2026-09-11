# Reading transaction-related objects through Streamable

This stage connects StateVariable, Address, ScriptProof, Token, Coin and CoinProof
to the existing native interface. Scalar and MMR codecs remain the building blocks.

StateVariable assigns port and type before reading its value. A failed value read
can leave new tags with old text. An unknown type throws on read, but the Java
writer subsequently emits just the two tag bytes. Address replaces its hash and
script before recomputing its Mx text. ScriptProof replaces its script and proof
before calculating its address. Token similarly reads six fields before updating
its ID. Failed earlier reads therefore leave the previous derived identity intact.

Coin and CoinProof already have in-place readers that preserve retained owners.
Their writers now append each nested field directly. If a later hash is oversized
or a state list is null, bytes written before the failure remain visible. Token's
calculation and conversion failures now use the existing logger with source catch
boundaries; Java Error and native allocation failures escape the calculation catch.

The interface test compares every prefix of small fixtures, derived IDs, state tags,
invalid tags, noncanonical booleans, nested writer errors, actual generic file/hash/
network-message calls and Token logging. Token IDs and script-proof roots are also
computed independently from their field framing and hashes. Existing object and
coin-reader suites regress real callers and retained ownership. Both complete builds
and all eight targeted suites pass. Each interface suite passes 2,771 comparisons
and 20,914 assertions across 20 types, with matching transcripts and empty native
stderr. See the [saved evidence](../../validation/streamable-transaction-objects-work-in-progress/progress.json).
Another 24 explicit source implementers remain. Default/null/field-reference APIs
and complete node parity remain open.

For students: explain why new serialized fields can coexist with an old derived ID
after a failed read. Why is writing a whole nested temporary different from letting
that nested object append to the shared output? Why do the state reader and writer
handle an unknown type differently?

The initial boolean assertion assumed only 1 meant true. Reading MiniByte.isTrue
showed that every nonzero byte is true; Java and C++ already agreed. The corrected
expectation checks 0, 1, 2 and 255. A failed assertion can expose a fixture error,
so compare it with the actual source before changing production behavior.
