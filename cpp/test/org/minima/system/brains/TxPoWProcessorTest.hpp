#pragma once
#include <cstdint>
#include <functional>
namespace minima::testing {
class TxPoWProcessorTest {
public:
    // Runtime boundary follows MainServices/rpc. Thread-local to isolate fixtures.
    static thread_local std::function<void(int64_t)> sleep;
    void testTxPoWProcessor();
    void testTxPoWProcessor2();
    static void delay();
    static void delay(int64_t millis);
};
}
