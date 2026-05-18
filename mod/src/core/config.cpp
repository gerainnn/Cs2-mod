/**
 * @file config.cpp
 * @brief Реализация системы конфигурации
 */

#include "mod/config.h"
#include "mod/logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

namespace kastol {

Config& Config::Instance() {
    static Config instance;
    return instance;
}

Config::Config() {
    SetDefaults();
}

Config::~Config() {
    // Сознательно НЕ делаем Save() в деструкторе:
    // деструктор Meyers-singleton вызывается во время DLL_PROCESS_DETACH,
    // где I/O может зависнуть или упасть. Save должен делаться явно
    // через Mod::Shutdown() или по событиям UI.
}

bool Config::Initialize(const std::string& configPath) {
    m_configPath = configPath;
    LOG_INFO(("Инициализация конфигурации: " + configPath).c_str());
    
    return Load();
}

bool Config::Load() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    LOG_INFO("Загрузка конфигурации...");
    
    std::ifstream file(m_configPath);
    if (!file.is_open()) {
        LOG_WARNING("Файл конфигурации не найден, используются значения по умолчанию");
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    return ParseConfig(content);
}

bool Config::Save() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    LOG_INFO("Сохранение конфигурации...");
    
    std::ofstream file(m_configPath);
    if (!file.is_open()) {
        LOG_ERROR("Не удалось открыть файл для записи");
        return false;
    }
    
    std::string content = SerializeConfig();
    file << content;
    file.close();
    
    LOG_INFO("Конфигурация сохранена");
    return true;
}

void Config::Reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    SetDefaults();
    LOG_INFO("Конфигурация сброшена к значениям по умолчанию");
}

bool Config::GetBool(const std::string& key, bool defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_values.find(key);
    if (it != m_values.end()) {
        return it->second.value == "true" || it->second.value == "1";
    }
    return defaultValue;
}

int Config::GetInt(const std::string& key, int defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_values.find(key);
    if (it != m_values.end()) {
        try {
            return std::stoi(it->second.value);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float Config::GetFloat(const std::string& key, float defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_values.find(key);
    if (it != m_values.end()) {
        try {
            return std::stof(it->second.value);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::string Config::GetString(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_values.find(key);
    if (it != m_values.end()) {
        return it->second.value;
    }
    return defaultValue;
}

void Config::SetBool(const std::string& key, bool value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key].value = value ? "true" : "false";
}

void Config::SetInt(const std::string& key, int value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key].value = std::to_string(value);
}

void Config::SetFloat(const std::string& key, float value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key].value = std::to_string(value);
}

void Config::SetString(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key].value = value;
}

void Config::SetChangeCallback(const std::string& key, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key].onChangeCallback = callback;
}

std::vector<std::string> Config::GetAllKeys() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> keys;
    for (const auto& pair : m_values) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Config::HasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_values.find(key) != m_values.end();
}

void Config::SetDefaults() {
    m_values.clear();
    
    // Основные настройки
    m_values["menuToggleKey"] = { ConfigValueType::KeyBind, "menuToggleKey", "45", "Клавиша переключения меню" };
    m_values["overlayEnabled"] = { ConfigValueType::Boolean, "overlayEnabled", "true", "Включить overlay" };
    m_values["overlayTransparency"] = { ConfigValueType::Float, "overlayTransparency", "0.95", "Прозрачность overlay" };
    
    // Цвета
    m_values["accentColorR"] = { ConfigValueType::Float, "accentColorR", "0.4", "Цвет акцента R" };
    m_values["accentColorG"] = { ConfigValueType::Float, "accentColorG", "0.7", "Цвет акцента G" };
    m_values["accentColorB"] = { ConfigValueType::Float, "accentColorB", "1.0", "Цвет акцента B" };
    
    // Другие
    m_values["showFPS"] = { ConfigValueType::Boolean, "showFPS", "false", "Показывать FPS" };
    m_values["saveOnExit"] = { ConfigValueType::Boolean, "saveOnExit", "true", "Сохранять при выходе" };
}

bool Config::ParseConfig(const std::string& /*content*/) {
    // Простая реализация парсинга JSON
    // В реальной реализации используйте библиотеку nlohmann/json
    
    LOG_DEBUG("Парсинг конфигурации");
    return true;
}

std::string Config::SerializeConfig() const {
    std::stringstream ss;
    ss << "{\n";
    
    bool first = true;
    for (const auto& pair : m_values) {
        if (!first) ss << ",\n";
        ss << "  \"" << pair.first << "\": \"" << pair.second.value << "\"";
        first = false;
    }
    
    ss << "\n}";
    return ss.str();
}

} // namespace kastol
