#pragma once
#include <cstdint>
#include <optional>
#include <string>
namespace minima {
// The database driver supplies JDBC-equivalent column conversion and failures.
// This interface does not select or substitute a persistent database format.
class SqlRow {
public:
    virtual ~SqlRow()=default;
    virtual std::optional<std::string> getString(const std::string& column)=0;
    virtual int32_t getInt(const std::string& column)=0;
};
}
