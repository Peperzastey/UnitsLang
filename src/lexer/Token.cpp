#include "Token.h"

#include "source/Source.h"

Token::Position::Position(const PosInStream &pos)
    : line(pos.lineNumber)
    , column(pos.posInLine) {}

Token::Position::Position(unsigned int line, unsigned int col)
    : line(line)
    , column(col) {}
