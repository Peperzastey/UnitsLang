#include "source/Source.h"
#include "source/StringSource.h"
#include "Parser.h"
#include "utils/printUtils.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <gtest/gtest.h>

class TestParser : private Parser {
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
        std::string input = "2+2";
        std::string expectedRPN = "22+";
        std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
        Lexer lexer(*src);
        TestParser parser(lexer);
        parser.advance();
        std::unique_ptr<Expression> expr = parser.parseExpression();
        ASSERT_NE(nullptr, expr);
        EXPECT_EQ(expectedRPN, expr->getRPN());
}

TEST(ParserTests, ComplexCalculation) {
        std::string input = "2 + 3*5 + ( 1+4 - 2 * (2-a+f()) ) * 6 / 2 / 3 - 1";
        std::string expectedRPN = "235*+14+22a-f()+*-6*2/3/+1-";
        std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
        Lexer lexer(*src);
        TestParser parser(lexer);
        parser.advance();
        std::unique_ptr<Expression> expr = parser.parseExpression();
        ASSERT_NE(nullptr, expr);
        EXPECT_EQ(expectedRPN, expr->getRPN());
}
