#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <array>
#include <optional>
namespace minima {
class BIP39 {
public:
    using Words=std::vector<std::optional<std::string>>;
    // Like Java's final array: the collection length is fixed, entries mutable.
    static std::array<std::optional<std::string>,2048> WORD_LIST;
    static Words getNewWordList(const std::string& language={});
    static std::string convertWordListToString(const Words& words);
    static MiniData convertWordListToSeed(const Words& words);
    static MiniData convertStringToSeed(const std::optional<std::string>& phrase);
    static std::string cleanSeedPhrase(const std::optional<std::string>& phrase,const std::string& language={});
    static void main();
};
}
