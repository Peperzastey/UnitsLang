#ifndef TKOMSIUNITS_PARSER_H_INCLUDED
#define TKOMSIUNITS_PARSER_H_INCLUDED

#include "lexer/Lexer.h"
#include "codeObjects/Program.h"
#include "codeObjects/Instruction.h"
#include "codeObjects/FuncCall.h"
#include "codeObjects/VarDef.h"
#include "codeObjects/Expression.h"
#include "codeObjects/BinaryExpression.h"
#include <memory>

template <typename TokenSource = Lexer>
class Parser {
public:
    Parser(TokenSource &lexer);
    
    std::unique_ptr<Program> parse();

protected:
    void advance();
    std::unique_ptr<FuncDef> parseFuncDef();
    std::unique_ptr<Instruction> parseInstruction();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAddExpression();
    std::unique_ptr<Expression> parseMultExpression();
    std::unique_ptr<Expression> parseExpressionElement();
    //std::unique_ptr<Expression> parseParenthesisedExpression();

    std::unique_ptr<FuncCall> tryParseFuncCall(Token id);
    std::unique_ptr<VarDef> tryParseVarDef(Token id);
    
    Token requireToken(TokenType expected);
    template <std::size_t N>
    void reportUnexpectedToken(const std::array<TokenType, N> &expected);
    void skipTokens(TokenType toBeSkipped);

private:
    TokenSource &lexer_;
    Token currToken_;
};

#include "Parser.hpp"

#endif // TKOMSIUNITS_PARSER_H_INCLUDED
