#include "Source.h"
#include <cassert>

char Source::getChar() {
    if (!ungetCharsBuff_.empty()) {
        char result = ungetCharsBuff_.top();
        ungetCharsBuff_.pop();
        return result;
    }
    char result = provideChar();
    
    if (endOfLineEncountered_) {
        ++lineNumber_;
        posInLine_ = 1;
        endOfLineEncountered_ = false;
    } else {
        ++posInLine_;
    }
    if (result == '\n') {
        endOfLineEncountered_ = true;
    }

    return result;
}

void Source::ungetChar(char c) {
    // ungetChar() cannot be called before first call to getChar()
    assert(posInLine_ > 0);
    ungetCharsBuff_.push(c);
}

PosInStream Source::getCurrentPosition() const {
    return { lineNumber_, posInLine_ };
}

Source::~Source() {}
