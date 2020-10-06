#ifndef EXPR_HPP
#define EXPR_HPP

#include "lukobject.hpp"
#include "token.hpp"
#include <memory>

// forward declarations
class Expr;
class AssignExpr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class UnaryExpr;
class VariableExpr;

using PExpr = std::unique_ptr<Expr>;
using TObject = LukObject;

// create visitor object
class ExprVisitor {
    public:
        virtual TObject visitAssignExpr(AssignExpr&) =0;
        virtual TObject visitBinaryExpr(BinaryExpr&) =0;
        virtual TObject visitGroupingExpr(GroupingExpr&) =0;
        virtual TObject visitLiteralExpr(LiteralExpr&) =0;
        virtual TObject visitLogicalExpr(LogicalExpr&) =0;
        virtual TObject visitUnaryExpr(UnaryExpr&) =0;
        virtual TObject visitVariableExpr(VariableExpr&) =0;
};

// Base class for different objects
class  Expr {
public:
    virtual TObject accept(ExprVisitor &v) =0;
    virtual bool isVariableExpr() const { return false; }
    virtual std::string typeName() const { return "Expr"; }
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
    // : value(_value) 
    {
    value = _value;

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
