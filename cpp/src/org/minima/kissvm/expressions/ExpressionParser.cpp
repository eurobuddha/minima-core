#include "ExpressionParser.hpp"
#include "ConstantExpression.hpp"
#include "VariableExpression.hpp"
#include "GlobalExpression.hpp"
#include "BooleanExpression.hpp"
#include "OperatorExpression.hpp"
#include "FunctionExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
ExpressionPtr ExpressionParser::getExpression(const std::vector<ScriptToken>& tokens,int depth) {
    if(tokens.empty()) throw MinimaParseException("Cannot have EMPTY expression");LexicalTokenizer lexical(tokens,depth);auto result=getExpression(lexical);
    if(!lexical.checkAllTokensUsed()) throw MinimaParseException("Incorrect token number in expression @ "+lexical.getNextToken().getToken());return result;
}
ExpressionPtr ExpressionParser::getExpression(LexicalTokenizer& tokens) {
    if(tokens.getStackDepth()>Contract::MAX_STACK_DEPTH) throw MinimaParseException("Stack too deep (MAX 64) "+std::to_string(tokens.getStackDepth()));
    auto result=getRelation(tokens);while(tokens.hasMoreElements()) {auto text=tokens.getNextToken().getToken();
        if(text=="AND") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_AND);}
        else if(text=="OR") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_OR);}
        else if(text=="XOR") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_XOR);}
        else if(text=="NAND") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_NAND);}
        else if(text=="NOR") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_NOR);}
        else if(text=="NXOR") {auto right=getRelation(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_NXOR);}
        else {tokens.goBackToken();break;}
    }return result;
}
ExpressionPtr ExpressionParser::getRelation(LexicalTokenizer& tokens) {
    auto result=getLogic(tokens);while(tokens.hasMoreElements()) {auto text=tokens.getNextToken().getToken();
        if(text=="EQ") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_EQ);}
        else if(text=="NEQ") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_NEQ);}
        else if(text=="GT") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_GT);}
        else if(text=="GTE") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_GTE);}
        else if(text=="LT") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_LT);}
        else if(text=="LTE") {auto right=getLogic(tokens);result=std::make_shared<BooleanExpression>(result,right,BooleanExpression::BOOLEAN_LTE);}
        else {tokens.goBackToken();break;}
    }return result;
}
ExpressionPtr ExpressionParser::getLogic(LexicalTokenizer& tokens) {
    auto result=getAddSub(tokens);while(tokens.hasMoreElements()) {auto text=tokens.getNextToken().getToken();
        if(text=="&") {auto right=getAddSub(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_AND);}
        else if(text=="|") {auto right=getAddSub(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_OR);}
        else if(text=="^") {auto right=getAddSub(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_XOR);}
        else {tokens.goBackToken();break;}
    }return result;
}
ExpressionPtr ExpressionParser::getAddSub(LexicalTokenizer& tokens) {
    auto result=getMulDiv(tokens);while(tokens.hasMoreElements()) {auto text=tokens.getNextToken().getToken();
        if(text=="+") {auto right=getMulDiv(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_ADD);}
        else if(text=="-") {auto right=getMulDiv(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_SUB);}
        else if(text=="%") {auto right=getMulDiv(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_MODULO);}
        else if(text=="<<") {auto right=getMulDiv(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_SHIFTL);}
        else if(text==">>") {auto right=getMulDiv(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_SHIFTR);}
        else {tokens.goBackToken();break;}
    }return result;
}
ExpressionPtr ExpressionParser::getMulDiv(LexicalTokenizer& tokens) {
    auto result=getPrimary(tokens);while(tokens.hasMoreElements()) {auto text=tokens.getNextToken().getToken();
        if(text=="*") {auto right=getPrimary(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_MUL);}
        else if(text=="/") {auto right=getPrimary(tokens);result=std::make_shared<OperatorExpression>(result,right,OperatorExpression::OPERATOR_DIV);}
        else {tokens.goBackToken();break;}
    }return result;
}

ExpressionPtr ExpressionParser::getPrimary(LexicalTokenizer& tokens) {
    auto text=tokens.getNextToken().getToken();
    if(text=="NOT") return std::make_shared<BooleanExpression>(getPrimary(tokens),BooleanExpression::BOOLEAN_NOT);
    if(text=="NEG") return std::make_shared<OperatorExpression>(getPrimary(tokens),OperatorExpression::OPERATOR_NEG);
    if(text=="~") return std::make_shared<OperatorExpression>(getPrimary(tokens),OperatorExpression::OPERATOR_NOT);
    tokens.goBackToken();return getBaseUnit(tokens);
}
ExpressionPtr ExpressionParser::getBaseUnit(LexicalTokenizer& tokens) {
    auto token=tokens.getNextToken();auto text=token.getToken();int type=token.getTokenType();
    if(type==ScriptToken::TOKEN_VALUE) return std::make_shared<ConstantExpression>(Value::getValue(text));
    if(text=="-") {auto number=MiniNumber(tokens.getNextToken().getToken()).mult(MiniNumber::MINUSONE);return std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(number));}
    if(type==ScriptToken::TOKEN_GLOBAL) return std::make_shared<GlobalExpression>(text);
    if(type==ScriptToken::TOKEN_VARIABLE) return std::make_shared<VariableExpression>(text);
    if(type==ScriptToken::TOKEN_TRUE||type==ScriptToken::TOKEN_FALSE) return std::make_shared<ConstantExpression>(Value::getValue(type==ScriptToken::TOKEN_TRUE?"TRUE":"FALSE"));
    if(type==ScriptToken::TOKEN_FUNCTIION) {
        auto function=MinimaFunction::getFunction(text);
        if(tokens.getNextToken().getTokenType()!=ScriptToken::TOKEN_OPENBRACKET) throw MinimaParseException("Missing opening bracket at start of function "+function->getName());
        while(true) {
            if(tokens.getNextToken().getTokenType()==ScriptToken::TOKEN_CLOSEBRACKET) break;
            tokens.goBackToken();tokens.incrementStackDepth();function->addParameter(getExpression(tokens));
            if(function->getParameterNum()>Contract::MAX_FUNCTION_PARAMS) throw MinimaParseException("Too many function params, max 32");tokens.decrementStackDepth();
        }
        function->checkParamNumberCorrect();return std::make_shared<FunctionExpression>(function);
    }
    if(type==ScriptToken::TOKEN_OPENBRACKET) {
        tokens.incrementStackDepth();auto result=getExpression(tokens);tokens.decrementStackDepth();auto close=tokens.getNextToken();
        if(close.getTokenType()!=ScriptToken::TOKEN_CLOSEBRACKET) throw MinimaParseException("Missing close bracket. Found : "+close.getToken());return result;
    }
    throw MinimaParseException("Incorrect Token in script "+text+" @ "+std::to_string(tokens.getCurrentPosition()));
}
}
