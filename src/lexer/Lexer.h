#ifndef TKOMSIUNITS_LEXER_H_INCLUDED
#define TKOMSIUNITS_LEXER_H_INCLUDED

#include "source/Source.h"
#include <string>
#include <unordered_map>
#include <variant>

enum class TokenType {
    ID,
    NUMBER,
    KEYWORD_BOOL,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_ELSE,
    KEYWORD_FALSE,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_IN,
    //KEYWORD_PRINT,  //TODO???
    KEYWORD_RETURN,
    KEYWORD_TRUE,
    KEYWORD_WHILE,
    OP_MULT,
    OP_ADD,
    OP_SUFFIX,
    OP_REL,
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
    const uint MAX_INT_NUMBER_LENGTH = 12;
    const uint MAX_POST_DOT_NUMBER_LENGTH = 12;
    const uint MAX_STRING_LITERAL_LENGTH = 250;

public:
    explicit Lexer(Source &source) : source_(source) {}

    Token getToken();

private:
    char discardWhitespacesAndComments();
    bool discardComment();
    void discardInstrBreak();
    bool consumeNewline(char c);
    Token constructIdOrUnitOrKeyword(char c);
    Token constructNumber(char c);
    Token constructEndOfInstr(char c);
    Token constructAdditiveOpOrFuncResult(char c);
    Token constructAssignOrEq(char c);
    Token constructRelationalOp(char c);
    Token constructNotEq(char c);
    Token constructString(char c); //TODO parse inner lexemes

    struct IntWithLength {
        int value;
        int numberOfDigits;
    };
    IntWithLength parseIntFromSource();

    void assertIdLength(uint length) const;
    void assertIntNumberLength(uint length) const;
    void assertPostDotNumberLength(uint length) const;
    void assertStringLength(uint length) const;

private:
    Source &source_;
    mutable char currChar_;

    static std::unordered_map<std::string, TokenType> keywords_;
};

#endif // TKOMSIUNITS_LEXER_H_INCLUDED
