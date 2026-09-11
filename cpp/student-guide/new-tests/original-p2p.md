# Original P2P tests and their peer factories

This batch converts five pinned Java files: QuickInetLists, QuickState,
UtilFuncsTest, WalkLinksFuncsTest and SwapLinksFunctionsTest. The inventory rises
from 413 to 418 counterparts; 39 files remain absent. A counterpart and a passing
test do not establish complete behavior for the production APIs it exercises.

## Follow the data

QuickInetLists turns a prefix such as `60.0.0.` into resolved socket addresses
`60.0.0.1`, `60.0.0.2`, and so on. Its map also assigns one-based UID keys.
QuickState uses those values to construct named starting states. The three test
classes then pass these real states into existing native peer functions.

The reused building blocks are P2PState, PeerMap/JavaHashMap, SocketAddressSet,
JavaRandom48 and InetSocketAddress. The smallest new production adapter resolves
a host once, throws for an unresolved host, then checks the port. This preserves
the evaluation order of Java's nested InetAddress and InetSocketAddress calls.
There is no substitute peer algorithm in the test fixtures.

## Predict the boundaries before running

| Input | Required observation | Why |
| --- | --- | --- |
| List, null prefix, count 0 | Empty list | The loop never dereferences the prefix. |
| Hostname overload, null prefix, count 0 | NullPointerException | Concatenation happens before delegation. |
| List, `bad/`, count 1, port 65536 | UnknownHostException | Resolution precedes port validation. |
| List, `192.168.0.`, count 1, port 65536 | IllegalArgumentException | Resolution succeeds; the port is invalid. |
| Map, port 0 or negative | Every port in 7000–9999 | The random-port policy is captured before the loop. |
| Known-peer-only state | Same object in set and map | Java stores the set's address reference. |
| Clear known set after taking its copy | Copy still has one address | Collection copying retains child references. |
| No address requested | Wildcard port 9001; host flag false | That factory leaves the constructor default intact. |

Random ports are independently checked against their required range in both
runtimes, then normalized to compare the rest of the structure. This deliberately
makes no claim that separately seeded generators emit identical sequences.

## What the original assertions establish

| Original test class | Active methods | Assertions per runtime | Observations |
| --- | ---: | ---: | --- |
| UtilFuncsTest | 2 | 2 | Nonempty selection is non-null; empty selection is null. |
| WalkLinksFuncsTest | 3 | 3 | Filtering leaves five; empty input stays empty; full outgoing links yield no request. |
| SwapLinksFunctionsTest | 8 | 10 | Disconnection membership, greeting transitions, secret echo, incorrect-secret address preservation. |

The Java driver executes the unchanged pinned classes through real JUnit.
Its assertion total comes from the inspected source. The native test increments
a counter for each executed assertion. Both must finish successfully with their
expected method and assertion totals. Commented tests are not counted.

The separate helper corpus has 1,302 requests and 16,100 independent expected-field
checks. It exercises all three generators and all seven factories, numeric IPv4
and IPv6, null inputs, negative/zero counts, invalid ports, map contents, collection
aliases and source defaults. It also compares the two runtimes directly.

## Results and the mistakes retained as evidence

Normal and sanitizer builds pass both suites. Each original suite executes 13
methods and 15 assertions per runtime. Each helper suite passes all 1,302
comparisons and 16,100 independent checks with transcript SHA-256
`67b6c08f901023386b45216a419930d7965bc7c78f954a841f3bcb8880d51190`.

The first build caught a nonexistent JSON convenience accessor in the new test.
It was replaced with the native JSONObject variant access already provided.
The first helper run then rejected an incorrect expected secret of `0x00`.
Both runtimes actually returned an empty string: the source constructs zero
bytes, not one zero byte. We checked that source and corrected the expectation.
Neither failure justified changing the production peer algorithms.

[Saved evidence](../../validation/original-p2p-work-in-progress/progress.json)
includes initial failures, final results, build logs and the remaining inventory.
Run `python3 cpp/tools/original_p2p_tests.py --build cpp/build` and
`python3 cpp/tools/original_p2p_helpers_differential.py --build cpp/build` from
the repository root; use `cpp/build-sanitized` for the sanitizer targets.

External DNS changes, allocation failures, concurrency and complete node/P2P
behavior remain outside this focused corpus. Existing wider tests retain their
own evidence. No full parity label is awarded here.

Exercise: move the random-port decision inside the map loop in a paper version.
Explain why the first draw would accidentally make later ports fixed, and design
an observation that would catch that mistake without assuming a particular seed.
