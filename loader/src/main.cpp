/**
 * @file main.cpp
 * @brief Точка входа загрузчика модификации CS2
 * 
 * Загрузчик отвечает за:
 * - Инициализацию логгера
 * - Запуск CS2 через Steam
 * - Инжектирование модификации
 * - Проверку обновлений
 */

#include <windows.h>
#include <string>
#include <iostream>
#include "steam_launcher.h"
#include "injector.h"
#include "updater.h"
#include "logger.h"

using namespace cs2_mod::loader;

/**
 * @brief Получить путь к DLL модификации
 */
std::wstring GetModDllPath() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    std::wstring exePath = std::wstring(buffer).substr(0, pos);
    return exePath + L"\\cs2_mod.dll";
}

/**
 * @brief Точка входа приложения
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Инициализация логгера
    if (!Logger::Instance().Initialize("cs2_mod_loader.log")) {
        MessageBoxA(nullptr, "Не удалось инициализировать логгер", "Ошибка", MB_ICONERROR);
        return 1;
    }

    LOG_INFO("=== Запуск загрузчика CS2 Mod ===");
    LOG_INFO("Версия: 1.0.0");

    // Проверка обновлений
    Updater updater;
    updater.SetUpdateUrl("https://api.example.com/cs2-mod/version");
    
    if (updater.CheckForUpdates()) {
        LOG_INFO("Доступно обновление!");
        LOG_INFO(("Текущая версия: " + updater.GetCurrentVersion()).c_str());
        LOG_INFO(("Новая версия: " + updater.GetLatestVersion()).c_str());
        
        int result = MessageBoxA(nullptr, 
            "Доступно обновление модификации. Обновить сейчас?",
            "Обновление", 
            MB_YESNO | MB_ICONQUESTION);
        
        if (result == IDYES) {
            if (updater.DownloadUpdate()) {
                LOG_INFO("Обновление успешно загружено");
                MessageBoxA(nullptr, "Обновление завершено!", "Обновление", MB_ICONINFORMATION);
            } else {
                LOG_ERROR("Ошибка загрузки обновления");
            }
        }
    }

    // Инициализация Steam и запуск CS2
    SteamLauncher steamLauncher;
    
    LOG_INFO("Инициализация Steam API...");
    if (!steamLauncher.Initialize()) {
        LOG_ERROR("Не удалось инициализировать Steam API");
        MessageBoxA(nullptr, 
            "Не удалось подключиться к Steam. Убедитесь, что Steam запущен.",
            "Ошибка Steam", 
            MB_ICONERROR);
        return 1;
    }
    LOG_INFO("Steam API инициализирован успешно");

    // Проверяем, запущена ли уже CS2
    if (steamLauncher.IsCS2Running()) {
        LOG_INFO("CS2 уже запущена");
    } else {
        LOG_INFO("Запуск Counter-Strike 2...");
        if (!steamLauncher.LaunchCS2()) {
            LOG_ERROR("Не удалось запустить CS2");
            MessageBoxA(nullptr, 
                "Не удалось запустить Counter-Strike 2.",
                "Ошибка запуска", 
                MB_ICONERROR);
            return 1;
        }
        
        LOG_INFO("Ожидание запуска игры...");
        if (!steamLauncher.WaitForGameLaunch(60000)) {
            LOG_ERROR("Таймаут ожидания запуска игры");
            MessageBoxA(nullptr, 
                "Игра не запустилась в течение отведенного времени.",
                "Ошибка", 
                MB_ICONERROR);
            return 1;
        }
        LOG_INFO("CS2 успешно запущена");
    }

    // Получаем путь к DLL модификации
    std::wstring modDllPath = GetModDllPath();
    LOG_INFO(("Путь к модификации: " + std::string(modDllPath.begin(), modDllPath.end())).c_str());

    // Инжектирование модификации
    Injector injector;
    HANDLE cs2Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, steamLauncher.GetCS2ProcessId());
    
    if (cs2Process == nullptr) {
        LOG_ERROR("Не удалось открыть процесс CS2");
        MessageBoxA(nullptr, 
            "Не удалось получить доступ к процессу CS2.",
            "Ошибка инжекции", 
            MB_ICONERROR);
        return 1;
    }

    LOG_INFO("Инжектирование модификации...");
    if (!injector.Inject(cs2Process, modDllPath)) {
        LOG_ERROR(("Ошибка инжекции: " + std::string(injector.GetLastErrorString().begin(), 
                     injector.GetLastErrorString().end())).c_str());
        MessageBoxA(nullptr, 
            "Не удалось загрузить модификацию в игру.",
            "Ошибка инжекции", 
            MB_ICONERROR);
        CloseHandle(cs2Process);
        return 1;
    }

    LOG_INFO("Модификация успешно загружена!");
    MessageBoxA(nullptr, 
        "Модификация успешно загружена!\n\nНажмите INSERT для открытия меню.",
        "CS2 Mod", 
        MB_ICONINFORMATION);

    // Освобождение ресурсов
    CloseHandle(cs2Process);

    LOG_INFO("=== Завершение работы загрузчика ===");
    return 0;
}
