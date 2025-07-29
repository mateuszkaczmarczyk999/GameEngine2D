#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

std::vector<LogEntry> Logger::logs;

std::string CurrentDateTimestamp() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    std::strftime(&output[0], output.size(), "%d-%m-%Y %H:%M:%S", std::localtime(&now));
    return output;
};

void Logger::Log(const std::string &message) {
    LogEntry log;
    log.type = LogType::INFO;
    log.message = "LOG [" + CurrentDateTimestamp() + "] - " + message;
    logs.push_back(log);
    std::cout << GREEN << log.message << RESET << std::endl;
};

void Logger::Err(const std::string &message) {
    LogEntry log;
    log.type = LogType::ERROR;
    log.message = "ERR [" + CurrentDateTimestamp() + "] - " + message;
    logs.push_back(log);
    std::cerr << RED << log.message << RESET << std::endl;
};
