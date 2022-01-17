#ifndef TKOMSIUNITS_LEXER_ERROR_H_INCLUDED
#define TKOMSIUNITS_LEXER_ERROR_H_INCLUDED

#include <stdexcept>

class LexerError : public std::runtime_error {
public:
    LexerError(const std::string &errorMsg, std::size_t line, std::size_t column)
        : std::runtime_error(
                "Lexer error: " + errorMsg +
                " in line " + std::to_string(line) +
                " column " + std::to_string(column)
            )
        , line_(line), column_(column) {}

    LexerError(std::size_t line, std::size_t column)
        : std::runtime_error(
                std::string("Lexer error: <generic>") +
                " in line " + std::to_string(line) +
                " column " + std::to_string(column)
            )
        , line_(line), column_(column) {}

    std::size_t getLine() const noexcept {
        return line_;
    }
    
    std::size_t getColumn() const noexcept {
        return column_;
    }

private:
    const std::size_t line_;
    const std::size_t column_;
};

#endif // TKOMSIUNITS_LEXER_ERROR_H_INCLUDED
