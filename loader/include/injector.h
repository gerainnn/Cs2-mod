#pragma once

#include <string>
#include <windows.h>

namespace cs2_mod {
namespace loader {

/**
 * @brief Класс для инжектирования DLL модификации в процесс CS2
 * 
 * Отвечает за загрузку модификации в адресное пространство игры.
 */
class Injector {
public:
    Injector();
    ~Injector();

    /**
     * @brief Инжектировать DLL модификации в процесс
     * @param processHandle Дескриптор процесса CS2
     * @param dllPath Путь к DLL модификации
     * @return true если инжекция успешна
     */
    bool Inject(HANDLE processHandle, const std::wstring& dllPath);

    /**
     * @brief Выгрузить DLL из процесса
     * @param processHandle Дескриптор процесса
     * @return true если выгрузка успешна
     */
    bool Eject(HANDLE processHandle);

    /**
     * @brief Проверить, загружена ли модификация
     * @param processHandle Дескриптор процесса
     * @return true если модификация загружена
     */
    bool IsModLoaded(HANDLE processHandle) const;

    /**
     * @brief Получить последнее сообщение об ошибке
     * @return Строка с описанием ошибки
     */
    std::wstring GetLastErrorString() const;

private:
    HMODULE m_injectedModule;
    std::wstring m_lastError;

    /**
     * @brief Выделить память в процессе
     * @param processHandle Дескриптор процесса
     * @param size Размер памяти
     * @return Указатель на выделенную память
     */
    LPVOID AllocateRemoteMemory(HANDLE processHandle, size_t size);

    /**
     * @brief Освободить память в процессе
     * @param processHandle Дескриптор процесса
     * @param address Адрес памяти
     * @return true если успешно
     */
    bool FreeRemoteMemory(HANDLE processHandle, LPVOID address);
};

} // namespace loader
} // namespace cs2_mod
