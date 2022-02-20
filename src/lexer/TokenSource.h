#ifndef TKOMSIUNITS_TOKEN_SOURCE_H_INCLUDED
#define TKOMSIUNITS_TOKEN_SOURCE_H_INCLUDED

#include "Token.h"

class TokenSource {
public: 
    virtual Token getToken() = 0;

public:
    virtual ~TokenSource() = default;
};

#endif // TKOMSIUNITS_TOKEN_SOURCE_H_INCLUDED
