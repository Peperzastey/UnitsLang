#ifndef TKOMSIUNITS_SOURCE_H_INCLUDED
#define TKOMSIUNITS_SOURCE_H_INCLUDED

#include <stack>

struct PosInStream {
    unsigned int lineNumber;
    unsigned int posInLine;
};

class Source {
public:
    char getChar();
    void ungetChar(char c);
    PosInStream getCurrentPosition() const;

    virtual ~Source();

private:
    virtual char provideChar() = 0;

private:
    std::stack<char> ungetCharsBuff_;
    unsigned int lineNumber_ = 1;
    unsigned int posInLine_ = 0;
    bool endOfLineEncountered_ = false;
};

#endif // TKOMSIUNITS_SOURCE_H_INCLUDED
