#pragma once
#include "SqlConnection.hpp"
namespace minima {
class NativeH2Engine;
// Owns an AOT-compiled H2 isolate, not a JVM or a Java Minima process. Returned
// connectors/connections retain this owner, including across native workers.
class NativeH2Runtime {
    std::shared_ptr<NativeH2Engine> mEngine;
public:
    NativeH2Runtime();
    SqlConnect connector() const;
    using NullableConnect=std::function<std::shared_ptr<SqlConnection>(const std::string&,const std::optional<std::string>&,const std::optional<std::string>&)>;
    NullableConnect nullableConnector() const;
    std::string version() const;
    int32_t liveHandles() const;
};
}
