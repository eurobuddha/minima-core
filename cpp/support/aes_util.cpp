#include "org/minima/utils/encrypt/javajs/AesUtil.hpp"
#include "org/minima/utils/json/JSONValue.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
using namespace minima;
namespace minima {
class AesUtilFixture {
public:
    static auto derive(AesUtil& aes, const AesUtil::Text& salt, const AesUtil::Text& password) { return aes.generateKey(salt, password); }
    static auto final(AesUtil& aes, int mode, const AesUtil::Bytes& key, const AesUtil::Text& iv, const AesUtil::Bytes& data) {
        std::optional<SecretKey> secret;
        if (key) secret = SecretKey{*key};
        return aes.doFinal(mode, secret, iv, data);
    }
    static bool fail(AesUtil& aes) { return aes.fail(std::runtime_error("fixture")) == nullptr; }
};
}
static int integer(const json::JSONValue& v) { return int(std::get<int64_t>(v.value)); }
static AesUtil::Text text(const json::JSONValue& v) {
    if (v.isNull()) return std::nullopt;
    std::u16string out;
    for (const auto& c : std::get<std::shared_ptr<json::JSONArray>>(v.value)->elements()) out.push_back(char16_t(integer(c)));
    return out;
}
static AesUtil::Bytes bytes(const json::JSONValue& v) {
    if (v.isNull()) return std::nullopt;
    return MiniData::fromHex(v.asString()).bytes();
}
static std::string units(const std::u16string& value) {
    std::string out;
    constexpr char digits[] = "0123456789abcdef";
    for (uint16_t c : value) for (int shift = 12; shift >= 0; shift -= 4) out += digits[(c >> shift) & 15];
    return out;
}
static json::JSONArray textResult(const AesUtil::Text& value) {
    json::JSONArray out; out.add(value ? "text" : "null"); if (value) out.add(units(*value)); return out;
}
static json::JSONArray bytesResult(const AesUtil::Bytes& value) {
    json::JSONArray out; out.add(value ? "bytes" : "null"); if (value) out.add(MiniData(*value).to0xString()); return out;
}
int main(int argc, char**) {
    if (argc > 1) { AesUtil::main(std::cout); return 0; }
    std::cout << std::unitbuf;
    std::string line;
    while (std::getline(std::cin, line)) {
        auto request = std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);
        AesUtil aes(integer(request->get("bits")), integer(request->get("iterations")));
        json::JSONArray results;
        for (const auto& value : std::get<std::shared_ptr<json::JSONArray>>(request->get("ops").value)->elements()) {
            auto op = std::get<std::shared_ptr<json::JSONObject>>(value.value);
            try {
                auto name = op->get("op").asString();
                json::JSONArray out;
                if (name == "encrypt") out = textResult(aes.encrypt(text(op->get("salt")), text(op->get("iv")), text(op->get("password")), text(op->get("data"))));
                else if (name == "decrypt") out = textResult(aes.decrypt(text(op->get("salt")), text(op->get("iv")), text(op->get("password")), text(op->get("data"))));
                else if (name == "derive") { auto key = AesUtilFixture::derive(aes, text(op->get("salt")), text(op->get("password"))); out = bytesResult(key ? AesUtil::Bytes(key->bytes) : std::nullopt); }
                else if (name == "decode") out = bytesResult(AesUtil::base64Decode(text(op->get("data"))));
                else if (name == "encode") out = textResult(AesUtil::base64Encode(bytes(op->get("data"))));
                else if (name == "hex") out = bytesResult(AesUtil::hex(text(op->get("data"))));
                else if (name == "final") out = bytesResult(AesUtilFixture::final(aes, integer(op->get("mode")), bytes(op->get("key")), text(op->get("iv")), bytes(op->get("data"))));
                else if (name == "raw") {
                    auto key = AesUtilFixture::derive(aes, text(op->get("salt")), text(op->get("password")));
                    auto encrypted = GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE, AesUtil::hex(text(op->get("iv"))), key->bytes).doFinal(*bytes(op->get("data")));
                    out = textResult(AesUtil::base64Encode(encrypted));
                } else if (name == "fail") { out.add(AesUtilFixture::fail(aes) ? "null" : "unexpected"); }
                else throw std::invalid_argument("Unknown fixture operation");
                results.add(out);
            } catch (const std::exception& e) {
                auto description = javaExceptionText(e);
                auto split = description.find(": ");
                auto cls = description.substr(0, split);
                auto message = split == std::string::npos ? "" : description.substr(split + 2);
                if (cls == "java.lang.NullPointerException") message.clear();
                json::JSONArray out; out.add("error"); out.add(cls); out.add(units(unicode::decodeUTF8(message))); results.add(out);
            }
        }
        std::cout << results.toJSONString() << '\n';
    }
}
