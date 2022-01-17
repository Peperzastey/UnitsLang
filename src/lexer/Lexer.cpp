#include "Lexer.h"
#include "error/ErrorHandler.h"
#include "error/LexerError.h"
#include "error/UnknownLexemeError.h"
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <sstream>
#include <vector>

const std::unordered_map<std::string, TokenType> Lexer::keywords_ {
    { "bool"     , TokenType::KEYWORD_BOOL     },
    { "break"    , TokenType::KEYWORD_BREAK    },
    { "continue" , TokenType::KEYWORD_CONTINUE },
    { "elif"     , TokenType::KEYWORD_ELIF     },
    { "else"     , TokenType::KEYWORD_ELSE     },
    { "false"    , TokenType::KEYWORD_FALSE    },
    { "func"     , TokenType::KEYWORD_FUNC     },
    { "if"       , TokenType::KEYWORD_IF       },
    { "in"       , TokenType::KEYWORD_IN       },
    { "return"   , TokenType::KEYWORD_RETURN   },
    { "str"      , TokenType::KEYWORD_STR      },
    { "true"     , TokenType::KEYWORD_TRUE     },
    { "while"    , TokenType::KEYWORD_WHILE    }
};

const std::vector<std::pair<std::string, UnitType>> units {
    { "s", UnitType::SECOND },
    { "g", UnitType::GRAM },
    { "m", UnitType::METER },
    { "N", UnitType::NEWTON },
    { "Pa", UnitType::PASCAL },
    { "J", UnitType::JOULE }
};

const std::vector<std::pair<std::string, int>> unitPowers {
    { "", 1 },
    { "2", 2 },
    { "3", 3 }
};

std::unordered_map<std::string, Unit> createUnitsMap() {
    std::unordered_map<std::string, Unit> unitsMap;
    for (auto &&[pref, _] : unitPrefixes) {
        (void)_;
        for (auto &&[unit, unitVal] : units) {
            for (auto &&[power, powerVal] : unitPowers) {
                std::string unitString = pref + unit + power;
                Unit unitStructure {pref, unitVal, powerVal};
                unitsMap.insert({ std::move(unitString), std::move(unitStructure) });
            }
        }
    }
    return unitsMap;
}

const std::unordered_map<std::string, Unit> Lexer::units_ = createUnitsMap();

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
    // it is a comment
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
        ErrorHandler::handleFromLexer("Instruction break ('\\') followed by non-space character!");
    }
}

bool Lexer::consumeNewline(char c) {
    bool gotR = false;
    if (c == EOF) {
        //TODO? unget
        return true;
    }
    if (c == '\r') {
        gotR = true;
        c = source_.getChar();
    }
    if (c != '\n') {
        if (gotR) {
            ErrorHandler::handleFromLexer("'\\r' not followed by '\\n'!");
        }
        return false;
    }
    return true;
}

Token Lexer::getToken() {
    char c = discardWhitespacesAndComments();
    PosInStream charPos = source_.getCurrentPosition();
    
    if (isalpha(c) || c == '_') {
        return constructIdOrUnitOrKeyword(c, charPos);
    }

    if (isdigit(c)) {
        return constructNumber(c, charPos);
    }

    switch (c) {
        case '\r':
        case '\n':
            return constructEndOfInstr(c, charPos);
        case '/':
        case '*':
            return { TokenType::OP_MULT, std::string{c}, charPos };
        case '+':
        case '-':
            return constructAdditiveOpOrFuncResult(c, charPos);
        case '=':
            return constructAssignOrEq(c, charPos);
        case '>':
        case '<':
            return constructRelationalOp(c, charPos);
        case '!':
            return constructNotEq(c, charPos);
        case '"':
            return constructString(c, charPos);
        case '(':
            return { TokenType::PAREN_OPEN, "", charPos };
        case ')':
            return { TokenType::PAREN_CLOSE, "", charPos };
        case '{':
            return { TokenType::BRACKET_OPEN, "", charPos };
        case '}':
            return { TokenType::BRACKET_CLOSE, "", charPos };
        case '[':
            return { TokenType::SQUARE_OPEN, "", charPos };
        case ']':
            return { TokenType::SQUARE_CLOSE, "", charPos };
        case ',':
            return { TokenType::COMMA, "", charPos };
        case EOF:
            return { TokenType::END_OF_STREAM, "", charPos };
        default: {
            //ErrorHandler::handleFromLexer("Unknown lexeme");
            auto [line, col] = source_.getCurrentPosition();
            throw UnknownLexemeError(c, line, col);
        }
    }
}

