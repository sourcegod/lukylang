#ifndef COMMON_HPP
#define COMMON_HPP
#include <memory> // for smart pointers
class Expr;
class Stmt;
class LukObject;
class Token;
class Environment;
class FunctionStmt;

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;
using ObjPtr = std::shared_ptr<LukObject>;
using TokPtr = std::shared_ptr<Token>;
using PEnvironment = std::shared_ptr<Environment>;
using FuncPtr = std::shared_ptr<FunctionStmt>;

#endif // COMMON_HPP
