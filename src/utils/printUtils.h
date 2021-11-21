#ifndef TKOMSIUNITS_PRINTUTILS_H_INCLUDED
#define TKOMSIUNITS_PRINTUTILS_H_INCLUDED

#include "lexer/Lexer.h"
#include <array>
#include <iostream>

std::ostream& operator<<(std::ostream &os, TokenType tokenType) {
    static std::array typeNames = {
        "ID",
        "KEYWORD_WHILE",
        "KEYWORD_IF",
        "NUMBER",
        "OP_MULT",
        "OP_ADD",
        "OP_SUFFIX",
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
