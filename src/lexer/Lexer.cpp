#include "Lexer.h"
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <iostream> //TODO debugging

std::unordered_map<std::string, TokenType> Lexer::keywords_ {
    { "bool"     , TokenType::KEYWORD_BOOL     },
    { "break"    , TokenType::KEYWORD_BREAK    },
    { "continue" , TokenType::KEYWORD_CONTINUE },
    { "else"     , TokenType::KEYWORD_ELSE     },
    { "false"    , TokenType::KEYWORD_FALSE    },
    { "func"     , TokenType::KEYWORD_FUNC     },
    { "if"       , TokenType::KEYWORD_IF       },
    { "in"       , TokenType::KEYWORD_IN       },
    { "return"   , TokenType::KEYWORD_RETURN   },
    { "true"     , TokenType::KEYWORD_TRUE     },
    { "while"    , TokenType::KEYWORD_WHILE    }
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
    char c = source_.getChar();
    if (c != '/') {
        source_.ungetChar(c);
        return false;
    }
    // comment
    while (!consumeNewline(c)) {
        c = source_.getChar();
    }
    return true;
}

void Lexer::discardInstrBreak() {
    char c;
    do {
        c = source_.getChar();
    } while (isblank(c));
    if (!consumeNewline(c)) {
        throw std::runtime_error("Lexer error: Instruction break ('\\') followed by non-space character!");
    }
}

bool Lexer::consumeNewline(char c) {
    bool gotR = false;
    if (c == '\r') {
        gotR = true;
        c = source_.getChar();
    }
    if (c != '\n') {
        if (gotR) {
            throw std::runtime_error("Lexer error: '\\r' not followed by '\\n'!");
        }
        return false;
    }
    return true;
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
            return constructAdditiveOpOrFuncResult(c);
        case '=':
            return constructAssignOrEq(c);
        case '>':
        case '<':
            return constructRelationalOp(c);
        case '!':
            return constructNotEq(c);
        case '"':
            return constructString(c);
        case '(':
            return { TokenType::PAREN_OPEN, "" };
        case ')':
            return { TokenType::PAREN_CLOSE, "" };
        case '{':
            return { TokenType::BRACKET_OPEN, "" };
        case '}':
            return { TokenType::BRACKET_CLOSE, "" };
        case '[':
            return { TokenType::SQUARE_OPEN, "" };
        case ']':
            return { TokenType::SQUARE_CLOSE, "" };
        case ',':
            return { TokenType::COMMA, "" };
        case EOF:
            return { TokenType::END_OF_STREAM, "" };
        default:
            // string contents? (text)
            throw std::runtime_error("Lexer error: Unknown lexem");
    }
}

Token Lexer::constructEndOfInstr(char c) {
    bool newlineConstructed = consumeNewline(c);
    assert(newlineConstructed);
    return { TokenType::END_OF_INSTRUCTION, "" };
}

Token Lexer::constructAdditiveOpOrFuncResult(char c) {
    char nextCh = source_.getChar();
    if (nextCh == c) {
        //TODO? check if next char is blank
        return { TokenType::OP_SUFFIX, std::string{c, nextCh} };
    } else if (c == '-' && nextCh == '>') {
        return { TokenType::FUNC_RESULT, "" };
    }
    source_.ungetChar(nextCh);
    return { TokenType::OP_ADD, std::string{c} };
}

Token Lexer::constructAssignOrEq([[maybe_unused]] char c) {
    assert(c == '=');
    char nextCh = source_.getChar();
    if (nextCh == '=') {
        return { TokenType::OP_REL, "==" };
    }
    source_.ungetChar(nextCh);
    return { TokenType::ASSIGN, "" };
}

Token Lexer::constructRelationalOp(char c) {
    char nextCh = source_.getChar();
    if (nextCh == '=') {
        return { TokenType::OP_REL, std::string{c, nextCh} };
    }
    source_.ungetChar(nextCh);
    return { TokenType::OP_REL, std::string{c} };
}

Token Lexer::constructNotEq(char c) {
    c = source_.getChar();
    if (c != '=') {
        throw std::runtime_error("Lexer error: '!' not followed by '='!");
    }
    return { TokenType::OP_REL, "!=" };
}

Token Lexer::constructString(char c) {
    assert(c == '"');
    bool escaping = false;
    std::ostringstream buffer;
    buffer << c;
    uint length = 1;

    while (true) {
        c = source_.getChar();

        if (consumeNewline(c)) {
            throw std::runtime_error("Lexer error: closing '\"' not found in string before encountering newline!");
        }

        assertStringLength(++length);
        buffer << c;

        switch (c) {
            case '\\':
                escaping = true;
                break;
            case '"':
                if (!escaping) {
                    return { TokenType::STRING, buffer.str() };
                }
                [[fallthrough]];
            default:
                escaping = false;
            //TODO case '{' //formatting
        }
    }
}

Token Lexer::constructIdOrUnitOrKeyword(char c) {  
    std::stringstream buffer;
    buffer << c;
    uint length = 1;

    c = source_.getChar();
    while (isalpha(c) || c == '_' || isdigit(c)) {
        buffer << c;
        assertIdLength(++length);
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
    int digitInBlockCounter = 1;
    int separatorCounter = 0;
    int value = c - '0';
    uint length = 1;

    while (true) {
        char prevCh = c;
        c = source_.getChar();
        assertIntNumberLength(++length);

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
                if (!isdigit(prevCh)) {
                    throw std::runtime_error("Lexer error: Dot not following a digit in number!");
                }

                auto [intValue, intLength] = parseIntFromSource();
                if (intLength == 0) {
                    throw std::runtime_error("Lexer error: Fraction part is empty in number!");
                }
                int divisor = 1;
                for (; intLength > 0; --intLength) {
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
        assertPostDotNumberLength(++length);
        value = value * 10 + c - '0';
    }
}

void Lexer::assertIdLength(uint length) const {
    if (length > MAX_ID_LENGTH) {
        std::ostringstream msg;
        msg << "Lexer error: Id too long! Max: " << MAX_ID_LENGTH << "chars";
        throw std::runtime_error(msg.str());
    }
}

void Lexer::assertIntNumberLength(uint length) const {
    if (length > MAX_INT_NUMBER_LENGTH) {
        std::ostringstream msg;
        msg << "Lexer error: Number too long! Max: " << MAX_INT_NUMBER_LENGTH << "chars";
        throw std::runtime_error(msg.str());
    }
}

void Lexer::assertPostDotNumberLength(uint length) const {
    if (length > MAX_POST_DOT_NUMBER_LENGTH) {
        std::ostringstream msg;
        msg << "Lexer error: Fraction number too long! Max: " << MAX_POST_DOT_NUMBER_LENGTH << "chars";
        throw std::runtime_error(msg.str());
    }
}

void Lexer::assertStringLength(uint length) const {
    if (length > MAX_STRING_LITERAL_LENGTH) {
        std::ostringstream msg;
        msg << "Lexer error: String literal too long! Max: " << MAX_STRING_LITERAL_LENGTH << "chars";
        throw std::runtime_error(msg.str());
    }
}
