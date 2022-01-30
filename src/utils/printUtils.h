#ifndef TKOMSIUNITS_PRINTUTILS_H_INCLUDED
#define TKOMSIUNITS_PRINTUTILS_H_INCLUDED

#include "lexer/Lexer.h"
#include <array>
#include <iostream>

inline std::ostream& operator<<(std::ostream &os, TokenType tokenType) {
    static std::array typeNames = {
        "ID",
        "NUMBER",
        "UNIT",
        "KEYWORD_BOOL",
        "KEYWORD_BREAK",
        "KEYWORD_CONTINUE",
        "KEYWORD_ELIF",
        "KEYWORD_ELSE",
        "KEYWORD_FALSE",
        "KEYWORD_FUNC",
        "KEYWORD_IF",
        "KEYWORD_IN",
        "KEYWORD_RETURN",
        "KEYWORD_STR",
        "KEYWORD_TRUE",
        "KEYWORD_WHILE",
        "OP_MULT",
        "OP_ADD",
        "OP_SUFFIX",
        "OP_EQ",
        "OP_REL",
        "OP_AND",
        "OP_OR",
        "ASSIGN",
        "PAREN_OPEN",
        "PAREN_CLOSE",
        "BRACKET_OPEN",
        "BRACKET_CLOSE",
        "SQUARE_OPEN",
        "SQUARE_CLOSE",
        "COMMA",
        "FUNC_RESULT",
        "STRING",
        "TEXT_WITHIN_STRING",
        "END_OF_INSTRUCTION",
        "END_OF_STREAM"
    };
    return os << typeNames[static_cast<int>(tokenType)];
}

inline std::ostream& operator<<(std::ostream &os, UnitType unitType) {
    static std::array typeNames = {
        "s",
        "g",
        "m",
        "N",
        "Pa",
        "J",
        "SCALAR"
    };
    return os << typeNames[static_cast<int>(unitType)];
}

inline std::ostream& operator<<(std::ostream &os, Unit unit) {
    return os << unit.prefix << unit.unit << (unit.power > 1 ? std::to_string(unit.power) : "");
}

inline std::ostream& operator<<(std::ostream &os, Token token) {
    os << token.type << " : '";
    if (token.type == TokenType::NUMBER) {
        if (std::holds_alternative<double>(token.value)) {
            os << std::fixed << std::get<double>(token.value) << std::defaultfloat;
        } else {
            os << std::get<int>(token.value);
        }
    } else if (token.type == TokenType::UNIT ) {
        os << std::get<Unit>(token.value);
    } else {
        os << std::get<std::string>(token.value);
    }
    return os << '\'';
}

#endif // TKOMSIUNITS_PRINTUTILS_H_INCLUDED
