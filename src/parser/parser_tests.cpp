#include "source/Source.h"
#include "source/StringSource.h"
#include "Parser.h"
#include "utils/printUtils.h"
#include "lexer/Lexer.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

const std::unordered_map<std::string, Token> tokens {
    { "1"        , {TokenType::NUMBER             , 1   } },
    { "a"        , {TokenType::ID                 , "a" } },
    { "u"        , {TokenType::UNIT               , Unit{"", UnitType::METER, 1} } },
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
    { "str"      , {TokenType::KEYWORD_STR        , ""  } },
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
    using Parser::parseUnit;
    using Parser::parseType;
};

//TODO xD
class TestParserNotMock : private Parser<Lexer> {
public:
    using Parser::Parser;
    using Parser::advance;
    using Parser::parseFuncDef;
    using Parser::parseInstruction;
    using Parser::parseExpression;
    using Parser::parseAddExpression;
    using Parser::parseMultExpression;
    using Parser::parseExpressionElement;
    using Parser::parseUnit;
    using Parser::parseType;
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

TEST(ParserTests, FuncDefSingleParameter) {
    const std::string input = "func f(a [1]) {}\n";
    const std::string expectedRepr = "f(a[1])";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    ASSERT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncDefMultipleParameters) {
    const std::string input = "func f(a [1], b [mm2], c[bool]) {}\n";
    const std::string expectedRepr = "f(a[1],b[(mm2)/()],c[bool])";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    ASSERT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncDefIncorrectParameterListFormatThrows) {
    std::array inputs {
        "func f(a [1],) {}\n",
        "func f(a [1],,) {}\n",
        "func f(a [1],, b [1]) {}\n",
        "func f(,a [1]) {}\n",
        "func f(a [1] b [1]) {}\n",
    };
    
    for (const auto &input : inputs) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        EXPECT_THROW({
                std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
            },
            std::runtime_error
        ) << "not met for: " << input;
    }
}

TEST(ParserTests, FuncDefReturnTypeVoid) {
    const std::string input = "func f(a [1], b [mm2], c[bool]) {}\n";
    const std::string expectedRepr = "f(a[1],b[(mm2)/()],c[bool])";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    const VoidType *voidType = dynamic_cast<const VoidType *>(funcDef->getType());
    ASSERT_NE(nullptr, voidType);
    EXPECT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncDefReturnTypeBool) {
    const std::string input = "func f(a [1], b [mm2], c[bool]) -> [bool] {}\n";
    const std::string expectedRepr = "f(a[1],b[(mm2)/()],c[bool])->[bool]";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    const Bool *boolType = dynamic_cast<const Bool *>(funcDef->getType());
    ASSERT_NE(nullptr, boolType);
    EXPECT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncDefReturnTypeScalar) {
    const std::string input = "func f(a [1], b [mm2], c[bool]) -> [1] {}\n";
    const std::string expectedRepr = "f(a[1],b[(mm2)/()],c[bool])->[1]";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    const codeobj::Unit *scalar = dynamic_cast<const codeobj::Unit *>(funcDef->getType());
    ASSERT_NE(nullptr, scalar);
    EXPECT_EQ(true, scalar->isScalar());
    EXPECT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncDefReturnTypeUnit) {
    const std::string input = "func f(a [1], b [mm2], c[bool]) -> [mm2] {}\n";
    const std::string expectedRepr = "f(a[1],b[(mm2)/()],c[bool])->[(mm2)/()]";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<FuncDef> funcDef = parser.parseFuncDef();
    ASSERT_NE(nullptr, funcDef);
    const codeobj::Unit *unit = dynamic_cast<const codeobj::Unit *>(funcDef->getType());
    ASSERT_NE(nullptr, unit);
    EXPECT_EQ(false, unit->isScalar());
    EXPECT_EQ(expectedRepr, funcDef->toString());
}

TEST(ParserTests, FuncCall) {
    const std::string input = "a ( ) \n";
    const std::string expectedRepr = "a()";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    ASSERT_NE(nullptr, instr);
    FuncCall *funcCall = dynamic_cast<FuncCall *>(instr.get());
    ASSERT_NE(nullptr, funcCall);
    ASSERT_EQ(expectedRepr, funcCall->getRPN());
}

TEST(ParserTests, FuncCallSingleArgument) {
    const std::string input = "f(a)\n";
    const std::string expectedRepr = "f(a)";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    FuncCall *funcCall = dynamic_cast<FuncCall *>(instr.get());
    ASSERT_NE(nullptr, funcCall);
    ASSERT_EQ(expectedRepr, funcCall->getRPN());
}

TEST(ParserTests, FuncCallMultipleArguments) {
    const std::string input = "f(a, a, b)\n";
    const std::string expectedRepr = "f(a,a,b)";

    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<Instruction> instr = parser.parseInstruction();
    FuncCall *funcCall = dynamic_cast<FuncCall *>(instr.get());
    ASSERT_NE(nullptr, funcCall);
    ASSERT_EQ(expectedRepr, funcCall->getRPN());
}

