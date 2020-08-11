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
using PFunc = std::shared_ptr<FunctionStmt>;
class StmtVisitor {
public:
    virtual void visitBlockStmt(BlockStmt&) =0;
    virtual void visitClassStmt(ClassStmt&) =0;
    virtual void visitBreakStmt(BreakStmt&) =0;
    virtual void visitExpressionStmt(ExpressionStmt&) =0;
    virtual void visitFunctionStmt(FunctionStmt&) =0;
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
    BlockStmt(std::vector<PStmt> _statements) {
        statements = std::move(_statements);
    }

    void accept(StmtVisitor& v) override {
        v.visitBlockStmt(*this);
    }
    std::vector<PStmt> statements;

};

class ClassStmt : public Stmt {
public:
    ClassStmt(TokPtr name, std::shared_ptr<VariableExpr> superclass, std::vector<PFunc> methods) {
      m_name = name;  
      m_superclass = std::move(superclass);
      m_methods = std::move(methods);
    }

    void accept(StmtVisitor& v) override {
        v.visitClassStmt(*this);
    }
    TokPtr m_name;
    std::shared_ptr<VariableExpr> m_superclass;
    std::vector<PFunc> m_methods;

};


class BreakStmt : public Stmt {
public:
    BreakStmt(TokPtr& keyword) 
      : m_keyword(keyword) {}

    void accept(StmtVisitor& v) override {
        v.visitBreakStmt(*this);
    }
    TokPtr m_keyword;

};


class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(std::shared_ptr<Expr> _expr) {
        expression = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitExpressionStmt(*this);
    }

    std::shared_ptr<Expr> expression;
};

class IfStmt : public Stmt {
public:
    IfStmt(PExpr _condition, PStmt _thenBranch, 
            PStmt _elseBranch) {
        condition = std::move(_condition);
        thenBranch  = std::move(_thenBranch);
        elseBranch = std::move(_elseBranch);
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
    FunctionStmt(Token _name, std::vector<Token> _params, std::vector<PStmt> _body) {
        name = _name;
        params = std::move(_params);
        body  = std::move(_body);
    }

    
    // Note: to prevent deleting pointer object by user, you can use the delete operator
    // void operator delete(void *) = delete;
    // and to prevent the auto destructor for an object:
    // you can not defining a destructor: ~object();
    // or your can make the destructor private,
    // or in c++11, you can tell to the compiler do not provide a destructor deleted: ~object() = delete;
    // or make a "union" between base and derived object...
    // or create a smart pointer to this object.

    ~FunctionStmt() { 
    }
    
    void accept(StmtVisitor& v) override {
        v.visitFunctionStmt(*this);
    }
    Token name;
    std::vector<Token> params;
    std::vector<PStmt> body;

};


class PrintStmt : public Stmt {
public:
    PrintStmt(std::shared_ptr<Expr> _expr) {
        expression = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitPrintStmt(*this);
    }

    std::shared_ptr<Expr> expression;

};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(TokPtr _name, std::shared_ptr<Expr> _expr) {
        name = _name;
        value = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitReturnStmt(*this);
    }
    TokPtr name;
    std::shared_ptr<Expr> value;

};


class VarStmt : public Stmt {
public:
    VarStmt(TokPtr _name, std::shared_ptr<Expr> _expr) {
        name = _name;
        initializer = std::move(_expr);
    }

    void accept(StmtVisitor& v) override {
        v.visitVarStmt(*this);
    }
    TokPtr name;
    std::shared_ptr<Expr> initializer;

};

class WhileStmt : public Stmt {
public:
    WhileStmt(PExpr _condition, PStmt _body) { 
        condition = std::move(_condition);
        body  = std::move(_body);
    }

    void accept(StmtVisitor& v) override {
        v.visitWhileStmt(*this);
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};


#endif // STMT_HPP
