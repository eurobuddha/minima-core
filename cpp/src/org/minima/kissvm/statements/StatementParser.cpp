#include "StatementParser.hpp"
#include "../Contract.hpp"
#include "../expressions/ConstantExpression.hpp"
#include "../expressions/ExpressionParser.hpp"
#include "commands/ASSERTstatement.hpp"
#include "commands/RETURNstatement.hpp"
#include "commands/EXECstatement.hpp"
#include "commands/MASTstatement.hpp"
#include "commands/LETstatement.hpp"
#include "commands/IFstatement.hpp"
#include "commands/WHILEstatement.hpp"
#include <bit>
namespace minima::kissvm {
namespace {
using Tokens=std::vector<ScriptToken>;
Tokens nextCommand(const Tokens& tokens,size_t position) {Tokens result;while(position<tokens.size()&&tokens[position].getTokenType()!=ScriptToken::TOKEN_COMMAND) result.push_back(tokens[position++]);return result;}
Tokens required(const Tokens& tokens,size_t position,const std::string& required) {Tokens result;while(position<tokens.size()) {if(tokens[position].getToken()==required) return result;result.push_back(tokens[position++]);}throw MinimaParseException("Could not find required token : "+required);}
Tokens nextEquals(const Tokens& tokens,size_t position) {Tokens result;while(position<tokens.size()) {const auto& token=tokens[position++];if(token.getTokenType()==ScriptToken::TOKEN_OPERATOR&&token.getToken()=="=") break;result.push_back(token);}return result;}
// Equivalent to Java's recursive boundary scanners, without consuming native
// call stack while looking for delimiters. Missing terminators are left to the
// caller exactly as in the Java parser (including its lenient WHILE behavior).
Tokens boundary(const Tokens& tokens,size_t position,bool isIf) {
    Tokens result;int depth=0;
    for(;position<tokens.size();++position) {
        const auto& token=tokens[position];result.push_back(token);if(token.getTokenType()!=ScriptToken::TOKEN_COMMAND) continue;auto text=token.getToken();
        if(text==(isIf?"IF":"WHILE")) ++depth;
        else if(text==(isIf?"ENDIF":"ENDWHILE")) {if(depth==0) break;--depth;}
        else if(isIf&&depth==0&&(text=="ELSE"||text=="ELSEIF")) break;
    }return result;
}
std::string commandCase(std::string text) {for(auto& c:text) if(c>='a'&&c<='z') c-=32;return text;}
}
std::shared_ptr<StatementBlock> StatementParser::parseTokens(const Tokens& tokens,int depth) {
    depth=std::bit_cast<int32_t>(uint32_t(depth)+1);if(depth>Contract::MAX_STACK_DEPTH) throw MinimaParseException("Stack too deep (MAX 64) "+std::to_string(depth));
    std::vector<StatementPtr> statements;size_t position=0;
    while(position<tokens.size()) {
        auto token=tokens[position++];auto command=commandCase(token.getToken());
        if(token.getTokenType()!=ScriptToken::TOKEN_COMMAND) throw MinimaParseException("Invalid Token where there should be a COMMMAND - "+token.getToken());
        if(command=="LET") {
            auto variable=tokens.at(position++);
            if(variable.getTokenType()==ScriptToken::TOKEN_OPENBRACKET) {
                auto array=nextEquals(tokens,position);position+=array.size();variable=array.at(array.size()-1);
                if(variable.getTokenType()!=ScriptToken::TOKEN_CLOSEBRACKET) throw MinimaParseException("Incorrect LET statement, missing ) .. "+variable.getToken()+")");
                variable=tokens.at(position++);
                if(variable.getTokenType()!=ScriptToken::TOKEN_OPERATOR&&variable.getToken()!="=") throw MinimaParseException("Incorrect LET statement, missing = (.."+variable.getToken()+")");
                array.pop_back();if(array.empty()) throw MinimaParseException("Incorrect LET statement, EMPTY ARRAY POS @ "+std::to_string(position));
                LexicalTokenizer lexical(array,depth);std::vector<ExpressionPtr> expressions;while(!lexical.checkAllTokensUsed()) expressions.push_back(ExpressionParser::getExpression(lexical));
                auto expressionTokens=nextCommand(tokens,position);position+=expressionTokens.size();auto expression=ExpressionParser::getExpression(expressionTokens,depth);statements.push_back(std::make_shared<LETstatement>(std::move(expressions),expression));
            } else if(variable.getTokenType()==ScriptToken::TOKEN_VARIABLE) {
                auto name=variable.getToken();variable=tokens.at(position++);if(variable.getToken()!="=") throw MinimaParseException("Incorrect LET statement, missing = (.."+variable.getToken()+")");
                auto expressionTokens=nextCommand(tokens,position);position+=expressionTokens.size();statements.push_back(std::make_shared<LETstatement>(name,ExpressionParser::getExpression(expressionTokens,depth)));
            } else throw MinimaParseException("Not a variable or array after LET (.."+variable.getToken()+")");
        } else if(command=="EXEC"||command=="MAST"||command=="ASSERT"||command=="RETURN") {
            auto expressionTokens=nextCommand(tokens,position);position+=expressionTokens.size();auto expression=ExpressionParser::getExpression(expressionTokens,depth);
            if(command=="EXEC") statements.push_back(std::make_shared<EXECstatement>(expression));
            else if(command=="MAST") statements.push_back(std::make_shared<MASTstatement>(expression));
            else if(command=="ASSERT") statements.push_back(std::make_shared<ASSERTstatement>(expression));
            else statements.push_back(std::make_shared<RETURNstatement>(expression));
        } else if(command=="IF") {
            auto statement=std::make_shared<IFstatement>();auto conditionTokens=required(tokens,position,"THEN");auto condition=ExpressionParser::getExpression(conditionTokens,depth);position+=conditionTokens.size()+1;
            while(true) {
                auto actionTokens=boundary(tokens,position,true);position+=actionTokens.size();auto next=actionTokens.at(actionTokens.size()-1).getToken();actionTokens.pop_back();
                auto action=parseTokens(actionTokens,depth);statement->addCondition(condition,action);if(next=="ENDIF") break;
                if(next=="ELSE") condition=std::make_shared<ConstantExpression>(Value::getValue("TRUE"));
                else if(next=="ELSEIF") {conditionTokens=required(tokens,position,"THEN");condition=ExpressionParser::getExpression(conditionTokens,depth);position+=conditionTokens.size()+1;}
                else throw MinimaParseException("MISSING ELSE or ELSEIF in IF Statement");
            }statements.push_back(statement);
        } else if(command=="WHILE") {
            auto conditionTokens=required(tokens,position,"DO");auto condition=ExpressionParser::getExpression(conditionTokens,depth);position+=conditionTokens.size()+1;
            auto actionTokens=boundary(tokens,position,false);position+=actionTokens.size();if(actionTokens.empty()) throw std::out_of_range("Empty WHILE action tokens");actionTokens.pop_back();auto action=parseTokens(actionTokens,depth);statements.push_back(std::make_shared<WHILEstatement>(condition,action));
        } else throw MinimaParseException("Invalid Token where there should be a Command - "+token.getToken());
    }
    return std::make_shared<StatementBlock>(std::move(statements));
}
}
