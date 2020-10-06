#include "astprinter.hpp"
#include "../lukerror.hpp"
#include "../token.hpp"
#include "../scanner.hpp"
#include "../parser.hpp"
#include <iostream>
#include <string>
#include <vector>


std::string AstPrinter::print(Expr* expr) {
    expr->accept(*this);
    std::cout << m_result << std::endl;
    m_result = "";
    return m_result;
}

TObject AstPrinter::visitBinaryExpr(BinaryExpr& expr) {
    // method get allow to convert smart pointer to raw pointer
    TObject obj =  parenthesize(expr.op.lexeme, 
            {expr.left.get(), expr.right.get()});
    return obj;
}

TObject AstPrinter::visitGroupingExpr(GroupingExpr& expr) {
    TObject obj = parenthesize("group", 
            {expr.expression.get()});

    return obj;
}


TObject AstPrinter::visitLiteralExpr(LiteralExpr& expr) {
    m_result += expr.value.toString();

    return TObject(m_result);

}

TObject AstPrinter::visitUnaryExpr(UnaryExpr& expr) {
    TObject obj =  parenthesize(expr.op.lexeme, 
            {expr.right.get()});

    return obj;
}


std::string AstPrinter::parenthesize(std::string name, std::vector<Expr*> v_expr) {
    m_result += "(" + name;
    for (auto expr : v_expr) {
        m_result += " ";
        expr->accept(*this);
    }

    m_result += ")";

    return m_result;
}

static void run(const std::string& source, LukError& LukError) {
    // scanner
    Scanner scanner(source, LukError);
    const auto tokens = scanner.scanTokens();
    if (LukError.hadError) return;
    // parser
    Parser parser(tokens, LukError);
    auto expr = parser.parse();
    // if found error during parsing, report
    if (LukError.hadError) {
        std::cout << "There is an error." << std::endl;
        return;
    }
    
    // print ast
    AstPrinter ap;
    // convert smart pointer to raw pointer
    ap.print(expr.get());


    std::cout << std::endl;

}



static void runPrompt(LukError& LukError) {
    std::string line;
    while (1) {
        std::cout << "-> ";
        // manage ctrl-D to exit the input loop
        if (!getline(std::cin, line)) break;
        run(line, LukError);
        LukError.hadError = false;
    }

}

int main(int argc, char* argv[]) {
    LukError lukErr;
    runPrompt(lukErr);

    return 0;
}
