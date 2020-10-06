#ifndef STMT_HPP
#define STMT_HPP

#include "expr.hpp"
#include "lukobject.hpp"
#include "token.hpp"
#include <memory> // smart pointer
#include <vector>

// forward declarations
class Stmt;
class ExpressionStmt;
class PrintStmt;
class VarStmt;

using PStmt = std::unique_ptr<Stmt>;

class StmtVisitor {
public:
    virtual void visitExpressionStmt(ExpressionStmt&) =0;
    virtual void visitPrintStmt(PrintStmt&) =0;
    virtual void visitVarStmt(VarStmt&) =0;
};

class Stmt {
public:
    virtual void accept(StmtVisitor&) = 0;
};

class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(std::unique_ptr<Expr>&& _expr) {
        expression = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expression;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(std::unique_ptr<Expr>&& _expr) {
        expression = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitPrintStmt(*this);
    }

    std::unique_ptr<Expr> expression;

};

class VarStmt : public Stmt {
public:
    VarStmt(Token _name, std::unique_ptr<Expr>&& _expr) {
        name = _name;
        initializer = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitVarStmt(*this);
    }
    Token name;
    std::unique_ptr<Expr> initializer;

};


#endif // STMT_HPP
