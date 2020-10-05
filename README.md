# Luky language

A simple interpreter in C++11, where the language is based on  lox, written in
Java, following this excellent book, [Crafting Interpreters](http://craftinginterpreters.com/ "Crafting Interpreters") by Bob Nystrom. 
## Features
Including features are:

- Tokenizer
- Scanner
- Parser
- AST printer
- Interpreter
- Arithmetic operators (+, -, * /)
- Equality operators (==, !=)
- Comparison operators (<, <=, >, >=)
- Logical operators (or, and)
- Unary operators (!, -)
- REPL Interactive Interpreter
- Expressions
- Statements
- print statement
- Variable declaration
- Environment
- Block statements
- If statement
- While statement
- For statement
- Break statement
- Native clock function
- Functions
- Return statement
- Closures
- Resolver
- Classes
- Member functions
- Inheritance
- Superclass


## Additions 
Additional features are supported, mostly based on tasks from book:
- Multiline comments
- Nested /**/ multiline comments
- Break statement
- Continue statement
- Comma operator ,
- Bitwise operators (|, &, ^, ~)
- Bitwise shift operators (<<, >>)
- Modulus operator % 
- Exponentiation operator \*\*
- Ternary operator ?:
- Postfix and Prefix increment/decrement operators (++, --)
- Compound assignment operators (+=, -=, *=, /=, %=, **=, |=, &=, ^=, <<=, >>=)
- Lambda function
- Variables inused


## Extras features
Adding extras features to make the language more fun and usable:
- Debugging messages
- Int and Double types support
- String multiplier: ("abc"*2 = "abcabc")
- Escapes sequences
- Simple and Double quotes
- Automatic Semicolon Insertion
- Native println function with variadic arguments
- Native readln function

## Misc
Miscellaneous files are included:
- Scons script file for building, running, and debugging the Luky program.
- Examples files
- Tests files
- lukman script for building, running, debugging and testing the luky program.
- Grammar file for context-free grammar rules Parser.
## Examples 
- Example 1: hello
var name = "Lukylang";
print "Hello from " + name; // Hello from Lukylang

- Example 2: For loop
for (i=0; i < 5; i +=1) { print i; } // 0 1 2 3 4

- Example 3: factorial 1
fun fact(n) {
  if (n <= 1) return 1;
  return n*fact(n-1);
}
print fact(5); // 120

- Example 4: facttorial 2 with ternary operator
fun fact(n) {
  return n <= 1 ? 1 : nfact(n-1);
}
print fact(5) // 120

- Example 5: class
class Bacon {
  eat() {
    print "Crunch crunch crunch!";
  }
}
Bacon().eat(); // "Crunch crunch crunch!".


## Thanks
Many thanks to these C++ projects or sites that are inspired me:
- [stackoverflow](https://stackoverflow.com)
- [aggsol/lox-simple](https://gitlab.com/aggsol/lox-simple)
- [spraza/lox-cpp](https://github.com/spraza/lox-cpp)
- [ThorNielsen/loxint](https://github.com/ThorNielsen/loxint)

