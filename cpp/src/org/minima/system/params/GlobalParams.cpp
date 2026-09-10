#include "GlobalParams.hpp"
namespace minima {
std::string GlobalParams::MINIMA_BASE_VERSION = "1.1";
std::string GlobalParams::MINIMA_BUILD_NUMBER = "2";
std::string GlobalParams::MINIMA_VERSION = MINIMA_BASE_VERSION+"."+MINIMA_BUILD_NUMBER;
std::string GlobalParams::MINIMA_MICRO_BUILD = "8";
MiniNumber GlobalParams::MINIMA_BLOCK_SPEED = MiniNumber("0.02");
MiniNumber GlobalParams::MINIMA_BLOCKS_SPEED_CALC = MiniNumber(256);
MiniNumber GlobalParams::MINIMA_CONFIRM_DEPTH = MiniNumber("3");
MiniNumber GlobalParams::MINIMA_CASCADE_FREQUENCY = MiniNumber(100);
MiniNumber GlobalParams::MINIMA_CASCADE_START_DEPTH = MiniNumber(1024);
int32_t GlobalParams::MINIMA_CASCADE_LEVEL_NODES = 256;
int32_t GlobalParams::MINIMA_CASCADE_LEVELS = 32;
MiniNumber GlobalParams::MINIMA_MMR_PROOF_HISTORY = MiniNumber(256);
int32_t GlobalParams::MEDIAN_BLOCK_CALC = 64;
std::string GlobalParams::getFullMicroVersion() {return MINIMA_VERSION+"."+MINIMA_MICRO_BUILD;}
}
