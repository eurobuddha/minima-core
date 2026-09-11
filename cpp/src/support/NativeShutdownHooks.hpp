#pragma once
#include <functional>
namespace minima {
// Process hooks run outside the signal handler, with NativeThread's uncaught boundary.
void addNativeShutdownHook(std::function<void()> hook);
}
