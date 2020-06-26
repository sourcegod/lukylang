/*
 * Object base class for luky interpreter
 * Date: jeudi, 01/08/19
 * */

#include "lukobject.hpp"
#include "token.hpp"

#include <iostream> // cout and cerr
#include <sstream> // ostringstream
#include <stdexcept> // exception

int LukObject::next_id =0;
// constructors
LukObject::LukObject(Token tok) 
        : id(++next_id) {
    // std::cerr << "C.tor with token, id: " << id << "\n";
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
        case TokenType::NUMBER: 
            m_type = LukType::Number;
            m_number = std::stod(tok.literal); 
            break;
        case TokenType::STRING: 
            m_type = LukType::String;
            // m_string = tok.literal;
            p_string = std::make_shared<std::string>(tok.literal);
            break;
        default:
            std::runtime_error("Invalid Luky object.");
    }

}

// returns the current value to string
std::string LukObject::value() {
        switch(m_type) {
            case LukType::Nil: 
                return "nil";
            case LukType::Bool: 
                return m_bool ? "true" : "false";
            case LukType::Number: 
                return std::to_string(m_number);
            case LukType::String: 
                // return m_string;
                return *p_string;
            case LukType::Callable: 
            case LukType::Instance:  
              return *p_string;
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

double LukObject::toNumber() {
        if (m_type == LukType::Number) return m_number;
        m_number = _toNumber();
        m_type = LukType::Number;
        return m_number;
    }
    
// Note: convert string to number  
// not used, is just as an exercise for template
template<typename T>
T LukObject::stringToNumber(const std::string& stg)
    {
        T tValue;
        std::stringstream stream(stg);
        stream >> tValue;
        if (stream.fail()) {
            std::runtime_error e(stg);
            throw e;
        }
  
        return tValue;
    }
    // usage:
    // double number = stringToNumber<double>("0.6");

std::string LukObject::toString() {
        if (m_type == LukType::String) return m_string;
        m_string = _toString();
        m_type = LukType::String;
        return m_string;
}

bool LukObject::_toBool() const {
    switch(m_type) {
        case LukType::Nil: return false;
        case LukType::Bool: return m_bool != 0;
        case LukType::Number: return m_number != 0;
        case LukType::String: return p_string != nullptr;
        // callables and classes are true by default
        case LukType::Callable:
        case LukType::Instance: 
            return true;
    }
    
    throw std::runtime_error("Invalid convertion to bool\n");

    return false;
}

double LukObject::_toNumber() const {
    switch(m_type) {
        case LukType::Nil: return 0.;
        case LukType::Bool: return m_bool ? 1.0 : 0.0;
        case LukType::Number: return m_number;
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
        case LukType::Number: return std::to_string(m_number);
        case LukType::String: return *p_string;
        case LukType::Callable: 
        case LukType::Instance: 
          return *p_string;
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
        case LukType::Number: m_number = (double)(*this); break;
        case LukType::String: m_string = (std::string)(*this); break;
        case LukType::Callable: 
        case LukType::Instance: 
            break;
    
    }
    m_type = tp;

}

// assignment operators
/*
LukObject& LukObject::operator=(nullptr_t nullptr) {
        m_type = LukType::Nil;
        return *this;
}
*/

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
    m_type = LukType::Number;
    m_number = val;
    return *this;
}

LukObject& LukObject::operator=(const double&& val) 
{
    id = ++next_id;
    m_type = LukType::Number;
    m_number = val;
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

LukObject& LukObject::operator=(const LukObject& obj) { 
    // avoid copy of same object
    if (this == &obj) return *this;
    id = ++next_id;
    // std::cerr << "Copy Assignement, id: " << id << "\n";
    m_type = obj.m_type;
    m_bool = obj.m_bool; 
    m_number = obj.m_number;
    // m_string = obj.m_string;
    p_string = obj.p_string;
    p_callable = obj.p_callable;
    // std::cerr << "Voici obj.p_string: " << obj.p_string << std::endl;
    // std::cerr << "Voici p_string: " << p_string << std::endl;

    return *this;
}

/*
LukObject& LukObject::operator=(const LukObject&& obj) { 
    // avoid copy of same object
    if (this == &obj) return *this;
    id = ++next_id;
        // std::cerr << "Move Assignement, id: " << id << "\n";
    m_type = obj.m_type;
    m_bool = obj.m_bool; 
    m_number = obj.m_number;
    // m_string = std::move(obj.m_string);
    p_string = std::move(obj.p_string);
    p_callable = obj.p_callable;
    // std::cerr << "Voici obj.p_string: " << obj.p_string << std::endl;
    // std::cerr << "Voici p_string: " << p_string << std::endl;

    return *this;
}
*/


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
            case LukType::Number:
                m_number += obj.m_number; break;
            case LukType::String:
                // m_string += obj.m_string; break;
                // p_string  += obj.p_string;
                oss << *p_string << *obj.p_string;
                p_string = std::make_shared<std::string>(oss.str());
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
            case LukType::Number:
                m_number -= obj.m_number; break;
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
            case LukType::Number:
                m_number *= obj.m_number; break;
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
            case LukType::Number:
                m_number /= obj.m_number; break;
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
        case LukType::Number: a.m_number = -a.m_number; break;
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
        case LukType::Number: 
            a.m_bool = a.m_number == 0.; break;
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
            case LukType::Number: return a.m_number == b.m_number;
            case LukType::String: return a.m_string == b.m_string;
            default: 
                throw std::runtime_error("Cannot compare objects for equality.");
        }
    }

    if (a.m_type > b.m_type) return b == a;
    if (a.m_type == LukType::Nil || b.m_type == LukType::Nil) return false;
    switch(a.m_type) {
        case LukType::Bool: return a.m_bool == (bool)b;
        case LukType::Number: return a.m_number == (double)b;
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
            case LukType::Number: return a.m_number < b.m_number;
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