Token Lexer::constructEndOfInstr(char c, const PosInStream &cPos) {
    bool newlineConstructed = consumeNewline(c);
    assert(newlineConstructed);
    return { TokenType::END_OF_INSTRUCTION, "", cPos };
}

Token Lexer::constructAdditiveOpOrFuncResult(char c, const PosInStream &cPos) {
    char nextCh = source_.getChar();
    if (nextCh == c) {
        return { TokenType::OP_SUFFIX, std::string{c, nextCh}, cPos };
    } else if (c == '-' && nextCh == '>') {
        return { TokenType::FUNC_RESULT, "", cPos };
    }
    source_.ungetChar(nextCh);
    return { TokenType::OP_ADD, std::string{c}, cPos };
}

Token Lexer::constructAssignOrEq([[maybe_unused]] char c, const PosInStream &cPos) {
    assert(c == '=');
    char nextCh = source_.getChar();
    if (nextCh == '=') {
        return { TokenType::OP_EQ, "==", cPos };
    }
    source_.ungetChar(nextCh);
    return { TokenType::ASSIGN, "", cPos };
}

Token Lexer::constructRelationalOp(char c, const PosInStream &cPos) {
    char nextCh = source_.getChar();
    if (nextCh == '=') {
        return { TokenType::OP_REL, std::string{c, nextCh}, cPos };
    }
    source_.ungetChar(nextCh);
    return { TokenType::OP_REL, std::string{c}, cPos };
}

Token Lexer::constructNotEq(char c, const PosInStream &cPos) {
    c = source_.getChar();
    if (c != '=') {
        ErrorHandler::handleFromLexer("'!' not followed by '='!");
    }
    return { TokenType::OP_EQ, "!=", cPos };
}

Token Lexer::constructString(char c, const PosInStream &cPos) {
    assert(c == '"');
    bool escaping = false;
    std::ostringstream buffer;
    unsigned int length = 0;
    PosInStream charPos;
    //PosInStream textCharPos = source_.getCurrentPosition();
    String stringToken;

    while (true) {
        c = source_.getChar();
        charPos = source_.getCurrentPosition();

        if (consumeNewline(c)) {
            ErrorHandler::handleFromLexer("closing '\"' not found in string before encountering newline!");
        }

        switch (c) {
            case '\\':
                escaping = true;
                c = 0;
                break;
            case '{':
                if (!escaping) {
                    std::string text = buffer.str();
                    buffer.str("");
                    if (!text.empty()) {
                        stringToken.innerTokens.push_back({
                                TokenType::TEXT_WITHIN_STRING, text //TODO position
                            });
                    }
                        stringToken.innerTokens.push_back({
                            TokenType::BRACKET_OPEN, "", charPos
                        });
                    Token format;
                    do {
                        format = getToken();
                        stringToken.innerTokens.push_back(format);
                    }
                    while (format.type != TokenType::BRACKET_CLOSE);
                    c = 0;
                }
                escaping = false;
                break;
            /*case '}':
                if (!escaping) {
                    stringToken.innerTokens.emplace_back(
                            TokenType::BRACKET_CLOSE, "", charPos
                        );
                    c = 0;
                }
                escaping = false;
                break;*/
            case 'n':
                if (escaping) {
                    c = '\n';
                    escaping = false;
                }
                break;
            case 't':
                if (escaping) {
                    c = '\t';
                    escaping = false;
                }
                break;
            case '"':
                if (!escaping) {
                    std::string text = buffer.str();
                    if (!text.empty()) {
                        stringToken.innerTokens.push_back({
                                TokenType::TEXT_WITHIN_STRING, text //TODO position
                            });
                    }
                    return { TokenType::STRING, stringToken, cPos };
                }
                [[fallthrough]];
            default:
                escaping = false;
        }
        if (c) {
            buffer << c;
            assertStringLength(++length);
        }
    }
}

