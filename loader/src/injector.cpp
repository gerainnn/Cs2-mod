/**
 * @file injector.cpp
 * @brief Реализация инжектора DLL модификации
 */

#include "injector.h"
#include "logger.h"
#include <tlhelp32.h>

namespace cs2_mod {
namespace loader {

Injector::Injector()
    : m_injectedModule(nullptr) {
}

Injector::~Injector() {
}

bool Injector::Inject(HANDLE processHandle, const std::wstring& dllPath) {
    if (processHandle == nullptr) {
        m_lastError = L"Неверный дескриптор процесса";
        LOG_ERROR("Injector: неверный дескриптор процесса");
        return false;
    }

    LOG_INFO(("Инжектирование: " + std::string(dllPath.begin(), dllPath.end())).c_str());

    // Получаем адрес LoadLibraryW
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 == nullptr) {
        m_lastError = L"Не удалось получить kernel32.dll";
        LOG_ERROR("Не удалось получить kernel32.dll");
        return false;
    }

    FARPROC loadLibraryAddr = GetProcAddress(kernel32, "LoadLibraryW");
    if (loadLibraryAddr == nullptr) {
        m_lastError = L"Не удалось получить адрес LoadLibraryW";
        LOG_ERROR("Не удалось получить адрес LoadLibraryW");
        return false;
    }

    // Выделяем память под путь к DLL в процессе игры
    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remotePath = AllocateRemoteMemory(processHandle, pathSize);
    
    if (remotePath == nullptr) {
        m_lastError = L"Не удалось выделить память в процессе";
        LOG_ERROR("Не удалось выделить память в процессе");
        return false;
    }

    // Записываем путь к DLL в выделенную память
    if (!WriteProcessMemory(processHandle, remotePath, dllPath.c_str(), pathSize, nullptr)) {
        m_lastError = L"Не удалось записать путь в память процесса";
        LOG_ERROR("Не удалось записать путь в память процесса");
        FreeRemoteMemory(processHandle, remotePath);
        return false;
    }

    // Создаем удаленный поток для вызова LoadLibraryW
    HANDLE remoteThread = CreateRemoteThread(
        processHandle,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddr),
        remotePath,
        0,
        nullptr
    );

    if (remoteThread == nullptr) {
        m_lastError = L"Не удалось создать удаленный поток";
        LOG_ERROR("Не удалось создать удаленный поток");
        FreeRemoteMemory(processHandle, remotePath);
        return false;
    }

    // Ожидаем завершения инжекции
    WaitForSingleObject(remoteThread, INFINITE);

    // Получаем результат
    DWORD exitCode = 0;
    GetExitCodeThread(remoteThread, &exitCode);
    
    CloseHandle(remoteThread);
    FreeRemoteMemory(processHandle, remotePath);

    if (exitCode == 0) {
        m_lastError = L"LoadLibraryW вернул NULL";
        LOG_ERROR("LoadLibraryW вернул NULL - DLL не загружена");
        return false;
    }

    m_injectedModule = reinterpret_cast<HMODULE>(static_cast<UINT_PTR>(exitCode));
    LOG_INFO("Модификация успешно инжектирована");
    return true;
}

bool Injector::Eject(HANDLE processHandle) {
    if (m_injectedModule == nullptr) {
        m_lastError = L"Модификация не загружена";
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC freeLibraryAddr = GetProcAddress(kernel32, "FreeLibrary");

    if (freeLibraryAddr == nullptr) {
        m_lastError = L"Не удалось получить адрес FreeLibrary";
        return false;
    }

    HANDLE remoteThread = CreateRemoteThread(
        processHandle,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(freeLibraryAddr),
        m_injectedModule,
        0,
        nullptr
    );

    if (remoteThread == nullptr) {
        m_lastError = L"Не удалось создать поток для выгрузки";
        return false;
    }

    WaitForSingleObject(remoteThread, INFINITE);
    CloseHandle(remoteThread);

    m_injectedModule = nullptr;
    LOG_INFO("Модификация выгружена из процесса");
    return true;
}

bool Injector::IsModLoaded(HANDLE processHandle) const {
    return m_injectedModule != nullptr;
}

std::wstring Injector::GetLastErrorString() const {
    return m_lastError;
}

LPVOID Injector::AllocateRemoteMemory(HANDLE processHandle, size_t size) {
    return VirtualAllocEx(processHandle, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

bool Injector::FreeRemoteMemory(HANDLE processHandle, LPVOID address) {
    return VirtualFreeEx(processHandle, address, 0, MEM_RELEASE) != FALSE;
}

} // namespace loader
} // namespace cs2_mod
