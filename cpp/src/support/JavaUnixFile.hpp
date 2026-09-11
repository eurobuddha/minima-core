#pragma once
#include <filesystem>
#include <optional>
#include <cstdint>
#include <sys/stat.h>
namespace minima {
inline int64_t javaUnixFileLength(const std::filesystem::path& file) {
    auto path=file.string();if(path.find('\0')!=std::string::npos)return 0;
    struct stat info{};return ::stat(path.c_str(),&info)==0?int64_t(info.st_size):0;
}
inline std::string javaUnixAbsoluteFile(const std::filesystem::path& path) {
    return path.empty() ? std::filesystem::current_path().string() : std::filesystem::absolute(path).string();
}
// java.io.File's Unix constructor normalization, extracted from SqlDB.
inline std::optional<std::filesystem::path> javaUnixFile(const std::optional<std::filesystem::path>& file) {
    if (!file) return std::nullopt;
    std::string path;
    for (char c : file->string())
        if (c != '/' || path.empty() || path.back() != '/') path += c;
    while (path.size() > 1 && path.back() == '/') path.pop_back();
    return std::filesystem::path(path);
}
// File.mkdirs first tries the supplied path, then resolves its canonical form
// before recursively creating parents. In particular, do not create a missing
// "dot" directory merely because the requested path contains "dot/../inside".
inline bool javaUnixMkdirs(const std::filesystem::path& path) {
    const auto text=path.string();if(text.empty()||text.find('\0')!=text.npos)return false;
    std::error_code error;
    if(std::filesystem::exists(path,error))return false;
    if(::mkdir(text.c_str(),0777)==0)return true;
    auto absolute=std::filesystem::absolute(path,error);if(error)return false;
    auto canonical=std::filesystem::weakly_canonical(absolute,error);
    if(error||canonical.empty()||canonical==canonical.root_path())return false;
    // File.getCanonicalFile strips a trailing separator. Without this step,
    // missing/.. can make parent_path() equal the canonical directory itself.
    auto canonicalText=canonical.string();
    while(canonicalText.size()>1&&canonicalText.back()=='/')canonicalText.pop_back();
    canonical=std::filesystem::path(canonicalText);
    auto parent=canonical.parent_path();
    return !parent.empty()&&(javaUnixMkdirs(parent)||std::filesystem::exists(parent,error))&&::mkdir(canonical.c_str(),0777)==0;
}
// Unix File(parent, child): an absolute child still belongs below the parent.
inline std::filesystem::path javaUnixChildFile(const std::filesystem::path& parent,const std::string& child) {
    auto base=javaUnixFile(parent)->string();if(base.empty())base="/";
    auto name=javaUnixFile(std::filesystem::path(child))->string();
    return *javaUnixFile(std::filesystem::path(base+"/"+name));
}
}
