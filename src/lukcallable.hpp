#ifndef LUKCALLABLE_HPP
#define LUKCALLABLE_HPP

// #include "lukobject.hpp"
// #include "stmt.hpp"
// #include "interpreter.hpp"
#include <string>
#include <vector>

class LukObject;
class Interpreter;
using VArguments = std::vector<LukObject>;

class LukCallable {
public:
    LukCallable() {}
    virtual ~LukCallable() {}
    virtual size_t arity() const = 0;
    virtual LukObject call(Interpreter&, VArguments args)  const = 0;
    virtual std::string toString() const = 0;
};


#endif // LUKCALLABLE_HPP
