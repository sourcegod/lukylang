#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "common.hpp"
#include "token.hpp"
#include "lukobject.hpp"
#include "logger.hpp"

#include <string>
#include <unordered_map>
#include <memory> // smart pointers

class LukObject;
class Environment;
class CTracer;

// using EnvPtr = std::shared_ptr<Environment>;

class Environment {
protected:
    static int next_id;
public:
    int m_id;
    std::string m_name;
    
    EnvPtr m_enclosing;
    Environment() 
    : m_id(++next_id) { 
        m_enclosing = nullptr;
        setName();
        // DEBUG_MSG("Ceci est un debug message.");
        // std::cerr << "Env: ctor, " << m_name << "\n"; 
        logMsg("\nIn Environment constructor, name: ", m_name);
    }
    
    explicit Environment(EnvPtr encl)
        : m_id(++next_id), m_enclosing(encl) {
            setName();
            // std::cerr << "Env: copy ctor: " << m_name << "\n"; 
            logMsg("\nIn Environment copy constructor, name: ", m_name);
            // DEBUG_PRINT("Env: copy ctor: %s", m_name.c_str());
    }
    
    ~Environment() {
      logMsg("\n~Environment destructor, name: ", m_name, ", size: ", size());
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

    ObjPtr& get(TokPtr& name);
    
    void assign(TokPtr& name, ObjPtr& val);
    void assign(TokPtr& name, std::shared_ptr<LukCallable> callable);

    void define(const std::string& name, ObjPtr val);
    ObjPtr getAt(int distance, const std::string& name);
    Environment* ancestor(int distance);
    void assignAt(int distance, TokPtr& name, ObjPtr& val);

private:
    std::unordered_map<std::string, ObjPtr> m_values = {};

};

#endif // ENVIRONMENT_HPP
