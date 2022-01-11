#ifndef TKOMSIUNITS_PARSER_H_INCLUDED
#define TKOMSIUNITS_PARSER_H_INCLUDED

#include "lexer/Lexer.h"
#include "codeObjects/Program.h"
#include "codeObjects/Instruction.h"
#include "codeObjects/FuncCall.h"
#include "codeObjects/VarDef.h"
#include "codeObjects/Expression.h"
#include "codeObjects/BinaryExpression.h"
#include "codeObjects/If.h"
#include "codeObjects/While.h"
#include "codeObjects/Unit.h"
#include "codeObjects/Type.h"
#include "codeObjects/Type2.h"
#include "codeObjects/Variable.h"
#include <memory>
#include <optional>

template <typename TokenSource = Lexer>
class Parser {
public:
    Parser(TokenSource &lexer);
    
    std::unique_ptr<Program> parse();

protected:
    void advance();
    std::unique_ptr<FuncDef> parseFuncDef();
    std::unique_ptr<Instruction> parseInstruction();
    std::vector<std::unique_ptr<Instruction>> parseInstructionBlock();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAddExpression();
    std::unique_ptr<Expression> parseMultExpression();
    std::unique_ptr<Expression> parseExpressionElement();

    codeobj::Unit parseUnit();
    codeobj::Unit parseUnitTokens();
    std::optional<Type2> parseType();
    Type2 parseTypeTokens();
    //std::unique_ptr<codeObj::Unit> parseUnitExpressionElement()
    
    std::optional<Variable> parseFuncParameter();

    std::unique_ptr<FuncCall> tryParseFuncCall(Token id);
    std::unique_ptr<VarDef> tryParseVarDef(Token id);
    std::unique_ptr<If> tryParseIfInstr();
    std::unique_ptr<While> tryParseWhileInstr();
    
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
