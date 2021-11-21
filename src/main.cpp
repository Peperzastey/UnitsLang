#include "lexer/Lexer.h"
#include "source/Source.h"
#include "source/FileSource.h"
#include "utils/printUtils.h"
#include <iostream>
#include <memory>

const char FILENAME[] = "/home/przemek/studia/tkom-SIunits/input";

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    std::unique_ptr<Source> src = std::make_unique<FileSource>(FILENAME);

    /*char c;
    while ((c = src->getChar()) != EOF) {
        std::cout << c;
    }
    std::cout << "\n\n";*/

    Lexer lexer(*src);
    while (true) {
        Token token = lexer.getToken();
        std::cout << token << '\n';

        if (token.type == TokenType::END_OF_STREAM) {
            break;
        }
    }
}