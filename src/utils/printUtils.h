#ifndef TKOMSIUNITS_PRINTUTILS_H_INCLUDED
#define TKOMSIUNITS_PRINTUTILS_H_INCLUDED

#include "lexer/Lexer.h"
#include <array>
#include <iostream>

std::ostream& operator<<(std::ostream &os, TokenType tokenType) {
    static std::array typeNames = {
        "ID",
        "NUMBER",
        "KEYWORD_BOOL",
        "KEYWORD_BREAK",
        "KEYWORD_CONTINUE",
        "KEYWORD_ELSE",
        "KEYWORD_FALSE",
        "KEYWORD_FUNC",
        "KEYWORD_IF",
        "KEYWORD_IN",
        "KEYWORD_RETURN",
        "KEYWORD_TRUE",
        "KEYWORD_WHILE",
        "OP_MULT",
        "OP_ADD",
        "OP_SUFFIX",
        "OP_REL",
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
        "END_OF_INSTRUCTION",
        "END_OF_STREAM"
    };
    return os << typeNames[static_cast<int>(tokenType)];
}

std::ostream& operator<<(std::ostream &os, Token token) {
    os << token.type << " : '";
    if (token.type == TokenType::NUMBER) {
        if (std::holds_alternative<double>(token.value)) {
            os << std::fixed << std::get<double>(token.value) << std::defaultfloat;
        } else {
            os << std::get<int>(token.value);
        }
    } else {
        os << std::get<std::string>(token.value);
    }
    return os << '\'';
}

#endif // TKOMSIUNITS_PRINTUTILS_H_INCLUDED
