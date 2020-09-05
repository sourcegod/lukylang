/*
 * Object base class for luky interpreter
 * Date: jeudi, 01/08/19
 * */

#include "lukobject.hpp"
#include "token.hpp"
#include "lukcallable.hpp"
#include "lukfunction.hpp"
#include "lukinstance.hpp"

#include <iostream> // cout and cerr
#include <sstream> // ostringstream
#include <stdexcept> // exception

int LukObject::next_id =0;
ObjPtr LukObject::stat_nilPtr = LukObject::getNilPtr();

// constructors
    
LukObject::LukObject() 
    : id(++next_id) { 
    logMsg("\nLukObject constructor, id: ", id, ", val: Nil");
    m_type = LukType::Nil;  
    m_string = "nil";
    // p_string = nullptr;
    p_callable = nullptr;
    p_instance = nullptr;
    logMsg("\nExit out constructor nil");
}

LukObject::LukObject(bool val) 
    : id(++next_id) {
    logMsg("\nLukObject constructor bool,  id: ", id);
    m_type = LukType::Bool; m_bool = val; 
}

LukObject::LukObject(int val) 
    : id(++next_id) { 
    logMsg("\nLukObject constructor int,  id: ", id, "val: ", val);
      m_type = LukType::Double; m_double = val; 
}

LukObject::LukObject(double val) 
    : id(++next_id) {
    logMsg("\nLukObject constructor double,  id: ", id, "val: ", val);
  m_type = LukType::Double; 
  m_double = val; 
}

LukObject::LukObject(const std::string& val) 
    : id(++next_id) { 
    logMsg("\nLukObject constructor string,  id: ", id, "val: ", val);
    m_type = LukType::String; 
    m_string = val; 
    // p_string = std::make_shared<std::string>(val);
}

LukObject::LukObject(const char* val) 
    : id(++next_id) { 
    logMsg("\nLukObject constructor char*,  id: ", id, "val: ", val);
    m_type = LukType::String; 
    m_string = std::string(val); 
    // p_string = std::make_shared<std::string>(val);
}

LukObject::LukObject(std::shared_ptr<LukCallable> callable)
    : id(++next_id) { 
    logMsg("\nLukObject constructor callable,  id: ", id, "val: ", callable->toString());
    m_type = LukType::Callable;
    p_callable = callable; // std::make_shared<LukCallable>(callable);
    m_string = callable->toString();
    // p_string = std::make_shared<std::string>(callable->toString());
}

// std::shared_ptr<LukFunction> LukObject::getFunc() { return std::make_shared<LukFunction>(p_callable); }

LukObject::LukObject(std::shared_ptr<LukInstance> instance)
        : id(++next_id) { 
    logMsg("\nLukObject constructor instance,  id: ", id, "val: ", instance->toString());
    m_type = LukType::Instance;
    p_instance = instance;
    logMsg("\nIn LukObject instance toString: ", instance->toString());
    m_string = p_instance->toString();
        // p_string = std::make_shared<std::string>(p_instance->toString());
}

LukObject::LukObject(Token tok) 
        : id(++next_id) {
    logMsg("\nLukObject constructor token,  id: ", id, "val: ", tok.lexeme);
    switch(tok.type) {
        case TokenType::NIL:
            m_type = LukType::Nil; break;
        case TokenType::TRUE: 
            m_type = LukType::Bool;
            m_bool = true; 
            break;
        case TokenType::FALSE: 
            m_type = LukType::Bool;
            m_bool = false; 
            break;
        case TokenType::Double: 
            m_type = LukType::Double;
            m_double = std::stod(tok.literal); 
            break;
        case TokenType::STRING: 
            m_type = LukType::String;
            m_string = tok.literal;
            // p_string = std::make_shared<std::string>(tok.literal);
            break;
        default:
            std::runtime_error("Invalid Luky object.");
    }

}

