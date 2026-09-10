#pragma once
#include "../Command.hpp"
#include "org/minima/objects/TxPoW.hpp"
namespace minima::commands {
class burn:public Command {
public:
    using Values=std::vector<std::shared_ptr<MiniNumber>>;
    burn();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    virtual void checkBurn(TxPoW* txpow,int32_t counter);
    virtual std::shared_ptr<MiniNumber> getMediaValue(Values* values);
    std::shared_ptr<MiniNumber> getMediaValue(Values& values){return getMediaValue(&values);}
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<burn>();}
private:
    struct Window {MiniNumber minimum=MiniNumber::BILLION,maximum=MiniNumber::ZERO,total=MiniNumber::ZERO,count=MiniNumber::ZERO;Values values;};
    Window block1,block10,block50;
    json::JSONObject metrics(Window& window);
};
}
