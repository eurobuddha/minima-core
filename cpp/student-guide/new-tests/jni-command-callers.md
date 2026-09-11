# JNI callers: test a boundary without inventing the library

The source has three experimental commands and a Java native-method declaration
class. Their external library is absent from the inspected source and sibling
projects. A declaration tells us argument and return types; it does not establish
an algorithm. In particular, hashHeaderWithDiff's comment says the library can
return the supplied test nonce, but does not require that behavior. We therefore
port the callers and explicitly keep the library implementation unresolved.

## What is reused

The three complete pinned command files were read first. Native Command supplies
parameter validation and JSON envelopes; TxPoW and TxHeader supply body/header
serialization and ID calculation; MiniNumber and BigInt supply exact decimal and
signed-integer bytes; Crypto supplies SHA3. The hashtest command supplies the
existing command layout and clock convention. The inspected sibling Hash.cpp and
mining/TxPoWMiner.hpp do not implement these JNI calls or their fixed-offset nonce
mutation, so they cannot substitute for the missing library.

All three constructors, help strings, valid-parameter lists, fresh-command factories
and private signed-byte formatting helpers are retained. Even misleading source
help, an unused parameter and a duplicated maxattempts entry remain unchanged.
Correction: all three classes are in the pinned CommandRunner.java registry.
The earlier audit checked Command.java and incorrectly described them as absent.
The startup/registry checkpoint restores their exact source positions and tests
real dispatch, including the unloaded-library error.

## Follow the caller logic

All three create a fresh TxPoW, hash its body, and set the exact long fractional
START_NONCE_BYTES value. They serialize the header and copy a signed BigInteger
nonce into it starting at byte offset four. This is a raw byte edit, not ordinary
assignment of the logical nonce. The large fractional nonce reserves the space.
After calling sayHello, they pass test nonce bytes, an int attempt count, difficulty
bytes and header bytes to the external method. Its returned bytes are decoded as
a MiniNumber; the command assigns that value, recomputes the ID and compares it
strictly below the target. Extra returned bytes after the number are ignored,
matching the source stream read. Malformed, empty and null results fail.

- benchmark hashes every requested candidate in its ordinary native loop and
  reports ordinary/JNI durations. It computes validity after JNI but omits it from
  the reply, as the source does. Default amount: 1,000.
- jniminingtest stops its ordinary loop at the first strictly valid hash, records
  attempts, reads the mutated header back to recover the fractional nonce, and
  reports both paths' nonce, validity and ID. Default maximum: 1,000,000.
- jnitest runs only the JNI path and also prints every returned byte as a signed
  decimal followed by a comma. Default maximum: 1,000.

Zero/negative attempt counts skip the ordinary loops. Conversion to Java int can
wrap; the native caller reuses MiniNumber.getAsInt. No arbitrary smaller cap is
inserted. Defaults and original key sizes are unchanged.

## Mutable arrays and the numeric cache

Java MiniData.getBytes returns the actual backing array. The native adaptation
exposes a borrowed, fixed-size mutable span. It must not outlive its owner, and a
stream read replacing the storage expires the view. Writing bytes does not clear
MiniData's lazy BigInteger cache, matching Java. The default target retains its
shared Magic object; an explicit parameter creates a separate object.

This matters if an external library changes the target. The hexadecimal display
reads current bytes, but numeric comparisons may retain an earlier cached value.
jniminingtest warms that cache when its ordinary loop attempts a hash; jnitest does
not. Twelve additional fixture cases exercise zero/one prior attempts and zero/FF
target mutations in every caller. They supplement all 117 initial cases.

## Validation design

The unchanged Java commands execute through a tiny test-only JNI library. It
records the two calls, arguments and mutated target, and returns chosen bytes or
throws chosen exceptions. C++ uses the corresponding explicit callback provider.
Neither fixture implements or claims the absent production mining algorithm.
The default native boundary reports UnsatisfiedLinkError when no provider exists;
jnifunctions.cpp remains absent instead of counting a declaration as a working port.

The 129-case corpus includes metadata, all 256 signed-byte displays, defaults,
negative/fractional/wrapped attempt counts, byte-width boundaries 127/128 and
255/256, complete 513-attempt hard-target loops, multiple decimal nonces, truncated
and malformed stream results, ignored trailing bytes, missing-library failures,
call failures, parameter validation and twelve buffer-mutation cases. There are no
original JUnit methods for these three commands; these are supplemental cases.
Every case checks its expected success/error class, in addition to direct equality.

Python independently recomputes SHA3 IDs from each captured original header and
returned nonce, and independently replays jniminingtest's full requested loop.
Because each source TxPoW uses the current time and a random body PRNG, creation
timestamps and body hashes vary. Only after checking each result against its own
captured bytes does comparison normalize those fields and their validated IDs.
Durations must be nonnegative integers, then are normalized. JVM-specific NPE,
cast and numeric-parser diagnostic prose is normalized, retaining error classes.
The random body's hash preimage is not independently exposed by these callers;
existing body tests remain the evidence for that serialization/hashing layer.

## Results and remaining limits

See [checkpoint results](../../validation/jni-command-callers-work-in-progress/progress.json)
for terminal build/test outcomes and counts. Existing base-value and original
base-object tests are also run after adding the borrowed-byte accessor. No original
cases are removed. The full registered suite is not replaced by these focused tests.

```sh
cmake --build cpp/build --target minima_jni_commands MinimaCoreCpp base_value_api MiniNumberTests MiniDataTests
ctest --test-dir cpp/build -R '^(jni_command_callers|base_value_api_differential|original_base_tests)$' --output-on-failure
```

The actual external JNI algorithm, loader, arbitrary array retention/concurrency,
all default-object alias cases and full node parity remain unresolved. These are
working caller counterparts with explicitly partial parity, not three completed
native-library integrations. The goal remains a complete working port.

Exercise: after jniminingtest compares against an FF target, suppose JNI replaces
all target bytes with zero. Predict the displayed target and the numeric target
used for the final validity comparison. Explain why clearing the cache would be an
unfaithful change even though it might seem like cleaner behavior.
