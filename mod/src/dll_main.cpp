/**
 * @file dll_main.cpp
 * @brief Точка входа DLL модификации
 * 
 * Этот файл содержит точку входа DLL и экспортируемые функции,
 * которые вызываются загрузчиком при инжекции.
 */

#include <windows.h>
#include "mod/core.h"
#include "mod/logger.h"

using namespace cs2_mod;

/**
 * @brief Точка входа DLL
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Отключаем уведомления о потоках
            DisableThreadLibraryCalls(hModule);
            
            // Сохраняем дескриптор модуля
            // Mod::Instance().SetModule(hModule);
            break;
            
        case DLL_PROCESS_DETACH:
            // Завершаем работу при выгрузке
            Mod::Instance().Shutdown();
            break;
    }
    return TRUE;
}

/**
 * @brief Инициализировать модификацию
 * 
 * Эта функция вызывается загрузчиком после инжекции DLL.
 */
extern "C" MOD_API bool InitializeMod() {
    // Инициализация логгера
    Logger::Instance().Initialize("cs2_mod.log");
    LOG_INFO("=== Инициализация CS2 Mod ===");
    
    // Инициализация модификации
    if (!Mod::Instance().Initialize()) {
        LOG_ERROR("Не удалось инициализировать модификацию");
        return false;
    }
    
    LOG_INFO("Модификация успешно инициализирована");
    return true;
}

/**
 * @brief Завершить работу модификации
 */
extern "C" MOD_API void ShutdownMod() {
    LOG_INFO("Завершение работы модификации");
    Mod::Instance().Shutdown();
    LOG_INFO("Модификация выгружена");
}

/**
 * @brief Получить версию модификации
 */
extern "C" MOD_API const char* GetModVersion() {
    return Mod::Instance().GetVersion().c_str();
}
