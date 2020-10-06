#ifndef EXPR_HPP
#define EXPR_HPP

#include "lukobject.hpp"
#include "token.hpp"
#include <memory>

// forward declarations
class Expr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

using PExpr = std::unique_ptr<Expr>;
using TObject = LukObject;

// create visitor object
class ExprVisitor {
    public:
        virtual TObject visitBinaryExpr(BinaryExpr&) =0;
        virtual TObject visitGroupingExpr(GroupingExpr&) =0;
        virtual TObject visitLiteralExpr(LiteralExpr&) =0;
        virtual TObject visitUnaryExpr(UnaryExpr&) =0;
};

// Base class for different objects
class  Expr {
public:
    virtual TObject accept(ExprVisitor &v) =0;
};

// differents objects
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
    std::cerr << "in literalexpr \n";
    value = _value;
    std::cerr << "obj _value  id: " << _value.id << std::endl;
    std::cerr << "obj value  id: " << value.id << std::endl;

}
    TObject accept(ExprVisitor &v) override {
        return v.visitLiteralExpr(*this); 
    }

LukObject value;
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

#endif // EXPR_HPP
