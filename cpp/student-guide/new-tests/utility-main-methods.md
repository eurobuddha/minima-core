# Completing five original utility programs

The class mapping already contained Maths, BaseConverter, MiniFormat, POW and
REPLACE, but their public main methods were missing. A compiled Java declaration
audit found them; this batch adds the original calls and constants while reusing
the existing native implementations. Arguments are unused and may be null.

Maths compares version 1.0.4 with 1.0.3 and prints 1. REPLACE searches the literal
$1 four times and replaces it with 3, printing 3333. Its helper quotes the search
pattern; that matters because $ has a special meaning in an unquoted regex.

BaseConverter's original demonstration prints hex bytes 000001 and their base-32
form Mx1. It then passes Mx1 to MiniData's hex-only constructor, which throws a
NumberFormatException. The source's conversion-back and equality prints are therefore
unreachable. The port keeps those calls and reproduces the failure instead of
substituting decode32 to make the demonstration succeed.

POW calls MiniNumber.pow(1000) on 0.01 in all 1000 loop iterations, printing each
result and elapsed wall-clock milliseconds. MiniNumber truncates to its allowed
decimal scale; the extremely small result becomes zero. The test keeps every
iteration and output line, normalizing only the final elapsed value for comparison.

MiniFormat parses and pretty-prints the entire original balance sample. Its full
embedded data URL is copied exactly; byte length and SHA-256 provide a separate
literal integrity check. Debug mode must call MinimaLogger with the UTF-16 length
and a second serialization of the object, allowing normal notification behavior.
An ordinary formatting failure logs a fallback message with notifications disabled;
that message must not disappear or be routed to a different output stream.
The native method now also applies Java trim before formatting.

The test invokes actual entry points in both languages and saves raw stdout. A
second matrix gives JSONPretty controlled JSONObject renderings: whitespace,
Unicode, valid JSON, escaped newlines and unterminated quoted strings. It compares
formatted text, exact logs, exception messages, render counts and notification
counts. Logger failures test which effects happen before an exception. Source logger
prefixes vary with time and heap use and are removed; message text is retained.

Both normal and address/undefined sanitizer builds pass the five complete main
comparisons and a 36-case formatter matrix: 31 finite comparisons and five bounded
loop observations, with 251 driver assertions. The leading unmatched quote is a
source no-progress loop: substring(0,0) leaves the cursor unchanged. Tests observe
the child after an entry marker and terminate only their own child; elapsed waiting
alone is not used as proof of infinite execution.

The existing utility comparison passes 1364 cases and original POW tests pass all
25 assertions in each build. The actual node executable builds pass. The full suite
was not rerun. [Saved evidence and raw outputs](../../validation/utility-mains-work-in-progress/progress.json),
[Java fixture](../../support/UtilityMainsOracle.java),
[native fixture](../../support/utility_mains.cpp), and
[comparison driver](../../tools/utility_mains_differential.py).

Existing utility and KISS behavior remains subject
to broader API review; these finite programs do not prove whole-file parity.

Check your understanding: why does base-32 Mx1 not preserve the leading zero bytes,
and why does this particular demonstration fail even before equality is checked?
Why does Java count an emoji as two UTF-16 code units for the debug length?
