#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <unordered_map>
#include "token.hpp"
#include "lukobject.hpp"
#include <memory> // shared_ptr

class Environment;
using TObject = LukObject;
using PEnvironment = std::shared_ptr<Environment>;

class Environment {
public:
    Environment() { m_enclosing = nullptr; };
    Environment(PEnvironment encl)
        : m_enclosing(encl) {}
    
    TObject& get(Token name);
    void assign(Token name, TObject value);
    void define(std::string name, TObject value);

private:
    std::unordered_map<std::string, TObject> values;
    PEnvironment m_enclosing;

};

#endif // ENVIRONMENT_HPP
