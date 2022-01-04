#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include "source/Source.h"
#include "source/FileSource.h"
#include "utils/printUtils.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path-to-input-file-to-analyze-lexically>"
            << std::endl;
        return 1;
    }

    std::unique_ptr<Source> src = std::make_unique<FileSource>(argv[1]);
    Lexer lexer(*src);
    Parser parser(lexer);

    try {
        std::unique_ptr<Program> program = parser.parse();
        program->printFunctions();
        program->printInstructionsTypes();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}