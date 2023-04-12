#ifndef EXPR_HPP
#define EXPR_HPP
#include "common.hpp"
#include "lukobject.hpp"
#include "token.hpp"
#include <memory>
#include <vector>
#include <map>

namespace luky {
    // forward declarations
    class AssignExpr;
    class BinaryExpr;
    class CallExpr;
    class FunctionExpr;
    class GetExpr;
    class GroupingExpr;
    class InterpolateExpr;
    class LiteralExpr;
    class LogicalExpr;
    class SetExpr;
    class SuperExpr;
    class TernaryExpr;
    class ThisExpr;
    class UnaryExpr;
    class VariableExpr;

    // using ExprPtr = std::shared_ptr<Expr>;
    // using StmtPtr = std::shared_ptr<Stmt>;

    // create visitor object
    class ExprVisitor {
        public:
            virtual ObjPtr visitAssignExpr(AssignExpr&) =0;
            virtual ObjPtr visitBinaryExpr(BinaryExpr&) =0;
            virtual ObjPtr visitCallExpr(CallExpr&) =0;
            virtual ObjPtr visitFunctionExpr(FunctionExpr&) =0;
            virtual ObjPtr visitGetExpr(GetExpr&) =0;
            virtual ObjPtr visitGroupingExpr(GroupingExpr&) =0;
            virtual ObjPtr visitInterpolateExpr(InterpolateExpr&) =0;
            virtual ObjPtr visitLiteralExpr(LiteralExpr&) =0;
            virtual ObjPtr visitLogicalExpr(LogicalExpr&) =0;
            virtual ObjPtr visitSetExpr(SetExpr&) =0;
            virtual ObjPtr visitSuperExpr(SuperExpr&) =0;
            virtual ObjPtr visitTernaryExpr(TernaryExpr&) =0;
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
        virtual bool isAssignExpr() const { return false; }
        virtual bool isCallExpr() const { return false; }
        virtual bool isFunctionExpr() const { return false; }
        virtual bool isGetExpr() const { return false; }
        virtual bool isSetExpr() const { return false; }
        virtual bool isVariableExpr() const { return false; }
        // Note: the two folowing virtual func must be implemented
        // in callexpr, variableexpr, getexpr, setexpr objects.
        virtual std::string typeName() const { return "Expr"; }
        // TODO: will better to returns static TokPtr
        virtual TokPtr getName() const { return std::make_shared<Token>(); }
        virtual ExprPtr getObject() const { return nullptr; }

        virtual unsigned id() const { return m_id; }

    private:
      unsigned m_id;

    };

    // differents objects

    class AssignExpr : public Expr {
    public:
        AssignExpr(TokPtr& name, TokPtr& equals, ExprPtr value) : 
            m_name(name),
            m_equals(equals),
            m_value(std::move(value))
            {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitAssignExpr(*this); 
        }
        bool isAssignExpr() const override { return true; }
        std::string typeName() const override { return "AssignExpr"; }
        TokPtr getName() const override { return m_name; }
        ExprPtr getObject() const override { return m_value; }


        TokPtr m_name;
        TokPtr m_equals;
        ExprPtr m_value;
    };


    class BinaryExpr : public Expr {
    public:
        BinaryExpr(ExprPtr& left, TokPtr& op, ExprPtr& right) :
            m_left(std::move(left)),
            m_op(op),
            m_right(std::move(right))
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitBinaryExpr(*this); 
        }

