# Base-value public helper and constant correction

Source-named stream helpers reuse existing checked native serialization. MiniData
version conversion reuses the existing IO-only catch/log/null helper through a
Streamable interface. MMR decimal access uses the existing unbounded value carrier.
MiniByte FALSE/TRUE are mutable objects, and isFalse compares with current FALSE.

The base-value API differential suite retains its previous 1,164 cases and adds
689 cases; every case has independent expected observations as well as Java/native
comparison. Final per-configuration JSON and build/test logs identify actual
outcomes. The initial native fixture compilation failure used an unavailable
exception alias; it was replaced with the existing JavaTypedException adapter.
No production behavior or test assertion was weakened to resolve that error.

The method-name candidate search was a read-only lexical lead, not a completeness
check. One numeric token was a false positive. JNI native bodies remain unavailable.
Added entry points do not promote these five files to full API parity.
