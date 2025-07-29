#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <vector>

enum LogType {
    INFO,
    WARNING,
    ERROR
};

struct LogEntry {
    LogType type;
    std::string message;
};

class Logger {
public:
    static std::vector<LogEntry> logs;

    static void Log(const std::string &message);

    static void Err(const std::string &message);
};

#endif
