#ifndef TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED
#define TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED

#include "Expression.h"
#include "Value.h"
#include <memory>
#include <vector>

namespace codeobj {

class String : public Expression {
public:
    String(std::vector<std::unique_ptr<Expression>> &&parts)
        : parts_(std::move(parts)) {}
    
    Value calculate([[maybe_unused]] Interpreter &interpreter) override {
        std::string value;
        for (auto &&part : parts_) {
            value += part->calculate(interpreter).toString();
        }
        return Value(std::move(value));
    }
    
    std::string getRPN() const override {
        return "<str>";
    }

private:
    std::vector<std::unique_ptr<Expression>> parts_;
};

} // namespace codeobj

#endif // TKOMSIUNITS_CODE_OBJECTS_STRING_H_INCLUDED
