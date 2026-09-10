#pragma once
#include "StatementBlock.hpp"
#include "../tokens/ScriptToken.hpp"
namespace minima::kissvm {
class StatementParser {
public:
    static std::shared_ptr<StatementBlock> parseTokens(const std::vector<ScriptToken>& tokens,int depth);
};
}
