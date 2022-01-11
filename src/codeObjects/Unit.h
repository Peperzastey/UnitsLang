#ifndef TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED

#include "Type.h"
#include "error/ErrorHandler.h"
#include "lexer/Lexer.h"
#include "utils/printUtils.h"
#include <map>
#include <sstream>
#include <ostream>

namespace codeobj {
class Unit;
}

std::ostream& operator<<(std::ostream &os, const codeobj::Unit &unit);

namespace codeobj {

class Unit : public Type {
public:
    Unit() : isScalar_(true) {}

    explicit Unit(::Unit unitToken) : isScalar_(false) {
        numerator_.insert({ unitToken.unit, unitToken });
    }
    
    Unit(Unit &&) = default;

    /*void addUnitComponent(Token op, Unit &&unit) {
        if (std::get<std::string>(op.value) == "*") {
            //if (auto it = numerator_.find())
        } else if (std::get<std::string>(op.value) == "/") {
            
        } else {
            ErrorHandler::handleFromParser("Multiplicative operator different than '*' and '/'");
        }
    }*/
    
    bool isScalar() const noexcept {
        return isScalar_;
    }
    
    std::string toString() const override {
        std::ostringstream os;
        os << *this;
        return os.str();
    }

private:
    std::map<UnitType, ::Unit> numerator_;
    std::map<UnitType, ::Unit> denominator_;
    bool isScalar_;
    
    friend std::ostream& ::operator<<(std::ostream &os, const Unit &unit);
};

/*inline std::ostream& operator<<(std::ostream &os, const codeobj::Unit &unit) {
    if (unit.isScalar()) {
        return os;
    }
    os << "[(";
    for (auto &&[t, u] : unit.numerator_) {
        (void)t;
        os << u << '*';
    }
    os << ") / (";
    for (auto &&[t, u] : unit.denominator_) {
        (void)t;
        os << u << '*';
    }
    os << ")]";
    return os;
}*/

} // namespace codeobj

inline std::ostream& operator<<(std::ostream &os, const codeobj::Unit &unit) {
    if (unit.isScalar()) {
        return os << "[1]";
    }
    os << "[(";
    bool first = true;
    for (auto &&[t, u] : unit.numerator_) {
        (void)t;
        if (!first) {
            os << '*';
        }
        os << u;
        first = false;
    }
    os << ")/(";
    first = true;
    for (auto &&[t, u] : unit.denominator_) {
        (void)t;
        if (!first) {
            os << '*';
        }
        os << u;
        first = false;
    }
    os << ")]";
    return os;
}

#endif // TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED
