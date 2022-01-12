//#include "Parser.h"
#ifndef TKOMSIUNITS_PARSER_HPP_INCLUDED
#define TKOMSIUNITS_PARSER_HPP_INCLUDED

#include "codeObjects/VarReference.h"
#include "codeObjects/Value.h"
#include "codeObjects/Return.h"
#include "codeObjects/Break.h"
#include "codeObjects/Continue.h"
#include "codeObjects/Bool.h"
#include "codeObjects/VoidType.h"

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
            // VarDefOrAssignment or FuncCall or SuffixInstr
            Token id = currToken_;
            advance();
            instr = tryParseFuncCall(id);
            if (!instr) {
                instr = tryParseVarDefOrAssignment(id);
            }
            //TODO ...suffix
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
        case TokenType::KEYWORD_FUNC:
            // handled in parseFuncDef()
        case TokenType::BRACKET_CLOSE:
            // end of instruction block
        case TokenType::END_OF_STREAM:
        case TokenType::END_OF_INSTRUCTION: //TODO should be handled by filter before parser
            return nullptr;
        default: {
            // invalid instruction
            std::ostringstream os;
            os << "Invalid instruction starting with token: " << currToken_;
            ErrorHandler::handleFromParser(os.str());
        }
    }
    requireToken(TokenType::END_OF_INSTRUCTION);
    return instr;
}

template <typename TokenSource>
std::unique_ptr<InstructionBlock> Parser<TokenSource>::parseInstructionBlock() {
    requireToken(TokenType::BRACKET_OPEN);
    skipTokens(TokenType::END_OF_INSTRUCTION);
    //TODO allow empty lines!!! also between instructions!
    std::vector<std::unique_ptr<Instruction>> block;
    while (std::unique_ptr<Instruction> instr = parseInstruction()) {
        block.push_back(std::move(instr));
    }
    requireToken(TokenType::BRACKET_CLOSE);
    return std::make_unique<InstructionBlock>(std::move(block));
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
    std::vector<std::unique_ptr<Expression>> arguments;
    std::unique_ptr<Expression> arg = parseExpression();
    if (arg) {
        arguments.push_back(std::move(arg));
        while (currToken_.type == TokenType::COMMA) {
            advance();
            arg = parseExpression();
            if (!arg) {
                ErrorHandler::handleFromParser("Comma in function-call argument list not followed by an argument");
            }
            arguments.push_back(std::move(arg));
        }
    }

    requireToken(TokenType::PAREN_CLOSE);
    return std::make_unique<FuncCall>(std::get<std::string>(id.value), std::move(arguments));
}

template <typename TokenSource>
std::unique_ptr<VarDefOrAssignment> Parser<TokenSource>::tryParseVarDefOrAssignment(Token id) {
    assert(id.type == TokenType::ID);
    
    //optional type designation
    std::optional<Type2> type = parseType();
    
    if (currToken_.type != TokenType::ASSIGN) {
        return nullptr;
    }
    advance();

    std::unique_ptr<Expression> expr = parseExpression();
    if (!expr) {
        ErrorHandler::handleFromParser("Expected expression after '=' in variable definition");
    }

    return std::make_unique<VarDefOrAssignment>(std::get<std::string>(id.value), std::move(expr), std::move(type));
}

