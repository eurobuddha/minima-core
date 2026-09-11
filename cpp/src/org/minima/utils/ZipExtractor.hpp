#pragma once
#include <filesystem>
#include <functional>
#include <optional>
#include <span>
#include <cstdint>
namespace minima {
class ZipExtractor {
public:
    using Reader=std::function<size_t(std::span<uint8_t>)>;
    static void unzip(Reader archive,const std::optional<std::filesystem::path>& destination,std::function<void()> close={});
private:
    static std::filesystem::path newFile(const std::optional<std::filesystem::path>& destination,const std::string& name);
    friend class ZipExtractorFixture;
};
}
