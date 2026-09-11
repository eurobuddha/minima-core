# Lab: concatenating byte arrays

This plan precedes execution of the mapped CONCATTests. The production CONCAT
function already exists; the new file will reproduce the unchanged Java tests.
We reuse LENTests' contract, expression, factory and typed-exception adapters.

CONCAT joins bytes in argument order. Two four-byte arguments become eight bytes;
five four-byte arguments become twenty. The result must still be a HexValue.
Two empty HexValues are accepted and display as an empty string. Although the
source places that case under “invalid parameters,” its active assertions expect
success. We preserve the assertions rather than interpreting the heading as code.

Zero or one argument fails. Boolean, number and script arguments also fail,
including a pair of script strings. The nearby sibling implementation supports
string concatenation and exactly two arguments; that behavior does not fit this
pinned revision. The current native function supports the source's two-or-more
hexadecimal arguments and is reused without modification.

The three source methods execute 21 assertions on the successful test path:
four factory/registry checks, four nonempty-result checks, two empty-result checks,
three argument-count rejections and eight type rejections. We will run those
unchanged Java methods and the same assertions in C++, in both builds. A C++
ExecutionException catch corresponds to JUnit's assertThrows; any other exception
must fail the test. These source tests do not exercise the maximum data size or
side effects during expression evaluation, so they do not establish full parity.

Driver: `cpp/tools/script_value_original_tests.py --test-class CONCATTests`.
Both complete builds and both registered tests pass in each build. Each runtime
executes three original methods and 21 assertions. Normal CTest total: 5.05 seconds;
sanitizer total: 3.87 seconds. Native stderr is empty and UBSAN halts on errors.
No production function changed.
