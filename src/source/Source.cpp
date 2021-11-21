#include "Source.h"

char Source::getChar() {
    if (!ungetCharsBuff_.empty()) {
        char c = ungetCharsBuff_.top();
        ungetCharsBuff_.pop();
        return c;
    }
    return provideChar();
}

void Source::ungetChar(char c) {
    ungetCharsBuff_.push(c);
}

Source::~Source() {}