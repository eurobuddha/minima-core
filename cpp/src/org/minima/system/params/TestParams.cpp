#include "TestParams.hpp"
namespace minima {
TestParams& TestParams::values(){static TestParams settings;return settings;}
void TestParams::setTestParams() {
    const auto& settings=values();
    GlobalParams::MINIMA_BLOCK_SPEED=settings.MINIMA_BLOCK_SPEED;
    GlobalParams::MINIMA_BLOCKS_SPEED_CALC=settings.MINIMA_BLOCKS_SPEED_CALC;
    GlobalParams::MINIMA_CASCADE_FREQUENCY=settings.MINIMA_CASCADE_FREQUENCY;
    GlobalParams::MINIMA_CASCADE_LEVEL_NODES=settings.MINIMA_CASCADE_LEVEL_NODES;
    GlobalParams::MINIMA_CASCADE_LEVELS=settings.MINIMA_CASCADE_LEVELS;
    GlobalParams::MINIMA_CASCADE_START_DEPTH=settings.MINIMA_CASCADE_START_DEPTH;
    GlobalParams::MINIMA_CONFIRM_DEPTH=settings.MINIMA_CONFIRM_DEPTH;
    GlobalParams::MINIMA_MMR_PROOF_HISTORY=settings.MINIMA_MMR_PROOF_HISTORY;
    GlobalParams::MINIMA_VERSION=settings.MINIMA_VERSION;
    GlobalParams::MEDIAN_BLOCK_CALC=settings.MEDIAN_BLOCK_CALC;
}
}
