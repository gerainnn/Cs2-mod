/**
 * @file utils.cpp
 * @brief Реализация общих утилит
 */

#include "mod/utils.h"
#include <cstdarg>
#include <cstdio>
#include <windows.h>

namespace kastol {

// Получаем HMODULE текущей DLL — нужен для пути к DLL, а не к EXE-процессу.
// Используем трюк с адресом любой функции внутри этой DLL.
static HMODULE GetSelfModule() {
    HMODULE module = nullptr;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&GetSelfModule),
        &module);
    return module;
}

std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return std::string();
    
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size <= 1) return std::string();
    std::string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();
    
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (size <= 1) return std::wstring();
    std::wstring result(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], size);
    return result;
}

std::string GetModuleDirectory() {
    char buffer[MAX_PATH] = {};
    // Берём именно нашу DLL, а не EXE процесса (важно после инжекции в cs2.exe).
    GetModuleFileNameA(GetSelfModule(), buffer, MAX_PATH);
    std::string path(buffer);
    auto pos = path.find_last_of("\\/");
    if (pos == std::string::npos) {
        return std::string(".");
    }
    return path.substr(0, pos);
}

std::string GetModuleFilePath(const std::string& fileName) {
    return GetModuleDirectory() + "\\" + fileName;
}

bool FileExists(const std::string& path) {
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string ReadFileToString(const std::string& path) {
    std::string content;
    FILE* file = nullptr;
    fopen_s(&file, path.c_str(), "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        content.resize(size);
        fread(&content[0], 1, size, file);
        fclose(file);
    }
    return content;
}

bool WriteStringToFile(const std::string& path, const std::string& content) {
    FILE* file = nullptr;
    fopen_s(&file, path.c_str(), "wb");
    if (file) {
        fwrite(content.c_str(), 1, content.size(), file);
        fclose(file);
        return true;
    }
    return false;
}

uint64_t GetTickCountMs() {
    return GetTickCount64();
}

std::string FormatString(const char* fmt, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return std::string(buffer);
}

MemoryProtectGuard::MemoryProtectGuard(void* address, size_t size, DWORD newProtect)
    : m_address(address)
    , m_size(size)
    , m_oldProtect(0)
    , m_valid(false) {
    m_valid = VirtualProtect(address, size, newProtect, &m_oldProtect) != FALSE;
}

MemoryProtectGuard::~MemoryProtectGuard() {
    if (m_valid) {
        VirtualProtect(m_address, m_size, m_oldProtect, &m_oldProtect);
    }
}

} // namespace kastol
