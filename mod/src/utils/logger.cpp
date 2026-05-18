/**
 * @file logger.cpp
 * @brief Реализация системы логирования
 */

#include "mod/logger.h"
#include "mod/utils.h"
#include <windows.h>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace kastol {

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_initialized(false) {
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

bool Logger::Initialize(const std::string& logPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        return true;
    }
    
    m_logFile.open(logPath, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        return false;
    }
    
    m_initialized = true;
    
    // Пишем заголовок
    m_logFile << "\n========================================\n";
    m_logFile << "CS2 Mod Log Started\n";
    m_logFile << "Time: " << GetTimestamp() << "\n";
    m_logFile << "========================================\n\n";
    
    return true;
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }
    
    m_logFile << "[" << GetTimestamp() << "] "
              << "[" << LevelToString(level) << "] "
              << message << "\n";
    
    m_logFile.flush();
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::Debug, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::Error, message);
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

std::string Logger::GetTimestamp() const {
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << st.wDay << "."
       << std::setw(2) << st.wMonth << "."
       << std::setw(4) << st.wYear << " "
       << std::setw(2) << st.wHour << ":"
       << std::setw(2) << st.wMinute << ":"
       << std::setw(2) << st.wSecond << "."
       << std::setw(3) << st.wMilliseconds;
    
    return ss.str();
}

} // namespace kastol
