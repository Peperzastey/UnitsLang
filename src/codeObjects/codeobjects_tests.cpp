#include "Program.h"
#include "FuncDef.h"
#include "Unit.h"
#include <memory>
#include <gtest/gtest.h>

TEST(CodeObjectsTests, FuncDefWithDuplicateParamNamesThrows) {
    std::vector<Variable> params;
    params.emplace_back("a", std::make_unique<codeobj::Unit>());
    params.emplace_back("b", std::make_unique<codeobj::Unit>());
    params.emplace_back("a", std::make_unique<codeobj::Unit>());

    EXPECT_THROW({
            FuncDef funcDef("f", std::move(params), nullptr, {});
        },
        std::runtime_error
    );
}

TEST(CodeObjectsTests, ProgramWithDuplicateFunctionNamesThrows) {
    std::vector<Variable> emptyParams{};
    std::vector<std::unique_ptr<Instruction>> emptyBody{};

    std::vector<std::unique_ptr<FuncDef>> functions;
    functions.push_back(std::make_unique<FuncDef>("f", std::move(emptyParams), nullptr, std::move(emptyBody)));
    functions.push_back(std::make_unique<FuncDef>("g", std::move(emptyParams), nullptr, std::move(emptyBody)));
    functions.push_back(std::make_unique<FuncDef>("f", std::move(emptyParams), nullptr, std::move(emptyBody)));

    EXPECT_THROW({
            Program program(std::move(functions), {});
        },
        std::runtime_error
    );
}
