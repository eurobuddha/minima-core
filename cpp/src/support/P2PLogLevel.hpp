#pragma once
namespace minima::p2p {
// Shared definition of P2PFunctions::Level avoids a parameter/runtime include cycle.
enum class P2PLogLevel { NODE_RUNNER_MSG, INFO, DEBUG };
}
