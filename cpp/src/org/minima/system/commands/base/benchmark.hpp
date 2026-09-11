#pragma once
#include "../Command.hpp"
namespace minima::commands {
class benchmark:public Command {
private:
    friend class JNICommandFixture;
    static const MiniNumber START_NONCE_BYTES;
    static std::string outputByteArray(const std::vector<uint8_t>& bytes);
public:
    benchmark();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<benchmark>();}
};
}