TEST(ParserTests, FuncCallIncorrectArgumentListFormatThrows) {
    std::array inputs {
        "f(a,)\n",
        "f(a,,)\n",
        "f(a,, b)\n",
        "f(,a)\n",
        "f(a b)\n",
    };
    
    for (const auto &input : inputs) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        EXPECT_THROW({
                std::unique_ptr<Instruction> instr = parser.parseInstruction();
            },
            std::runtime_error
        ) << "not met for: " << input;
    }
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

TEST(ParserTests, SimpleUnits) {
    std::array units = {
        std::pair{ "[mm3]", "[(mm3)/()]" },
        std::pair{ "[mm2]", "[(mm2)/()]" },
        std::pair{ "[mm]" , "[(mm)/()]"  },
        std::pair{ "[m]"  , "[(m)/()]"   },
        std::pair{ "[kg]" , "[(kg)/()]"  },
        std::pair{ "[MN]" , "[(MN)/()]"  },
        std::pair{ "[J]"  , "[(J)/()]"   },
        std::pair{ "[kPa]", "[(kPa)/()]" }
    };

    for (const auto &[str, expectedStr] : units) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(str);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        codeobj::Unit unit = parser.parseUnit();
        EXPECT_EQ(expectedStr, unit.toString()) << "not met for: " << str;
    }
}

TEST(ParserTests, NumbersWithSimpleUnits) {
    std::array inputs = {
        std::pair{ "1 000.56 [mm3]", "1000.56[(mm3)/()]" },
        std::pair{ "0[mm2]"        , "0[(mm2)/()]"       },
        std::pair{ "1 [mm]"        , "1[(mm)/()]"        },
        std::pair{ "0.5[m]"        , "0.5[(m)/()]"       }
    };

    for (const auto &[str, expectedStr] : inputs) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(str);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        std::unique_ptr<Expression> expr = parser.parseExpression();
        EXPECT_EQ(expectedStr, expr->getRPN()) << "not met for: " << str;
    }
}

//TODO IdsCannotBeFollowedByUnitInExpression

TEST(ParserTests, ExpressionWithSimpleUnits) {
    std::string input = "1 + 1 [ u ] * 1 [ u ] ";
    std::string expectedRPN = "11[(m)/()]1[(m)/()]*+";
    MockLexer lexer(input);
    TestParser parser(lexer);
    parser.advance();
    std::unique_ptr<Expression> expr = parser.parseExpression();
    ASSERT_NE(nullptr, expr);
    EXPECT_EQ(expectedRPN, expr->getRPN());
}

TEST(ParserTests, UnitType) {
    std::array types = {
        std::tuple{ "[mm3]", "[(mm3)/()]", false },
        std::tuple{ "[mm2]", "[(mm2)/()]", false },
        std::tuple{ "[mm]" , "[(mm)/()]" , false },
        std::tuple{ "[m]"  , "[(m)/()]"  , false },
        std::tuple{ "[kg]" , "[(kg)/()]" , false },
        std::tuple{ "[MN]" , "[(MN)/()]" , false },
        std::tuple{ "[J]"  , "[(J)/()]"  , false },
        std::tuple{ "[kPa]", "[(kPa)/()]", false },
        std::tuple{ "[1]"  , "[1]"       , true  }
    };

    for (const auto &[str, expectedStr, isScalar] : types) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(str);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        std::unique_ptr<Type> type = parser.parseType();
        codeobj::Unit *unit = dynamic_cast<codeobj::Unit *>(type.get());
        ASSERT_NE(nullptr, unit);
        EXPECT_EQ(expectedStr, unit->toString()) << "not met for: " << str;
        EXPECT_EQ(isScalar, unit->isScalar()) << "not met for: " << str;
    }
}

TEST(ParserTests, BoolType) {
    std::unique_ptr<Source> src = std::make_unique<StringSource>("[bool]");
    Lexer lexer(*src);
    TestParserNotMock parser(lexer);
    parser.advance();
    std::unique_ptr<Type> type = parser.parseType();
    Bool *boolType = dynamic_cast<Bool *>(type.get());
    ASSERT_NE(nullptr, boolType);
    EXPECT_EQ("[bool]", boolType->toString());
}

TEST(ParserTests, ScalarTypeIsOnly1InSquares) {
    std::array inputs = {
        std::tuple{ "[1]"  , "[1]", true  },
        std::tuple{ "[0]"  , ""   , false },
        std::tuple{ "[1.0]", ""   , false }
    };

    for (const auto &[str, expectedStr, isCorrect] : inputs) {
        std::unique_ptr<Source> src = std::make_unique<StringSource>(str);
        Lexer lexer(*src);
        TestParserNotMock parser(lexer);
        parser.advance();
        std::unique_ptr<Type> type = nullptr;
        if (!isCorrect) {
            EXPECT_THROW({
                    type = parser.parseType();
                },
                std::runtime_error
            ) << "not met for: " << str;
        } else {
            type = parser.parseType();
            codeobj::Unit *unit = dynamic_cast<codeobj::Unit *>(type.get());
            ASSERT_NE(nullptr, unit) << "not met for: " << str;
            EXPECT_EQ(expectedStr, unit->toString()) << "not met for: " << str;
            EXPECT_EQ(true, unit->isScalar()) << "not met for: " << str;
        }
    }
}
