#include <fstream> // for file
#include <iostream> // for IO buffer
#include <sstream> // for string buffer
#include "error_handler.hpp"
#include "token.hpp"
#include "scanner.hpp"

using namespace std;
// test macro from the preprocessor to retriev string
#define stringify( name ) #name

namespace luky {

    static void printer(const vector<Token>& v_tokens) {
        size_t pos =0;
        cout << "Scanner Tokens list" << endl;
        for (auto it: v_tokens) {
            cout << "Pos: " << pos << ", name: " << it.toString() 
                << " line: " << it.line << ":" << it.col << endl; 
            pos++;
        }
        cout << "Finish..." << endl;

    }

    static void run(const std::string& source, ErrorHandler& errorHandler) {
        /// scanner
        Scanner scanner(source, errorHandler);
        const auto tokens = scanner.scanTokens();
        if (errorHandler.hadError) return;
        // printer
        printer(tokens);
        /*
        /// parser
        // Parser parser(tokens, errorHandler);
        // auto expr = parser.parse();
        // if found error during parsing, report
        if (errorHandler.foundError) {
            errorHandler.report();
            return;
        }
        */
        
        /*
        /// print ast
        ASTPrinter pp;
        pp.print(expr);
        std::cout << std::endl;
        */

    }

    static void runFile(const std::string& path, ErrorHandler& errorHandler) {
        std::ifstream file(path);
        std::ostringstream stream;
        stream << file.rdbuf();
        file.close();
        run(stream.str(), errorHandler);
    }

    static void runPrompt(ErrorHandler& errorHandler) {
        std::string line;
        while (1) {
            std::cout << "-> ";
            // manage ctrl-D to exit the input loop
            if (!getline(std::cin, line)) break;
            run(line, errorHandler);
            errorHandler.hadError = false;
        }

    }
}

static void test() {
    luky::ErrorHandler err;
    const std::string source = "1+2; while { print }";
    luky::Scanner scan(source, err);
    const auto v_tokens = scan.scanTokens();
    size_t pos =0;
    for (auto it: v_tokens) {
        cout << "Pos: " << pos << ", name: " << it.toString() 
            << " line: " << it.line << ":" << it.col << endl; 
        pos++;
    }
    cout << "It's ok" << endl;

}

int main(int argc, char* argv[]) {
    // test();
    luky::ErrorHandler err;
    if (argc >2) {
        cout << "Usage: luky [filename]" << endl;
    } else if (argc == 2) {
        cout << "Run file " << argv[1] << endl;
        luky::runFile(argv[1], err);
    } else {
        luky::runPrompt(err);
    }

    return 0;
}