Token Lexer::constructIdOrUnitOrKeyword(char c, const PosInStream &cPos) {
    std::stringstream buffer;
    buffer << c;
    unsigned int length = 1;

    c = source_.getChar();
    while (isalpha(c) || c == '_' || isdigit(c)) {
        buffer << c;
        assertIdLength(++length);
        c = source_.getChar();
    }
    source_.ungetChar(c);
    std::string value = buffer.str();

    // check if it is a keyword
    if (auto it = keywords_.find(value); it != keywords_.end()) {
        return { it->second, value, cPos };
    }
    // check if it is a unit
    if (auto it = units_.find(value); it != units_.end()) {
        return { TokenType::UNIT, it->second, cPos };
    }
    return { TokenType::ID, value, cPos };
}

Token Lexer::constructNumber(char c, const PosInStream &cPos) {
    assert(isdigit(c));
    int value = c - '0';
    std::vector<int> digitsInBlocks({0});
    unsigned int length = 1;

    while (true) {
        char prevCh = c;
        c = source_.getChar();
        assertIntNumberLength(++length);

        if (isdigit(c)) {
            if (value == 0) { // first digit is 0 and is not followed by dot
                ErrorHandler::handleFromLexer("First digit is 0 and is not followed by dot in number!");
            }
            ++digitsInBlocks.back();
            if (
                digitsInBlocks.size() > 1 // digit blocks are being used
                && (
                    digitsInBlocks.back() > 3
                    || digitsInBlocks[0] > 3
                )
            ) {
                ErrorHandler::handleFromLexer("Digit block in number longer than 3 digits!");
            }
            value = value * 10 + c - '0';
            continue;
        }

        switch (c) {
            case ' ': // this could be the space ending the number
                if (!isdigit(prevCh)) {
                    ErrorHandler::handleFromLexer("Digit block separator not following a digit in number!");
                }
                digitsInBlocks.emplace_back(0);
                break;

            case '.': {
                if (!isdigit(prevCh)) {
                    ErrorHandler::handleFromLexer("Dot not following a digit in number!");
                }

                auto [intValue, intLength] = parseIntFromSource();
                if (intLength == 0) {
                    ErrorHandler::handleFromLexer("Fraction part is empty in number!");
                }
                int divisor = 1;
                for (; intLength > 0; --intLength) {
                    divisor *= 10;
                }
                double fraction = static_cast<double>(intValue) / divisor;
                double doubleValue = static_cast<double>(value) + fraction;
                return { TokenType::NUMBER, doubleValue, cPos };
            }

            default:
                source_.ungetChar(c); // number can be followed by a unit
                if (prevCh == ' ') {
                    source_.ungetChar(prevCh);
                }
                return { TokenType::NUMBER, value, cPos };
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

void Lexer::assertIdLength(unsigned int length) const {
    if (length > MAX_ID_LENGTH) {
        std::ostringstream msg;
        msg << "Id too long! Max: " << MAX_ID_LENGTH << "chars";
        ErrorHandler::handleFromLexer(msg.str());
    }
}

void Lexer::assertIntNumberLength(unsigned int length) const {
    if (length > MAX_INT_NUMBER_LENGTH) {
        std::ostringstream msg;
        msg << "Number too long! Max: " << MAX_INT_NUMBER_LENGTH << "chars";
        ErrorHandler::handleFromLexer(msg.str());
    }
}

void Lexer::assertPostDotNumberLength(unsigned int length) const {
    if (length > MAX_POST_DOT_NUMBER_LENGTH) {
        std::ostringstream msg;
        msg << "Fraction number too long! Max: " << MAX_POST_DOT_NUMBER_LENGTH << "chars";
        ErrorHandler::handleFromLexer(msg.str());
    }
}

void Lexer::assertStringLength(unsigned int length) const {
    if (length > MAX_STRING_LITERAL_LENGTH) {
        std::ostringstream msg;
        msg << "String literal too long! Max: " << MAX_STRING_LITERAL_LENGTH << "chars";
        ErrorHandler::handleFromLexer(msg.str());
    }
}
