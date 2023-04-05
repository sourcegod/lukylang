/*
 * Object base class for luky interpreter
 * Date: jeudi, 01/08/19
 * */

#include "lukobject.hpp"
#include "lukcallable.hpp"
#include "lukfunction.hpp"
#include "lukinstance.hpp"
#include "runtimeerror.hpp"
#include <iostream> // cout and cerr
#include <sstream> // ostringstream
#include <stdexcept> // exception
#include <cmath> // for fmod

using namespace luky;

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
        : id(++next_id), m_bool(val) {
    logMsg("\nLukObject constructor bool,  id: ", id);
    m_type = LukType::Bool; 
}

LukObject::LukObject(TLukInt val) 
        : id(++next_id), m_int(val) {
    logMsg("\nLukObject constructor int,  id: ", id, "val: ", val);
    m_type = LukType::Int;
}

LukObject::LukObject(double val) 
        : id(++next_id), m_double(val) {
    logMsg("\nLukObject constructor double,  id: ", id, "val: ", val);
    m_type = LukType::Double; 
}

LukObject::LukObject(const std::string& val) 
        : id(++next_id), m_string(val) {
          if (m_string == "") m_string = "''";
    logMsg("\nLukObject constructor string,  id: ", id, "val: ", val);
    m_type = LukType::String; 
}

LukObject::LukObject(const char* val) 
        : id(++next_id), m_string(std::string(val)) {
    logMsg("\nLukObject constructor char*,  id: ", id, "val: ", val);
    m_type = LukType::String; 
}

LukObject::LukObject(std::shared_ptr<LukCallable> callable)
        : id(++next_id), p_callable(callable) {
    logMsg("\nLukObject constructor callable,  id: ", id, "val: ", callable->toString());
    m_type = LukType::Callable;
    m_string = callable->toString();
}

// std::shared_ptr<LukFunction> LukObject::getFunc() { return std::make_shared<LukFunction>(p_callable); }

LukObject::LukObject(std::shared_ptr<LukInstance> instance)
        : id(++next_id), p_instance(instance) {
    logMsg("\nLukObject constructor instance,  id: ", id, "val: ", instance->toString());
    m_type = LukType::Instance;
    logMsg("\nIn LukObject instance toString: ", instance->toString());
    m_string = p_instance->toString();
}

LukObject::LukObject(nullptr_t) 
      : id(++next_id) {
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
      logMsg("\nIn LukObject, copy constructor, id: ", id, ", val: ");
      
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

std::string LukObject::typeOf() const {
    switch(m_type) {
        case LukType::Nil: return "nil";
        case LukType::Bool: return "bool";
        case LukType::Int: return "int";
        case LukType::Double: return "double";
        case LukType::String: return "string";
        case LukType::Callable:  return "callable";
        case LukType::Instance:  return "instance";
    }
    throw RuntimeError("Cannot determine the object's type.");

    return "''";
}

// returns the current value to string
std::string LukObject::value() {
       return _toString();
}

// convertions
bool LukObject::toBool() {
        if (m_type == LukType::Bool) return m_bool;
        m_bool = _toBool();
        m_type = LukType::Bool;
        return m_bool;
}

TLukInt LukObject::toInt() {
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
        case LukType::Int: return m_int != 0;
        case LukType::Double: return m_double != 0;
        case LukType::String: return m_string != "";
        // return p_string != nullptr;
        // callables and classes are true by default
        case LukType::Callable:
        case LukType::Instance: 
            return true;
    }
    
    throw RuntimeError("Invalid convertion to bool\n");

    return false;
}

