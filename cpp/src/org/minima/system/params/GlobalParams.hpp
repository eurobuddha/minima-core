#pragma once
#include "org/minima/objects/base/MiniNumber.hpp"
namespace minima {
struct GlobalParams {
    static std::string MINIMA_BASE_VERSION;
    static std::string MINIMA_BUILD_NUMBER;
    static std::string MINIMA_VERSION;
    static std::string MINIMA_MICRO_BUILD;
    static MiniNumber MINIMA_BLOCK_SPEED;
    static MiniNumber MINIMA_BLOCKS_SPEED_CALC;
    static MiniNumber MINIMA_CONFIRM_DEPTH;
    static MiniNumber MINIMA_CASCADE_FREQUENCY;
    static MiniNumber MINIMA_CASCADE_START_DEPTH;
    static int32_t MINIMA_CASCADE_LEVEL_NODES;
    static int32_t MINIMA_CASCADE_LEVELS;
    static MiniNumber MINIMA_MMR_PROOF_HISTORY;
    static int32_t MEDIAN_BLOCK_CALC;
    static std::string getFullMicroVersion();
};
}
