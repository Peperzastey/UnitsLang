#include "source/Source.h"
#include "source/StringSource.h"
#include "Lexer.h"
#include <memory>
#include <unordered_map>
#include <gtest/gtest.h>

TEST(LexerTests, Keywords) {
    std::unordered_map<std::string, TokenType> keywords = {
        { "bool"     , TokenType::KEYWORD_BOOL     },
        { "break"    , TokenType::KEYWORD_BREAK    },
        { "continue" , TokenType::KEYWORD_CONTINUE },
        { "elif"     , TokenType::KEYWORD_ELIF     },
        { "else"     , TokenType::KEYWORD_ELSE     },
        { "false"    , TokenType::KEYWORD_FALSE    },
        { "func"     , TokenType::KEYWORD_FUNC     },
        { "if"       , TokenType::KEYWORD_IF       },
        { "in"       , TokenType::KEYWORD_IN       },
        { "return"   , TokenType::KEYWORD_RETURN   },
        { "true"     , TokenType::KEYWORD_TRUE     },
        { "while"    , TokenType::KEYWORD_WHILE    }
    };

    for (const auto & [keywordStr, keywordType] : keywords) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(keywordStr);
        Lexer lexer(*src);
        Token token = lexer.getToken();
        EXPECT_EQ(keywordType, token.type);
        token = lexer.getToken();
        EXPECT_EQ(TokenType::END_OF_STREAM, token.type);
    }
}

TEST(LexerTests, IdsThatStartLikeKeywords) {
    std::array ids = {
        "bool_",
        "breaka",
        "continue3",
        "elif_",
        "else1",
        "fals",
        "funcc",
        "if_",
        "in_",
        "return1",
        "truee",
        "while0"
    };

    for (const auto &id : ids) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(id);
        Lexer lexer(*src);
        Token token = lexer.getToken();
        EXPECT_EQ(TokenType::ID, token.type);
        token = lexer.getToken();
        EXPECT_EQ(TokenType::END_OF_STREAM, token.type);
    }
}
