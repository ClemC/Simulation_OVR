#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <iostream>
#include <ostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <cstdint>

using namespace std;

/**
 * @brief The Severity enum
 * @details 4 levels of logs. Used in a bitmask.
 */
enum Severity
{
    trace = 1,
    debug = 2,
    info = 4,
    error = 8
};

/**
 * @brief The Logger class
 */
class Logger
{
public:
    /**
     * @brief Logger constructor
     * @details No files are specified so we are implicity only logging to console. No files are opened/closed.
     * @param logToConsole If true, we log to console. Else we do nothing.
     * @param severity Bitmask of the severity levels we want to see
     */
    Logger(bool logToConsole, uint severity):
        logFileName_ {""},
        errorFileName_ {""},
        logToFile_ {false},
        logToConsole_ {logToConsole},
        startTime_ {std::chrono::high_resolution_clock::now()},
        lastLogTime_ {startTime_},
        logCount_ {0},
        severity_ {severity}
    {
    }

    /**
     * @brief Logger constructor
     * @param logFileName The file we log to for the 'trace', 'debug' and 'info' levels
     * @param errorFileName The file we log to for the 'error' level
     * @param logToConsole Whether we show the logs in the console or not
     * @param severity Bitmask of the severity levels we want to see
     */
    Logger(string const & logFileName, string const & errorFileName, bool logToConsole, uint severity):
        logFileName_ {logFileName},
        errorFileName_ {errorFileName},
        logFile_ {logFileName},
        errorFile_ {errorFileName},
        logToFile_ {true},
        logToConsole_ {logToConsole},
        startTime_ {std::chrono::high_resolution_clock::now()},
        lastLogTime_ {startTime_},
        logCount_ {0},
        severity_ {severity}
    {
    }

    ~Logger()
    {
        if(logFile_)
        {
            logFile_.close();
        }
        if(errorFile_)
        {
            errorFile_.close();
        }
    }

    /**
     * @brief get
     * @details Enables the particular log syntax
     * @return The internal buffer that stores logs
     */
    stringstream & get ()
    {
        return buffer_;
    }

    /**
     * @brief operator () Simpler syntax for get()
     * @return The internal buffer that stores logs
     */
    stringstream & operator() ()
    {
        return get();
    }

    /**
     * @brief log The main log function
     * @param message The log message
     * @param severityLevel The log severity level in a string format that is prepended to the final log
     * @param consoleOutLog What stream we log to in the console among clog, cout, cerr
     * @param fileOutLog What file we log to. Log file or Error file.
     */
    void log (ostream & message, string const & severityLevel, ostream & consoleOutLog, ofstream & fileOutLog)
    {
        logCount_++;

        stringstream res;
        auto now = std::chrono::high_resolution_clock::now();


        res << "<" << severityLevel << "> "
            << "(" << logCount_ << ") "
            << "[" << std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_).count() << " micros] "
            << message.rdbuf()
            << " (" << std::chrono::duration_cast<std::chrono::microseconds>(now - lastLogTime_).count()  << " micros) "
            << endl;

        lastLogTime_ = now;

        if(logToConsole_)
        {
            consoleOutLog << res.str();
        }
        if(logToFile_)
        {
            fileOutLog << res.str();
        }
    }

    /**
     * @brief trace Log to the trace level
     * @param stream The log message
     */
    void trace(ostream & stream)
    {
        Severity severity = Severity::trace;

        if(severity_ & severity)
        {
            log(stream, "trace", clog, logFile_);
        }

        clear();
    }

    /**
     * @brief debug Log to the debug level
     * @param stream The log message
     */
    void debug(ostream & stream)
    {
        Severity severity = Severity::debug;

        if(severity_ & severity)
        {
            log(stream, "debug", clog, logFile_);
        }

        clear();
    }

    /**
     * @brief info Log to the info level
     * @param stream The log message
     */
    void info(ostream & stream)
    {
        Severity severity = Severity::info;

        if(severity_ & severity)
        {
            log(stream, "info", cout, logFile_);
        }

        clear();
    }

    /**
     * @brief error Log to the error level
     * @param stream The log message
     */
    void error(ostream & stream)
    {
        Severity severity = Severity::error;

        if(severity_ & severity)
        {
            log(stream, "error", cerr, errorFile_);
        }

        clear();
    }

    /**
     * @brief clear Clear the internal log buffer
     */
    void clear()
    {
        buffer_.str("");
    }

    uint severity() const
    {
        return severity_;
    }
    void setSeverity(const uint& severity)
    {
        severity_ = severity;
    }

private:
    /**
     * @brief buffer_ The internal log buffer
     */
    stringstream buffer_;

    /**
     * @brief logFileName_ The name of the log file for the trace, debug and info levels
     */
    string logFileName_;

    /**
     * @brief errorFileName_ The name of the log file for the error level
     */
    string errorFileName_;

    /**
     * @brief logFile_ The log file for the trace, debug and info levels
     */
    ofstream logFile_;

    /**
     * @brief errorFile_  the error file for the error level
     */
    ofstream errorFile_;

    /**
     * @brief logToFile_ Whether we log to files or not
     */
    bool logToFile_;

    /**
     * @brief logToConsole_ Whether we log to console or not
     */
    bool logToConsole_;

    /**
     * @brief startTime_ Time the logger was instantiated
     */
    chrono::high_resolution_clock::time_point startTime_;

    /**
     * @brief lastLogTime_ Time of the last log. Useful for performance measures
     */
    chrono::high_resolution_clock::time_point lastLogTime_;

    /**
     * @brief The number of logs since the start of the program
     */
    long long logCount_;

    /**
      * @brief severity_ The severity bitmask that filters the logs
      */
     uint severity_;
};

extern unique_ptr<Logger> logger;

#endif // LOG_H
