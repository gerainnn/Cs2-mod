/**
 * @file core.h
 * @brief Ядро модификации KASTOL
 * 
 * Основной класс, управляющий жизненным циклом модификации.
 * Инициализирует все подсистемы и обеспечивает их взаимодействие.
 */

#include <memory>
#include <string>
#include <windows.h>

namespace kastol {

// Экспорт функций DLL
#ifdef KASTOL_EXPORTS
    #define KASTOL_API __declspec(dllexport)
#else
    #define KASTOL_API __declspec(dllimport)
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
 * @brief Основной класс модификации KASTOL
 */
class KASTOL_API Mod {
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

    bool InitializeSubsystems();
    void ShutdownSubsystems();
    bool CreateRenderThread();
    bool InstallDirectXHooks();
};

// Функции экспорта DLL
extern "C" {
    KASTOL_API bool InitializeKastol();
    KASTOL_API void ShutdownKastol();
    KASTOL_API const char* GetKastolVersion();
}

} // namespace kastol
