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
// namespace luky {

using PExpr = std::unique_ptr<Expr>;

// create visitor object
class ExprVisitor {
    public:
        virtual void visitBinaryExpr(BinaryExpr&) =0;
        virtual void visitGroupingExpr(GroupingExpr&) =0;
        virtual void visitLiteralExpr(LiteralExpr&) =0;
        virtual void visitUnaryExpr(UnaryExpr&) =0;
};

// Base class for different objects
class  Expr {
public:
    virtual void accept(class ExprVisitor &v) =0;
};

// differents objects
class BinaryExpr : public Expr {
public:
    BinaryExpr(PExpr&& _left, Token _op, PExpr&& _right) {
        left = std::move(_left);
        op = _op;
        right = std::move(_right);
    }
    
    void accept(ExprVisitor &v) override {
        v.visitBinaryExpr(*this); 
    }

    PExpr left;
    Token op;
    PExpr right;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(PExpr&& _expr) {
        expr = std::move(_expr);
    }
    
    void accept(ExprVisitor &v) override {
        v.visitGroupingExpr(*this); 
    }

    PExpr expr;
};

class LiteralExpr: public Expr {
public:
    LiteralExpr(LukObject _value) { value = _value; }
    void accept(ExprVisitor &v) override {
        v.visitLiteralExpr(*this); 
    }

    LukObject value;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token _op, PExpr&& _right) {
        op = _op;
        right = std::move(_right);
    }
    
    void accept(ExprVisitor &v) override {
        v.visitUnaryExpr(*this); 
    }

    Token op;
    PExpr right;
};
// }
//
// temporary methods
void ExprVisitor::visitBinaryExpr(BinaryExpr& expr) {}
void ExprVisitor::visitLiteralExpr(LiteralExpr& expr) {}


#endif // EXPR_HPP
