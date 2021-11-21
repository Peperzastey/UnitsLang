#ifndef TKOMSIUNITS_LEXER_H_INCLUDED
#define TKOMSIUNITS_LEXER_H_INCLUDED

#include "source/Source.h"
#include <string>
#include <unordered_map>
#include <variant>

enum class TokenType {
    ID,
    KEYWORD_WHILE,
    KEYWORD_IF,
    NUMBER,
    OP_MULT,
    OP_ADD,
    OP_SUFFIX,
    END_OF_INSTRUCTION,
    END_OF_STREAM
    //, ...
};

struct Token {
    TokenType type;
    std::variant<std::string, int, double> value; //TODO only double?
};

class Lexer {
public:
    const uint MAX_ID_LENGTH = 250;
    const uint MAX_NUMBER_LENGTH = 250;
    const uint MAX_STRING_LITERAL_LENGTH = 250;

public:
    explicit Lexer(Source &source) : source_(source) {}

    Token getToken();

private:
    char discardWhitespacesAndComments();
    bool discardComment();
    void discardInstrBreak();
    Token constructIdOrUnitOrKeyword(char c);
    Token constructNumber(char c);
    Token constructEndOfInstr(char c);
    Token constructAdditiveOp(char c);

    struct IntWithLength {
        int value;
        int numberOfDigits;
    };
    IntWithLength parseIntFromSource();

private:
    Source &source_;
    mutable char currChar_;

    static std::unordered_map<std::string, TokenType> keywords_;
};

#endif // TKOMSIUNITS_LEXER_H_INCLUDED
