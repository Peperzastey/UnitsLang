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
    { "1"        , {TokenType::NUMBER             , 1   } },
    { "a"        , {TokenType::ID                 , "a" } },
    { "*"        , {TokenType::OP_MULT            , "*" } },
    { "/"        , {TokenType::OP_MULT            , "/" } },
    { "+"        , {TokenType::OP_ADD             , "+" } },
    { "-"        , {TokenType::OP_ADD             , "-" } },
    { "++"       , {TokenType::OP_SUFFIX          , "++"} },
    { "--"       , {TokenType::OP_SUFFIX          , "--"} },
    { "="        , {TokenType::ASSIGN             , ""  } },
    { "("        , {TokenType::PAREN_OPEN         , ""  } },
    { ")"        , {TokenType::PAREN_CLOSE        , ""  } },
    { "{"        , {TokenType::BRACKET_OPEN       , ""  } },
    { "}"        , {TokenType::BRACKET_CLOSE      , ""  } },
    { "["        , {TokenType::SQUARE_OPEN        , ""  } },
    { "]"        , {TokenType::SQUARE_CLOSE       , ""  } },
    { "\n"       , {TokenType::END_OF_INSTRUCTION , ""  } },
    // keywords
    { "bool"     , {TokenType::KEYWORD_BOOL       , ""  } },
    { "break"    , {TokenType::KEYWORD_BREAK      , ""  } },
    { "continue" , {TokenType::KEYWORD_CONTINUE   , ""  } },
    { "elif"     , {TokenType::KEYWORD_ELIF       , ""  } },
    { "else"     , {TokenType::KEYWORD_ELSE       , ""  } },
    { "false"    , {TokenType::KEYWORD_FALSE      , ""  } },
    { "func"     , {TokenType::KEYWORD_FUNC       , ""  } },
    { "if"       , {TokenType::KEYWORD_IF         , ""  } },
    { "in"       , {TokenType::KEYWORD_IN         , ""  } },
    { "return"   , {TokenType::KEYWORD_RETURN     , ""  } },
    { "true"     , {TokenType::KEYWORD_TRUE       , ""  } },
    { "while"    , {TokenType::KEYWORD_WHILE      , ""  } }
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

TEST(ParserTests, InstructionExpectsEndOfInstructionToken) {
    std::vector<std::string> incorrectInputs {
        "break",
        "continue",
        "return",
        "a = 1",
        "a ( )"
    };
    for (const auto &input : incorrectInputs) {
        MockLexer lexer(input);
        TestParser parser(lexer);
        parser.advance();
        EXPECT_THROW({
                std::unique_ptr<Instruction> instr = parser.parseInstruction();
            },
            std::runtime_error
        );
    }
    
    std::vector<std::string> correctInputs {
        "break \n",
        "continue \n",
        "return \n",
        "a = 1 \n",
        "a ( ) \n"
    };
    for (const auto &input : correctInputs) {
        MockLexer lexer(input);
        TestParser parser(lexer);
        parser.advance();
        std::unique_ptr<Instruction> instr = nullptr;
        ASSERT_NO_THROW({
            instr = parser.parseInstruction();
        });
        ASSERT_NE(nullptr, instr);
    }
}

TEST(ParserTests, ReturnVoid) {
    std::string input = "return \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    Return *returnObj = dynamic_cast<Return *>(instr.get());
    ASSERT_NE(nullptr, returnObj);
}

TEST(ParserTests, ReturnWithExpression) {
    std::string input = "return a + 1 * a ( ) \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    Return *returnObj = dynamic_cast<Return *>(instr.get());
    ASSERT_NE(nullptr, returnObj);
}

TEST(ParserTests, ReturnWithInvalidExpressionFails) {
    std::string input = "return while \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    EXPECT_THROW({
            std::unique_ptr<Instruction> instr = parser.parseInstruction();
        },
        std::runtime_error
    );
}

TEST(ParserTests, VarDef) {
    std::string input = "a = 1 + 1 \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    VarDef *varDef = dynamic_cast<VarDef *>(instr.get());
    ASSERT_NE(nullptr, varDef);
}

TEST(ParserTests, VarDefExpectsExpression) {
    std::vector<std::string> inputs {
        "a = \n",
        "a = return \n"
    };
    for (const auto &input : inputs) {
        MockLexer lexer(input);
        TestParser parser(lexer);
        parser.advance();
        EXPECT_THROW({
                std::unique_ptr<Instruction> instr = parser.parseInstruction();
            },
            std::runtime_error
        );
    }
}

TEST(ParserTests, FuncDef) {
    std::string input = "func a ( ) { } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
}

TEST(ParserTests, FuncDefParenAndBracketIgnoreNewlines) {
    std::string input = "func a ( \n \n ) { \n } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
}

TEST(ParserTests, FuncDefBlockWithInstructions) {
    std::string input = "func a ( ) { return \n } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
}

TEST(ParserTests, FuncCall) {
    std::string input = "a ( ) \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    ASSERT_NE(nullptr, instr);
}

TEST(ParserTests, Break) {
    std::string input = "break \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    Break *breakInstr = dynamic_cast<Break *>(instr.get());
    ASSERT_NE(nullptr, breakInstr);
}

TEST(ParserTests, Continue) {
    std::string input = "continue \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    Continue *continueInstr = dynamic_cast<Continue *>(instr.get());
    ASSERT_NE(nullptr, continueInstr);
}

TEST(ParserTests, If) {
    std::string input = "if a ( ) + a { \n a = a \n return \n } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    If *ifInstr = dynamic_cast<If *>(instr.get());
    ASSERT_NE(nullptr, ifInstr);
}

TEST(ParserTests, IfElse) {
    std::string input = "if a { } else { } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    If *ifInstr = dynamic_cast<If *>(instr.get());
    ASSERT_NE(nullptr, ifInstr);
}

TEST(ParserTests, IfElifChainElse) {
    std::string input = "if a { } elif a { } elif a { } else { } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    If *ifInstr = dynamic_cast<If *>(instr.get());
    ASSERT_NE(nullptr, ifInstr);
}

TEST(ParserTests, NestedIfs) {
    std::string input = "if a { if a { } elif a { } else { } \n } else { } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    If *ifInstr = dynamic_cast<If *>(instr.get());
    ASSERT_NE(nullptr, ifInstr);
}

TEST(ParserTests, While) {
    std::string input = "while a { return \n } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    While *whileInstr = dynamic_cast<While *>(instr.get());
    ASSERT_NE(nullptr, whileInstr);
}

TEST(ParserTests, NestedWhiles) {
    std::string input = "while a { while a { } \n } \n";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    While *whileInstr = dynamic_cast<While *>(instr.get());
    ASSERT_NE(nullptr, whileInstr);
}
