#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
namespace minima {
struct Maths {
    static void main(const std::vector<std::string>* args=nullptr);
    static const BigInt BI_TWO;
    static const MiniNumber BD_TWO;
    static double log2(double value);
    static double log2BI(const BigInt& value);
    static int getSuperLevel(const MiniData& difficulty,const MiniData& actual);
    static int compareVersions(const std::string& first,const std::string& second);
};
}
