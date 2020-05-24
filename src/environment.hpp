#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>
#include <unordered_map>
#include "token.hpp"
#include "lukobject.hpp"
#include "logger.hpp"

#include <memory> // smart pointers

class LukObject;
class Environment;
class CTracer;
using TObject = LukObject;
using PEnvironment = std::shared_ptr<Environment>;

class Environment {
protected:
    static int next_id;
public:
    int id;
    std::string m_name;
    
    Environment() 
    : id(++next_id) { 
        m_enclosing = nullptr;
        setName();
        // DEBUG_MSG("Ceci est un debug message.");
        // std::cerr << "Env: ctor, " << m_name << "\n"; 
        logMsg("Env: ctor, ", m_name);
    }
    
    explicit Environment(PEnvironment encl)
        : id(++next_id), m_enclosing(encl) {
            setName();
            // std::cerr << "Env: copy ctor: " << m_name << "\n"; 
            logMsg("Env: copy ctor", m_name);
            // DEBUG_PRINT("Env: copy ctor: %s", m_name.c_str());
     }

     // get the address of object
    const std::string addressOf() { 
        std::ostringstream oss;
        oss << (void const *)this;
        return oss.str(); 
    }

    // construct name
    const std::string setName() { 
        return m_name = "id: " + std::to_string(id) + ", (" + addressOf() + ")";
    }

    TObject& get(Token name);
    void assign(Token name, TObject val);

    void define(const std::string& name, TObject val);
    void define(const std::string& name, std::shared_ptr<TObject> val);

    size_t size() {  return values.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<TObject>> values = {};
    PEnvironment m_enclosing;

};

#endif // ENVIRONMENT_HPP
