#pragma once
// Formatting adapters for the original tests' observations, not production substitutes.
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/objects/mmr/MMREntryNumber.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "support/ByteIO.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaStringIgnoreCase.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
namespace minima {
inline std::ostream& operator<<(std::ostream& out,const MiniNumber& value){return out<<value.toString();}
inline std::ostream& operator<<(std::ostream& out,const MiniData& value){return out<<value.to0xString();}
inline std::ostream& operator<<(std::ostream& out,const BigInt& value){return out<<value.toDecimalString();}
inline std::ostream& operator<<(std::ostream& out,const MMREntryNumber& value){return out<<value.toBigDecimalString();}
inline bool decimalEquals(const MMREntryNumber& a,const MMREntryNumber& b){return a.getScale()==b.getScale()&&a.getUnscaled()==b.getUnscaled();}
template<size_t N> std::string arrayText(const std::array<MiniData,N>& values){std::string text="[";for(size_t i=0;i<N;++i){if(i)text+=", ";text+=values[i].to0xString();}return text+"]";}
}
inline std::ostream& operator<<(std::ostream& out,const std::vector<uint8_t>& bytes){
    // Java's array identity spelling has no cross-process equality guarantee.
    std::ostringstream identity;identity<<std::hex<<reinterpret_cast<uintptr_t>(&bytes);
    return out<<"[B@"<<identity.str();
}
