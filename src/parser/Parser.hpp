//#include "Parser.h"
#ifndef TKOMSIUNITS_PARSER_HPP_INCLUDED
#define TKOMSIUNITS_PARSER_HPP_INCLUDED

#include "codeObjects/VarReference.h"
#include "codeObjects/NumberValue.h"
#include "codeObjects/Return.h"
#include "codeObjects/Break.h"
#include "codeObjects/Continue.h"

#include "error/ErrorHandler.h"
#include "utils/printUtils.h"
#include <cassert>
#include <vector>
#include <sstream>

template <typename TokenSource>
Parser<TokenSource>::Parser(TokenSource &lexer)
    : lexer_(lexer) {
    }

template <typename TokenSource>
void Parser<TokenSource>::advance() {
    currToken_ = lexer_.getToken();
}

template <typename TokenSource>
Token Parser<TokenSource>::requireToken(TokenType expected) {
    if (currToken_.type != expected) {
        reportUnexpectedToken(std::array{expected});
    }
    Token token = currToken_;
    advance();
    return token;
}

template <typename TokenSource>
template <std::size_t N>
void Parser<TokenSource>::reportUnexpectedToken(const std::array<TokenType, N> &expected/*, const std::string &rule*/) {
    std::cout << "unexpected token\n";
    std::ostringstream os;
    os << "Unexpected token: {" << currToken_ << "}, expecting: [";
    for (const auto &tokenType : expected) {
        os << tokenType << ' ';
    }
    os << ']';
    ErrorHandler::handleFromParser(os.str());
}

template <typename TokenSource>
void Parser<TokenSource>::skipTokens(TokenType toBeSkipped) {
    while (currToken_.type == toBeSkipped) {
        advance();
    }
}

template <typename TokenSource>
std::unique_ptr<Program> Parser<TokenSource>::parse() {
    std::unique_ptr<Instruction> instr;
    std::unique_ptr<FuncDef> funcDef;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<std::unique_ptr<FuncDef>> funcDefs;
    
    advance();
    while ((funcDef = parseFuncDef())
        || (instr = parseInstruction())) {
        if (funcDef) {
            funcDefs.push_back(std::move(funcDef));
        } else {
            instructions.push_back(std::move(instr));
        }
    }

    return std::make_unique<Program>(
            std::move(funcDefs),
            std::move(instructions)
        );
}

template <typename TokenSource>
std::unique_ptr<Instruction> Parser<TokenSource>::parseInstruction() {
    std::unique_ptr<Instruction> instr = nullptr;
    switch (currToken_.type) {
        case TokenType::ID: {
            // VarDef or FuncCall or VarAssign or SuffixInstr
            Token id = currToken_;
            advance();
            instr = tryParseFuncCall(id);
            if (!instr) {
                instr = tryParseVarDef(id);
            }
            //...
            break;
        }
        case TokenType::KEYWORD_RETURN: {
            advance();
            instr = std::make_unique<Return>(parseExpression());
            break;
        }
        case TokenType::KEYWORD_BREAK: {
            advance();
            instr = std::make_unique<Break>();
            break;
        }
        case TokenType::KEYWORD_CONTINUE: {
            advance();
            instr = std::make_unique<Continue>();
            break;
        }
        case TokenType::KEYWORD_IF: {
            advance();
            instr = tryParseIfInstr();
            break;
        }
        case TokenType::KEYWORD_WHILE: {
            advance();
            instr = tryParseWhileInstr();
            break;
        }
        default:
            return nullptr;
    }
    requireToken(TokenType::END_OF_INSTRUCTION);
    return instr;
}

template <typename TokenSource>
std::vector<std::unique_ptr<Instruction>> Parser<TokenSource>::parseInstructionBlock() {
    requireToken(TokenType::BRACKET_OPEN);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    //TODO allow empty lines!!! also between instructions!
    std::vector<std::unique_ptr<Instruction>> block;
    while (std::unique_ptr<Instruction> instr = parseInstruction()) {
        block.push_back(std::move(instr));
    }
    requireToken(TokenType::BRACKET_CLOSE);
    return block;
}

template <typename TokenSource>
std::unique_ptr<FuncCall> Parser<TokenSource>::tryParseFuncCall(Token id) {
    assert(id.type == TokenType::ID);
    if (currToken_.type != TokenType::PAREN_OPEN) {
        return nullptr;
    }
    advance();
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse arguments
    requireToken(TokenType::PAREN_CLOSE);
    return std::make_unique<FuncCall>(std::get<std::string>(id.value));
}

template <typename TokenSource>
std::unique_ptr<VarDef> Parser<TokenSource>::tryParseVarDef(Token id) {
    assert(id.type == TokenType::ID);
    //TODO optional type designation
    if (currToken_.type != TokenType::ASSIGN) {
        return nullptr;
    }
    advance();
    std::unique_ptr<Expression> expr = parseExpression();
    if (!expr) {
        ErrorHandler::handleFromParser("Expected expression after '=' in variable definition");
    }
    return std::make_unique<VarDef>(std::get<std::string>(id.value));
}

template <typename TokenSource>
std::unique_ptr<If> Parser<TokenSource>::tryParseIfInstr() {
    std::unique_ptr<Expression> cond = parseExpression();

    std::vector<std::unique_ptr<Instruction>> positiveBlock = parseInstructionBlock();
    
    std::unique_ptr<If> elseIf = nullptr;
    switch (currToken_.type) {
        case TokenType::KEYWORD_ELIF:
            advance();
            elseIf = tryParseIfInstr();
            break;
        case TokenType::KEYWORD_ELSE:
            advance();
            elseIf = std::make_unique<If>(parseInstructionBlock());
            break;
        default:
            ;
    }
    return std::make_unique<If>(
        std::move(cond),
        std::move(positiveBlock),
        std::move(elseIf)
    );
}

template <typename TokenSource>
std::unique_ptr<While> Parser<TokenSource>::tryParseWhileInstr() {
    std::unique_ptr<Expression> cond = parseExpression();
    std::vector<std::unique_ptr<Instruction>> body = parseInstructionBlock();
    return std::make_unique<While>(
        std::move(cond),
        std::move(body)
    );
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseExpression() {
    std::unique_ptr<Expression> expr = parseAddExpression();
    //TODO wyrażenie logiczne
    return expr;
    //Token op = parseOperator();
    //std::unique_ptr<Instruction> rightOperand = parseExpressionOperand();
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseAddExpression() {
    std::unique_ptr<Expression> leftOperand = parseMultExpression();
    if (!leftOperand) {
        return nullptr;
    }
    while (currToken_.type == TokenType::OP_ADD) {
        Token op = currToken_;
        advance();
        std::unique_ptr<Expression> rightOperand = parseMultExpression();
        if (!rightOperand) {
            ErrorHandler::handleFromParser("Expected MultExpression after OP_ADD in AddExpression");
        }
        leftOperand = std::make_unique<BinaryExpression>(
                std::move(leftOperand),
                op,
                std::move(rightOperand)
            );
    }
    return leftOperand;
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseMultExpression() {
    std::unique_ptr<Expression> leftOperand = parseExpressionElement();
    if (!leftOperand) {
        return nullptr;
    }
    while (currToken_.type == TokenType::OP_MULT) {
        Token op = currToken_;
        advance();
        std::unique_ptr<Expression> rightOperand = parseExpressionElement();
        if (!rightOperand) {
            ErrorHandler::handleFromParser("Expected ExpressionElement after OP_MULT in MultExpression");
        }
        leftOperand = std::make_unique<BinaryExpression>(
                std::move(leftOperand),
                op,
                std::move(rightOperand)
            );
    }
    return leftOperand;
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseExpressionElement() {
    std::unique_ptr<Expression> element = nullptr;

    switch (currToken_.type) {
        case TokenType::ID: {
            Token id = currToken_;
            advance();
            element = tryParseFuncCall(id);
            if (!element) {
                element = std::make_unique<VarReference>(std::get<std::string>(id.value));
            }
            break;
        }
        case TokenType::PAREN_OPEN:
            advance();
            element = parseAddExpression();
            requireToken(TokenType::PAREN_CLOSE);
            break;
        case TokenType::NUMBER: {
            //std::cout << "parsing number in exprElem\n";
            double numberValue;
            if (std::holds_alternative<double>(currToken_.value)) {
                numberValue = std::get<double>(currToken_.value);
            } else {
                numberValue = std::get<int>(currToken_.value);
            }
            element = std::make_unique<NumberValue>(numberValue);
            advance();
            break;
        }
        default:
            ;
    }
    return element;
}

//TODO check if this function name wasn't yet used -- here
template <typename TokenSource>
std::unique_ptr<FuncDef> Parser<TokenSource>::parseFuncDef() {
    if (currToken_.type != TokenType::KEYWORD_FUNC) {
        return nullptr;
    }
    advance();

    Token id = requireToken(TokenType::ID);
    requireToken(TokenType::PAREN_OPEN);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse parameters
    requireToken(TokenType::PAREN_CLOSE);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse optional return type

    std::vector<std::unique_ptr<Instruction>> body = parseInstructionBlock();

    requireToken(TokenType::END_OF_INSTRUCTION);

    return std::make_unique<FuncDef>(std::get<std::string>(id.value), std::move(body));
}

#endif // TKOMSIUNITS_PARSER_HPP_INCLUDED
