#include <fstream> // for file
#include <iostream> // for IO buffer
#include <sstream> // for string buffer
#include "lukerror.hpp"
#include "token.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

using namespace std;
// test macro from the preprocessor to retriev string
// #define stringify( name ) #name

/*
static void printer(const vector<Token>& v_tokens) {
    size_t pos =1;
    std::string  type, result;
    cout << "Scanner Tokens list" << endl;
    for (auto tok: v_tokens) {
        type = tok.stringType();
        result = "Pos: "  + to_string(pos); 
        if (type != "") 
            result += ", type: " + type;
        result += ", name: " + tok.lexeme;
        if (tok.literal != "")
             result += ", literal: " + tok.literal;
        result += ", line: " + to_string(tok.line) + 
            ":" + to_string(tok.col);
        cout << result << endl;
        pos++;
    }
    cout << "Finish..." << endl;

}
*/

static void run(const std::string& source, LukError& lukErr) {
    // scanner
    Scanner scanner(source, lukErr);
    const auto tokens = scanner.scanTokens();
    if (lukErr.hadError) return;
    // printer
    // printer(tokens);
    // /*
    // parser
    Parser parser(tokens, lukErr);
    // auto expr = parser.parse();
    auto stmts = parser.parse();
    // if found error during parsing, report
    if (lukErr.hadError) {
        std::cout << "There is an error." << std::endl;
        return;
    }
    // */
    
    // Interpreter
    static Interpreter  interp;
    // convert smart pointer to raw pointer
    // interp.print(expr.get());
    interp.interpret(std::move(stmts));


    std::cout << std::endl;

}

static void runFile(const std::string& path, LukError& LukError) {
    std::ifstream file(path);
    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();
    run(stream.str(), LukError);
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

static void runCommand(const std::string& line, LukError& LukError) {
    run(line, LukError);
    LukError.hadError = false;

}


/*
static void test() {
    LukError lukErr;
    const std::string source = "1+2; while { print }";
    Scanner scan(source, lukErr);
    const auto v_tokens = scan.scanTokens();
    size_t pos =0;
    for (auto it: v_tokens) {
        cout << "Pos: " << pos << ", name: " << it.toString() 
            << " line: " << it.line << ":" << it.col << endl; 
        pos++;
    }
    cout << "It's ok" << endl;

}
*/

int main(int argc, char* argv[]) {
    // test();
    LukError lukErr;
    if (argc >2) {
        const std::string opt = std::string(argv[1]);
        if (opt == "-c") {
            const std::string line = argv[2];
            runCommand(line, lukErr);
        } else {
            cout << "Usage: luky [filename]" << endl;
        }
    } else if (argc == 2) {
        cout << "Run file " << argv[1] << endl;
        runFile(argv[1], lukErr);
    } else {
        runPrompt(lukErr);
    }

    return 0;
}
