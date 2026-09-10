# Lab: measure the hash-speed command without assuming equal runtimes

Written before the new comparison. Reuse the pinned hashtest command body,
TxPoWMiner.calculateHashSpeed, MiniNumber arithmetic, MinimaLogger, and the existing
Main command fixture. The wrapper parses amount (default one million), measures
elapsed wall-clock milliseconds around the real miner call, logs the raw speed,
and reports four significant digits in megahashes per second.

Java and native timing differ. First validate each result independently, then
normalize only its validated clock and speed fields. The returned hashes must
retain the requested MiniNumber, including a fraction or a number whose loop
count narrows to a Java int. The miner uses the original amount in its rate
numerator but the narrowed int in its loop. Negative counts perform no iterations;
zero or backward internal elapsed time is clamped to one millisecond. Existing
miner worker tests exercise that clamp with an injected clock. This command test
uses real clocks and real hashing, including the default million-hash operation.

Capture the raw Speed log and verify the reported MH/s string is its division by
one million followed by four-digit truncation toward zero. Also check that the
raw speed is consistent with an integer internal elapsed time bounded by the
outer command measurement, allowing the miner's one-millisecond clamp. Validate
outer millitime inside the fixture's surrounding clock interval. Preserve all
other metadata, replies, error categories, logs and state for exact comparison.

Include missing, null, wrong-type, blank, malformed and Unicode amounts; zero,
negative, fractional, positive and int-wrapping values; direct calls, registered
runner calls and MiniDapp permission failures. Avoid huge positive narrowed loop
counts. A speed calculation can itself exceed MiniNumber's range: preserve that
failure instead of clamping the rate. Run both normal and ASAN/UBSAN builds.

The existing Main fixture is compiled into a separate target for this comparison,
so the already-running full suites keep their existing executables. Their results
must not be described as a full regression of the newly registered command.

## First comparison finding

The first run reached amount zero and found hashes was a native JSON string
while Java emitted a JSON number. Pinned JSONValue.writeJSONString falls back
to an object's toString without quoting; MiniNumber is stored as an object here.
The wrapper now uses the existing JSONValue::raw(number.toString()) composition
from Token/TxPoW. No floating conversion is introduced into production output.
The [initial failure](../../validation/hashtest-work-in-progress/initial-number-type-failure.json)
shows how identical visible digits can still represent different JSON types.

The normal 59-program corpus passed after the JSON correction. The sanitizer
run then exceeded the fixture's generic 30-second request deadline during the
real million-hash benchmark. Its captured traceback is saved separately. Keep
the default workload and give only this command group a bounded 120-second
response deadline (240-second CTest limit); other groups keep their existing
deadlines. This changes test scheduling, not production hashing or its result.

## Results

Both builds pass all 59 programs and 61 operations. CTest measured 15.65 seconds
normal and 84.39 seconds under ASAN/UBSAN, including fixture setup and Java
compilation; these are suite durations, not the reported hashing rates.
[Saved evidence](../../validation/hashtest-work-in-progress/progress.json) separates
this result from the affected and full regression suites still running.
