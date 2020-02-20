#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <unordered_map>
#include "token.hpp"
#include "lukobject.hpp"
#include <memory> // smart pointers

class LukObject;
class Environment;
using TObject = LukObject;
using PEnvironment = std::shared_ptr<Environment>;

class Environment {
public:
    std::string m_name;
    
    Environment() { 
        m_enclosing = nullptr; 
        m_name = addressOf();
        std::cerr << "Env: ctor, " << m_name << "\n"; 
    }
    
    explicit Environment(PEnvironment& encl)
        : m_enclosing(encl) { 
            m_name = addressOf();
            std::cerr << "Env: copy ctor: " << m_name << "\n"; 
        }

     // get the address of object
    const std::string addressOf() { 
        std::ostringstream oss;
        oss << (void const *)this;
        return oss.str(); 
    }

    TObject& get(Token name);
    void assign(Token name, TObject value);
    void define(const std::string& name, TObject value);
    size_t size() {  return values.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<TObject>> values = {};
    PEnvironment m_enclosing;

};

#endif // ENVIRONMENT_HPP
