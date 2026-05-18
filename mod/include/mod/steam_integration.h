#pragma once

/**
 * @file steam_integration.h
 * @brief Интеграция со Steam API в модификации
 * 
 * Позволяет модификации взаимодействовать со Steam.
 */

#include <string>
#include <cstdint>

namespace cs2_mod {

/**
 * @brief Класс интеграции со Steam
 */
class SteamIntegration {
public:
    static SteamIntegration& Instance();

    /**
     * @brief Инициализировать интеграцию
     * @return true если успешно
     */
    bool Initialize();

    /**
     * @brief Завершить работу
     */
    void Shutdown();

    /**
     * @brief Обновить состояние
     */
    void Update();

    /**
     * @brief Получить Steam ID текущего пользователя
     */
    uint64_t GetSteamId() const;

    /**
     * @brief Получить имя пользователя
     */
    std::string GetUsername() const;

    /**
     * @brief Получить App ID игры
     */
    uint32_t GetAppId() const;

    /**
     * @brief Проверить, запущен ли Steam
     */
    bool IsSteamRunning() const;

    /**
     * @brief Получить директорию установки Steam
     */
    std::string GetSteamInstallPath() const;

private:
    SteamIntegration();
    ~SteamIntegration();
    SteamIntegration(const SteamIntegration&) = delete;
    SteamIntegration& operator=(const SteamIntegration&) = delete;

    bool m_initialized;
    uint64_t m_steamId;
    std::string m_username;
};

} // namespace cs2_mod
