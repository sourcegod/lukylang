#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <map>
#include "token.hpp"
#include "lukobject.hpp"

using TObject = LukObject;
class Environment {
public:
    TObject& get(Token name);
    void assign(Token name, TObject value);
    void define(std::string name, TObject value);

private:
    std::map<std::string, TObject> values;

};

#endif // ENVIRONMENT_HPP
