#pragma once

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

// Экспорт функций DLL.
// Определяется один раз для всего проекта — используется и в core.h, и в dll_main.cpp.
#ifdef KASTOL_EXPORTS
    #define KASTOL_API __declspec(dllexport)
#else
    #define KASTOL_API __declspec(dllimport)
#endif

namespace kastol {

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
 *
 * Внимание: класс намеренно НЕ помечен KASTOL_API. Он живёт целиком внутри DLL,
 * наружу через границу DLL передаются только C-функции extern "C" (см. ниже).
 * Если бы класс был экспортирован, std::string поля давали бы C4251 — это
 * корректное предупреждение, и убирать его pragma-ой плохая идея.
 */
class Mod {
public:
    static Mod& Instance();

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    ModState GetState() const;
    bool IsInitialized() const;

    std::string GetVersion() const;
    std::string GetName() const;

    void SetModuleHandle(HMODULE module) { m_module = module; }
    HMODULE GetModuleHandle() const { return m_module; }

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

} // namespace kastol

// Функции экспорта DLL — снаружи namespace, чтобы имена не были покорёжены C++ mangling.
extern "C" {
    KASTOL_API bool InitializeKastol();
    KASTOL_API void ShutdownKastol();
    KASTOL_API const char* GetKastolVersion();
}
