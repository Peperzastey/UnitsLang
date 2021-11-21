#include "Lexer.h"
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iostream> //TODO debugging

std::unordered_map<std::string, TokenType> Lexer::keywords_ {
    { "while"  , TokenType::KEYWORD_WHILE },
    { "if"     , TokenType::KEYWORD_IF    }
};

char Lexer::discardWhitespacesAndComments() {
    char c;
    while (true) {
        c = source_.getChar();
        if (c == '/') {
            if (!discardComment()) {
                break;
            }
        } else if (c == '\\') {
            discardInstrBreak();
        } else if (!isblank(c)) {
            break;
        }
    }

    return c;
}

bool Lexer::discardComment() {
    /*if (c != '/') {
        return false;
    }*/
    char c = source_.getChar();
    if (c != '/') {
        source_.ungetChar(c);
        return false;
    }
    // comment
    while (c != '\r' && c != '\n') {
        c = source_.getChar();
    }
    if (c == '\r') {
        c = source_.getChar();
    }
    if (c != '\n') {
        throw std::runtime_error("Lexer error: '\\r' not followed by '\\n'!");
    }
    return true;
}

void Lexer::discardInstrBreak() {
    char c;
    bool gotR = false;
    do {
        c = source_.getChar();
    } while (isblank(c));
    if (c == '\r') {
        gotR = true;
        c = source_.getChar();
    }
    if (c != '\n') {
        if (gotR) {
            throw std::runtime_error("Lexer error: '\\r' not followed by '\\n'!");
        }
        throw std::runtime_error("Lexer error: Instruction break ('\\') followed by non-space character!");
    }
}

Token Lexer::getToken() {
    char c = discardWhitespacesAndComments();
    
    // id/unit/keyword:
    if (isalpha(c) || c == '_') {
        return constructIdOrUnitOrKeyword(c);
    }
    // number:
    if (isdigit(c)) {
        return constructNumber(c);
    }

    switch (c) {
        case '\r':
        case '\n':
            return constructEndOfInstr(c);
        case '/':
        case '*':
            return { TokenType::OP_MULT, std::string{c} };
        case '+':
        case '-':
            return constructAdditiveOp(c);
        case EOF:
            return { TokenType::END_OF_STREAM, "" };
        default:
            // string contents? (text)
            throw std::runtime_error("Lexer error: Unknown lexem");
    }
}

Token Lexer::constructEndOfInstr(char c) {
    if (c == '\r') {
        c = source_.getChar();
        if (c != '\n') {
            throw std::runtime_error("Lexer error: '\\r' not followed by '\\n'!");
        }
    }
    assert(c == '\n');
    return { TokenType::END_OF_INSTRUCTION, "" };
}

Token Lexer::constructAdditiveOp(char c) {
    char nextCh = source_.getChar();
    if (nextCh == c) {
        //TODO? check if next char is blank
        return { TokenType::OP_SUFFIX, std::string{c, nextCh} };
    }
    source_.ungetChar(nextCh);
    return { TokenType::OP_ADD, std::string{c} };
}

Token Lexer::constructIdOrUnitOrKeyword(char c) {
    std::stringstream buffer;
    buffer << c;
    c = source_.getChar();
    while (isalpha(c) || c == '_' || isdigit(c)) {
        buffer << c;
        c = source_.getChar();
    }
    source_.ungetChar(c);
    std::string value = buffer.str();
    // check which is it
    if (auto it = keywords_.find(value); it != keywords_.end()) {
        return { it->second, value };
    }
    return { TokenType::ID, value };
}

Token Lexer::constructNumber(char c) {
    //std::stringstream buffer;
    //buffer << c;
    int digitInBlockCounter = 1;
    int separatorCounter = 0;
    //int dotCounter = 0;
    int value = c - '0';

    while (true) {
        char prevCh = c;
        c = source_.getChar();
        //buffer << c;

        if (isdigit(c)) {
            if (value == 0) { // first digit is 0 and is not followed by dot
                throw std::runtime_error("Lexer error: First digit is 0 and is not followed by dot in number!");
            }
            ++digitInBlockCounter;
            value = value * 10 + c - '0';
            continue;
        }

        switch (c) {
            case ' ':
                ++separatorCounter;
                if (!isdigit(prevCh)) {
                    throw std::runtime_error("Lexer error: Digit block separator not following a digit in number!");
                    //TODO error handling: numer linii, pozycja w tekście
                }
                if (digitInBlockCounter > 3) {
                    throw std::runtime_error("Lexer error: Digit block in number longer than 3 digits!");
                }
                digitInBlockCounter = 0; // separator starts a new block
                break;

            case '.': {
                //++dotCounter;
                if (!isdigit(prevCh)) {
                    throw std::runtime_error("Lexer error: Dot not following a digit in number!");
                }
                /*if (dotCounter > 1) {
                    throw std::runtime_error("Lexer error: More than one dot in number!");
                }*/
                auto [intValue, length] = parseIntFromSource();
                if (length == 0) {
                    throw std::runtime_error("Lexer error: Fraction part is empty in number!");
                }
                int divisor = 1;
                for (; length > 0; --length) {
                    divisor *= 10;
                }
                double fraction = static_cast<double>(intValue) / divisor;
                double doubleValue = static_cast<double>(value) + fraction;
                return { TokenType::NUMBER, doubleValue };
            }

            default:
                source_.ungetChar(c); // number can be followed by a unit
                if (prevCh == ' ') {
                    source_.ungetChar(prevCh);
                }
                return { TokenType::NUMBER, value };
        }
    }
}

Lexer::IntWithLength Lexer::parseIntFromSource() {
    int value = 0;
    int length = 0;

    while (true) {
        char c = source_.getChar();
        if (!isdigit(c)) {
            source_.ungetChar(c);
            return { value, length };
        }
        ++length;
        value = value * 10 + c - '0';
    }
}
