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
    int m_id;
    std::string m_name;
    
    PEnvironment m_enclosing;
    Environment() 
    : m_id(++next_id) { 
        m_enclosing = nullptr;
        setName();
        // DEBUG_MSG("Ceci est un debug message.");
        // std::cerr << "Env: ctor, " << m_name << "\n"; 
        logMsg("Env: ctor, ", m_name);
    }
    
    explicit Environment(PEnvironment encl)
        : m_id(++next_id), m_enclosing(encl) {
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
        return m_name = "id: " + std::to_string(m_id) + ", (" + addressOf() + ")";
    }

    size_t size() {  return m_values.size(); }
    auto& getValues() { return m_values; }

    ObjPtr& get(Token name);
    
    // TODO: must be deleted after factorization
    void assign(Token name, TObject val);
    void assign(Token name, ObjPtr& val);
    void assign(Token name, std::shared_ptr<LukCallable> callable);

    void define(const std::string& name, TObject val);
    void define(const std::string& name, ObjPtr& val);
    ObjPtr getAt(int distance, const std::string& name);
    Environment* ancestor(int distance);
    void assignAt(int distance, Token& name, ObjPtr& val);

private:
    std::unordered_map<std::string, ObjPtr> m_values = {};

};

#endif // ENVIRONMENT_HPP