        ExprPtr m_left;
        TokPtr m_op;
        ExprPtr m_right;
    };

    class CallExpr : public Expr {
    public:
        CallExpr(ExprPtr callee, TokPtr& paren, std::vector<ExprPtr> args, std::map<TokPtr, ExprPtr> m_keywords) :
            m_callee(std::move(callee)),
            m_paren(paren),
            m_args(std::move(args)),
            m_keywords(std::move(m_keywords))
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitCallExpr(*this); 
        }

        bool isCallExpr() const override { return true; }
        std::string typeName() const override { return "CallExpr"; }
        TokPtr getName() const override { return m_paren; }

        ExprPtr m_callee;
        TokPtr m_paren;
        std::vector<ExprPtr> m_args;
        std::map<TokPtr, ExprPtr> m_keywords;
    };

    class FunctionExpr : public Expr {
    public:
        FunctionExpr(std::vector<TokPtr>& params, std::vector<StmtPtr>& body) :
            m_params(std::move(params)),
            m_body(std::move(body))
        {}
        

        ObjPtr accept(ExprVisitor& v) override {
            return v.visitFunctionExpr(*this);
        }
        
        std::vector<TokPtr> m_params;
        std::vector<StmtPtr> m_body;

    };


    class GetExpr : public Expr {
    public:
        GetExpr(ExprPtr object, TokPtr& name) :
          m_object(std::move(object)),
          m_name(name) 
        {}
        
        bool isGetExpr() const override { return true; }
        std::string typeName() const override { return "GetExpr"; }
        TokPtr getName() const override { return m_name; }
        ExprPtr getObject() const override { return m_object; }


        ObjPtr accept(ExprVisitor &v) override {
            return v.visitGetExpr(*this); 
        }

        ExprPtr m_object;
        TokPtr m_name;
    };

    class GroupingExpr : public Expr {
    public:
        GroupingExpr(ExprPtr& expr) :
            m_expression(std::move(expr))
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitGroupingExpr(*this); 
        }

        ExprPtr m_expression;
    };

    class InterpolateExpr : public Expr {
    public:
        InterpolateExpr(std::vector<ExprPtr> args) :
            m_args(std::move(args))
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitInterpolateExpr(*this); 
        }


        std::vector<ExprPtr> m_args;
    };


    class LiteralExpr: public Expr {
    public:
        LiteralExpr(ObjPtr& value) :
            m_value(value) {
            logMsg("\nLiteralExpr constructor");
            logMsg("value->id: ", value->id);
        }

        ~LiteralExpr() {
            logMsg("~LiteralExpr destructor");
        }

        ObjPtr accept(ExprVisitor &v) override {
            return v.visitLiteralExpr(*this); 
        }

        ObjPtr m_value;
    };

    class LogicalExpr : public Expr {
    public:
        LogicalExpr(ExprPtr& left, TokPtr& op, ExprPtr& right) :
            m_left(std::move(left)),
            m_op(op),
            m_right(std::move(right))
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitLogicalExpr(*this); 
        }

        ExprPtr m_left;
        TokPtr m_op;
        ExprPtr m_right;
    };

    class SetExpr : public Expr {
    public:
        SetExpr(ExprPtr object, TokPtr name, ExprPtr value) :
          m_object(std::move(object)),
          m_name(name),
          m_value(std::move(value)) 
        {}

        bool isSetExpr() const override { return true; }
        std::string typeName() const override { return "SetExpr"; }
        TokPtr getName() const override { return m_name; }
        // Fix: can now an instance of shared_ptr instead unique_ptr
        ExprPtr getObject() const override { return m_object; }

        ObjPtr accept(ExprVisitor &v) override {
            return v.visitSetExpr(*this); 
        }

        ExprPtr m_object;
        TokPtr m_name;
        ExprPtr m_value;
    };

    class SuperExpr : public Expr {
    public:
        SuperExpr(TokPtr& keyword, TokPtr& method) :
          m_keyword(keyword),
          m_method(method) 
        {}

        ObjPtr accept(ExprVisitor &v) override {
            return v.visitSuperExpr(*this); 
        }

        TokPtr m_keyword;
        TokPtr m_method;
    };

    class TernaryExpr : public Expr {
    public:
        TernaryExpr(ExprPtr& condition, ExprPtr& thenBranch, ExprPtr& elseBranch) :
            m_condition(std::move(condition)),
            m_thenBranch(std::move(thenBranch)),
            m_elseBranch(std::move(elseBranch))
        {}
        
        ObjPtr accept(ExprVisitor& v) override {
            return v.visitTernaryExpr(*this); 
        }

        ExprPtr m_condition;
        ExprPtr m_thenBranch;
        ExprPtr m_elseBranch;

    };

    class ThisExpr : public Expr {
    public:
        ThisExpr(TokPtr& keyword) :
          m_keyword(keyword) 
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitThisExpr(*this); 
        }

        TokPtr m_keyword;
    };

    class UnaryExpr : public Expr {
    public:
        UnaryExpr(TokPtr& op, ExprPtr& right, bool isPostfix) :
            m_op(op),
            m_right(std::move(right)),
            m_isPostfix(isPostfix) {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitUnaryExpr(*this); 
        }

        TokPtr m_op;
        ExprPtr m_right;
        bool m_isPostfix;
    };

    class VariableExpr : public Expr {
    public:
        VariableExpr(TokPtr& name) :
            m_name(name)
        {}
        
        ObjPtr accept(ExprVisitor &v) override {
            return v.visitVariableExpr(*this); 
        }
        
        bool isVariableExpr() const override { return true; }
        std::string typeName() const override { return "VariableExpr"; }
        TokPtr getName() const override { return m_name; }

        TokPtr m_name;
    };
}


#endif // EXPR_HPP