TLukInt LukObject::_toInt() const {
    switch(m_type) {
        case LukType::Nil: return 0;
        case LukType::Bool: return m_bool ? 1 : 0;
        case LukType::Int: return m_int;
        case LukType::Double: return int(m_double);
        case LukType::String: {
            TLukInt i;
            try {
                i = std::stol(m_string);
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
    throw RuntimeError("Cannot convert object to int.");

    return 0;
}


double LukObject::_toDouble() const {
    switch(m_type) {
        case LukType::Nil: return 0.;
        case LukType::Bool: return m_bool ? 1.0 : 0.0;
        case LukType::Int: return double(m_int);
        case LukType::Double: return m_double;
        case LukType::String: {
            double d;
            try {
                d = std::stod(m_string);
            } catch (const std::invalid_argument &) {
                // not throw exception
                // std::cerr << "Argument is invalid\n";
                // throw 
                return 0.;
            } catch(const std::out_of_range &) {
                // std::cerr << "Argument is out of range for a double\n";
                // throw;
                return 0.;
            }
            
            return d;
        }
        break;

        case LukType::Callable:  
        case LukType::Instance:  
        break;
        
    }
    throw RuntimeError("Cannot convert object to double.");

    return 0.;
}

std::string LukObject::_toString() const {
    switch(m_type) {
        case LukType::Nil: return "nil";
        case LukType::Bool: return (m_bool ? "true" : "false");
        case LukType::Int: return std::to_string(m_int);
        case LukType::Double: return stripZeros( std::to_string(m_double) );
        case LukType::String: return m_string;
        case LukType::Callable: 
        case LukType::Instance: 
          return m_string;
          // return *p_string;
    }
    throw RuntimeError("Cannot convert object to string.");

    return "''";
}

std::string LukObject::stripZeros(std::string str) const {
    // erasing trailing zeros
    auto pos = str.find_last_not_of('0');
    // keeping the first zero whether they are only zeros after the dot
    if (str[pos] == '.') str.erase(pos +2, std::string::npos);
    else str.erase(pos +1, std::string::npos);
    // returning a constant string
    auto result = str;
    
    return result;


}

// casting to the right type
void LukObject::cast(LukType tp) {
    if (m_type == tp) return;
    switch(tp) {
        case LukType::Nil: break;
        case LukType::Bool: m_bool = bool(*this); break;
        case LukType::Int: m_int = int(*this); break;
        case LukType::Double: m_double = double(*this); break;
        case LukType::String: m_string = std::string(*this); break;
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
        m_int = obj->m_int;
        m_double = obj->m_double;
        m_string = obj->m_string;
        p_callable = obj->p_callable;
        p_instance = obj->p_instance;
        return std::make_shared<LukObject>(*this);
}


LukObject& LukObject::operator=(const bool&& val) { 
    id = ++next_id;
    m_type = LukType::Bool;
    m_bool = val;
    return *this;
}

LukObject& LukObject::operator=(const TLukInt&& val)  {
    id = ++next_id;
    m_type = LukType::Int;
    m_int = val;
    return *this;
}

LukObject& LukObject::operator=(const double&& val) { 
    id = ++next_id;
    m_type = LukType::Double;
    m_double = val;
    return *this;
}

// implement overloaded char* to avoid implicit casting to bool
LukObject& LukObject::operator=(const char* &&val) {
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
    m_int = obj.m_int; 
    m_double = obj.m_double;
    m_string = obj.m_string;
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
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw RuntimeError("Cannot add nil");
            case LukType::Bool:
                throw RuntimeError("Cannot add bools");
            case LukType::Int:
                m_int += obj.m_int; break;
            case LukType::Double:
                m_double += obj.m_double; break;
            case LukType::String:
                m_string += obj.m_string; break;
                // oss << m_string << obj.m_string;
                break;
            default:
                throw RuntimeError("Cannot add objects.");
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
                throw RuntimeError("Cannot substract nil");
            case LukType::Bool:
                throw RuntimeError("Cannot substract bools");
            case LukType::Int:
                m_int -= obj.m_int; break;
            case LukType::Double:
                m_double -= obj.m_double; break;
            case LukType::String:
                throw RuntimeError("Cannot substract strings.");
            default:
                throw RuntimeError("Cannot substract objects.");
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
                throw RuntimeError("Cannot multiply nil");
            case LukType::Bool:
                throw RuntimeError("Cannot multiply bools");
            case LukType::Int:
                m_int *= obj.m_int; break;
            case LukType::Double:
                m_double *= obj.m_double; break;
            case LukType::String:
                throw RuntimeError("Cannot multiply strings.");
            default:
                throw RuntimeError("Cannot multiply objects.");
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
    double num;
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw RuntimeError("Cannot divide nil");
            case LukType::Bool:
                throw RuntimeError("Cannot divide bools");
            case LukType::Int:
                num = double(m_int) / obj.m_int;
                if ( std::fmod(num, 1) == 0) m_int = int(num); 
                else {
                    m_double = num;
                    m_type = LukType::Double;
                }
                break;
            case LukType::Double:
                m_double /= obj.m_double; break;
            case LukType::String:
                throw RuntimeError("Cannot divide strings.");
            default:
                throw RuntimeError("Cannot divide objects.");
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

LukObject& LukObject::operator%=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Nil:
                throw RuntimeError("Cannot modulus nil");
            case LukType::Bool:
                throw RuntimeError("Cannot modulus bools");
            case LukType::Int:
                m_int %= obj.m_int; break;
            case LukType::Double:
                m_double = std::fmod(m_double, obj.m_double); break;
            case LukType::String:
                throw RuntimeError("Cannot modulus strings.");
            default:
                throw RuntimeError("Cannot modulus objects.");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) %= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) %= ob;
    }

}

// bitwise operators
// bitwise OR operator
LukObject& LukObject::operator|=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Bool:
                m_bool |= obj.m_bool; break;
            case LukType::Int:
                m_int |= obj.m_int; break;
            default:
                throw RuntimeError("Operands must be bools or integers");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) |= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) |= ob;
    }

}

