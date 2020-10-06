#ifndef LUKOBJECT_HPP
#define LUKOBJECT_HPP

#include "lukcallable.hpp"
#include <sstream> // ostreamstring
#include <string>
#include <iostream>
#include <memory>

enum class LukType { 
    Nil=0, Bool=1, Number=2, String=3,
    Callable =4
};

class Token;
class LukCallable;

class LukObject {
protected:
    static int next_id;

public:
    int id;
    LukType type_id;
    bool m_bool = false;
    double m_number =0;
    std::string m_string = "";
    std::shared_ptr<std::string> p_string;
    std::shared_ptr<LukCallable> p_callable;

 
    // constructors
    LukObject() 
        : id(++next_id) { 
        // std::cerr << "C.tor, id: " << id << "\n";
        type_id = LukType::Nil;  
    }

    LukObject(bool val) 
        : id(++next_id) {
        type_id = LukType::Bool; m_bool = val; 
    }
    
    LukObject(int val) 
        : id(++next_id) { type_id = LukType::Number; m_number = val; }

    LukObject(double val) 
        : id(++next_id)
    { type_id = LukType::Number; m_number = val; }
    LukObject(const std::string& val) 
        : id(++next_id) { 
        // std::cerr << "Copy C.tor with string&, id: " << id << "\n";
        type_id = LukType::String; 
        // m_string = val; 
        p_string = std::make_shared<std::string>(val);
    }

    LukObject(const char* val) 
        : id(++next_id) { 
        type_id = LukType::String; 
        // m_string = std::string(val); 
        p_string = std::make_shared<std::string>(val);
    }
    
    // /* 
    LukObject(std::shared_ptr<LukCallable> callable)
        : id(++next_id) { 
        type_id = LukType::Callable;
        p_callable = callable; // std::make_shared<LukCallable>(callable);
        p_string = std::make_shared<std::string>(callable->toString());
    }
    // */

        
    LukObject(Token tok);
   
    // destructor is necessary
    ~LukObject() {
        // std::cerr << "D.tor id: " << id << std::endl;
    }

       
    // get the type id
    LukType getType() { return type_id; }

    // get the id name
    std::string getName() { 
        std::ostringstream oss;
        oss << this;
        return oss.str(); 
    }
        
    // convertions
    bool toBool();
    double toNumber();
    std::string toString();
    std::string value();

   // convert string to number  
    template<typename T>
    T stringToNumber(const std::string& stg);

    // test type state
    bool isNil() { return type_id == LukType::Nil; }
    bool isBool() { return type_id == LukType::Bool; }
    bool isNumber() { return type_id == LukType::Number; }
    bool isDouble() { return type_id == LukType::Number; }
    bool isString() { return type_id == LukType::String; }
    bool isCallable() { return type_id == LukType::Callable; }

    // getters
    LukObject getNil() {
        LukObject obj;
        return obj;
    }

    bool getBool() { return m_bool; }
    double getNumber() { return m_number; }
    std::string& getString() { return m_string; }
    std::shared_ptr<std::string> getPtrString() { return p_string; }
    std::shared_ptr<LukCallable> getCallable() { return p_callable; }
    

    // casting to the right type
    void cast(LukType tp);

    // convertion operators to cast the LukObject type
    operator bool() const { return _toBool(); }
    operator double() const { return _toNumber(); }
    operator std::string() const { return _toString(); }

    // assignment operators 
    // LukObject& operator=(nullptr_t);
    LukObject& operator=(const bool&& val);
    LukObject& operator=(const int&& val);
    LukObject& operator=(const double&& val);
    // implement overloaded char* to avoid implicit casting to bool
    LukObject& operator=(const char* &&val);
    LukObject& operator=(const std::string&& val);
    LukObject& operator=(const LukObject& obj); 
    // LukObject& operator=(const LukObject&& obj); 
 
    // compound assignment operators
    LukObject& operator+=(const LukObject& obj);
    LukObject& operator-=(const LukObject& obj);
    LukObject& operator*=(const LukObject& obj);
    LukObject& operator/=(const LukObject& obj);

    /*
        Note: these operators are non member functions
        therefore there are declared friends
        Also, for binary operators, object a is passing by copy not by reference
        so the original object a stay unchanged
    */

    // Equality operators
    friend bool operator==(const LukObject& a, const LukObject& b);
    friend inline bool operator!=(LukObject& a, LukObject& b);

    // Unary operators
    friend LukObject operator-(LukObject a);
    friend LukObject operator!(LukObject a);

    // Binary operators
    friend inline LukObject& operator+(LukObject a, const LukObject& b);
    friend inline LukObject& operator-(LukObject a, const LukObject& b); 
    friend inline LukObject& operator*(LukObject a, const LukObject& b); 
    friend inline LukObject& operator/(LukObject a, const LukObject& b); 

    // comparison operators
     friend bool operator<(const LukObject& a, const LukObject& b);
    friend inline bool operator<=(const LukObject& a, const LukObject& b);
    friend inline bool operator>(const LukObject& a, const LukObject& b);
    friend inline bool operator>=(const LukObject& a, const LukObject& b);

    // Output operators
    // friend declaration cause ostream accept only one argument 
    friend inline std::ostream& operator<<(std::ostream& ost, LukObject& obj);
    friend inline std::ostream& operator<<(std::ostream& ost, LukType tp);
 
private:
    // methods
    bool _toBool() const;
    double _toNumber() const;
    std::string _toString() const;
    

};

// binary operators 
inline LukObject& operator+(LukObject a, const LukObject & b) { return a += b; }
inline LukObject& operator-(LukObject a, const LukObject& b) { return a -= b; }
inline LukObject& operator*(LukObject a, const LukObject& b) { return a *= b; }
inline LukObject& operator/(LukObject a, const LukObject& b) { return a /= b; }
/// Note: "==" and "<" operators are implemented in the source filefor better readability
// equality != operator
inline bool operator!=(LukObject& a, LukObject& b) { return !(a == b); }

// comparison operators
bool operator<(const LukObject& a, const LukObject& b);
inline bool operator<=(const LukObject& a, const LukObject& b) { return a< b || a == b; }
inline bool operator>(const LukObject& a, const LukObject& b) { return !(a <= b); }
inline bool operator>=(const LukObject& a, const LukObject& b) { return !(a < b); }



// /*
// comparison operators for LukType object
bool operator<(LukType a, LukType b);
bool operator>(LukType a, LukType b);

// output operators
// friend function declaration, because ostream accept only one argument
inline std::ostream& operator<<(std::ostream& ost, LukObject& obj) { return ost << obj.value(); } 

// output << operator for LukType enum
inline std::ostream& operator<<(std::ostream& ost, LukType tp) {
    // test for alias
    // // using Type = LukObject::LukType
    using Type = LukType;
    switch(tp) {
        case Type::Nil: return ost << "<Nil>";
        case Type::Bool: return ost << "<Bool>";
        case Type::Number: return ost << "<Number>";
        case Type::String: return ost << "<String>";
        case Type::Callable: return ost << "<Callable>";
    }
    
    return ost << "Invalid Object type";
}


#endif // LUKOBJECT_HPP
