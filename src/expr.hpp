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
class UnaryExpr;
class VariableExpr;

using PExpr = std::shared_ptr<Expr>;
// create visitor object
class ExprVisitor {
    public:
        virtual TObject visitAssignExpr(AssignExpr&) =0;
        virtual TObject visitBinaryExpr(BinaryExpr&) =0;
        virtual TObject visitCallExpr(CallExpr&) =0;
        virtual TObject visitGetExpr(GetExpr&) =0;
        virtual TObject visitGroupingExpr(GroupingExpr&) =0;
        virtual TObject visitLiteralExpr(LiteralExpr&) =0;
        virtual TObject visitLogicalExpr(LogicalExpr&) =0;
        virtual TObject visitSetExpr(SetExpr&) =0;
        virtual TObject visitUnaryExpr(UnaryExpr&) =0;
        virtual TObject visitVariableExpr(VariableExpr&) =0;
};

// Base class for different objects
class  Expr {
public:
  Expr() : m_id(0) {
      static unsigned next_id =0;
      next_id++;
      m_id = next_id;
    }
    
    virtual TObject accept(ExprVisitor &v) =0;
    virtual bool isCallExpr() const { return false; }
    virtual bool isGetExpr() const { return false; }
    virtual bool isSetExpr() const { return false; }
    virtual bool isVariableExpr() const { return false; }
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
    
    TObject accept(ExprVisitor &v) override {
        return v.visitAssignExpr(*this); 
    }

    Token name;
    PExpr value;
};


class BinaryExpr : public Expr {
public:
    BinaryExpr(PExpr&& _left, Token _op, PExpr&& _right) {
        left = std::move(_left);
        op = _op;
        right = std::move(_right);
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitBinaryExpr(*this); 
    }

    PExpr left;
    Token op;
    PExpr right;
};

class CallExpr : public Expr {
public:
    CallExpr(PExpr&& _callee, Token _paren, std::vector<PExpr>&& _args) {
        callee = std::move(_callee);
        paren = _paren;
        args = std::move(_args);
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitCallExpr(*this); 
    }

    bool isCallExpr() const override { return true; }
    std::string typeName() const override { return "CallExpr"; }

    PExpr callee;
    Token paren;
    std::vector<PExpr> args;
};

class GetExpr : public Expr {
public:
    GetExpr(PExpr&& object, Token name) :
      m_object(std::move(object)),
      m_name(name) {}
    bool isGetExpr() const override { return true; }
    std::string typeName() const override { return "GetExpr"; }
    Token getName() const override { return m_name; }
    // PExpr getObject() const override { return m_object; }


    TObject accept(ExprVisitor &v) override {
        return v.visitGetExpr(*this); 
    }

    PExpr m_object;
    Token m_name;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(PExpr&& _expr) {
        expression = std::move(_expr);
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitGroupingExpr(*this); 
    }

    PExpr expression;
};

class LiteralExpr: public Expr {
public:

    LiteralExpr(LukObject _value) 
        : value(std::move(_value)) 
        {
        // std::cerr << "LiteralExpr, id: " << _value.id << "\n";
        // std::cerr << "value, id: " << value.id << "\n";
    }

    TObject accept(ExprVisitor &v) override {
        return v.visitLiteralExpr(*this); 
    }

LukObject value;
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(PExpr&& _left, Token _op, PExpr&& _right) {
        left = std::move(_left);
        op = _op;
        right = std::move(_right);
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitLogicalExpr(*this); 
    }

    PExpr left;
    Token op;
    PExpr right;
};

class SetExpr : public Expr {
public:
    SetExpr(PExpr&& object, Token name, PExpr&& value) :
      m_object(std::move(object)),
      m_name(name),
      m_value(std::move(value)) {}

    bool isSetExpr() const override { return true; }
    std::string typeName() const override { return "SetExpr"; }
    Token getName() const override { return m_name; }
    // note: cannot return an instance of unique_ptr
    // PExpr getObject() const override { return m_object; }

    TObject accept(ExprVisitor &v) override {
        return v.visitSetExpr(*this); 
    }

    PExpr m_object;
    Token m_name;
    PExpr m_value;
};


class UnaryExpr : public Expr {
public:
    UnaryExpr(Token _op, PExpr&& _right) {
        op = _op;
        right = std::move(_right);
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitUnaryExpr(*this); 
    }

    Token op;
    PExpr right;
};

class VariableExpr : public Expr {
public:
    VariableExpr(Token _name) {
        name = _name;
    }
    
    TObject accept(ExprVisitor &v) override {
        return v.visitVariableExpr(*this); 
    }

    bool isVariableExpr() const override { return true; }
    std::string typeName() const override { return "VariableExpr"; }

    Token name;
};


#endif // EXPR_HPP
