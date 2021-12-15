#include "source/Source.h"
#include "source/StringSource.h"
#include "Parser.h"
#include "utils/printUtils.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

const std::unordered_map<std::string, Token> tokens {
    { "1"        , {TokenType::NUMBER           , 1   } },
    { "a"        , {TokenType::ID               , "a" } },
    { "*"        , {TokenType::OP_MULT          , "*" } },
    { "/"        , {TokenType::OP_MULT          , "/" } },
    { "+"        , {TokenType::OP_ADD           , "+" } },
    { "-"        , {TokenType::OP_ADD           , "-" } },
    { "++"       , {TokenType::OP_SUFFIX        , "++"} },
    { "--"       , {TokenType::OP_SUFFIX        , "--"} },
    { "("        , {TokenType::PAREN_OPEN       , ""  } },
    { ")"        , {TokenType::PAREN_CLOSE      , ""  } },
    // keywords
    { "bool"     , {TokenType::KEYWORD_BOOL     , ""  } },
    { "break"    , {TokenType::KEYWORD_BREAK    , ""  } },
    { "continue" , {TokenType::KEYWORD_CONTINUE , ""  } },
    { "else"     , {TokenType::KEYWORD_ELSE     , ""  } },
    { "false"    , {TokenType::KEYWORD_FALSE    , ""  } },
    { "func"     , {TokenType::KEYWORD_FUNC     , ""  } },
    { "if"       , {TokenType::KEYWORD_IF       , ""  } },
    { "in"       , {TokenType::KEYWORD_IN       , ""  } },
    { "return"   , {TokenType::KEYWORD_RETURN   , ""  } },
    { "true"     , {TokenType::KEYWORD_TRUE     , ""  } },
    { "while"    , {TokenType::KEYWORD_WHILE    , ""  } }
};

class MockLexer {
public:
    explicit MockLexer(const std::string &tokensStr)
        : tokenSeq_{} {
        std::istringstream iss(tokensStr);
        std::string token;
        while (std::getline(iss, token, ' ')) {
            tokenSeq_.push_back(tokens.at(token));
        }
    }

    Token getToken() {
        if (pos < tokenSeq_.size()) {
            return tokenSeq_[pos++];
        }
        return { TokenType::END_OF_STREAM, "" };
    }

private:
    std::vector<Token> tokenSeq_;
    std::size_t pos = 0;
};

class TestParser : private Parser<MockLexer> {
public:
    using Parser::Parser;
    using Parser::advance;
    using Parser::parseFuncDef;
    using Parser::parseInstruction;
    using Parser::parseExpression;
    using Parser::parseAddExpression;
    using Parser::parseMultExpression;
    using Parser::parseExpressionElement;
};

TEST(ParserTests, SimpleAddition) {
    std::string input = "1 + 1";
    std::string expectedRPN = "11+";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Expression> expr = parser.parseExpression();
    ASSERT_NE(nullptr, expr);
    EXPECT_EQ(expectedRPN, expr->getRPN());
}

TEST(ParserTests, ComplexCalculation) {
    std::string input = "1 + 1 * a + ( 1 + a - 1 * ( 1 - a + a ( ) ) ) * 1 / 1 / 1 - 1";
    std::string expectedRPN = "11a*+1a+11a-a()+*-1*1/1/+1-";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Expression> expr = parser.parseExpression();
    ASSERT_NE(nullptr, expr);
    EXPECT_EQ(expectedRPN, expr->getRPN());
}
