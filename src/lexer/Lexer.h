#ifndef TKOMSIUNITS_LEXER_H_INCLUDED
#define TKOMSIUNITS_LEXER_H_INCLUDED

#include "TokenSource.h"
#include "source/Source.h"

class Lexer : public TokenSource {
public:
    const unsigned int MAX_ID_LENGTH = 250;
    const unsigned int MAX_INT_NUMBER_LENGTH = 25;
    const unsigned int MAX_POST_DOT_NUMBER_LENGTH = 12;
    const unsigned int MAX_STRING_LITERAL_LENGTH = 250;

public:
    explicit Lexer(Source &source) : source_(source) {}

    Token getToken() override;

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
