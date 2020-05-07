#include "Logger.h"
#include <iostream>
#include <fstream>

Logger::Logger() {
    logfile.open("log_file.txt");
}

Logger::~Logger() {
    logfile.close();
}

void Logger::Info(std::string message) {
    if (new_level != Level::Error) {
        std::cout << " [INFO] " << message << std::endl;
        logfile << " [INFO] " << message << std::endl;
    }
}

void Logger::Debug(std::string message) {
    if (new_level == Level::Debug || new_level == Level::Info) {
        std::cout << " [DEBUG] " << message << std::endl;
        logfile << " [DEBUG] " << message << std::endl;
    }
}

void Logger::Error(std::string message) {
    std::cout << " [ERROR] " << message << std::endl;
    logfile << " [ERROR] " << message << std::endl;
}

void Logger::SetLevel(Level level) {
    new_level = level;
}