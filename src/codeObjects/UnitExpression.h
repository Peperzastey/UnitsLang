#ifndef TKOMSIUNITS_CODE_OBJECTS_UNIT_EXPRESSION_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_UNIT_EXPRESSION_H_INCLUDED

namespace codeobj {

class UnitExpression {
public:
    UnitExpression() {}
    virtual ~UnitExpression() {}
    
    virtual std::string getRPN() const = 0;
};

}

#endif // TKOMSIUNITS_CODE_OBJECTS_UNIT_EXPRESSION_H_INCLUDED
