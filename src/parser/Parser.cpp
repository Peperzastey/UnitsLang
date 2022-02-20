#include "Parser.h"

#include "codeObjects/VarReference.h"
#include "codeObjects/Value.h"
#include "codeObjects/Return.h"
#include "codeObjects/Break.h"
#include "codeObjects/Continue.h"

#include "error/ErrorHandler.h"
#include "utils/printUtils.h"
#include <cassert>
#include <vector>
#include <sstream>

Parser::Parser(TokenSource &tokenSource)
    : tokenSource_(tokenSource) {
    }

// advance to next Token but skip empty lines and newlines after '(' and '{'
void Parser::advance() {
    Token prev;
    do {
        prev = currToken_;
        currToken_ = tokenSource_.getToken();
    } while (currToken_.type == TokenType::END_OF_INSTRUCTION
        && (prev.type == TokenType::END_OF_INSTRUCTION
            || prev.type == TokenType::PAREN_OPEN
            || prev.type == TokenType::BRACKET_OPEN)
    );
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
void Parser::reportUnexpectedToken(const std::array<TokenType, N> &expected) {
    std::cout << "unexpected token\n";
    std::ostringstream os;
    os << "Unexpected token: {" << currToken_ << "}, expecting: [";
    for (const auto &tokenType : expected) {
        os << tokenType << ' ';
    }
    os << ']';
    ErrorHandler::handleFromParser(os.str());
}

std::unique_ptr<Program> Parser::parse() {
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

std::unique_ptr<Instruction> Parser::parseInstruction() {
    std::unique_ptr<Instruction> instr = nullptr; 
    
    switch (currToken_.type) {
        case TokenType::ID: {
            // VarDefOrAssignment or FuncCall
            Token id = currToken_;
            advance();
            instr = tryParseFuncCall(id);
            if (!instr) {
                instr = tryParseVarDefOrAssignment(id);
            }
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

std::unique_ptr<InstructionBlock> Parser::parseInstructionBlock() {
    requireToken(TokenType::BRACKET_OPEN);
    std::vector<std::unique_ptr<Instruction>> block;
    while (std::unique_ptr<Instruction> instr = parseInstruction()) {
        block.push_back(std::move(instr));
    }
    requireToken(TokenType::BRACKET_CLOSE);
    return std::make_unique<InstructionBlock>(std::move(block));
}

std::unique_ptr<FuncCall> Parser::tryParseFuncCall(Token id) {
    assert(id.type == TokenType::ID);
    if (currToken_.type != TokenType::PAREN_OPEN) {
        return nullptr;
    }
    advance();

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

std::unique_ptr<VarDefOrAssignment> Parser::tryParseVarDefOrAssignment(Token id) {
    assert(id.type == TokenType::ID);
    
    //optional type designation
    std::optional<Type> type = parseType();
    
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

std::unique_ptr<If> Parser::tryParseIfInstr() {
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

std::unique_ptr<While> Parser::tryParseWhileInstr() {
    std::unique_ptr<Expression> cond = parseExpression();
    std::unique_ptr<InstructionBlock> body = parseInstructionBlock();
    return std::make_unique<While>(
        std::move(cond),
        std::move(body)
    );
}

std::unique_ptr<Expression> Parser::parseExpression() {
    switch (currToken_.type) {
        case TokenType::STRING:
            return parseString();
        default:
            return parseOrExpression();
    }
}

std::unique_ptr<Expression> Parser::parseXXXBinaryExpression(
        ParseExprFunc subExprFunc, TokenType opType,
        const std::string &exprName, const std::string &subExprName
    ) {
    std::unique_ptr<Expression> leftOperand = (this->*subExprFunc)();
    if (!leftOperand) {
        return nullptr;
    }
    while (currToken_.type == opType) {
        Token op = currToken_;
        advance();
        std::unique_ptr<Expression> rightOperand = (this->*subExprFunc)();
        if (!rightOperand) {
            std::ostringstream os;
            os << "Expected " << subExprName << " after " << opType << " in " << exprName;
            ErrorHandler::handleFromParser(os.str());
        }
        leftOperand = std::make_unique<BinaryExpression>(
                std::move(leftOperand),
                op,
                std::move(rightOperand)
            );
    }
    return leftOperand;
}

std::unique_ptr<Expression> Parser::parseOrExpression() {
    return parseXXXBinaryExpression(
            &Parser::parseAndExpression, TokenType::OP_OR,
            "OrExpression", "AndExpression"
        );
}

std::unique_ptr<Expression> Parser::parseAndExpression() {
    return parseXXXBinaryExpression(
            &Parser::parseEqualExpression, TokenType::OP_AND,
            "AndExpression", "EqualExpression"
        );
}

std::unique_ptr<Expression> Parser::parseEqualExpression() {
    return parseXXXBinaryExpression(
            &Parser::parseRelExpression,TokenType::OP_EQ,
            "EqualExpression", "RelExpression"
        );
}

std::unique_ptr<Expression> Parser::parseRelExpression() {
    switch (currToken_.type) {
        case TokenType::KEYWORD_TRUE:
            advance();
            return std::make_unique<Value>(true);
        case TokenType::KEYWORD_FALSE:
            advance();
            return std::make_unique<Value>(false);
        default:
            return parseXXXBinaryExpression(
                    &Parser::parseAddExpression, TokenType::OP_REL,
                    "RelExpression", "AddExpression"
                );
    }
}

std::unique_ptr<Expression> Parser::parseAddExpression() {
    return parseXXXBinaryExpression(
            &Parser::parseMultExpression, TokenType::OP_ADD,
            "AddExpression", "MultExpression"
        );
}

std::unique_ptr<Expression> Parser::parseMultExpression() {
    return parseXXXBinaryExpression(
            &Parser::parseExpressionElement, TokenType::OP_MULT,
            "MultExpression", "ExpressionElement"
        );
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
            element = parseOrExpression();
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
            element = std::make_unique<Value>(numberValue, Type(std::move(unit)));
            break;
        }
        default:
            ;
    }
    return element;
}

std::unique_ptr<codeobj::String> Parser::parseString() {
    assert(currToken_.type == TokenType::STRING);
    String strToken = std::get<String>(currToken_.value);
    std::vector<std::unique_ptr<Expression>> parts;
    std::optional<TokenType> expected = std::nullopt;
    
    for (auto iter = strToken.innerTokens.begin(); iter != strToken.innerTokens.end(); ++iter) {
        if (expected && iter->type != expected) {
            ErrorHandler::handleFromParser("Wrong formatted string format");
        }
        switch (iter->type) {
            case TokenType::TEXT_WITHIN_STRING:
                parts.emplace_back(std::make_unique<Value>(std::get<std::string>(iter->value)));
                break;
            case TokenType::BRACKET_OPEN:
                expected = TokenType::ID;
                break;
            case TokenType::ID:
                if (!expected) {
                    ErrorHandler::handleFromParser("Wrong formatted string format: Id not inside '{}'");
                }
                expected = TokenType::BRACKET_CLOSE;
                parts.emplace_back(std::make_unique<VarReference>(std::get<std::string>(iter->value)));
                break;
            case TokenType::BRACKET_CLOSE:
                if (!expected) {
                    ErrorHandler::handleFromParser("Wrong formatted string format: '}' not after id");
                }
                expected = std::nullopt;
                break;
            default:
                ErrorHandler::handleFromParser("Wrong formatted string format: Unexpected token");
        }
    }
    
    advance();
    return std::make_unique<codeobj::String>(std::move(parts));
}

codeobj::Unit Parser::parseUnit() {
    if (currToken_.type != TokenType::SQUARE_OPEN) {
        return codeobj::Unit();
    }
    advance();
    
    codeobj::Unit unit = parseComplexUnitTokens();
    
    requireToken(TokenType::SQUARE_CLOSE);
    return unit;
}

codeobj::Unit Parser::parseComplexUnitTokens() {
    codeobj::Unit leftOperand = parseUnitElementTokens();

    while (currToken_.type == TokenType::OP_MULT) {
        Token op = currToken_;
        advance();
        codeobj::Unit rightOperand = parseUnitElementTokens();

        leftOperand.combineWithUnit(op, rightOperand);
    }
    return leftOperand;
}

codeobj::Unit Parser::parseUnitElementTokens() {
    codeobj::Unit element;
    
    switch (currToken_.type) {
        case TokenType::PAREN_OPEN:
            advance();
            element = parseComplexUnitTokens();
            requireToken(TokenType::PAREN_CLOSE);
            break;
        default: {
            Token unit = requireToken(TokenType::UNIT);
            element = codeobj::Unit(std::get<Unit>(unit.value));
        }    
    }
    
    return element;
}

std::optional<Type> Parser::parseType() {
    if (currToken_.type != TokenType::SQUARE_OPEN) {
        return std::nullopt;
    }
    advance();
    
    Type type = parseTypeTokens();
    
    requireToken(TokenType::SQUARE_CLOSE);
    return std::move(type);
}

Type Parser::parseTypeTokens() {
    switch (currToken_.type) {
        case TokenType::NUMBER:
            if (!std::holds_alternative<int>(currToken_.value) || std::get<int>(currToken_.value) != 1) {
                ErrorHandler::handleFromParser("Type designation '[...]' holds number different than 1! Scalar type is denoted as '[1]'!");
            }
            advance();
            return Type(codeobj::Unit());
        case TokenType::KEYWORD_BOOL:
            advance();
            return Type(Type::BOOL);
         case TokenType::KEYWORD_STR:
            advance();
            return Type(Type::STRING);
        default:
            return Type(codeobj::Unit(parseComplexUnitTokens()));
    }
}

std::unique_ptr<FuncDef> Parser::parseFuncDef() {
    if (currToken_.type != TokenType::KEYWORD_FUNC) {
        return nullptr;
    }
    advance();

    Token id = requireToken(TokenType::ID);
    requireToken(TokenType::PAREN_OPEN);

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
    Type returnType(Type::VOID);
    if (currToken_.type == TokenType::FUNC_RESULT) {
        advance();
        std::optional<Type> funcResultType = parseType();
        if (!funcResultType) {
            ErrorHandler::handleFromParser("'->' not followed by return-type in function definition");
        } else {
            returnType = std::move(*funcResultType);
        }
    }

    std::unique_ptr<InstructionBlock> body = parseInstructionBlock();

    requireToken(TokenType::END_OF_INSTRUCTION);

    return std::make_unique<FuncDef>(
            std::get<std::string>(id.value),
            std::move(parameters),
            std::move(returnType),
            std::move(body)
        );
}

std::optional<Variable> Parser::parseFuncParameter() {
    if (currToken_.type != TokenType::ID) {
        return std::nullopt;
    }
    std::string paramName = std::get<std::string>(currToken_.value);
    advance();

    std::optional<Type> type = parseType();
    if (!type) {
        ErrorHandler::handleFromParser("Parameter name not followed by type");
    }
    
    return Variable(paramName, std::move(*type));
}
