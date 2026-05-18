#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

namespace cs2_mod {
namespace loader {

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
 * @brief Класс для логирования событий загрузчика
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
     * @brief Записать сообщение в лог
     * @param level Уровень сообщения
     * @param message Текст сообщения
     */
    void Log(LogLevel level, const std::string& message);

    /**
     * @brief Записать отладочное сообщение
     */
    void Debug(const std::string& message);

    /**
     * @brief Записать информационное сообщение
     */
    void Info(const std::string& message);

    /**
     * @brief Записать предупреждение
     */
    void Warning(const std::string& message);

    /**
     * @brief Записать ошибку
     */
    void Error(const std::string& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream m_logFile;
    std::mutex m_mutex;
    bool m_initialized;

    /**
     * @brief Получить строковое представление уровня
     */
    std::string LevelToString(LogLevel level) const;

    /**
     * @brief Получить текущее время
     */
    std::string GetTimestamp() const;
};

// Макросы для удобного логирования
#define LOG_DEBUG(msg) cs2_mod::loader::Logger::Instance().Debug(msg)
#define LOG_INFO(msg) cs2_mod::loader::Logger::Instance().Info(msg)
#define LOG_WARNING(msg) cs2_mod::loader::Logger::Instance().Warning(msg)
#define LOG_ERROR(msg) cs2_mod::loader::Logger::Instance().Error(msg)

} // namespace loader
} // namespace cs2_mod
