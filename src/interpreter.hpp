#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "common.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "lukerror.hpp"

#include <string>
#include <vector>
#include <unordered_map>
namespace luky {
    class Interpreter : public ExprVisitor,  public StmtVisitor {
    public:
        EnvPtr m_globals;
        LukError& m_lukErr;

        Interpreter(LukError& lukErr);
        ~Interpreter() { 
          logMsg("\n~Interpreter destructor\n");
        }

        void interpret(std::vector<std::shared_ptr<Stmt>> statements);
        void printResult();
        void logState();
        void logTest();

        ObjPtr evaluate(ExprPtr expr);
        void execute(StmtPtr& stmt);
       
        // expressions
        ObjPtr visitAssignExpr(AssignExpr& expr) override;
        ObjPtr visitBinaryExpr(BinaryExpr& expr) override;
        ObjPtr visitCallExpr(CallExpr& expr) override;
        ObjPtr visitFunctionExpr(FunctionExpr& expr);
        ObjPtr visitGetExpr(GetExpr& expr);
        ObjPtr visitGroupingExpr(GroupingExpr& expr) override;
        ObjPtr visitInterpolateExpr(InterpolateExpr& expr);
        ObjPtr visitLiteralExpr(LiteralExpr& expr) override; 
        ObjPtr visitLogicalExpr(LogicalExpr& expr) override;
        ObjPtr visitSetExpr(SetExpr& expr);
        ObjPtr visitSuperExpr(SuperExpr& expr);
        ObjPtr visitTernaryExpr(TernaryExpr& expr);
        ObjPtr visitThisExpr(ThisExpr& expr);
        ObjPtr visitUnaryExpr(UnaryExpr& expr) override;
        ObjPtr visitVariableExpr(VariableExpr& expr) override;

        void resolve(Expr& expr, int depth);
        void executeBlock(std::vector<StmtPtr>& statements, EnvPtr env);
        //
        // statements    
        void visitBlockStmt(BlockStmt& stmt) override;
        void visitBreakStmt(BreakStmt& stmt) override;
        void visitClassStmt(ClassStmt& stmt) override;
        void visitExpressionStmt(ExpressionStmt&) override;
        void visitFunctionStmt(FunctionStmt& stmt) override;
        void visitIfStmt(IfStmt& stmt) override;
        void visitPrintStmt(PrintStmt&) override;
        void visitReturnStmt(ReturnStmt& stmt) override;
        void visitVarStmt(VarStmt& stmt) override;
        void visitWhileStmt(WhileStmt& stmt) override;
     
    private:
        EnvPtr m_env;
        ObjPtr m_result;
        const std::string m_errTitle = "InterpretError: ";
        std::unordered_map<unsigned, int> m_locals;

        bool isTruthy(ObjPtr& obj);
        bool isEqual(ObjPtr& a, ObjPtr& b);
        void checkNumberOperand(TokPtr& op, ObjPtr& operand);
        void checkNumberOperands(TokPtr& op, ObjPtr& left, ObjPtr& right);
        ObjPtr lookUpVariable(TokPtr& name, Expr& expr);

        // starts and ends for string
        inline bool startsWith(const std::string& str, const std::string& start) {
            if (start.size() > str.size()) return false;
            return str.compare(0, start.size(), start) == 0;
        }

        inline bool endsWith(const std::string& str, const std::string& end) {
            if (end.size() > str.size()) return false;
            return std::equal(end.rbegin(), end.rend(), str.rbegin());
        }

        
        std::string format(ObjPtr& obj);
        std::string multiplyString(const std::string& str, const int num);
        std::string stringify(ObjPtr& obj);

    };
}

#endif // INTERPRETER_HPP
