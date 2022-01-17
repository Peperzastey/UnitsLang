#include "Program.h"
#include "FuncDef.h"
#include "Unit.h"
#include <memory>
#include <gtest/gtest.h>

TEST(CodeObjectsTests, FuncDefWithDuplicateParamNamesThrows) {
    std::vector<std::unique_ptr<Instruction>> emptyBody{};

    std::vector<Variable> params;
    params.emplace_back("a", Type(codeobj::Unit()));
    params.emplace_back("b", Type(codeobj::Unit()));
    params.emplace_back("a", Type(codeobj::Unit()));

    EXPECT_THROW({
            FuncDef funcDef("f", std::move(params), Type(), std::make_unique<InstructionBlock>(std::move(emptyBody)));
        },
        std::runtime_error
    );
}

TEST(CodeObjectsTests, ProgramWithDuplicateFunctionNamesThrows) {
    std::vector<std::unique_ptr<Instruction>> emptyBody{};

    std::vector<std::unique_ptr<FuncDef>> functions;
    functions.push_back(std::make_unique<FuncDef>("f", std::vector<Variable>(), Type(), std::make_unique<InstructionBlock>(std::move(emptyBody))));
    functions.push_back(std::make_unique<FuncDef>("g", std::vector<Variable>(), Type(), std::make_unique<InstructionBlock>(std::move(emptyBody))));
    functions.push_back(std::make_unique<FuncDef>("f", std::vector<Variable>(), Type(), std::make_unique<InstructionBlock>(std::move(emptyBody))));

    EXPECT_THROW({
            Program program(std::move(functions), {});
        },
        std::runtime_error
    );
}
