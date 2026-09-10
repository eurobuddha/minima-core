# Lab: sending and creating tokens from a custom address

Written before this test group. Read the pinned `sendfrom.java` and
`createtokenfrom.java` alongside their C++ counterparts. These are orchestration
commands: their job is to call existing transaction commands in the right order.

## Learning objectives

Trace parameter validation, child calls, database mutation and exception paths.
Distinguish a returned failure status from a thrown exception. Explain why
deleting a temporary transaction depends on where execution stops.

## Predict the behavior before running

1. `sendfrom` reads addresses, amount and burn before the signing parameters.
   A positive burn with a token other than the exact text `0x00` is rejected.
   Invalid optional state objects are ignored while invalid state entries fail
   later, after funding and script insertion.
2. Both commands create a random transaction ID, fund the row, add the script,
   calculate proofs, sign, post and delete. Funding status is checked before
   continuing; posting status is checked after deletion. Other returned child
   failures are ignored. A thrown exception interrupts this sequence.
3. Token supply is rounded down. The accepted supply is from 1 through one
   trillion. Decimals default to 8; only an upper bound of 16 is imposed.
   Decimal parsing uses the original raw String, so surrounding whitespace is
   significant. JSON names must contain a `name` key, even if its value is null.
4. Token creation computes `MINI_UNIT × 10^decimals × floor(supply)` using the
   existing fixed precision number class. It constructs a token coin and
   replaces output zero in the stored transaction. Remaining outputs must stay
   unchanged. A missing row or empty output list fails at this replacement.

## Experiment and evidence

Extend the existing WalletWrappersOracle and native fixture. Execute the actual
wrapper bodies, recording child text and row wire bytes before each call.
Inject returned failures and thrown exceptions separately at every child step.
For token replacement, a fixture funding option supplies explicit ordinary
outputs through the actual transaction commands. It is test input, not a
replacement funding algorithm. Observe the row through the real serializers
and JSON methods so token metadata, supply, scale, ID and change are compared.
Also execute the real child runner to expose the source's literal space after
`fromaddress:`. Do not repair this text in production merely to make a test pass.

Compare Java and C++ observations, normalizing only verified random ID fields
and runtime-specific null/cast exception wording. Independently assert call
order, cleanup, first-output replacement and preserved change. Run normal and
address/undefined-behavior sanitizer builds and relevant shared regressions.

## Limits and exercises

An injected successful funding/posting reply does not prove a funded transfer
or network acceptance. Native Main and durable SQL remain unfinished. Explain
which observations require a live node and which can be established offline.
Draw the two paths for funding failure and posting failure, marking deletion.
Predict what negative decimals do after the number class truncates tiny values.
Explain why changing ignored failures into exceptions would change the port.

## First discrepancy: exponent rejection

The first 492-program comparison passed 489 programs in both builds. Two cases
with decimals -1000000000 and -2147483648 exposed the existing MiniNumber.pow
range error: Java reports `ArithmeticException: Invalid operation`; native code
used a generic out_of_range exception. Preserve that baseline and use the
already-proven JavaArithmeticException carrier. Before extending the tests,
plan four direct supplemental probes for both out-of-range signs and the signed
integer extremes. Reuse NumberValueOriginalTests and its mapped native test;
keep the 99 original assertions separately labelled from these added probes.
The remaining difference was JVM null-pointer prose inside CommandException;
normalize only that nested runtime category while retaining the wrapper class.

## Final evidence

Both full C++ builds pass without warnings. The custom-address comparison passes
492 programs and 2,052 child calls per build (13.69s normal, 25.24s sanitized).
All ten selected checks pass per build, taking 40.23s and 124.63s. The nine
comparison reports per build were inspected: custom wrappers 492, previous wallet
wrappers 659, raw wrappers 656, commands 12,289, runner 1,265, runner errors 1,120,
objects 9,464, primitives 3,802, and original NumberValue 99 assertions plus eleven
supplemental exact error observations. The tenth check validates source mapping.
This is selected regression coverage, not all 172 CTest entries or full node parity.
The preserved discrepancy report is `cpp/build/custom-send-error-baseline.json`.
