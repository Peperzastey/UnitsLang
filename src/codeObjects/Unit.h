#ifndef TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED

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

class Unit {
public:
    Unit() : isScalar_(true) {}

    explicit Unit(::Unit unitToken) : isScalar_(false) {
        numerator_.insert({ unitToken.unit, unitToken });
    }

    void combineWithUnit(Token op, const Unit &unit);

    void multWithUnit(const Unit &unit) {
        combineWithUnit(Token{TokenType::OP_MULT, "*"}, unit);
    }
    void divWithUnit(const Unit &unit) {
        combineWithUnit(Token{TokenType::OP_MULT, "/"}, unit);
    }
    
    bool isScalar() const noexcept {
        return isScalar_;
    }
    
    std::string toString() const {
        std::ostringstream os;
        os << *this;
        return os.str();
    }
    
    bool isAddCompatibileWith(const Unit &other) const;

private:
    void updateIsScalar();

    void reduceFraction();

private:
    // map, not unordered_map, beceuse the elements have to be ordered
    std::map<UnitType, ::Unit> numerator_;
    std::map<UnitType, ::Unit> denominator_;
    bool isScalar_;
    
    friend std::ostream& ::operator<<(std::ostream &os, const Unit &unit);
};

} // namespace codeobj

#endif // TKOMSIUNITS_CODE_OBJECTS_UNIT_H_INCLUDED
