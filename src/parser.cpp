# include "parser.hpp"
#include "lukerror.hpp"

#include <vector>
#include <typeinfo>
#include <memory>
#include <map>
using namespace luky;

ParseError::ParseError(const std::string& msg, TokPtr& tokP)
    : std::runtime_error(msg)
    , m_token(tokP) {}

Parser::Parser(const std::vector<TokPtr>&& tokens, LukError& _lukErr)
      : m_current(0),
      m_tokens(std::move(tokens)),
      lukErr(_lukErr) {
    logMsg("\nIn Parser constructor");
}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    try {
        while (!isAtEnd()) {
        statements.emplace_back(declaration() );
    }
    // Note: catching exception should be by reference, not by value
    } catch(ParseError& err) {
            std::cerr << errTitle << err.what() << std::endl;
    }

        
    return statements;
    
}

StmtPtr Parser::statement() {
    // manage semicolon with empty statement
    if (match({TokenType::SEMICOLON})) return expressionStatement();
     
    if (match({TokenType::BREAK, TokenType::CONTINUE})) 
        return breakStatement();
    if (match({TokenType::DO})) 
        return doStatement();
    if (match({TokenType::FOR})) 
        return forStatement();
    if (match({TokenType::IF})) 
        return ifStatement();
    if (match({TokenType::PRINT})) 
        return printStatement();
    if (match({TokenType::RETURN})) 
        return returnStatement();
    if (match({TokenType::WHILE})) 
        return whileStatement();
    if (match({TokenType::LEFT_BRACE}))
        return std::make_shared<BlockStmt>( block() );
    
    return  expressionStatement();
}

std::vector<StmtPtr> Parser::block() {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back( declaration() );
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return statements;
}


StmtPtr Parser::breakStatement() {
    TokPtr keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after break statement");
    return std::make_shared<BreakStmt>(keyword);
}

StmtPtr Parser::doStatement() {
    StmtPtr body = statement();
    consume(TokenType::WHILE, "Expect 'while' after 'do' body");
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");
    checkEndLine("Expect ';' after value.", true);

    return std::make_shared<WhileStmt>(condition, body, false);
}

StmtPtr Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'");
    StmtPtr initializer; 
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    
    StmtPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = std::make_shared<ExpressionStmt>(expression() );
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    StmtPtr body = statement();

    if (increment != nullptr) {
        std::vector<StmtPtr> stmts;
        stmts.push_back(body);
        stmts.push_back(increment);
        body = std::make_shared<BlockStmt>( std::move(stmts) );
    }
    body = std::make_shared<WhileStmt>(condition, body, true);
    if (initializer) {
        std::vector<StmtPtr> stmts;
        stmts.push_back( initializer );
        stmts.push_back( body );
        return std::make_shared<BlockStmt>( std::move(stmts) );
    }

    return body;
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");
    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch,
                elseBranch);
}

StmtPtr Parser::printStatement() {
    /// Adding multiple arguments to the print statement
    std::vector<ExprPtr> v_args;
    // ExprPtr value = expression();
    do {
        /// calling assignment instead expression function to avoid comma operator
        v_args.emplace_back(assignment());
    } while (match({TokenType::COMMA}));
    // consume(TokenType::SEMICOLON, "Expect ';' after value.");
    // No require semicolon
    // checking whether not end line for automatic semicolon insertion
    checkEndLine("Expect ';' after value.", true);

    return std::make_shared<PrintStmt>(std::move(v_args));
}

StmtPtr Parser::returnStatement() {
    TokPtr keyword = previous();
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");

    return std::make_shared<ReturnStmt>(keyword, value);
}


StmtPtr Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");
    StmtPtr body = statement();

    return std::make_shared<WhileStmt>(condition, body, true);
}

std::vector<std::pair<TokPtr, ExprPtr>> Parser::multiVars() {
    std::vector<std::pair<TokPtr, ExprPtr>> v_vars;
    TokPtr name;
    ExprPtr initializer;
    do {
        do {
            if (m_isFuncBody) m_isFuncBody = false;
            name = consume(TokenType::IDENTIFIER, "Expect variable name.");
            initializer = nullptr;
            if (match({TokenType::EQUAL})) {
                // we do not call expression function to avoid comma operator
                initializer = assignment();
            }
            // v_vars.emplace_back(std::make_pair(name, initializer));
            /// Note: we can also pass an initializer list to push_back function
            v_vars.push_back({name, initializer});
        } while (match({TokenType::COMMA}));
        // consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        // checking end line whether is a function or simple variable for automatic semicolon insertion
        if (m_isFuncBody) checkEndLine("", false);
        else checkEndLine("Expect ';' after variable declaration.", true);
        m_isFuncBody = false;
        
    } while (match({TokenType::VAR}));
        
    return std::move(v_vars);
}

