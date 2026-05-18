#pragma once

#include <string>

namespace cs2_mod {
namespace loader {

/**
 * @brief Класс для проверки обновлений модификации
 * 
 * Проверяет актуальность версии модификации и
 * при необходимости загружает обновления.
 */
class Updater {
public:
    Updater();
    ~Updater();

    /**
     * @brief Проверить наличие обновлений
     * @return true если обновления доступны
     */
    bool CheckForUpdates();

    /**
     * @brief Загрузить обновление
     * @return true если загрузка успешна
     */
    bool DownloadUpdate();

    /**
     * @brief Получить текущую версию
     * @return Строка с версией
     */
    std::string GetCurrentVersion() const;

    /**
     * @brief Получить последнюю доступную версию
     * @return Строка с версией
     */
    std::string GetLatestVersion() const;

    /**
     * @brief Установить URL для проверки обновлений
     * @param url URL сервера обновлений
     */
    void SetUpdateUrl(const std::string& url);

private:
    std::string m_currentVersion;
    std::string m_latestVersion;
    std::string m_updateUrl;
    bool m_updateAvailable;

    /**
     * @brief Загрузить данные с сервера
     * @param url URL для запроса
     * @return Полученные данные
     */
    std::string FetchUrl(const std::string& url) const;
};

} // namespace loader
} // namespace cs2_mod
