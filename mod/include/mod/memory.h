#pragma once

/**
 * @file memory.h
 * @brief Утилиты для работы с памятью
 * 
 * Функции для работы с памятью процесса, сканирования паттернов и т.д.
 */

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

namespace kastol {

/**
 * @brief Класс для работы с памятью процесса
 */
class Memory {
public:
    /**
     * @brief Найти паттерн в памяти
     * @param moduleBase Базовый адрес модуля
     * @param moduleSize Размер модуля
     * @param pattern Паттерн для поиска (например "48 89 5C 24 ?? 48 89 74 24")
     * @return Адрес найденного паттерна или nullptr
     */
    static uintptr_t FindPattern(uintptr_t moduleBase, size_t moduleSize, const char* pattern);

    /**
     * @brief Найти паттерн в модуле
     * @param moduleName Имя модуля
     * @param pattern Паттерн для поиска
     * @return Адрес найденного паттерна или 0
     */
    static uintptr_t FindPattern(const char* moduleName, const char* pattern);

    /**
     * @brief Прочитать память
     * @param address Адрес для чтения
     * @param buffer Буфер для данных
     * @param size Размер данных
     * @return true если успешно
     */
    static bool Read(uintptr_t address, void* buffer, size_t size);

    /**
     * @brief Записать в память
     * @param address Адрес для записи
     * @param data Данные для записи
     * @param size Размер данных
     * @return true если успешно
     */
    static bool Write(uintptr_t address, const void* data, size_t size);

    /**
     * @brief Получить базовый адрес модуля
     * @param moduleName Имя модуля
     * @return Базовый адрес или 0
     */
    static uintptr_t GetModuleBase(const char* moduleName);

    /**
     * @brief Получить размер модуля
     * @param moduleName Имя модуля
     * @return Размер модуля или 0
     */
    static size_t GetModuleSize(const char* moduleName);

    /**
     * @brief Вычислить относительный адрес
     * @param address Базовый адрес инструкции
     * @param offset Смещение до относительного адреса
     * @param instructionSize Размер инструкции
     * @return Абсолютный адрес
     */
    static uintptr_t ResolveRelativeAddress(uintptr_t address, uint32_t offset, uint32_t instructionSize);

    /**
     * @brief Защитить память для записи
     * @param address Адрес
     * @param size Размер
     * @param oldProtect Старая защита
     * @return true если успешно
     */
    static bool ProtectMemory(uintptr_t address, size_t size, DWORD newProtect, DWORD* oldProtect);

private:
    /**
     * @brief Конвертировать строку паттерна в байты
     */
    static std::vector<std::pair<uint8_t, bool>> PatternToBytes(const char* pattern);
};

} // namespace kastol
