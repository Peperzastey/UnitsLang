#ifndef TKOMSIUNITS_UNKNOWN_LEXEME_ERROR_H_INCLUDED
#define TKOMSIUNITS_UNKNOWN_LEXEME_ERROR_H_INCLUDED

#include "LexerError.h"

class UnknownLexemeError : public LexerError {
public:
    UnknownLexemeError(char c, std::size_t line, std::size_t column)
        : LexerError(
                std::string("Unknown lexeme beginning with character '") + c + "'"
                , line, column
            ) {}
};

#endif // TKOMSIUNITS_UNKNOWN_LEXEME_ERROR_H_INCLUDED
