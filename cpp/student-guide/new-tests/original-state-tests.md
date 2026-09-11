# Original state tests: values, ports and missing data

This batch translates PREVSTATETests, STATETests and SAMESTATETests from the
pinned Java source into native test counterparts. It reuses the production
StateVariable, Transaction, Contract and state function classes. No production
algorithm changes are required by these fixtures.

## Follow the data

A state variable has a port number and typed text. The source builds 16 groups
of four ports, giving 64 entries. Within each group the entries are boolean TRUE,
hex `0x12345678`, the group number, and a bracketed string containing its port.
The original spaces and bracket construction are retained in C++.

STATE obtains its value from the current Transaction. PREVSTATE obtains it from
the Contract's previous-state list. Both lookups pass the stored text through
Value.getValue, which returns a BooleanValue, HexValue, NumberValue or StringValue.
For example, the brackets identify a stored script string; they are removed when
the parsed StringValue displays its contents. The tests check both the result's
runtime type and its text, preserving the source casts as well as equality checks.

SAMESTATE compares previous and current values over an inclusive port range.
The original successful tests always pass the same start and end port. One
transaction contains separately constructed copies of the 64 previous values;
all singleton checks return TRUE. The second rearranges the four value types;
all 64 singleton checks return FALSE. Those are real value comparisons, but they
do not test a range that visits several ports.

## Preserve the assertions

| Original class | Constructors | Valid fixtures | Invalid fixtures | Total |
| --- | ---: | ---: | ---: | ---: |
| PREVSTATETests | 4 | 128 | 7 | 139 |
| STATETests | 4 | 128 | 7 | 139 |
| SAMESTATETests | 4 | 256 | 21 | 281 |

There are nine active source methods and 559 assertions per runtime. The native
helper increments a counter at each executed assertion. The Java runner follows
the existing OriginalSHATests direct runner: it checks the annotated method set,
default options and absence of lifecycle hooks, then invokes unchanged source
methods on fresh instances using real JUnit assertions. The reported Java count
comes from the reviewed successful source paths; it is not runtime instrumentation
of JUnit itself. A source fail() branch fails the run rather than contributing
to a successful-path count.

Wrong argument counts, invalid types, missing state and invalid SAMESTATE ranges
must raise ExecutionException. The native helper catches that specific type;
a different exception cannot satisfy the assertion. In the empty fixture,
SAMESTATE(100,200) rejects because previous port 100 is missing. It does not prove
an upper bound of 200. Similarly, a missing STATE(0) is an error even though zero
is a valid port number.

## Reuse and validation

The shared OriginalFunctionTestSupport fixture already used by SHA tests supplies
real contracts, ConstantExpression arguments and fresh functions. This batch only
adds an optional report marker so state and SHA results remain distinguishable.
The SHA tests are rerun because their shared helper changed; previously covered
production suites are not broadly repeated.

The inspected sibling implementation in
`tools/minima-core-cpp/src/kissvm/functions/Functions.cpp` returns empty hex for
missing STATE/PREVSTATE and compares absent state as empty text. That differs from
the pinned source's missing-state exceptions. The current native state functions
already implement the required behavior and are used unchanged.

Run from the repository root:

```sh
cmake --build cpp/build --target PREVSTATETests STATETests SAMESTATETests MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_state_tests$' --output-on-failure
```

Use `cpp/build-sanitized` for the configured sanitizer build. Both builds execute
the original Java and C++ tests. The [checkpoint evidence](../../validation/original-state-tests-work-in-progress/progress.json)
records their outcomes, source pin and remaining files. Passing these fixtures
does not establish every state API, mutable-reference behavior, arbitrary ranges,
concurrency or complete node parity.

Exercise: compare STATE(0) on an empty transaction with STATE(0) after storing
TRUE at port zero. Explain why the same argument is valid in one case and raises
an exception in the other. Then propose a multi-port SAMESTATE case that would
detect an implementation which checked only the first port.
