#pragma once

/**
 * @file config.h
 * @brief Система конфигурации модификации
 * 
 * Управляет настройками модификации, сохранением и загрузкой конфигов.
 */

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <windows.h> // для VK_INSERT и других виртуальных кодов клавиш

namespace kastol {

/**
 * @brief Тип значения конфигурации
 */
enum class ConfigValueType {
    Boolean,
    Integer,
    Float,
    String,
    Color,
    KeyBind
};

/**
 * @brief Структура для хранения значения конфигурации
 */
struct ConfigValue {
    ConfigValueType type;
    std::string name;
    std::string value;
    std::string description;
    
    // Callback при изменении значения
    std::function<void()> onChangeCallback;
};

/**
 * @brief Класс управления конфигурацией модификации
 */
class Config {
public:
    static Config& Instance();

    /**
     * @brief Инициализировать систему конфигурации
     * @param configPath Путь к файлу конфигурации
     * @return true если успешно
     */
    bool Initialize(const std::string& configPath);

    /**
     * @brief Загрузить конфигурацию из файла
     * @return true если успешно
     */
    bool Load();

    /**
     * @brief Сохранить конфигурацию в файл
     * @return true если успешно
     */
    bool Save();

    /**
     * @brief Сбросить к значениям по умолчанию
     */
    void Reset();

    // Геттеры
    bool GetBool(const std::string& key, bool defaultValue = false) const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Сеттеры
    void SetBool(const std::string& key, bool value);
    void SetInt(const std::string& key, int value);
    void SetFloat(const std::string& key, float value);
    void SetString(const std::string& key, const std::string& value);

    /**
     * @brief Установить callback на изменение значения
     */
    void SetChangeCallback(const std::string& key, std::function<void()> callback);

    /**
     * @brief Получить все ключи конфигурации
     */
    std::vector<std::string> GetAllKeys() const;

    /**
     * @brief Проверить существование ключа
     */
    bool HasKey(const std::string& key) const;

    // Стандартные настройки модификации
    struct Settings {
        // Основные
        bool menuOpen = false;
        int menuToggleKey = VK_INSERT;
        
        // Overlay
        bool overlayEnabled = true;
        float overlayTransparency = 0.95f;
        
        // Цвета
        float accentColor[4] = { 0.4f, 0.7f, 1.0f, 1.0f };
        float bgColor[4] = { 0.1f, 0.1f, 0.15f, 0.95f };
        
        // Другие настройки
        bool showFPS = false;
        bool saveOnExit = true;
    } settings;

private:
    Config();
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::string m_configPath;
    std::map<std::string, ConfigValue> m_values;
    mutable std::mutex m_mutex;

    /**
     * @brief Установить значения по умолчанию
     */
    void SetDefaults();

    /**
     * @brief Парсинг конфигурационного файла
     */
    bool ParseConfig(const std::string& content);

    /**
     * @brief Сериализация конфигурации
     */
    std::string SerializeConfig() const;
};

} // namespace kastol
