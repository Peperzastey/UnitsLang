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
    KEYWORD_ELIF,
    KEYWORD_ELSE,
    KEYWORD_FALSE,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_IN,
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
    END_OF_STREAM,
    INVALID_TOKEN
};

struct Token {
    TokenType type;
    std::variant<std::string, int, double> value;
};

class Lexer {
public:
    const unsigned int MAX_ID_LENGTH = 250;
    const unsigned int MAX_INT_NUMBER_LENGTH = 12;
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
    Token constructIdOrUnitOrKeyword(char c);
    Token constructNumber(char c);
    Token constructEndOfInstr(char c);
    Token constructAdditiveOpOrFuncResult(char c);
    Token constructAssignOrEq(char c);
    Token constructRelationalOp(char c);
    Token constructNotEq(char c);
    Token constructString(char c);

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

    static std::unordered_map<std::string, TokenType> keywords_;
};

#endif // TKOMSIUNITS_LEXER_H_INCLUDED
