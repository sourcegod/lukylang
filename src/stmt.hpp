#ifndef STMT_HPP
#define STMT_HPP

#include "expr.hpp"
#include "lukobject.hpp"
#include "token.hpp"
#include <memory> // smart pointer
#include <vector>


// forward declarations
class BlockStmt;
class ExpressionStmt;
class IfStmt;
class PrintStmt;
class Stmt;
class VarStmt;

using PStmt = std::unique_ptr<Stmt>;
using PExpr =  std::unique_ptr<Expr>;

class StmtVisitor {
public:
    virtual void visitBlockStmt(BlockStmt&) =0;
    virtual void visitExpressionStmt(ExpressionStmt&) =0;
    virtual void visitIfStmt(IfStmt&) =0;
    virtual void visitPrintStmt(PrintStmt&) =0;
    virtual void visitVarStmt(VarStmt&) =0;
};

class Stmt {
public:
    virtual void accept(StmtVisitor&) = 0;
    virtual std::string typeName() const { return "Stmt"; }
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<PStmt>&& _statements) {
        statements = std::move(_statements);
    }

    void accept(StmtVisitor& v) override {
        v.visitBlockStmt(*this);
    }
    std::vector<PStmt> statements;

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

class IfStmt : public Stmt {
public:
    IfStmt(PExpr&& _condition, PStmt&& _thenBranch, 
            PStmt&& _elseBranch) {
        condition = std::move(_condition);
        thenBranch  = std::move(_thenBranch);
        elseBranch = std::move(_elseBranch);
    }

    void accept(StmtVisitor& v) override {
        v.visitIfStmt(*this);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

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