// bitwise AND operator
LukObject& LukObject::operator&=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Bool:
                m_bool &= obj.m_bool; break;
            case LukType::Int:
                m_int &= obj.m_int; break;
            default:
                throw RuntimeError("Operands must be bools or integers");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) &= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) &= ob;
    }

}

// bitwise XOR operator
LukObject& LukObject::operator^=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Bool: m_bool ^= obj.m_bool; break;
            case LukType::Int: m_int ^= obj.m_int; break;
            default:
                throw RuntimeError("Operands must be bools or integers");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) ^= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) ^= ob;
    }

}

// bitwise NOT operator
LukObject operator~(LukObject a) {
    switch(a.m_type) {
        // Note: for ~ operator, bool value returns -1 or -2, 
        // so it's an integer
      case LukType::Bool:  a.m_int = a.m_bool == 0 ? -1 : -2;
            a.m_type = LukType::Int; break;
        case LukType::Int: a.m_int = ~a.m_int; break;
        default:
            throw RuntimeError("cannot bitwise NOT object.");
    }

    return a;
}

// bitwise shift left operator
LukObject& LukObject::operator<<=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Bool: m_int <<= obj.m_bool; 
                m_type = LukType::Int; break;
            case LukType::Int: m_int <<= obj.m_int; break;
            default:
                throw RuntimeError("Operands must be bools or integers");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) <<= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) <<= ob;
    }

}

// bitwise shift right operator
LukObject& LukObject::operator>>=(const LukObject& obj) {
    if (m_type == obj.m_type)  {
        switch(m_type) {
            case LukType::Bool: m_int >>= obj.m_bool; 
                m_type = LukType::Int; break;
            case LukType::Int: m_int >>= obj.m_int; break;
            default:
                throw RuntimeError("Operands must be bools or integers");
        }

        return (*this);
    }
    
    if (m_type < obj.m_type) {
        cast(obj.m_type);
        return (*this) >>= obj;
    } else {
        auto ob = obj;
        ob.cast(m_type);
        return (*this) >>= ob;
    }

}


// unary operators
// unary minus operator
LukObject operator-(LukObject a) {
    switch(a.m_type) {
        case LukType::Bool: a.m_bool = -a.m_bool; break;
        case LukType::Int: a.m_int = -a.m_int; break;
        case LukType::Double: a.m_double = -a.m_double; break;
        default: 
            throw RuntimeError("Unary minus cannot apply for this type.");
    }

    return a;
}
   
// unary not operator
LukObject operator!(LukObject a) {
    switch(a.m_type) {
        case LukType::Nil: a.m_bool = true; break;
        case LukType::Bool:  a.m_bool = !a.m_bool; break;
        case LukType::Int: a.m_bool = a.m_int == 0; break;
        case LukType::Double: a.m_bool = a.m_double == 0.; break;
        case LukType::String: a.m_bool = a.m_string == ""; break;
        default:
            throw RuntimeError("cannot negate object.");
    }

    a.m_type = LukType::Bool;
    return a;
}

// equality operators
// equality == operator
bool operator==(const luky::LukObject& a, const luky::LukObject& b) {
    if (a.m_type == b.m_type) {
        switch(a.m_type) {
            case LukType::Nil: return true;
            case LukType::Bool: return a.m_bool == b.m_bool;
            case LukType::Int: return a.m_int == b.m_int;
            case LukType::Double: return a.m_double == b.m_double;
            case LukType::String: return a.m_string == b.m_string;
            default: 
                throw RuntimeError("Cannot compare objects for equality.");
        }
    }

    if (a.m_type > b.m_type) return b == a;
    if (a.m_type == LukType::Nil || b.m_type == LukType::Nil) return false;
    
    // whether a.m_type < b.m_type
    switch(a.m_type) {
        case LukType::Bool: return a.m_bool == bool(b);
        case LukType::Int: 
              if (b.isDouble()) return a.m_int == b.m_double;
              break;
        case LukType::Double: return false;
        case LukType::String: return false;
        default:
            throw RuntimeError("Cannot compare objects for equality.");
    }
    
    return false;
}

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
                throw RuntimeError("Nil and Bool cannot odered.");
            case LukType::Int: return a.m_int < b.m_int;
            case LukType::Double: return a.m_double < b.m_double;
            case LukType::String: return a.m_string < b.m_string;
            default: 
                  throw RuntimeError("Objects cannot ordered.");
        }
    }
    if (a.isNumber() && b.isNumber()) {
        if (a.isInt()) return a.m_int < a.m_double;
        if (a.isDouble()) return a.m_double < a.m_int;
    }


    throw RuntimeError("Only objects of the same type can be ordered.");
}

/* 
 * Note: other operators like binary operators, are implemented inline in the header .hpp file
 * */