LukObject::LukObject(TokPtr& tokP) 
        : id(++next_id) {
    logMsg("\nLukObject constructor token pointer,  id: ", id, "val: ", tokP->lexeme);
    // LukObject(*tokP.get());
    
    
    switch(tokP->type) {
        case TokenType::NIL:
            m_type = LukType::Nil; break;
        case TokenType::TRUE: 
            m_type = LukType::Bool;
            m_bool = true; 
            break;
        case TokenType::FALSE: 
            m_type = LukType::Bool;
            m_bool = false; 
            break;
        case TokenType::Double: 
            m_type = LukType::Double;
            m_double = std::stod(tokP->literal); 
            break;
        case TokenType::STRING: 
            m_type = LukType::String;
            m_string = tokP->literal;
            // p_string = std::make_shared<std::string>(tokP->literal);
            break;
        default:
            std::runtime_error("Invalid Luky object.");
    }
  
}


LukObject::LukObject(nullptr_t) {
  logMsg("\nIn LukObject constructor nullptr: ");
        m_type = LukType::Nil;
        m_string = "nil";
        // p_string = nullptr;
        p_callable = nullptr;
        p_instance = nullptr;
}


// copy constructor
LukObject::LukObject(const LukObject& obj) {
    // avoid copy of same object
    if (this != &obj) {
      swap(obj);
      // cannot use obj.getId function, cause obj is constant
      logMsg("\nIn LukObject, copy constructor, id: ", id, ", val: ", obj);
      
    }

}

// move constructor
LukObject::LukObject(const LukObject&& obj) {
    // avoid copy of same object
    if (this != &obj) {
      swap(obj);
      // cannot use obj.getId function, cause obj is constant
      logMsg("\nIn LukObject, move constructor, id: ", id, ", val: ", obj);
      
    }

}


// returns the current value to string
std::string LukObject::value() {
        switch(m_type) {
            case LukType::Nil: 
                return "nil";
            case LukType::Bool: 
                return m_bool ? "true" : "false";
            case LukType::Double: 
                return std::to_string(m_double);
            case LukType::String: 
                return m_string;
                // return *p_string;
            case LukType::Callable: 
            case LukType::Instance:  
              return m_string;
              // return *p_string;
        }

        return "";
    }

// convertions
bool LukObject::toBool() {
        if (m_type == LukType::Bool) return m_bool;
        m_bool = _toBool();
        m_type = LukType::Bool;
        return m_bool;
}

int LukObject::toInt() {
        if (m_type == LukType::Int) return m_int;
        m_int = _toInt();
        m_type = LukType::Int;
        return m_int;
}
 
double LukObject::toDouble() {
        if (m_type == LukType::Double) return m_double;
        m_double = _toDouble();
        m_type = LukType::Double;
        return m_double;
    }
    
std::string LukObject::toString() {
  // Warning: not change the m_type to string      
  if (m_type == LukType::String) return m_string;
    // return *p_string;
  // m_type = LukType::String;
  return _toString();
}

bool LukObject::_toBool() const {
    switch(m_type) {
        case LukType::Nil: return false;
        case LukType::Bool: return m_bool != 0;
        case LukType::Double: return m_double != 0;
        case LukType::String: return m_string != "";
        // return p_string != nullptr;
        // callables and classes are true by default
        case LukType::Callable:
        case LukType::Instance: 
            return true;
    }
    
    throw std::runtime_error("Invalid convertion to bool\n");

    return false;
}

int LukObject::_toInt() const {
    switch(m_type) {
        case LukType::Nil: return 0;
        case LukType::Bool: return m_bool ? 1 : 0;
        case LukType::Int: return m_int;
        case LukType::Double: return (int)m_double;
        case LukType::String: {
            int i;
            try {
                i = std::stoi(m_string);
            } catch (const std::invalid_argument &) {
                // not throw exception
                // std::cerr << "Argument is invalid\n";
                // throw 
                return 0;
            } catch(const std::out_of_range &) {
                // std::cerr << "Argument is out of range for a double\n";
                // throw;
                return 0;
            }
            
            return i;
        }
        break;

        case LukType::Callable:  
        case LukType::Instance:  
        break;
        
    }
    throw std::runtime_error("Cannot convert object to int.");

    return 0;
}


