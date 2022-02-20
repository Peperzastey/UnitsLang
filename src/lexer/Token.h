#ifndef TKOMSIUNITS_TOKEN_H_INCLUDED
#define TKOMSIUNITS_TOKEN_H_INCLUDED

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class TokenType {
    ID,
    NUMBER,
    UNIT,
    KEYWORD_BOOL,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_ELIF,
    KEYWORD_ELSE,
    KEYWORD_FALSE,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_IN,
    KEYWORD_RETURN,
    KEYWORD_STR,
    KEYWORD_TRUE,
    KEYWORD_WHILE,
    OP_MULT,
    OP_ADD,
    OP_SUFFIX,
    OP_EQ,
    OP_REL,
    OP_AND,
    OP_OR,
    ASSIGN,
    PAREN_OPEN,
    PAREN_CLOSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    SQUARE_OPEN,
    SQUARE_CLOSE,
    COMMA,
    FUNC_RESULT,
    STRING,
    TEXT_WITHIN_STRING,
    END_OF_INSTRUCTION,
    END_OF_STREAM
};

enum class UnitType {
    SECOND,
    GRAM,
    METER,
    NEWTON,
    PASCAL,
    JOULE
};

const std::unordered_map<std::string, double> unitPrefixes {
    { "T", 1'000'000'000'000.0 },
    { "G", 1'000'000'000.0 },
    { "M", 1'000'000.0 },
    { "k", 1'000.0 },
    { "h", 100.0 },
    { "da", 10.0 },
    { "", 1.0 },
    { "d", 0.1 },
    { "c", 0.01 },
    { "m", 0.001 }
};

struct Unit {
    std::string prefix = std::string("");
    UnitType unit;
    int power = 1;
};

struct Token;

struct String {
    std::vector<Token> innerTokens;
};

struct PosInStream;

struct Token {
    struct Position {
        Position (const PosInStream &pos);
        Position (unsigned int line, unsigned int col);

        unsigned int line;
        unsigned int column;
    };
    
    TokenType type;
    std::variant<std::string, int, double, Unit, String> value;
    Position pos = {0,0};
};

#endif // TKOMSIUNITS_TOKEN_H_INCLUDED
