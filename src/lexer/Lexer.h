#ifndef TKOMSIUNITS_LEXER_H_INCLUDED
#define TKOMSIUNITS_LEXER_H_INCLUDED

#include "source/Source.h"
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

struct Token {
    struct Position {
        Position (const PosInStream &pos)
            : line(pos.lineNumber), column(pos.posInLine) {}
        Position (unsigned int line, unsigned int col)
            : line(line), column(col) {}
        unsigned int line;
        unsigned int column;
    };
    
    TokenType type;
    std::variant<std::string, int, double, Unit, String> value;
    Position pos = {0,0};
};

class Lexer {
public:
    const unsigned int MAX_ID_LENGTH = 250;
    const unsigned int MAX_INT_NUMBER_LENGTH = 25;
    const unsigned int MAX_POST_DOT_NUMBER_LENGTH = 12;
    const unsigned int MAX_STRING_LITERAL_LENGTH = 250;

public:
    explicit Lexer(Source &source) : source_(source) {}

    Token getToken();

private:
    char discardWhitespacesAndComments();
    bool discardComment();
    void discardInstrBreak();
    bool consumeNewline(char c);
    Token constructIdOrUnitOrKeyword(char c, const PosInStream &cPos);
    Token constructNumber(char c, const PosInStream &cPos);
    Token constructEndOfInstr(char c, const PosInStream &cPos);
    Token constructAdditiveOpOrFuncResult(char c, const PosInStream &cPos);
    Token constructAssignOrEq(char c, const PosInStream &cPos);
    Token constructRelationalOp(char c, const PosInStream &cPos);
    Token constructNotEq(char c, const PosInStream &cPos);
    Token constructAnd(char c, const PosInStream &cPos);
    Token constructOr(char c, const PosInStream &cPos);
    Token constructString(char c, const PosInStream &cPos);

    struct IntWithLength {
        int value;
        int numberOfDigits;
    };
    IntWithLength parseIntFromSource();

    void assertIdLength(unsigned int length) const;
    void assertIntNumberLength(unsigned int length) const;
    void assertPostDotNumberLength(unsigned int length) const;
    void assertStringLength(unsigned int length) const;

private:
    Source &source_;
    mutable char currChar_;

    static const std::unordered_map<std::string, TokenType> keywords_;
    static const std::unordered_map<std::string, Unit> units_;
};

#endif // TKOMSIUNITS_LEXER_H_INCLUDED
