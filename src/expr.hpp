#ifndef EXPR_HPP
#define EXPR_HPP

#include "lukobject.hpp"
#include "token.hpp"
#include <memory>
#include <vector>
// forward declarations

class Expr;
class LukObject;
class AssignExpr;
class BinaryExpr;
class CallExpr;
class GetExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class SetExpr;
class SuperExpr;
class ThisExpr;
class UnaryExpr;
class VariableExpr;

using PExpr = std::shared_ptr<Expr>;
// create visitor object
class ExprVisitor {
    public:
        virtual ObjPtr visitAssignExpr(AssignExpr&) =0;
        virtual ObjPtr visitBinaryExpr(BinaryExpr&) =0;
        virtual ObjPtr visitCallExpr(CallExpr&) =0;
        virtual ObjPtr visitGetExpr(GetExpr&) =0;
        virtual ObjPtr visitGroupingExpr(GroupingExpr&) =0;
        virtual ObjPtr visitLiteralExpr(LiteralExpr&) =0;
        virtual ObjPtr visitLogicalExpr(LogicalExpr&) =0;
        virtual ObjPtr visitSetExpr(SetExpr&) =0;
        virtual ObjPtr visitSuperExpr(SuperExpr&) =0;
        virtual ObjPtr visitThisExpr(ThisExpr&) =0;
        virtual ObjPtr visitUnaryExpr(UnaryExpr&) =0;
        virtual ObjPtr visitVariableExpr(VariableExpr&) =0;
};

// Base class for different objects
class  Expr {
public:
  Expr() : m_id(0) {
      static unsigned next_id =0;
      next_id++;
      m_id = next_id;
    }
    
    virtual ObjPtr accept(ExprVisitor &v) =0;
    virtual bool isCallExpr() const { return false; }
    virtual bool isGetExpr() const { return false; }
    virtual bool isSetExpr() const { return false; }
    virtual bool isVariableExpr() const { return false; }
    // Note: the two folowing virtual func must be implemented
    // in callexpr, variableexpr, getexpr, setexpr objects.
    virtual std::string typeName() const { return "Expr"; }
    virtual Token getName() const { return Token(); }
    virtual PExpr getObject() const { return nullptr; }

    virtual unsigned id() const { return m_id; }

private:
  unsigned m_id;

};

// differents objects

class AssignExpr : public Expr {
public:
    AssignExpr(Token _name, PExpr _value) {
        name = _name;
        value = std::move(_value);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitAssignExpr(*this); 
    }

    Token name;
    PExpr value;
};


class BinaryExpr : public Expr {
public:
    BinaryExpr(PExpr _left, Token _op, PExpr _right) {
        left = std::move(_left);
        op = _op;
        right = std::move(_right);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitBinaryExpr(*this); 
    }

    PExpr left;
    Token op;
    PExpr right;
};

class CallExpr : public Expr {
public:
    CallExpr(PExpr _callee, Token _paren, std::vector<PExpr> _args) {
        callee = std::move(_callee);
        paren = _paren;
        args = std::move(_args);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitCallExpr(*this); 
    }

    bool isCallExpr() const override { return true; }
    std::string typeName() const override { return "CallExpr"; }
    virtual Token getName() const { return paren; }

    PExpr callee;
    Token paren;
    std::vector<PExpr> args;
};

class GetExpr : public Expr {
public:
    GetExpr(PExpr object, Token name) :
      m_object(std::move(object)),
      m_name(name) {}
    bool isGetExpr() const override { return true; }
    std::string typeName() const override { return "GetExpr"; }
    Token getName() const override { return m_name; }
    PExpr getObject() const override { return m_object; }


    ObjPtr accept(ExprVisitor &v) override {
        return v.visitGetExpr(*this); 
    }

    PExpr m_object;
    Token m_name;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(PExpr _expr) {
        expression = std::move(_expr);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitGroupingExpr(*this); 
    }

    PExpr expression;
};

class LiteralExpr: public Expr {
public:

    LiteralExpr(ObjPtr& _value) 
        : value(_value) {
        logMsg("\nLiteralExpr constructor");
        logMsg("_value.id: ", _value->id);
        logMsg("value.id: ", value->id);
    }
    ~LiteralExpr() {
        logMsg("~LiteralExpr destructor");
    }

    ObjPtr accept(ExprVisitor &v) override {
        return v.visitLiteralExpr(*this); 
    }

ObjPtr value;
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(PExpr _left, Token _op, PExpr _right) {
        left = std::move(_left);
        op = _op;
        right = std::move(_right);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitLogicalExpr(*this); 
    }

    PExpr left;
    Token op;
    PExpr right;
};

class SetExpr : public Expr {
public:
    SetExpr(PExpr object, Token name, PExpr value) :
      m_object(std::move(object)),
      m_name(name),
      m_value(std::move(value)) {}

    bool isSetExpr() const override { return true; }
    std::string typeName() const override { return "SetExpr"; }
    Token getName() const override { return m_name; }
    // Fix: can now an instance of shared_ptr instead unique_ptr
    PExpr getObject() const override { return m_object; }

    ObjPtr accept(ExprVisitor &v) override {
        return v.visitSetExpr(*this); 
    }

    PExpr m_object;
    Token m_name;
    PExpr m_value;
};

class SuperExpr : public Expr {
public:
    SuperExpr(Token keyword, Token method) :
      m_keyword(keyword),
      m_method(method) {}

    ObjPtr accept(ExprVisitor &v) override {
        return v.visitSuperExpr(*this); 
    }

    Token m_keyword;
    Token m_method;
};

class ThisExpr : public Expr {
public:
    ThisExpr(Token& keyword) {
        m_keyword = keyword;
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitThisExpr(*this); 
    }

    Token m_keyword;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token _op, PExpr _right) {
        op = _op;
        right = std::move(_right);
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitUnaryExpr(*this); 
    }

    Token op;
    PExpr right;
};

class VariableExpr : public Expr {
public:
    VariableExpr();
    VariableExpr(Token _name) {
        name = _name;
    }
    
    ObjPtr accept(ExprVisitor &v) override {
        return v.visitVariableExpr(*this); 
    }
    bool isVariableExpr() const override { return true; }
    std::string typeName() const override { return "VariableExpr"; }
    Token getName() const override { return name; }


    Token name;
};


#endif // EXPR_HPP
