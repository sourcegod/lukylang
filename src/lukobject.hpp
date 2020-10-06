#ifndef LUKOBJECT_HPP
#define LUKOBJECT_HPP

#include <sstream> // ostreamstring
#include <string>
#include <iostream>
#include <memory>
#include "token.hpp"

enum class LukType { Nil=0, Bool=1, Number=2, String=3 };

class Token;
class LukObject {
protected:
    static int next_id;

public:
    int id;
    LukType type_id;
    bool m_bool = false;
    double m_number =0;
    std::string m_string = "";

 
    // constructors
    LukObject() 
        : id(++next_id)
    { 
        type_id = LukType::Nil;  
        std::cerr << "Luk Constructor id: " << id << std::endl;
    }
    LukObject(bool val) 
        : id(++next_id)
    { type_id = LukType::Bool; m_bool = val;
        std::cerr << "LukBool constructor\n"; 
    }
    LukObject(int val) 
        : id(++next_id)
    { type_id = LukType::Number; m_number = val; 
        std::cerr << "LukInt constructor\n"; 
    }
    LukObject(double val) 
        : id(++next_id)
    { type_id = LukType::Number; m_number = val; 
        std::cerr << "LukDouble constructor id: " << id << std::endl; 
    }
    LukObject(const std::string& val) 
        : id(++next_id)
    { type_id = LukType::String; m_string = val; 
        std::cerr << "LukString constructor id: " << id << std::endl; 
    }

    LukObject(const char* val) 
        : id(++next_id)
    { type_id = LukType::String; m_string = std::string(val); 
        std::cerr << "LukChar constructor id: " << id << std::endl; 
    }
    LukObject(Token tok);
   
    // destructor is necessary
    ~LukObject() {
        std::cerr << "Destructor id: " << id << std::endl;
    }

       
    // get the type id
    LukType type() { return type_id; }

    // get the id name
    std::string name() { 
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

    // casting to the right type
    void cast(LukType tp);

    // convertion operators to cast the LukObject type
    operator bool() const { return _toBool(); }
    operator double() const { return _toNumber(); }
    operator std::string() const { return _toString(); }

    // assignment operators 
    // LukObject& operator=(nullptr_t val);
    LukObject& operator=(const bool&& val);
    LukObject& operator=(const int&& val);
    LukObject& operator=(const double&& val);
    // implement overloaded char* to avoid implicit casting to bool
    LukObject& operator=(const char* &&val);
    LukObject& operator=(const std::string&& val);
 
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
        case Type::Nil: return ost << "Nil";
        case Type::Bool: return ost << "Bool";
        case Type::Number: return ost << "Number";
        case Type::String: return ost << "String";
    }
    
    return ost << "Invalid Object type";
}


#endif // LUK_OBJECT_HPP
