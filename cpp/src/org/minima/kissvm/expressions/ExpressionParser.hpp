#pragma once
#include "Expression.hpp"
#include "../tokens/LexicalTokenizer.hpp"
namespace minima::kissvm {
class ExpressionParser {
    static ExpressionPtr getRelation(LexicalTokenizer& tokens);
    static ExpressionPtr getLogic(LexicalTokenizer& tokens);
    static ExpressionPtr getAddSub(LexicalTokenizer& tokens);
    static ExpressionPtr getMulDiv(LexicalTokenizer& tokens);
    static ExpressionPtr getPrimary(LexicalTokenizer& tokens);
    static ExpressionPtr getBaseUnit(LexicalTokenizer& tokens);
public:
    static ExpressionPtr getExpression(const std::vector<ScriptToken>& tokens,int depth);
    static ExpressionPtr getExpression(LexicalTokenizer& tokens);
};
}
