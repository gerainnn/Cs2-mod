#pragma once

/**
 * @file core.h
 * @brief Ядро модификации CS2
 * 
 * Основной класс, управляющий жизненным циклом модификации.
 * Инициализирует все подсистемы и обеспечивает их взаимодействие.
 */

#include <memory>
#include <string>
#include <windows.h>

namespace cs2_mod {

// Экспорт функций DLL
#ifdef CS2_MOD_EXPORTS
    #define MOD_API __declspec(dllexport)
#else
    #define MOD_API __declspec(dllimport)
#endif

/**
 * @brief Состояние модификации
 */
enum class ModState {
    Uninitialized,  // Не инициализирована
    Initializing,   // Инициализация
    Running,        // Работает
    ShuttingDown,   // Завершение работы
    Error           // Ошибка
};

/**
 * @brief Основной класс модификации
 */
class MOD_API Mod {
public:
    /**
     * @brief Получить единственный экземпляр модификации
     */
    static Mod& Instance();

    /**
     * @brief Инициализировать модификацию
     * @return true если успешно
     */
    bool Initialize();

    /**
     * @brief Завершить работу модификации
     */
    void Shutdown();

    /**
     * @brief Обновить состояние модификации
     * @param deltaTime Время между кадрами
     */
    void Update(float deltaTime);

    /**
     * @brief Получить текущее состояние
     */
    ModState GetState() const;

    /**
     * @brief Проверить, инициализирована ли модификация
     */
    bool IsInitialized() const;

    /**
     * @brief Получить версию модификации
     */
    std::string GetVersion() const;

    /**
     * @brief Получить имя модификации
     */
    std::string GetName() const;

private:
    Mod();
    ~Mod();
    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;

    ModState m_state;
    std::string m_version;
    std::string m_name;
    HMODULE m_module;

    // Подсистемы (будут добавлены позже)
    // std::unique_ptr<Overlay> m_overlay;
    // std::unique_ptr<Renderer> m_renderer;
    // std::unique_ptr<Config> m_config;

    /**
     * @brief Инициализировать подсистемы
     */
    bool InitializeSubsystems();

    /**
     * @brief Завершить работу подсистем
     */
    void ShutdownSubsystems();

    /**
     * @brief Создать потоки для рендеринга
     */
    bool CreateRenderThread();

    /**
     * @brief Установить хуки для DirectX
     */
    bool InstallDirectXHooks();
};

// Функции экспорта DLL
extern "C" {
    MOD_API bool InitializeMod();
    MOD_API void ShutdownMod();
    MOD_API const char* GetModVersion();
}

} // namespace cs2_mod
