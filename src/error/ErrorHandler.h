#ifndef TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED
#define TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED

#include <stdexcept>
#include <sstream>

class ErrorHandler {
public:
    static void handleFromParser(const std::string &msg /*, Token*/) {
        std::ostringstream os;
        os << "Parser error: " << msg;
        throw std::runtime_error(os.str());
    }
    
    static void handleFormLexer(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Lexer error: " << msg;
        throw std::runtime_error(os.str());
    }
};

#endif // TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED
