#pragma once

/**
 * @file logger.h
 * @brief Система логирования модификации
 */

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

namespace cs2_mod {

/**
 * @brief Уровни логирования
 */
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

/**
 * @brief Класс для логирования
 */
class Logger {
public:
    static Logger& Instance();

    /**
     * @brief Инициализировать логгер
     * @param logPath Путь к файлу логов
     * @return true если успешно
     */
    bool Initialize(const std::string& logPath);

    /**
     * @brief Записать сообщение
     * @param level Уровень
     * @param message Сообщение
     */
    void Log(LogLevel level, const std::string& message);

    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream m_logFile;
    std::mutex m_mutex;
    bool m_initialized;

    std::string LevelToString(LogLevel level) const;
    std::string GetTimestamp() const;
};

// Макросы для удобного логирования
#define LOG_DEBUG(msg) cs2_mod::Logger::Instance().Debug(msg)
#define LOG_INFO(msg) cs2_mod::Logger::Instance().Info(msg)
#define LOG_WARNING(msg) cs2_mod::Logger::Instance().Warning(msg)
#define LOG_ERROR(msg) cs2_mod::Logger::Instance().Error(msg)

} // namespace cs2_mod
