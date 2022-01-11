#include "Program.h"
#include "FuncDef.h"
#include "Unit.h"
#include <memory>
#include <gtest/gtest.h>

TEST(CodeObjectsTests, FuncDefWithDuplicateParamNamesThrows) {
    std::vector<Variable> params;
    params.emplace_back("a", Type2(codeobj::Unit()));
    params.emplace_back("b", Type2(codeobj::Unit()));
    params.emplace_back("a", Type2(codeobj::Unit()));

    EXPECT_THROW({
            FuncDef funcDef("f", std::move(params), Type2(), {});
        },
        std::runtime_error
    );
}

TEST(CodeObjectsTests, ProgramWithDuplicateFunctionNamesThrows) {
    std::vector<Variable> emptyParams{};
    std::vector<std::unique_ptr<Instruction>> emptyBody{};

    std::vector<std::unique_ptr<FuncDef>> functions;
    functions.push_back(std::make_unique<FuncDef>("f", std::move(emptyParams), Type2(), std::move(emptyBody)));
    functions.push_back(std::make_unique<FuncDef>("g", std::move(emptyParams), Type2(), std::move(emptyBody)));
    functions.push_back(std::make_unique<FuncDef>("f", std::move(emptyParams), Type2(), std::move(emptyBody)));

    EXPECT_THROW({
            Program program(std::move(functions), {});
        },
        std::runtime_error
    );
}
