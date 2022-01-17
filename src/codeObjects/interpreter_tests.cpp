#include "Program.h"
#include "Interpreter.h"
#include "Value.h"
#include "BinaryExpression.h"
#include "source/StringSource.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include <memory>
#include <gtest/gtest.h>
#include <iostream>

/*TEST(InterpreterTests, ExpressionSingleValueCalculate) {
    Unit unit{ "m", UnitType::METER, 2 };
    std::unique_ptr<Expression> expr = std::make_unique<Value>(5.0, Type2(codeobj::Unit(unit)));
    Value result = expr->calculate(interpreter);
    ASSERT_EQ(Type2::NUMBER, result.type.getTypeClass());
    EXPECT_EQ("5[(mm2)/()]", result.toString());
    EXPECT_EQ(5.0, result.asDouble());
}

TEST(InterpreterTests, SimpleExpressionCalculate) {
    Unit unit{ "", UnitType::METER, 1 };
    auto val1 = std::make_unique<Value>(3.5, Type2(codeobj::Unit(unit)));
    auto val2 = std::make_unique<Value>(5.0, Type2(codeobj::Unit(unit)));
    std::unique_ptr<Expression> expr = std::make_unique<BinaryExpression>(
            std::move(val1), Token{TokenType::OP_ADD, "-"}, std::move(val2)
        );
    Value result = expr->calculate(interpreter);
    ASSERT_EQ(Type2::NUMBER, result.type.getTypeClass());
    EXPECT_EQ("-1.5[(m)/()]", result.toString());
    EXPECT_EQ(-1.5, result.asDouble());
}*/

TEST(InterpreterTests, VarDefinition) {
    Unit resultUnit{ "", UnitType::METER, 1 };
    std::string varName = "a";
    std::string input = varName + " = 1[m] + 5[m] - 3[m]\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(3.0, codeobj::Unit(resultUnit)), interp.getVariable(varName));*/
}

TEST(InterpreterTests, VarDefinitionWithTypeDecl) {
    Unit resultUnit{ "", UnitType::SECOND, 2 };
    std::string varName = "a";
    std::string input = varName + "[s2] = 1[s2] + 5[s2] - 3[s2]\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(3.0, codeobj::Unit(resultUnit)), interp.getVariable(varName));*/
}

TEST(InterpreterTests, VarAssignment) {
    Unit resultUnit{ "", UnitType::METER, 1 };
    std::string varName = "a";
    std::string input = varName + " = 1[m] + 5[m] - 3[m]\n" +
                        varName + " = 45[m]\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(45.0, codeobj::Unit(resultUnit)), interp.getVariable(varName));*/
}

TEST(InterpreterTests, IfTrue) {
    Unit resultUnit{ "", UnitType::METER, 1 };
    std::string input = "if true { a = 5[m]\n } else { a = 0\n }\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(5, codeobj::Unit(resultUnit)), interp.getVariable("a"));*/
}

TEST(InterpreterTests, IfFalseElse) {
    std::string input = "if false { a = 5[m]\n } else { a = 0\n }\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(0, codeobj::Unit()), interp.getVariable("a"));*/
}

TEST(InterpreterTests, IfElifChainExecutesFirstTrueElif) {
    std::string input = "if false { a = 5[m]\n } elif false { a = 0\n } elif true { a = 3\n } elif true { a = 2\n }\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(3, codeobj::Unit()), interp.getVariable("a"));*/
}

TEST(InterpreterTests, IfFalseElifFalseChainExecutesElse) {
    std::string input = "if false { a = 5[m]\n } elif false { a = 0\n } elif false { a = 3\n } else { b = 2\n }\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(2, codeobj::Unit()), interp.getVariable("b"));
    EXPECT_EQ(std::nullopt, interp.getVariable("a"));*/
}

TEST(InterpreterTests, VarReference) {
    std::string input = "a = true\n if a { a = false\n } \n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(false), interp.getVariable("a"));*/
}

TEST(InterpreterTests, While) {
    std::string input = "a = 3\n b = 0\n"
                        "while a > 0 {"
                        "b = b + 1\n a = a - 1\n"
                        "}\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(0, codeobj::Unit()), interp.getVariable("a"));
    EXPECT_EQ(Value(3, codeobj::Unit()), interp.getVariable("b"));*/
}

TEST(InterpreterTests, WhileBreak) {
    std::string input = "a = 3\n b = 0\n"
                        "while a > 0 {"
                        "b = b + 1\n a = a - 1\n"
                        "if a < 2 { break\n }\n"
                        "}\n"
                        "c = 7\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(1, codeobj::Unit()), interp.getVariable("a"));
    EXPECT_EQ(Value(2, codeobj::Unit()), interp.getVariable("b"));
    EXPECT_EQ(Value(7, codeobj::Unit()), interp.getVariable("c"));*/
}

