#pragma once
#include "GlobalParams.hpp"
namespace minima {
class TestParams {
    TestParams()=default;
public:
    // Like the existing P2PTestParams adapter, initialize on first class use.
    static TestParams& values();
    std::string MINIMA_VERSION=GlobalParams::MINIMA_VERSION+"-TEST";
    MiniNumber MINIMA_BLOCK_SPEED{"0.05"};
    MiniNumber MINIMA_BLOCKS_SPEED_CALC{16};
    MiniNumber MINIMA_CONFIRM_DEPTH{"3"};
    MiniNumber MINIMA_CASCADE_FREQUENCY{3};
    MiniNumber MINIMA_CASCADE_START_DEPTH{32};
    int32_t MINIMA_CASCADE_LEVEL_NODES=4;
    int32_t MINIMA_CASCADE_LEVELS=32;
    MiniNumber MINIMA_MMR_PROOF_HISTORY{8};
    int32_t MEDIAN_BLOCK_CALC=8;
    static void setTestParams();
};
}
