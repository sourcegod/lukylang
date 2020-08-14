#ifndef LUKOBJECT_HPP
#define LUKOBJECT_HPP

#include "lukinstance.hpp"
#include "logger.hpp"
#include "token.hpp"

#include <sstream> // ostreamstring
#include <string>
#include <iostream>
#include <memory>

// Note: best practice:
// to avoid circular dependencies files,
// make forward declarations in the header files,
// and make definitions classes in the source (.cpp) files
// and the include headers files.

// forward declarations
class Token;
class LukCallable;
class LukFunction;
class LukObject;
class LukInstance;
using TObject = LukObject;
using ObjPtr = std::shared_ptr<LukObject>;
// macro to make global variable the static nilptr
#define nilptr LukObject::getStatNilPtr()

enum class LukType { 
    Nil=0, Bool=1, Number=2, String=3,
    Callable =4, Instance=5
};

class LukObject {
protected:
    static int next_id;
    // Note: static variable must defining in the .cpp file
    // to avoid multiple header inclusion and compile error
    static ObjPtr stat_nilPtr;

public:
    int id;
    LukType m_type;
    bool m_bool = false;
    double m_number =0;
    std::string m_string = "";
    std::shared_ptr<std::string> p_string;
    std::shared_ptr<LukCallable> p_callable;
    std::shared_ptr<LukInstance> p_instance;
 
    // constructors
    LukObject();
    LukObject(bool val); 
    LukObject(int val); 
    LukObject(double val);
    LukObject(const std::string& val);
    LukObject(const char* val);
    LukObject(std::shared_ptr<LukCallable> callable);
    LukObject(std::shared_ptr<LukInstance> instance);
    LukObject(Token tok);
    LukObject(TokPtr& tokP);
    LukObject(nullptr_t nulp);
    
    // copy constructor
    LukObject(const LukObject& obj);
    
    // move constructor
    LukObject(const LukObject&& obj);
     
    // destructor is necessary
    ~LukObject() {
        logMsg("\n~LukObject destructor,  id: ", id, ", val: ", toString());
    }
    

    // get the type id
    LukType getType() { return m_type; }

    // get id number
    size_t getId() { return id; }
    // get id name
    std::string getName() { 
        std::ostringstream oss;
        oss << this;
        return oss.str(); 
    }
    // clonable
    std::shared_ptr<LukObject> clone()  { return std::make_shared<LukObject>(*this); }

    // convertions
    bool toBool();
    double toNumber();
    std::string toString();
    std::string value();

    // convert string to number  
    // Note: template function must be defining in the header file, not in the implementation file.
    // not used, is just as an exercise for template
    // Note: usage:
    // double number = stringToNumber<double>("0.6");
    template<typename T>
        T stringToNumber(const std::string& stg) {
        T tValue;
        std::stringstream stream(stg);
        stream >> tValue;
        if (stream.fail()) {
            std::runtime_error e(stg);
            throw e;
        }

        return tValue;
    }
 
    // Note: make dynamic casting to convert base object to derived one.
    // Note: template function must be defining in the header file, not in the implementation file.
    template <typename T>
    std::shared_ptr<T> getDynCast()  {
      if (p_callable == nullptr) {
        std::runtime_error err("LukObjectError: cannot casting p_callable when its null");
        throw err;
      }

      return std::dynamic_pointer_cast<T>(p_callable);
    }

    // test type state
    bool isNil() const { return m_type == LukType::Nil; }
    bool isBool() const { return m_type == LukType::Bool; }
    bool isNumber() const { return m_type == LukType::Number; }
    bool isDouble() const { return m_type == LukType::Number; }
    bool isString() const { return m_type == LukType::String; }
    bool isCallable() const { return m_type == LukType::Callable; }
    bool isInstance() const { return m_type == LukType::Instance; }

    // getters
    static LukObject getNil() {
      logMsg("In static getNil");
        static LukObject obj;
        return obj;
    }
    
    static ObjPtr getNilPtr()  {
      logMsg("In static getNilPtr");
      static ObjPtr nilP = std::make_shared<LukObject>();
      return nilP;
    }
  // Note: static stat_nilPtr is protected for access control
  // so, we need a static function to get it out of the class
    static ObjPtr& getStatNilPtr() { return stat_nilPtr; }

    bool getBool() const noexcept { return m_bool; }
    double getNumber() const noexcept { return m_number; }
    std::string getString() const noexcept { return m_string; }
    std::shared_ptr<std::string> getPtrString() const noexcept { return p_string; }
    std::shared_ptr<LukCallable> getCallable() const noexcept { return p_callable; }
    // std::shared_ptr<LukFunction> getFunc(); 
    std::shared_ptr<LukInstance> getInstance() const noexcept { return p_instance; }
    

    // casting to the right type
    void cast(LukType tp);

    // convertion operators to cast the LukObject type
    operator bool() const { return _toBool(); }
    operator double() const { return _toNumber(); }
    operator std::string() const { return _toString(); }

    // assignment operators 
    // TODO: convert nullptr to shared_ptr lukobject
    std::shared_ptr<LukObject> operator=(nullptr_t);
    LukObject& operator=(const bool&& val);
    LukObject& operator=(const int&& val);
    LukObject& operator=(const double&& val);
    // implement overloaded char* to avoid implicit casting to bool
    LukObject& operator=(const char* &&val);
    LukObject& operator=(const std::string&& val);
    
    // Note: swap function to avoid duplicate code between copy constructor and copy assignment operator
    void swap(const LukObject& obj);   
    // copy assignment operator
    LukObject& operator=(const LukObject& obj); 
    // move assignment operator
    LukObject& operator=(const LukObject&& obj); 
 
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
    friend inline std::ostream& operator<<(std::ostream& ost, ObjPtr& obj);
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
inline std::ostream& operator<<(std::ostream& ost, LukObject& obj) { return ost << obj.toString(); } 
inline std::ostream& operator<<(std::ostream& ost, ObjPtr& obj) { return ost << obj->toString(); } 

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
        case Type::Instance: return ost << "<Instance>";
    }
    
    return ost << "Invalid Object type";
}


#endif // LUKOBJECT_HPP
