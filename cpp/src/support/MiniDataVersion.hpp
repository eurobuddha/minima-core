#pragma once
#include <optional>
#include "org/minima/objects/base/MiniData.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
// Extracted from FORSSignature: Java MiniData.getMiniDataVersion catches only
// IOException, logs it, and returns null. Runtime exceptions still propagate.
template<class T> std::optional<MiniData> miniDataVersion(const T& object) {
    try {return MiniData(object.serialise());}
    catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}
}
}
