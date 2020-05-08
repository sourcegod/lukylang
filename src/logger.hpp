#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>
#include <string>
#include <chrono> // for datetime
#include <ctime> // to convert chrono datetime
#include <sstream>
// #define DEBUG  1 
// Simple object logging
enum TLogLevel { 
  log_OFF, log_DEBUG, log_INFO, log_WARN, log_ERROR 
};

struct TLog {
  bool headers = false;
  TLogLevel level = log_OFF;
};

extern TLog LogConf;

class CLog {
public:
  CLog() { CLog(log_DEBUG); }
  CLog(TLogLevel level) {
    m_level = level;
    if (m_level > log_OFF && m_level <= LogConf.level && LogConf.headers) {
      dateNow();
      operator << (" [" + getLabel(level) + "] ");
    }
  }
  
  ~CLog() {
    if (m_opened) {
      std::cerr << "\n";
    }
    m_opened = false;
  }
  
  template <class T>
  CLog &operator <<(const T &msg) {
    if (m_level > log_OFF && m_level <= LogConf.level) {
        std::cerr << msg;
        m_opened = true;
    }

    return *this;
  }

  // std::string getDate() {
  void dateNow() {
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cerr << "Date: " << std::ctime(&start_time);
    // oss << std::ctime(&start_time);

    // return oss.str();

  }

private:
  bool m_opened = false;
  TLogLevel m_level = log_DEBUG;
  std::ostringstream oss;
  
  inline std::string getLabel(TLogLevel lvl) {
    std::string label;
    switch(lvl) {
      case log_OFF: label = "OFF "; break;
      case log_DEBUG: label = "DEBUG "; break;
      case log_INFO: label = "INFO "; break;
      case log_WARN: label = "WARN "; break;
      case log_ERROR: label = "ERROR "; break;
    }

    return label;
  }

};

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
  if (LogConf.level >= log_DEBUG) {
    std::cerr << val << "\n";
  }

}

// recursive case
template <typename T, typename... TArgs>
void logMsg(T first, TArgs... args) {
  if (LogConf.level >= log_DEBUG) {
    std::cerr << first << " ";
    logMsg(args...);
  }

}


// macros to logging
#ifdef DEBUG
   
    // print by default file, line, and function name
    #define LOG_FILE              std::cerr << __FILE__ \
        << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n";
    // print string value for the variable name and the content
    #define LOG_DUMP(x)         do { std::cerr << #x " is value " << (x) << ", "; LOG_FILE; } while(false)
    
       
    // print an message with file, line, and functions name
    #define DEBUG_MSG(msg)       std::cerr << msg << "\n" \
                                    << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n";
    // debugging on variadic arguments using printf format
    #define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) { fprintf(stderr, fmt, __VA_ARGS__); \
            fprintf(stderr, "\n");} } while (0)
#else
    #define LOG_FILE
    #define LOG_DUMP
    #define DEBUG_MSG
    #define DEBUG_PRINT

#endif // DEBUG

#ifdef TRACER
    // pass argument to CTracer object
    #define TRACE_ENTRY_EXIT_FOR(x)       CTracer varLogObj(x)
    // print only unction name
    #define TRACE_ENTRY_EXIT              TRACE_ENTRY_EXIT_FOR(__func__)
    // print all arguments as file, line, and function name
    #define TRACE_ENTRY_EXIT_ALL(x, y, z)       CTracer varLogAll(x, y, z)
    // print by default file, line, and function name without arguments
    #define TRACE_ALL              TRACE_ENTRY_EXIT_ALL(__FILE__, __LINE__, __PRETTY_FUNCTION__)
    // print message and all traces like file, line, and function name
    #define TRACE_MSG(msg)         std::cerr << msg << "\n"; \
                                    TRACE_ALL

#else
    #define TRACE_ENTRY_EXIT
    #define TRACE_ALL
    #define TRACE_MSG

#endif // TRACER

#endif // LOGGER_HPP
