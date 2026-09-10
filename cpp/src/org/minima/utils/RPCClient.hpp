#pragma once
#include <curl/curl.h>
#include <functional>
#include <memory>
#include "support/JavaStringReference.hpp"
#include <optional>
#include <string>
namespace minima {
class RPCClient {
public:
    using String=std::optional<std::string>;
    // Native equivalent of the caller-supplied SSLContext socket configuration.
    // The callback configures this connection's native TLS options, including
    // its trust/key material. It must outlive the synchronous call.
    struct SSLContext {
        std::function<void(CURL*)> configure;
        explicit SSLContext(std::function<void(CURL*)> configuration):configure(std::move(configuration)){}
    };
    using UserAgent=JavaStringReference;
    inline static UserAgent USER_AGENT{String("Minima/1.0")};
    static std::string sendGET(const String& host);
    static std::string sendGETBasicAuth(const String& host,const String& user,const String& password);
    static std::string sendGETHTTPS(const String& host);
    static std::string sendGETSSL(const String& host);
    static std::string sendGETBasicAuthSSL(const String& host,const String& user,const String& password,const std::shared_ptr<SSLContext>& context);
    static std::string sendPUT(const String& host);
    static std::string sendPOST(const String& host,const String& params);
    static std::string sendPOST(const String& host,const String& params,const String& type);
    static std::string sendPOSTHTTPS(const String& host,const String& params,const String& type);
    static std::string sendGETAuth(const String& host,const String& authToken);
};
}
