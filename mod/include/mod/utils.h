#pragma once

/**
 * @file utils.h
 * @brief Общие утилиты
 */

#include <string>
#include <vector>
#include <windows.h>

namespace kastol {

/**
 * @brief Преобразовать широкую строку в обычную
 */
std::string WideToUtf8(const std::wstring& wide);

/**
 * @brief Преобразовать обычную строку в широкую
 */
std::wstring Utf8ToWide(const std::string& utf8);

/**
 * @brief Получить текущую директорию модуля
 */
std::string GetModuleDirectory();

/**
 * @brief Получить путь к файлу в директории модуля
 */
std::string GetModuleFilePath(const std::string& fileName);

/**
 * @brief Проверить существование файла
 */
bool FileExists(const std::string& path);

/**
 * @brief Прочитать файл в строку
 */
std::string ReadFileToString(const std::string& path);

/**
 * @brief Записать строку в файл
 */
bool WriteStringToFile(const std::string& path, const std::string& content);

/**
 * @brief Получить текущее время в миллисекундах
 */
uint64_t GetTickCountMs();

/**
 * @brief Форматировать строку
 */
std::string FormatString(const char* fmt, ...);

/**
 * @brief Хеширование строки (FNV-1a)
 */
constexpr uint32_t HashString(const char* str) {
    uint32_t hash = 0x811c9dc5;
    while (*str) {
        hash ^= static_cast<uint8_t>(*str++);
        hash *= 0x01000193;
    }
    return hash;
}

/**
 * @brief Обертка для VirtualProtect
 */
class MemoryProtectGuard {
public:
    MemoryProtectGuard(void* address, size_t size, DWORD newProtect);
    ~MemoryProtectGuard();
    
    bool IsValid() const { return m_valid; }
    
private:
    void* m_address;
    size_t m_size;
    DWORD m_oldProtect;
    bool m_valid;
};

} // namespace kastol
