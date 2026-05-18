/**
 * @file dll_main.cpp
 * @brief Точка входа DLL модификации KASTOL.
 *
 * Содержит DllMain и экспортируемые C-функции, которые вызывает loader
 * после инжекции DLL в процесс игры.
 */

#include <windows.h>
#include "mod/core.h"
#include "mod/logger.h"
#include "mod/utils.h"

namespace {

/// Стартует инициализацию мода в отдельном потоке, чтобы не блокировать
/// загрузчик и не выполнять тяжёлую работу прямо в DllMain.
DWORD WINAPI KastolInitThread(LPVOID lpParameter) {
    HMODULE selfModule = static_cast<HMODULE>(lpParameter);
    kastol::Mod::Instance().SetModuleHandle(selfModule);

    // Лог рядом с DLL (по абсолютному пути), а не в CWD процесса игры.
    const std::string logPath = kastol::GetModuleFilePath("kastol.log");
    kastol::Logger::Instance().Initialize(logPath);

    LOG_INFO("=== Инициализация KASTOL ===");

    if (!kastol::Mod::Instance().Initialize()) {
        LOG_ERROR("Не удалось инициализировать модификацию");
        return 1;
    }

    LOG_INFO("Модификация успешно инициализирована");
    return 0;
}

} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*reserved*/) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            // Инициализация в отдельном потоке: безопасно для DllMain.
            CreateThread(nullptr, 0, KastolInitThread, hModule, 0, nullptr);
            break;

        case DLL_PROCESS_DETACH:
            // На detach не делаем тяжёлый I/O: процесс уже умирает.
            kastol::Mod::Instance().Shutdown();
            break;
    }
    return TRUE;
}

// ---- Экспортируемые функции (на случай, если loader захочет вызвать их вручную) ----

extern "C" KASTOL_API bool InitializeKastol() {
    return kastol::Mod::Instance().Initialize();
}

extern "C" KASTOL_API void ShutdownKastol() {
    kastol::Mod::Instance().Shutdown();
}

extern "C" KASTOL_API const char* GetKastolVersion() {
    static std::string version = kastol::Mod::Instance().GetVersion();
    return version.c_str();
}
