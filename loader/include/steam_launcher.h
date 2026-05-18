#pragma once

#include <string>
#include <windows.h>

namespace cs2_mod {
namespace loader {

/**
 * @brief Класс для запуска CS2 через Steam
 * 
 * Обеспечивает корректный запуск игры через Steam API,
 * позволяя модификации работать с игрой.
 */
class SteamLauncher {
public:
    SteamLauncher();
    ~SteamLauncher();

    /**
     * @brief Инициализация Steam API
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * @brief Запуск Counter-Strike 2
     * @return true если игра запущена успешно
     */
    bool LaunchCS2();

    /**
     * @brief Получить ID процесса CS2
     * @return ID процесса или 0 если игра не найдена
     */
    DWORD GetCS2ProcessId() const;

    /**
     * @brief Ожидание полного запуска игры
     * @param timeout_ms Таймаут ожидания в миллисекундах
     * @return true если игра запустилась
     */
    bool WaitForGameLaunch(DWORD timeout_ms = 60000);

    /**
     * @brief Проверить, запущена ли CS2
     * @return true если игра запущена
     */
    bool IsCS2Running() const;

    /**
     * @brief Получить путь к исполняемому файлу CS2
     * @return Путь к cs2.exe
     */
    std::wstring GetCS2Path() const;

private:
    bool m_initialized;
    HANDLE m_processHandle;
    DWORD m_processId;

    /**
     * @brief Найти процесс CS2
     * @return true если процесс найден
     */
    bool FindCS2Process();

    /**
     * @brief Получить путь к CS2 через Steam
     * @return Путь к игре
     */
    std::wstring ResolveCS2Path() const;
};

} // namespace loader
} // namespace cs2_mod
