#include <fstream> // for file
#include <iostream> // for IO buffer
#include <sstream> // for string buffer
#include "error_handler.hpp"
#include "token.hpp"
#include "scanner.hpp"

using namespace std;
// test macro from the preprocessor to retriev string
#define stringify( name ) #name

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
    test();
    return 0;
}
