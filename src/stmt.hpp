#ifndef STMT_HPP
#define STMT_HPP
#include "common.hpp"
#include "expr.hpp"
#include "lukobject.hpp"
#include "token.hpp"
#include <memory> // smart pointer
#include <vector>


// forward declarations
class BlockStmt;
class BreakStmt;
class ClassStmt;
class ExpressionStmt;
class FunctionStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class VarStmt;
class WhileStmt;

// using StmtPtr = std::shared_ptr<Stmt>;
// using FuncPtr = std::shared_ptr<FunctionStmt>;

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
    BlockStmt(std::vector<StmtPtr>&& statements) :
        m_statements(std::move(statements))
    {}

    void accept(StmtVisitor& v) override {
        v.visitBlockStmt(*this);
    }
    std::vector<StmtPtr> m_statements;

};

class ClassStmt : public Stmt {
public:
    ClassStmt(TokPtr& name, std::shared_ptr<VariableExpr>& superclass, std::vector<FuncPtr>&& methods) :
      m_name(name),
      m_superclass(std::move(superclass)),
      m_methods(std::move(methods))
    {}

    void accept(StmtVisitor& v) override {
        v.visitClassStmt(*this);
    }
    TokPtr m_name;
    std::shared_ptr<VariableExpr> m_superclass;
    std::vector<FuncPtr> m_methods;

};


class BreakStmt : public Stmt {
public:
    BreakStmt(TokPtr& keyword)  :
      m_keyword(keyword) 
    {}

    void accept(StmtVisitor& v) override {
        v.visitBreakStmt(*this);
    }
    
    TokPtr m_keyword;
};


class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(ExprPtr expr) :
        m_expression(std::move(expr))
    {}

    void accept(StmtVisitor& v) override {
        v.visitExpressionStmt(*this);
    }

    ExprPtr m_expression;
};

class FunctionStmt : public Stmt {
public:
    FunctionStmt() {}
    FunctionStmt(TokPtr& name, std::shared_ptr<FunctionExpr> function) :
        m_name(name),
        m_function(function)
    {}

    
    // Note: to prevent deleting pointer object by user, you can use the delete operator
    // void operator delete(void *) = delete;
    // and to prevent the auto destructor for an object:
    // you can not defining a destructor: ~object();
    // or your can make the destructor private,
    // or in c++11, you can tell to the compiler do not provide a destructor deleted: ~object() = delete;
    // or make a "union" between base and derived object...
    // or create a smart pointer to this object.

    ~FunctionStmt() {}
    
    void accept(StmtVisitor& v) override {
        v.visitFunctionStmt(*this);
    }
    
    TokPtr m_name;
    std::shared_ptr<FunctionExpr> m_function;
};


class IfStmt : public Stmt {
public:
    IfStmt(ExprPtr condition, StmtPtr thenBranch, 
            StmtPtr elseBranch) :
        m_condition(std::move(condition)),
        m_thenBranch(std::move(thenBranch)),
        m_elseBranch(std::move(elseBranch))
    {}

    void accept(StmtVisitor& v) override {
        v.visitIfStmt(*this);
    }

    ExprPtr m_condition;
    StmtPtr m_thenBranch;
    StmtPtr m_elseBranch;

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
    WhileStmt(ExprPtr _condition, StmtPtr _body) { 
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