double LukObject::_toDouble() const {
    switch(m_type) {
        case LukType::Nil: return 0.;
        case LukType::Bool: return m_bool ? 1.0 : 0.0;
        case LukType::Double: return m_double;
        case LukType::String: {
            double d;
            try {
                d = std::stod(*p_string);
            } catch (const std::invalid_argument &) {
                // not throw exception
                // std::cerr << "Argument is invalid\n";
                // throw 
                return 0;
            } catch(const std::out_of_range &) {
                // std::cerr << "Argument is out of range for a double\n";
                // throw;
                return 0;
            }
            
            return d;
        }
        break;

        case LukType::Callable:  
        case LukType::Instance:  
        break;
        
    }
    throw std::runtime_error("Cannot convert object to number.");

    return 0;
}

std::string LukObject::_toString() const {
    switch(m_type) {
        case LukType::Nil: return "nil";
        case LukType::Bool: return (m_bool ? "true" : "false");
        case LukType::Double: return std::to_string(m_double);
        case LukType::String: return m_string;
          // return *p_string;
        case LukType::Callable: 
        case LukType::Instance: 
          return m_string;
          // return *p_string;
    }
    throw std::runtime_error("Cannot convert object to string.");

    return "";
}
// casting to the right type
void LukObject::cast(LukType tp) {
    if (m_type == tp) return;
    switch(tp) {
        case LukType::Nil: break;
        case LukType::Bool: m_bool = (bool)(*this); break;
        case LukType::Double: m_double = (double)(*this); break;
        case LukType::String: m_string = (std::string)(*this); break;
        case LukType::Callable: 
        case LukType::Instance: 
            break;
    
    }
    m_type = tp;

}

// assignment operators
std::shared_ptr<LukObject> LukObject::operator=(nullptr_t) {
  logMsg("\nIn LukObject nullptr assignment operator: ");
        m_type = LukType::Nil;
        m_string = "nil";
        // p_string = nullptr;
        p_callable = nullptr;
        p_instance = nullptr;
        return std::make_shared<LukObject>(*this);
}

std::shared_ptr<LukObject> LukObject::operator=(ObjPtr obj) {
  logMsg("\nIn LukObject shared pointer  assignment operator: ");
        m_type = obj->m_type;
        m_string = obj->m_string;
        p_callable = obj->p_callable;
        p_instance = obj->p_instance;
        return std::make_shared<LukObject>(*this);
}


LukObject& LukObject::operator=(const bool&& val) 
{
    id = ++next_id;
    m_type = LukType::Bool;
    m_bool = val;
    return *this;
}

LukObject& LukObject::operator=(const int&& val) 
{
    id = ++next_id;
    m_type = LukType::Double;
    m_double = val;
    return *this;
}

LukObject& LukObject::operator=(const double&& val) 
{
    id = ++next_id;
    m_type = LukType::Double;
    m_double = val;
    return *this;
}

// implement overloaded char* to avoid implicit casting to bool
LukObject& LukObject::operator=(const char* &&val) 
{
    id = ++next_id;
    m_type = LukType::String;
    m_string = std::string(val);
    return *this;
}


LukObject& LukObject::operator=(const std::string&& val) { 
    id = ++next_id;
    m_type = LukType::String;
    m_string = val;
    return *this;
}

// Note: swap function to avoid duplicate code between copy constructor and copy assignment operator
void LukObject::swap(const LukObject& obj) {
    id = ++next_id;
    m_type = obj.m_type;
    m_bool = obj.m_bool; 
    m_double = obj.m_double;
    m_string = obj.m_string;
    // p_string = obj.p_string;
    p_callable = obj.p_callable;
    // Note: dont forget to associate p_instance in operator =
    p_instance = obj.p_instance;
}

// copy assignment operator
LukObject& LukObject::operator=(const LukObject& obj) { 
    // avoid copy of same object
    if (this == &obj) return *this;
    swap(obj);
    logMsg("\nIn LukObject, Copy Assignment operator, id: ", id, ", val: ", obj);

    return *this;
}


// move assignment operator
LukObject& LukObject::operator=(const LukObject&& obj) { 
    // avoid copy of same object
    if (this == &obj) return *this;
    swap(obj);
    logMsg("Move Assignment operator, id: ", id, ", val: ", obj);

    return *this;
}

