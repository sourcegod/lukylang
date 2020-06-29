#ifndef STMT_HPP
#define STMT_HPP

#include "expr.hpp"
#include "lukobject.hpp"
#include "token.hpp"
#include <memory> // smart pointer
#include <vector>


// forward declarations
class BlockStmt;
class BreakStmt;
class ClassStmt;
class Expr;
class ExpressionStmt;
class FunctionStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class Stmt;
class VarStmt;
class WhileStmt;

using PStmt = std::shared_ptr<Stmt>;
class StmtVisitor {
public:
    virtual void visitBlockStmt(BlockStmt&) =0;
    virtual void visitClassStmt(ClassStmt&) =0;
    virtual void visitBreakStmt(BreakStmt&) =0;
    virtual void visitExpressionStmt(ExpressionStmt&) =0;
    virtual void visitFunctionStmt(FunctionStmt*) =0;
    virtual void visitIfStmt(IfStmt&) =0;
    virtual void visitPrintStmt(PrintStmt&) =0;
    virtual void visitReturnStmt(ReturnStmt&) =0;
    virtual void visitVarStmt(VarStmt&) =0;
    virtual void visitWhileStmt(WhileStmt&) =0;
};

class Stmt {
public:
    virtual void accept(StmtVisitor&) = 0;
    virtual std::string typeName() const { return "Stmt"; }
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<PStmt>&& _statements) {
        statements = (_statements);
    }

    void accept(StmtVisitor& v) override {
        v.visitBlockStmt(*this);
    }
    std::vector<PStmt> statements;

};

class ClassStmt : public Stmt {
public:
    ClassStmt(Token name, std::vector<PStmt>&& methods) {
      m_name = name;  
      m_methods = (methods);
    }

    void accept(StmtVisitor& v) override {
        v.visitClassStmt(*this);
    }
    Token m_name;
    std::vector<PStmt> m_methods;

};


class BreakStmt : public Stmt {
public:
    BreakStmt(Token _keyword) {
        keyword = _keyword;
    }

    void accept(StmtVisitor& v) override {
        v.visitBreakStmt(*this);
    }
    Token keyword;

};


class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(std::shared_ptr<Expr>&& _expr) {
        expression = (_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitExpressionStmt(*this);
    }

    std::shared_ptr<Expr> expression;
};

class IfStmt : public Stmt {
public:
    IfStmt(PExpr&& _condition, PStmt&& _thenBranch, 
            PStmt&& _elseBranch) {
        condition = (_condition);
        thenBranch  = (_thenBranch);
        elseBranch = (_elseBranch);
    }

    void accept(StmtVisitor& v) override {
        v.visitIfStmt(*this);
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;

};

class FunctionStmt : public Stmt {
public:
    FunctionStmt() {}
    FunctionStmt(Token _name, std::vector<Token>&& _params, std::vector<PStmt>&& _body) {
        name = _name;
        params = (_params);
        body  = (_body);
    }

    void accept(StmtVisitor& v) override {
        v.visitFunctionStmt(this);
    }
    Token name;
    std::vector<Token> params;
    std::vector<PStmt> body;

};


class PrintStmt : public Stmt {
public:
    PrintStmt(std::shared_ptr<Expr>&& _expr) {
        expression = (_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitPrintStmt(*this);
    }

    std::shared_ptr<Expr> expression;

};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Token _name, std::shared_ptr<Expr>&& _expr) {
        name = _name;
        value = (_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitReturnStmt(*this);
    }
    Token name;
    std::shared_ptr<Expr> value;

};


class VarStmt : public Stmt {
public:
    VarStmt(Token _name, std::shared_ptr<Expr>&& _expr) {
        name = _name;
        initializer = (_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitVarStmt(*this);
    }
    Token name;
    std::shared_ptr<Expr> initializer;

};

class WhileStmt : public Stmt {
public:
    WhileStmt(PExpr&& _condition, PStmt&& _body) { 
        condition = (_condition);
        body  = (_body);
    }

    void accept(StmtVisitor& v) override {
        v.visitWhileStmt(*this);
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};


#endif // STMT_HPP
