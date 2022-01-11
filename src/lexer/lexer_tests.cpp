#include "source/Source.h"
#include "source/StringSource.h"
#include "Lexer.h"
#include "utils/printUtils.h"
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
        { "str"      , TokenType::KEYWORD_STR      },
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

//TODO test automatically all combinations (like they are generated)
TEST(LexerTests, Units) {
    std::array units = {
        /*std::pair{ "mm3", Unit{0.001      , UnitType::METER , 3} },
        std::pair{ "mm2", Unit{0.001      , UnitType::METER , 2} },
        std::pair{ "mm" , Unit{0.001      , UnitType::METER , 1} },
        std::pair{ "m"  , Unit{1          , UnitType::METER , 1} },
        std::pair{ "kg" , Unit{1'000.0    , UnitType::GRAM  , 1} },
        std::pair{ "MN" , Unit{1'000'000.0, UnitType::NEWTON, 1} },
        std::pair{ "J"  , Unit{1          , UnitType::JOULE , 1} },
        std::pair{ "kPa", Unit{1'000.0    , UnitType::PASCAL, 1} }*/
        std::pair{ "mm3", Unit{"m", UnitType::METER , 3} },
        std::pair{ "mm2", Unit{"m", UnitType::METER , 2} },
        std::pair{ "mm" , Unit{"m", UnitType::METER , 1} },
        std::pair{ "m"  , Unit{"" , UnitType::METER , 1} },
        std::pair{ "kg" , Unit{"k", UnitType::GRAM  , 1} },
        std::pair{ "MN" , Unit{"M", UnitType::NEWTON, 1} },
        std::pair{ "J"  , Unit{"" , UnitType::JOULE , 1} },
        std::pair{ "kPa", Unit{"k", UnitType::PASCAL, 1} }
    };

    for (const auto &[str, expectedUnit] : units) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(str);
        Lexer lexer(*src);
        Token token = lexer.getToken();
        EXPECT_EQ(TokenType::UNIT, token.type) << "not met for: " << str;

        Unit unit = std::get<Unit>(token.value);
        EXPECT_EQ(expectedUnit.prefix, unit.prefix) << "not met for: " << str;
        EXPECT_EQ(expectedUnit.unit, unit.unit) << "not met for: " << str;
        EXPECT_EQ(expectedUnit.power, unit.power) << "not met for: " << str;

        token = lexer.getToken();
        EXPECT_EQ(TokenType::END_OF_STREAM, token.type) << "not met for: " << str;
    }
}

TEST(LexerTests, UnitsAreCaseSensitive) {
    std::array ids = {
        "mM3",
        "mM2",
        "MM",
        "M",
        "kG",
        "tN",
        "j",
        "KPa"
    };

    for (const auto &id : ids) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(id);
        Lexer lexer(*src);
        Token token = lexer.getToken();
        EXPECT_EQ(TokenType::ID, token.type) << "not met for: " << id;
        token = lexer.getToken();
        EXPECT_EQ(TokenType::END_OF_STREAM, token.type) << "not met for: " << id;
    }
}

TEST(LexerTests, IdsThatStartLikeUnits) {
    std::array ids = {
        "mm3_",
        "mm2a",
        "mmx",
        "ma",
        "kg_",
        "TNx",
        "JM",
        "KPa_"
    };

    for (const auto &id : ids) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(id);
        Lexer lexer(*src);
        Token token = lexer.getToken();
        EXPECT_EQ(TokenType::ID, token.type) << "not met for: " << id;
        token = lexer.getToken();
        EXPECT_EQ(TokenType::END_OF_STREAM, token.type) << "not met for: " << id;
    }
}