TEST(InterpreterTests, WhileContinue) {
    std::string input = "a = 3\n b = 0\n"
                        "while a > 0 {"
                        "a = a - 1\n"
                        "if a > 0 { continue\n }\n"
                        "b = b + 1\n"
                        "}\n"
                        "c = 7\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    /*EXPECT_EQ(Value(0, codeobj::Unit()), interp.getVariable("a"));
    EXPECT_EQ(Value(1, codeobj::Unit()), interp.getVariable("b"));
    EXPECT_EQ(Value(7, codeobj::Unit()), interp.getVariable("c"));*/
}

TEST(InterpreterTests, WhileReturn) {
    std::string input = "a = 3\n b = 0\n"
                        "while a > 0 {"
                        "b = b + 1\n a = a - 1\n"
                        "if a < 2 { return b\n }\n"
                        "}\n"
                        "c = 7\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout, *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(2, result);
    /*EXPECT_EQ(Value(1, codeobj::Unit()), interp.getVariable("a"));
    EXPECT_EQ(Value(2, codeobj::Unit()), interp.getVariable("b"));
    EXPECT_EQ(std::nullopt, interp.getVariable("c"));*/
}

TEST(InterpreterTests, FunctionRecursionFibonacci) {
    std::string input =
        "func fibonacci (steps [1]) -> [m] {"
        "    if steps == 0 {"
        "        return 0[m]\n"
        "    } elif steps == 1 {"
        "        return 1[m]\n"
        "    } else {"
        "        return fibonacci(steps - 1) + fibonacci(steps - 2)\n"
        "    }\n"
        "}\n"
        "if fibonacci(20) == 6 765[m] {"
        "    return 20\n"
        "}\n"
        "return 1\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(20, result);
}

TEST(InterpreterTests, FunctionFibonacciIterative) {
    std::string input =
        "func fibonacci_iterative (steps [1]) -> [1] {"
        "    elem0 = 0\n"
        "    elem1 = 1\n"
        "    count = 1\n"
        "    while count < steps {"
        "        elem1 = elem1 + elem0\n"
        "        elem0 = elem1 - elem0\n"
        "        count = count + 1\n"
        "    }\n"
        "    return elem1\n"
        "}\n"
        "if fibonacci_iterative(20) == 6765 {"
        "    return 20\n"
        "}\n"
        "return 1\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(20, result);
}

TEST(InterpreterTests, Print) {
    std::string input =
        "print(\"Ala ma kota\")\n";
    std::string expectedOutput = "Ala ma kota\n";
    std::stringstream testStdout;
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(testStdout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    EXPECT_EQ(expectedOutput, testStdout.str());
}

TEST(InterpreterTests, PrintStringVariable) {
    std::string input =
        "a[str] = \"Kot ma Ale\"\n"
        "print(a)\n";
    std::string expectedOutput = "Kot ma Ale\n";
    std::stringstream testStdout;
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(testStdout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    EXPECT_EQ(expectedOutput, testStdout.str());
}

TEST(InterpreterTests, FormattedString) {
    std::string input =
        "a = 15.3[m/s2]\n"
        "print(\"a = {a}\")\n";
    std::string expectedOutput = "a = 15.3[(m)/(s2)]\n";
    std::stringstream testStdout;
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(testStdout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
    EXPECT_EQ(expectedOutput, testStdout.str());
}

TEST(InterpreterTests, ComplexUnitEquality) {
    std::string input =
        "a = 5[kg/(m*s2)]\n"
        "b = 5[kg/m/s/s]\n"
        "if a != b {"
        "    return 2\n"
        "}\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
}

TEST(InterpreterTests, DividingUnitsCreatesNewUnit) {
    std::string input =
        "a = 5[m]\n"
        "b = 2[s]\n"
        "if a/b != 2.5[m/s] {"
        "    return 2\n"
        "}\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
}

TEST(InterpreterTests, MultiplicatingUnitsCreatesNewUnit) {
    std::string input =
        "a = 5[m]\n"
        "b = 2[kg/s]\n"
        "if a*b != 10[m*kg/s] {"
        "    return 2\n"
        "}\n";
    std::unique_ptr<Source> src = std::make_unique<StringSource>(input);
    Lexer lexer(*src);
    Parser parser(lexer);
    std::unique_ptr<Program> program = parser.parse();
    Interpreter interp(std::cout,  *program.get());
    int result = interp.executeProgram();
    EXPECT_EQ(0, result);
}
