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

    while (true) {
        Token token = lexer.getToken();
        std::cout << token << '\n';

        if (token.type == TokenType::END_OF_STREAM) {
            break;
        }
    }
    return 0;
}