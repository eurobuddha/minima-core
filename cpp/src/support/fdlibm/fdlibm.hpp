#pragma once
#include <bit>
#include <cstdint>
namespace minima::fdlibm {
inline int32_t high(double value) {return std::bit_cast<int32_t>(uint32_t(std::bit_cast<uint64_t>(value)>>32));}
inline uint32_t low(double value) {return uint32_t(std::bit_cast<uint64_t>(value));}
inline double withHigh(double value,int32_t word) {return std::bit_cast<double>((uint64_t(uint32_t(word))<<32)|low(value));}
double log(double value);
double log10(double value);
}