template <typename TokenSource>
std::unique_ptr<If> Parser<TokenSource>::tryParseIfInstr() {
    std::unique_ptr<Expression> cond = parseExpression();

    std::unique_ptr<InstructionBlock> positiveBlock = parseInstructionBlock();
    
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
    std::unique_ptr<InstructionBlock> body = parseInstructionBlock();
    return std::make_unique<While>(
        std::move(cond),
        std::move(body)
    );
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseExpression() {
    return parseRelExpression();
}

template <typename TokenSource>
std::unique_ptr<Expression> Parser<TokenSource>::parseRelExpression() {
    switch (currToken_.type) {
        case TokenType::KEYWORD_TRUE:
            advance();
            return std::make_unique<Value>(true);
        case TokenType::KEYWORD_FALSE:
            advance();
            return std::make_unique<Value>(false);
        default:
            ;
    }

    std::unique_ptr<Expression> leftOperand = parseAddExpression();
    if (!leftOperand) {
        return nullptr;
    }
    while (currToken_.type == TokenType::OP_REL) {
        Token op = currToken_;
        advance();
        std::unique_ptr<Expression> rightOperand = parseAddExpression();
        if (!rightOperand) {
            ErrorHandler::handleFromParser("Expected AddExpression after OP_REL in RelExpression");
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
            double numberValue;
            if (std::holds_alternative<double>(currToken_.value)) {
                numberValue = std::get<double>(currToken_.value);
            } else {
                numberValue = std::get<int>(currToken_.value);
            }
            advance();
            codeobj::Unit unit = parseUnit();
            element = std::make_unique<Value>(numberValue, Type2(std::move(unit)));
            break;
        }
        default:
            ;
    }
    return element;
}

template <typename TokenSource>
codeobj::Unit Parser<TokenSource>::parseUnit() {
    if (currToken_.type != TokenType::SQUARE_OPEN) {
        return codeobj::Unit();
    }
    advance();
    
    codeobj::Unit unit = parseUnitTokens();
    
    requireToken(TokenType::SQUARE_CLOSE);
    return unit;
}

template <typename TokenSource>
codeobj::Unit Parser<TokenSource>::parseUnitTokens() {
    //TODO parse complex units (expressions)
    Token unit = requireToken(TokenType::UNIT);

    return codeobj::Unit(std::get<Unit>(unit.value));
}

template <typename TokenSource>
std::optional<Type2> Parser<TokenSource>::parseType() {
    if (currToken_.type != TokenType::SQUARE_OPEN) {
        return std::nullopt;
    }
    advance();
    
    Type2 type = parseTypeTokens();
    
    requireToken(TokenType::SQUARE_CLOSE);
    return std::move(type);
}

template <typename TokenSource>
Type2 Parser<TokenSource>::parseTypeTokens() {
    switch (currToken_.type) {
        case TokenType::NUMBER:
            if (!std::holds_alternative<int>(currToken_.value) || std::get<int>(currToken_.value) != 1) {
                ErrorHandler::handleFromParser("Type designation '[...]' holds number different than 1! Scalar type is denoted as '[1]'!");
            }
            advance();
            return Type2(codeobj::Unit());
        case TokenType::KEYWORD_BOOL:
            advance();
            return Type2(Type2::BOOL);
        default:
            return Type2(codeobj::Unit(parseUnitTokens()));
    }
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
    std::vector<Variable> parameters;
    std::optional<Variable> param = parseFuncParameter();
    if (param) {
        parameters.push_back(std::move(*param));
        while (currToken_.type == TokenType::COMMA) {
            advance();
            param = parseFuncParameter();
            if (!param) {
                ErrorHandler::handleFromParser("Comma in function parameter list not followed by a parameter");
            }
            parameters.push_back(std::move(*param));
        }
    }

    requireToken(TokenType::PAREN_CLOSE);
    
    // parse optional return type
    Type2 returnType(Type2::VOID);
    if (currToken_.type == TokenType::FUNC_RESULT) {
        advance();
        std::optional<Type2> funcResultType = parseType();
        if (!funcResultType) {
            ErrorHandler::handleFromParser("'->' not followed by return-type in function definition");
        } else {
            returnType = std::move(*funcResultType);
        }
    }
    
    skipTokens(TokenType::END_OF_INSTRUCTION);
    std::unique_ptr<InstructionBlock> body = parseInstructionBlock();

    requireToken(TokenType::END_OF_INSTRUCTION);

    return std::make_unique<FuncDef>(
            std::get<std::string>(id.value),
            std::move(parameters),
            std::move(returnType),
            std::move(body)
        );
}

template <typename TokenSource>
std::optional<Variable> Parser<TokenSource>::parseFuncParameter() {
    if (currToken_.type != TokenType::ID) {
        return std::nullopt;
    }
    std::string paramName = std::get<std::string>(currToken_.value);
    advance();

    std::optional<Type2> type = parseType();
    if (!type) {
        ErrorHandler::handleFromParser("Parameter name not followed by type");
    }
    
    return Variable(paramName, std::move(*type));
}

#endif // TKOMSIUNITS_PARSER_HPP_INCLUDED