// compound assignment operators
// += operator
LukObject& LukObject::operator+=(const LukObject& obj) {
    std::ostringstream oss;
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw std::runtime_error("Cannot add nil");
            case LukType::Bool:
                throw std::runtime_error("Cannot add bools");
            case LukType::Double:
                m_double += obj.m_double; break;
            case LukType::String:
                m_string += obj.m_string; break;
                // p_string  += obj.p_string;
                oss << m_string << obj.m_string;
                // oss << *p_string << *obj.p_string;
                // p_string = std::make_shared<std::string>(oss.str());
                break;
            default:
                throw std::runtime_error("Cannot add objects.");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) += obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) += ob;
    }

}

// -= operator
LukObject& LukObject::operator-=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw std::runtime_error("Cannot substract nil");
            case LukType::Bool:
                throw std::runtime_error("Cannot substract bools");
            case LukType::Double:
                m_double -= obj.m_double; break;
            case LukType::String:
                throw std::runtime_error("Cannot substract strings.");
            default:
                throw std::runtime_error("Cannot substract objects.");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) -= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) -= ob;
    }

}


// *= operator
LukObject& LukObject::operator*=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw std::runtime_error("Cannot multiply nil");
            case LukType::Bool:
                throw std::runtime_error("Cannot multiply bools");
            case LukType::Double:
                m_double *= obj.m_double; break;
            case LukType::String:
                throw std::runtime_error("Cannot multiply strings.");
            default:
                throw std::runtime_error("Cannot multiply objects.");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) *= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) *= ob;
    }

}

// /= operator
LukObject& LukObject::operator/=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw std::runtime_error("Cannot divide nil");
            case LukType::Bool:
                throw std::runtime_error("Cannot divide bools");
            case LukType::Double:
                m_double /= obj.m_double; break;
            case LukType::String:
                throw std::runtime_error("Cannot divide strings.");
            default:
                throw std::runtime_error("Cannot divide objects.");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) /= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) /= ob;
    }

}

// unary operators
// unary minus operator
LukObject operator-(LukObject a) {
    switch(a.m_type) {
        case LukType::Double: a.m_double = -a.m_double; break;
        default: 
            throw std::runtime_error("Unary minus cannot apply for this type.");
    }

    return a;
}
   
// unary not operator
LukObject operator!(LukObject a) {
    switch(a.m_type) {
        case LukType::Nil: a.m_bool = true; break;
        case LukType::Bool:  a.m_bool = !a.m_bool; break;
        case LukType::Double: 
            a.m_bool = a.m_double == 0.; break;
        case LukType::String:
            a.m_bool = a.m_string == ""; break;
        default:
            throw std::runtime_error("cannot negate object.");
    }

    a.m_type = LukType::Bool;
    return a;
}

// equality operators
// equality == operator
// /*
bool operator==(const LukObject& a, const LukObject& b) {
    if (a.m_type == b.m_type) {
        switch(a.m_type) {
            case LukType::Nil: return true;
            case LukType::Bool: return a.m_bool == b.m_bool;
            case LukType::Double: return a.m_double == b.m_double;
            case LukType::String: return a.m_string == b.m_string;
            default: 
                throw std::runtime_error("Cannot compare objects for equality.");
        }
    }

    if (a.m_type > b.m_type) return b == a;
    if (a.m_type == LukType::Nil || b.m_type == LukType::Nil) return false;
    switch(a.m_type) {
        case LukType::Bool: return a.m_bool == (bool)b;
        case LukType::Double: return a.m_double == (double)b;
        case LukType::String: return a.m_string == (std::string)b;
        default:
            throw std::runtime_error("Cannot compare objects for equality.");
    }
    
    return false;
}
// */

// comparison operators for luktype
bool operator<(LukType a, LukType b) {
    return static_cast<int>(a) < static_cast<int>(b);
}

bool operator>(LukType a, LukType b) {
    return static_cast<int>(a) > static_cast<int>(b);
}


bool operator<(const LukObject& a, const LukObject& b) {
    if (a.m_type == b.m_type) {
        switch(a.m_type) {
            case LukType::Nil:
            case LukType::Bool:
                throw std::runtime_error("Nil and Bool cannot odered.");
            case LukType::Double: return a.m_double < b.m_double;
            case LukType::String: return a.m_string < b.m_string;
            default: 
                                  throw std::runtime_error("Objects are cannot ordered.");
        }
    }

    throw std::runtime_error("Only objects of the same type can be ordered.");
}

/* 
 * Note: other operators like binary operators, are implemented inline in .hpp file
 * */

