#include "common.hpp"
#include "lukerror.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include "interpreter.hpp"

#include <fstream> // for file
#include <iostream> // for IO buffer
#include <sstream> // for string buffer

using namespace std;
// test macro from the preprocessor to retriev string
// #define stringify( name ) #name
const std::string m_errTitle = "LukyError: ";
LukError m_lukErr;

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
bool hasOnlySpaces(const std::string& str) {
    // ignore spaces, tabs, newlines,
    // vertical tabs, feeds and carriage returns
    return str.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
}

static void run(const std::string& source) {
    if (source.empty() || hasOnlySpaces(source)) return;

    // scanner
    Scanner scanner(source, m_lukErr);
    const std::vector<TokPtr>&& v_tokens = scanner.scanTokens();
    if (m_lukErr.hadError) return;
    // printer
    // printer(tokens);
    // /*
    // parser
    Parser parser(std::move(v_tokens), m_lukErr);
    auto stmts = parser.parse();
    // if found error during parsing, report
    if (m_lukErr.hadError)  return;
    static Interpreter  interp(m_lukErr);
    Resolver resol(interp, m_lukErr);
    resol.resolve((stmts));
    
    // Stop if there was a resolution error.
    if (m_lukErr.hadError) return;
    
    // Interpreter
    interp.interpret(std::move(stmts));
    // interp.interpret((stmts));


    std::cout << std::endl;

}

static void runFile(const std::string& path) {
  // TODO: use filesystem library in C++17 to check whether file exists
    std::ifstream file(path);
    if (!file.is_open()) {
      const std::string msg = "cannot open file " + path;
      m_lukErr.error(m_errTitle, msg);
      return;
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();
    run(stream.str());
}

static void runPrompt() {
    std::string line;
    while (1) {
        std::cout << "-> ";
        // manage ctrl-D to exit the input loop
        if (!getline(std::cin, line)) break;
        if (line == "") continue;
        run(line);
        m_lukErr.hadError = false;
    }

}

static void runCommand(const std::string& line) {
    run(line);
    m_lukErr.hadError = false;

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
    // LukError lukErr;
    if (argc >2) {
        const std::string opt = std::string(argv[1]);
        if (opt == "-c") {
            const std::string line = argv[2];
            runCommand(line);
        } else {
            cout << "Usage: luky [filename]\n" 
              << "-c: line" << endl;
        }
    } else if (argc == 2) {
        cout << "Run file " << argv[1] << endl;
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}
