#ifndef TKOMSIUNITS_SOURCE_H_INCLUDED
#define TKOMSIUNITS_SOURCE_H_INCLUDED

#include <stack>

//TODO private ctor and factory! -- user cannot use any concrete Source class not using this base class as handler
class Source {
public:
    char getChar();
    void ungetChar(char c);

    virtual ~Source();

private:
    virtual char provideChar() = 0;

private:
    std::stack<char> ungetCharsBuff_;
    //TODO all Sources must return EOF as last character!
    //TODO make sure ungetChar is the same as last returned char!
};

#endif // TKOMSIUNITS_SOURCE_H_INCLUDED
