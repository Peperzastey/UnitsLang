#ifndef TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED
#define TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED

#include <stdexcept>
#include <sstream>

class ErrorHandler {
public:
    [[noreturn]] static void handleFromParser(const std::string &msg /*, Token*/) {
        std::ostringstream os;
        os << "Parser error: " << msg;
        throw std::runtime_error(os.str());
    }

    [[noreturn]] static void handleFromLexer(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Lexer error: " << msg;
        throw std::runtime_error(os.str());
    }

    [[noreturn]] static void handleFromCodeObject(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "CodeObj error: " << msg;
        throw std::runtime_error(os.str());
    }
    
    [[noreturn]] static void handleFromInterpreter(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Interpreter error: " << msg;
        throw std::runtime_error(os.str());
    }

    [[noreturn]] static void handleTypeMismatch(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Type Mismatch error: " << msg;
        throw std::runtime_error(os.str());
    }

    [[noreturn]] static void handleVariableAlreadyDefined(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Variable Already Defined error: " << msg;
        throw std::runtime_error(os.str());
    }

    [[noreturn]] static void handleVariableNotDefined(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Variable Not Defined error: " << msg;
        throw std::runtime_error(os.str());
    }
    
    [[noreturn]] static void handleFunctionNotDefined(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Function Not Defined error: " << msg;
        throw std::runtime_error(os.str());
    }
    
    [[noreturn]] static void handleFunctionCallError(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Function Call error: " << msg;
        throw std::runtime_error(os.str());
    }
    
    [[noreturn]] static void handleJumpInstrOutsideWhile(const std::string &msg /*, PosInText*/) {
        std::ostringstream os;
        os << "Jump Instr Outside While error: " << msg;
        throw std::runtime_error(os.str());
    }
};

#endif // TKOMSIUNITS_ERROR_HANDLER_H_INCLUDED
