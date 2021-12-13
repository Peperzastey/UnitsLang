#include "Parser.h"
#include "codeObjects/VarReference.h"
#include "codeObjects/NumberValue.h"
#include "error/ErrorHandler.h"
#include "utils/printUtils.h"
#include <cassert>
#include <vector>
#include <sstream>

Parser::Parser(Lexer &lexer)
    : lexer_(lexer) {
    }

void Parser::advance() {
    currToken_ = lexer_.getToken();
}

Token Parser::requireToken(TokenType expected) {
    if (currToken_.type != expected) {
        reportUnexpectedToken(std::array{expected});
    }
    Token token = currToken_;
    advance();
    return token;
}

template <std::size_t N>
void Parser::reportUnexpectedToken(const std::array<TokenType, N> &expected/*, const std::string &rule*/) {
    std::cout << "unexpected token\n";
    std::ostringstream os;
    os << "Unexpected token: {" << currToken_ << "}, expecting: [";
    for (const auto &tokenType : expected) {
        os << tokenType << ' ';
    }
    os << ']';
    ErrorHandler::handleFromParser(os.str());
}

void Parser::skipTokens(TokenType toBeSkipped) {
    while (currToken_.type == toBeSkipped) {
        advance();
    }
}

std::unique_ptr<Program> Parser::parse() {
    advance();

    std::unique_ptr<Instruction> instr;
    std::vector<std::unique_ptr<Instruction>> instructions;
    
    while ((instr = parseInstruction())) {
        instructions.push_back(std::move(instr));
    }

    return std::make_unique<Program>(std::move(instructions));
}

std::unique_ptr<Instruction> Parser::parseInstruction() {
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
        case TokenType::KEYWORD_FUNC:
            instr = parseFuncDef();
            break;
        default:
            return nullptr;
    }
    requireToken(TokenType::END_OF_INSTRUCTION);
    return instr;
}

std::unique_ptr<FuncCall> Parser::tryParseFuncCall(Token id) {
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

std::unique_ptr<VarDef> Parser::tryParseVarDef(Token id) {
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

std::unique_ptr<Expression> Parser::parseExpression() {
    std::unique_ptr<Expression> expr = parseAddExpression();
    //TODO wyrażenie logiczne
    return expr;
    //Token op = parseOperator();
    //std::unique_ptr<Instruction> rightOperand = parseExpressionOperand();
}

std::unique_ptr<Expression> Parser::parseAddExpression() {
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

std::unique_ptr<Expression> Parser::parseMultExpression() {
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

std::unique_ptr<Expression> Parser::parseExpressionElement() {
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
std::unique_ptr<FuncDef> Parser::parseFuncDef() {
    assert(currToken_.type == TokenType::KEYWORD_FUNC);
    advance();

    Token id = requireToken(TokenType::ID);
    requireToken(TokenType::PAREN_OPEN);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse parameters
    requireToken(TokenType::PAREN_CLOSE);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse optional return type
    requireToken(TokenType::BRACKET_OPEN);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    // parse optional (may be empty) list of instructions
    requireToken(TokenType::BRACKET_CLOSE);
    //requireToken(TokenType::END_OF_INSTRUCTION);

    return std::make_unique<FuncDef>(std::get<std::string>(id.value));
}
