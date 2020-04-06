#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>
#include <string>

// #define DEBUG  1 
// object to tracing
class CTracer {
public:
    const std::string m_in {"Enter "};
    const std::string m_out {"\nExit "};

    CTracer(const char* name): m_name(name) {
        std::cerr << m_in << m_name << "\n";
    }

    CTracer(const char* file, const int line, const char* name)
        : m_file(file), m_line(line), m_name(name) {
        std::cerr << m_in << m_file << ":" << m_line << " " << m_name << "\n";
    }


    ~CTracer() {
        std::cerr << m_out << m_name << "\n\n";
    }

private:
    const char* m_file;
    const int m_line =0;
    // passing name in const char* prevent the construction of string
    const char* m_name;
};


// std::string CTracer::m_in {"Enter "};
// std::string CTracer::m_out {"Exit "};

// variadic template
// base case
template <typename T>
void logMsg(T val) { 
#ifdef DEBUG
        std::cerr << val << "\n";
            // << __PRETTY_FUNCTION__ << ", " << __FILE__ << ":" << __LINE__ << "\n";
#endif

}

// recursive case
template <typename T, typename... TArgs>
void logMsg(T first, TArgs... args) {
#ifdef DEBUG
        std::cerr << first << " ";
        logMsg(args...);
#endif

}


// macros to logging
#ifdef DEBUG
    // pass argument to CTracer object
    #define TRACE_ENTRY_EXIT_FOR(x)       CTracer varLogObj(x)
    // print only unction name
    #define TRACE_ENTRY_EXIT              TRACE_ENTRY_EXIT_FOR(__func__)
    // print all arguments as file, line, and function name
    #define TRACE_ENTRY_EXIT_ALL(x, y, z)       CTracer varLogAll(x, y, z)
    // print by default file, line, and function name without arguments
    #define TRACE_ALL              TRACE_ENTRY_EXIT_ALL(__FILE__, __LINE__, __PRETTY_FUNCTION__)
    
    // print by default file, line, and function name
    #define LOG_FILE              std::cerr << __FILE__ \
        << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n";
    // print string value for the variable name and the content
    #define LOG_DUMP(x)         do { std::cerr << #x " is value " << (x) << ", "; LOG_FILE; } while(false)
    
    // print message and all traces like file, line, and function name
    #define TRACE_MSG(msg)         std::cerr << msg << "\n"; \
                                    TRACE_ALL
    
    // print an message with file, line, and functions name
    #define DEBUG_MSG(msg)       std::cerr << msg << "\n" \
                                    << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n";
    // debugging on variadic arguments using printf format
    #define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) { fprintf(stderr, fmt, __VA_ARGS__); \
            fprintf(stderr, "\n");} } while (0)
#else
    #define TRACE_ENTRY_EXIT
    #define TRACE_ALL
    #define LOG_FILE
    #define LOG_DUMP
    #define TRACE_MSG
    #define DEBUG_MSG
    #define DEBUG_PRINT

#endif


#endif // LOGGER_HPP
