# Help is executable metadata

The pinned help command has two paths. With no command parameter it constructs a
curated list of 94 command objects and asks each for its short description. With a
name it asks the real registry for a fresh command, then reads short and full help.
Neither path should execute the target command. The new test uses a probe whose
runCommand throws: successfully describing it proves help reads metadata only.

The C++ translation reuses Command, CommandRunner and JSONObject. It keeps the
curated constructor list separate from the registry. A controlled registry change
must change detailed lookup while leaving the default catalogue unchanged.

Java String.length counts UTF-16 code units. An emoji usually occupies two units,
so a cut can split its surrogate pair. The public padding helper is tested using
arrays of integer units: decoding output as UTF-8 would conceal an isolated
surrogate. Tests include empty strings, negative lengths, null inputs, exact
length, truncation, padding, NUL and surrogate pairs. An independent array slice
and space-fill calculation supplies expected output.

The command tests compare actual Java/C++ metadata and replies, exact catalogue
keys and descriptions, trimmed/blank/wrong-type parameters, case-sensitive detailed
lookup, fresh factories, parameter aliases, chained commands and non-admin help.
The three JNI-dependent commands are still absent from the native registry; that
gap must be reported, not replaced with fake command implementations.

Both complete builds pass. Each help suite passes 761 comparisons and 2,543
independent assertions, including exact serialized replies and catalogue insertion
order. The transcript hashes match. All three affected runner/text regressions
also pass in both builds. [Saved evidence](../../validation/help-command-work-in-progress/progress.json).
This does not certify full-node parity. For students: why does `HELP command:hash` work while
`help command:HASH` fails? Why does changing the registry not change the curated
list? What does truncating a two-unit emoji to one Java character return?
