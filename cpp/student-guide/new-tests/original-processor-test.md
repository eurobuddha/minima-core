# Empty tests and active helpers

This checkpoint addresses two unusual source files. File mapping is bookkeeping;
it must not turn absent behavior into a claim of working functionality.

## TxPoWProcessorTest

Both original JUnit test bodies are empty. Their commented experiments describe
out-of-order blocks and transactions but execute no operations or assertions.
The C++ class preserves both methods and their historical comments. Its real work
is in the two delay helpers: delay() delegates to delay(100); delay(milliseconds)
calls the sleep runtime and suppresses only InterruptedException. Negative delays
must propagate IllegalArgumentException. An unrelated failure must escape.

The native port reuses the sleep callback and typed-exception pattern inspected in
MainServices, rpc and JavaExceptions. NativeThread and FixedTaskExecutor were also
read: neither provides Thread.interrupt. A thread-local callback keeps injected
runtime behavior isolated between test threads. The default callback performs real
sleeping and validates negative durations. Very large durations use bounded chunks
to avoid overflow when converting milliseconds to the platform clock duration.
This does not add a native Thread.interrupt facility; that runtime parity gap is open.

The unchanged Java class is compiled with JUnit. A runner checks the exact two
annotated methods and options, then invokes them on fresh instances. A source guard
ensures their bodies remain empty, so future source assertions cannot be silently
missed. The native runner invokes both methods. Original assertion count: **zero**.

Ten supplemental observations compare Java and native: the default delay, zero,
one millisecond, three negative values including Long.MIN_VALUE, and four interrupt
cases including zero, Long.MAX_VALUE and negative duration. The default call must
last at least 90 ms, a coarse guard for the requested 100 ms rather than a clock
precision test. Positive delays are real sleeps. The native interruption cases use
an injected callback to test exception handling; Java uses actual Thread.interrupt.
Negative argument validation precedes clearing interrupt status. Both runtimes must
match the independently listed outcomes, giving twenty checks. A native-only
check verifies that an unrelated typed exception escapes. These checks validate
the helper boundary and do not validate block processing or a native interrupt API.

## P2PSwapLink

Every Java line is commented, including package and class declarations. Compiling
it produces no P2PSwapLink class. The native file retains every original comment
with an explicit inactive-source notice and produces no protocol implementation.
The source map labels it **inactive_source_preserved**, separate from active partial
implementations. It contributes one preserved inventory path and zero functional
conversion credit. No old stream API or swap-message behavior is activated.

Validation checks Java and C++ contain only comments, retains the exact original
comment suffix, and confirms javac emits no class. CMake compiles the empty
translation unit separately; compilation is not treated as functional parity.

## Results and limits

Normal and sanitizer builds pass the new targets and actual MinimaCoreCpp node.
The focused CTests pass in 1.47 s and 1.29 s. The full 282-test suite was not repeated
because existing production implementations did not change. There are 453 mapped
paths: 452 active partial counterparts and one inactive-source preservation; four
paths are still absent. Full port parity remains unverified.

```sh
cmake --build cpp/build --target TxPoWProcessorTest minima_inactive_source MinimaCoreCpp
ctest --test-dir cpp/build -R '^original_processor_test$' --output-on-failure
```

[Checkpoint evidence](../../validation/original-processor-test-work-in-progress/progress.json).
Exercise: explain why two passing empty test methods establish no processor
correctness. Then predict which exception delay(-1) propagates when interrupted.
