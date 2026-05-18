/**
 * @file steam_launcher.cpp
 * @brief Реализация запуска CS2 через Steam
 */

#include "steam_launcher.h"
#include "logger.h"
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>  // ShellExecuteW (с WIN32_LEAN_AND_MEAN не подтягивается автоматом)

namespace cs2_mod {
namespace loader {

// App ID Counter-Strike 2 в Steam
constexpr DWORD CS2_APP_ID = 730;

SteamLauncher::SteamLauncher()
    : m_initialized(false)
    , m_processHandle(nullptr)
    , m_processId(0) {
}

SteamLauncher::~SteamLauncher() {
    if (m_processHandle) {
        CloseHandle(m_processHandle);
    }
}

bool SteamLauncher::Initialize() {
    LOG_INFO("Инициализация Steam API...");
    
    // В реальной реализации здесь используется Steam API
    // SteamAPI_Init() и т.д.
    
    // Проверяем, запущен ли Steam
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE) {
        if (Process32FirstW(hProcessSnap, &pe32)) {
            do {
                if (std::wstring(pe32.szExeFile) == L"steam.exe") {
                    LOG_INFO("Steam найден в процессах");
                    m_initialized = true;
                    break;
                }
            } while (Process32NextW(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }
    
    if (!m_initialized) {
        LOG_WARNING("Steam не найден в запущенных процессах");
    }
    
    return m_initialized;
}

bool SteamLauncher::LaunchCS2() {
    LOG_INFO("Запуск Counter-Strike 2 через Steam...");
    
    // Формируем команду запуска через Steam
    std::wstring command = L"steam://rungameid/" + std::to_wstring(CS2_APP_ID);
    
    // Запускаем игру через Steam protocol
    HINSTANCE result = ShellExecuteW(
        nullptr,
        L"open",
        command.c_str(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );
    
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        LOG_ERROR("Не удалось запустить игру через Steam");
        return false;
    }
    
    LOG_INFO("Команда запуска отправлена в Steam");
    return true;
}

DWORD SteamLauncher::GetCS2ProcessId() const {
    return m_processId;
}

bool SteamLauncher::WaitForGameLaunch(DWORD timeout_ms) {
    LOG_INFO("Ожидание запуска процесса CS2...");
    
    DWORD startTime = GetTickCount();
    
    while (GetTickCount() - startTime < timeout_ms) {
        if (FindCS2Process()) {
            LOG_INFO("Процесс CS2 найден");
            return true;
        }
        Sleep(1000);
    }
    
    LOG_ERROR("Таймаут ожидания запуска CS2");
    return false;
}

bool SteamLauncher::IsCS2Running() const {
    // Локальный поиск без модификации m_processId/m_processHandle
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    bool found = false;

    if (Process32FirstW(hProcessSnap, &pe32)) {
        do {
            if (std::wstring(pe32.szExeFile) == L"cs2.exe") {
                found = true;
                break;
            }
        } while (Process32NextW(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return found;
}

std::wstring SteamLauncher::GetCS2Path() const {
    return ResolveCS2Path();
}

bool SteamLauncher::FindCS2Process() {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    bool found = false;
    
    if (Process32FirstW(hProcessSnap, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            if (processName == L"cs2.exe") {
                m_processId = pe32.th32ProcessID;
                // Закрываем предыдущий handle, если был — иначе утечка при повторном вызове.
                if (m_processHandle) {
                    CloseHandle(m_processHandle);
                    m_processHandle = nullptr;
                }
                m_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processId);
                found = true;
                break;
            }
        } while (Process32NextW(hProcessSnap, &pe32));
    }
    
    CloseHandle(hProcessSnap);
    return found;
}

std::wstring SteamLauncher::ResolveCS2Path() const {
    // Путь к CS2 через Steam library
    // В реальной реализации используется Steam API для получения пути
    
    wchar_t steamPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROGRAM_FILESX86, nullptr, 0, steamPath))) {
        std::wstring path = steamPath;
        path += L"\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\game\\bin\\win64\\cs2.exe";
        return path;
    }
    
    return L"";
}

} // namespace loader
} // namespace cs2_mod