StmtPtr Parser::varDeclaration() {
    std::vector<std::pair<TokPtr, ExprPtr>> v_vars;
    TokPtr name;
    ExprPtr initializer;
    do {
        if (m_isFuncBody) m_isFuncBody = false;
        name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        initializer = nullptr;
        if (match({TokenType::EQUAL})) {
            // we do not call expression function to avoid comma operator
            initializer = assignment();
        }
        // v_vars.emplace_back(std::make_pair(name, initializer));
        /// Note: we can also pass an initializer list to push_back function
        v_vars.push_back({name, initializer});
    } while (match({TokenType::COMMA}));
    // consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    // checking end line whether is a function or simple variable for automatic semicolon insertion
    if (m_isFuncBody) checkEndLine("", false);
    else checkEndLine("Expect ';' after variable declaration.", true);
    m_isFuncBody = false;
    
    return std::make_shared<VarStmt>(std::move(v_vars));
}

StmtPtr Parser::classDeclaration() {
    TokPtr name = consume(TokenType::IDENTIFIER, "Expect class name.");
    std::shared_ptr<VariableExpr> superclass = nullptr;
    if (match({TokenType::LESSER})) {
      consume(TokenType::IDENTIFIER, "Expect superclass name.");
      superclass = std::make_shared<VariableExpr>(previous());
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::pair<TokPtr, ExprPtr>>  v_vars;
    if (match({TokenType::VAR})) {
        v_vars = multiVars();
    }

    std::vector<FuncPtr> methods;
    // adding meta class
    std::vector<FuncPtr> classMethods;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        /// Note: good tip using ternary expression
        bool isClassMethod = match({TokenType::CLASS});
        (isClassMethod ? classMethods : methods).push_back( std::move(function("method")) );
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
  
    return std::make_shared<ClassStmt>(name, superclass, std::move(v_vars),
        std::move(methods), std::move(classMethods) );
}

StmtPtr Parser::declaration() {
    try {
        if  (match({TokenType::CLASS})) return classDeclaration();
        if ( check(TokenType::FUN) && checkNext(TokenType::IDENTIFIER)) {
          consume(TokenType::FUN, "");  
          return function("function");
        }
        
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (ParseError& err) {
        synchronize();
        return nullptr;
    }

}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    // consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    // checking whether not end line for automatic semicolon insertion
    checkEndLine("Expect ';' after expression.", true);

    return std::make_shared<ExpressionStmt>(expr);
}

FuncPtr Parser::function(const std::string& kind) {
    TokPtr name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    return std::make_shared<FunctionStmt>(name, functionBody(kind));
}

std::shared_ptr<FunctionExpr> Parser::functionBody(const std::string& kind) {
    m_isFuncBody = true;
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<TokPtr> params;
    std::vector<StmtPtr> body;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 8) {
                error(peek(), "Cannot have more than 8 parameters.");
            }
            
            params.emplace_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    // Adding function arrow expression
    if (match({TokenType::EQUAL_ARROW})) {
      auto keyword = previous();
      ExprPtr value = expression();
      // checking whether not end line for automatic semicolon insertion
      checkEndLine("Expect ';' after value.", true);

      auto retStmt = std::make_shared<ReturnStmt>(keyword, value);
      body.emplace_back( retStmt );
 
      return std::make_shared<FunctionExpr>(params, body);

    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    // std::vector<StmtPtr> body = block();
    body = block();

    return std::make_shared<FunctionExpr>(params, body);
}

ExprPtr Parser::expression() {
    return comma();
}

ExprPtr Parser::comma() {
    ExprPtr left = assignment();
    while (match({TokenType::COMMA})) {
        TokPtr op = previous();
        ExprPtr right = assignment();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}


ExprPtr Parser::assignment() {
    ExprPtr left = conditional();
    if (match({TokenType::EQUAL, // })) {
          TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL, 
          TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, 
          TokenType::MOD_EQUAL, TokenType::EXP_EQUAL,
          TokenType::BIT_AND_EQUAL, TokenType::BIT_OR_EQUAL, 
          TokenType::BIT_XOR_EQUAL,
          TokenType::BIT_LEFT_EQUAL,
          TokenType::BIT_RIGHT_EQUAL})) {

        TokPtr equals = previous();
        ExprPtr value = assignment();
        if ( left->isVariableExpr() ) {
            // TokPtr name = static_cast<VariableExpr*>( left.get() )->name;
            TokPtr name = left->getName();
            return  std::make_shared<AssignExpr>(name, equals, value);
        } else if (left->isGetExpr()) {
          return std::make_shared<SetExpr>(left->getObject(),
                left->getName(), value );
        }
        
        error(equals, "Invalid assignment target.");
    }

    return left;
}

ExprPtr Parser::compoundAssignment(ExprPtr left, TokPtr op) {
  // deprecated function, not used yet

    ExprPtr value = addition();
    if (left->isVariableExpr()) {
        TokPtr name = left->getName();
        ExprPtr val = std::make_shared<BinaryExpr>(left, op, value);
        return std::make_shared<AssignExpr>(name, op, val);
    }

    error(op, "Invalid compound assignment target.");

    return left;
}

ExprPtr Parser::conditional() {
      ExprPtr expr = logicOr();
      if (match({TokenType::QUESTION})) {
          ExprPtr thenBranch = expression();
          consume(TokenType::COLON, 
                  "Expect ':' after then branch of conditional expression.");
          ExprPtr elseBranch = conditional();
          return std::make_shared<TernaryExpr>(expr, thenBranch, elseBranch);
      }

      return expr;
}

ExprPtr Parser::logicOr() {
    ExprPtr left = logicAnd();
    while (match({TokenType::OR})) {
        TokPtr op = previous();
        ExprPtr right = logicAnd();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::logicAnd() {
    ExprPtr left = bitwiseOr();
    while (match({TokenType::AND})) {
        TokPtr op = previous();
        ExprPtr right = bitwiseOr();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::bitwiseOr() {
    ExprPtr left = bitwiseXor();
    while (match({TokenType::BIT_OR})) {
        TokPtr op = previous();
        ExprPtr right = bitwiseXor();
        left =  std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::bitwiseXor() {
    ExprPtr left = bitwiseAnd();
    while (match({TokenType::BIT_XOR})) {
        TokPtr op = previous();
        ExprPtr right = bitwiseAnd();
        left =  std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::bitwiseAnd() {
    ExprPtr left = equality();
    while (match({TokenType::BIT_AND})) {
        TokPtr op = previous();
        ExprPtr right = equality();
        left =  std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::equality() {
    ExprPtr left = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        TokPtr op = previous();
        ExprPtr right = comparison();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::comparison() {
    ExprPtr left = bitwiseShift();
    while (
        match({TokenType::GREATER, TokenType::LESSER, 
            TokenType::LESSER_EQUAL, TokenType::GREATER_EQUAL})) {
        TokPtr op = previous();
        ExprPtr right = bitwiseShift();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::bitwiseShift() {
    ExprPtr left = addition();
    while ( match({TokenType::BIT_LEFT, TokenType::BIT_RIGHT}) ) {
        TokPtr op = previous();
        ExprPtr right = addition();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::addition() {
    ExprPtr left = multiplication();
    
    // String Interpolation
    std::vector<ExprPtr> v_args;
    if (match({TokenType::INTERP_PLUS})) { 
        v_args.emplace_back(left);
        do {
            v_args.emplace_back(expression());
        } while (match({TokenType::INTERP_PLUS}));
        
        return std::make_shared<InterpolateExpr>(std::move(v_args));
    }

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        TokPtr op = previous();
        ExprPtr right = multiplication();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::multiplication() {
    ExprPtr left = unary();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::MOD})) {
        // Note: cannot use "operator" as variable name, cause it's a reserved keyword in C++
        TokPtr op = previous();
        ExprPtr right = unary();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS, 
          TokenType::PLUS, TokenType::BIT_NOT})) {
        TokPtr op = previous();
        ExprPtr right = unary();

        return std::make_shared<UnaryExpr>(op, right, false);
    }

    return exponentiation();
}

ExprPtr Parser::exponentiation() {
    ExprPtr left = prefix();
    while (match({TokenType::EXP})) {
        TokPtr op = previous();
        ExprPtr right = unary();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::prefix() {
    if (match({TokenType::MINUS_MINUS, TokenType::PLUS_PLUS})) {
        TokPtr op = previous();
        ExprPtr right = primary();
    
        return std::make_shared<UnaryExpr>(op, right, false);
    }

    return postfix();
}

ExprPtr Parser::postfix() {
    if (matchNext({TokenType::MINUS_MINUS, TokenType::PLUS_PLUS})) {
        TokPtr op = peek();
        m_current--;
        ExprPtr left = primary();
        advance();
        
        return std::make_shared<UnaryExpr>(op, left, true);
    }

    return call();
}

ExprPtr Parser::call() {
    ExprPtr expr = primary();
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
          TokPtr name = consume(TokenType::IDENTIFIER,
            "Expect property name after '.'.");
          expr = std::make_shared<GetExpr>(expr, name);
        } else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
    std::vector<ExprPtr> v_args;
    std::vector<ExprPtr> v_keywords;
    std::map<TokPtr, ExprPtr> mapKeywords; 
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (v_args.size() >= 32) {
                error(peek(), "Cannot have more than 32 arguments.");
            }
            
            /// Note: calling assignment function rather than expression to avoid the comma operator
            auto expr = assignment();
            if (expr->isAssignExpr()) {
              // std::cerr << "AssignExpr\n";
              // The AssignExpr->getobject function, returns m_value
              mapKeywords[expr->getName()] = expr->getObject();
              continue;
            } else { 
                // std::cerr << "Expr: \n";
                v_args.emplace_back(expr);
            }
        } while (match({TokenType::COMMA}));
    }

    TokPtr paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<CallExpr>(callee, paren, v_args, mapKeywords);
}

ExprPtr Parser::primary() {
    ObjPtr objP = nullptr;
    if (match( {TokenType::NIL})) 
        objP = std::make_shared<LukObject>();
    else if (match( {TokenType::FALSE})) 
        objP = std::make_shared<LukObject>( false );
    else if (match( {TokenType::TRUE})) 
        objP = std::make_shared<LukObject>( true );
    else if (match( {TokenType::INT})) 
        objP = std::make_shared<LukObject>(std::stol( previous()->literal ));
    else if (match( {TokenType::NUMBER, TokenType::DOUBLE})) 
        objP = std::make_shared<LukObject>(std::stod( previous()->literal ));
    else if (match( {TokenType::STRING})) 
        objP = std::make_shared<LukObject>(previous()->literal);
        
    /*
    else if (match(
                {TokenType::FALSE, TokenType::TRUE, 
                TokenType::NIL,
                TokenType::NUMBER, TokenType::STRING, 
                TokenType::INT, TokenType::DOUBLE})) { 
        ObjPtr objP = std::make_shared<LukObject>( previous() );
        */
    
    if (objP != nullptr) {
        logMsg("\nIn primary Parser, before literalExpr: ", objP);
        return std::make_shared<LiteralExpr>( objP );
    }
   
    if (match({TokenType::SUPER})) {
      TokPtr keyword = previous();
      consume(TokenType::DOT, "Expect '.' after 'super'.");

      TokPtr method = consume(TokenType::IDENTIFIER,
          "Expect superclass method name.");

      return std::make_shared<SuperExpr>(keyword, method);
    }
    
    if (match({TokenType::THIS})) {
      auto keyword = previous();
      return std::make_shared<ThisExpr>(keyword);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous());
    }
    
    // lambda function
    if (match({TokenType::FUN})) {
        return functionBody("function");
    }

    if (match({TokenType::LEFT_PAREN})) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exppect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }
    
    
    throw error(peek(), "Expect expression.");
    return nullptr;
}

bool Parser::checkEndLine(const std::string& msg, bool verbose=true) {
    if (isAtEnd()) return false;
    if (match({TokenType::SEMICOLON})) return true;
    
    if (verbose)
      throw error(peek(), msg);

    return false;
}

TokPtr& Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();
    throw error(peek(), message);
    
}

ParseError Parser::error(TokPtr& tokP, const std::string& message) {
    if (tokP->type == TokenType::END_OF_FILE) {
        lukErr.error(errTitle, tokP->line, tokP->col, " at end, " + message);
    } else {
        lukErr.error(errTitle, tokP->line, tokP->col, 
                "at '" + tokP->lexeme + "' " + message);
    }

    throw ParseError(message, tokP);
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::matchNext(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (checkNext(type)) {
            advance();
            return true;
        }
    }
    
    return false;
}

TokPtr& Parser::previous() {
    return m_tokens[m_current - 1];
}

TokPtr& Parser::advance() {
    if (!isAtEnd())
        ++m_current;
    return previous();
}

TokPtr& Parser::peek() {
    return m_tokens[m_current];
}

bool Parser::isAtEnd() {
    return peek()->type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek()->type == type;
}

bool Parser::checkNext(TokenType type) {
    if (isAtEnd()) return false;
    if (m_tokens[m_current+1]->type == TokenType::END_OF_FILE) return false;
    return m_tokens[m_current+1]->type == type;
}


void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous()->type == TokenType::SEMICOLON) return;

        switch(peek()->type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            
            default: break;
        }

        advance();
    }

}
