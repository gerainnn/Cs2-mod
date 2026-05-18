#pragma once

/**
 * @file imgui_impl.h
 * @brief ImGui реализация для overlay
 * 
 * Интеграция ImGui с DirectX 11 и Win32.
 */

#include <memory>

namespace cs2_mod {

/**
 * @brief Класс ImGui реализации
 */
class ImGuiImpl {
public:
    static ImGuiImpl& Instance();

    /**
     * @brief Инициализировать ImGui
     * @param hwnd Дескриптор окна
     * @param device Устройство D3D11
     * @param context Контекст устройства
     * @return true если успешно
     */
    bool Initialize(HWND hwnd, void* device, void* context);

    /**
     * @brief Завершить работу ImGui
     */
    void Shutdown();

    /**
     * @brief Начать новый кадр
     */
    void NewFrame();

    /**
     * @brief Отрисовать данные ImGui
     */
    void Render();

    /**
     * @brief Обновить размеры
     */
    void OnResize(int width, int height);

    /**
     * @brief Обработать событие Windows
     */
    bool HandleEvent(void* msg);

    /**
     * @brief Установить стиль
     */
    void SetupStyle();

    /**
     * @brief Установить шрифт
     */
    void SetupFonts();

private:
    ImGuiImpl();
    ~ImGuiImpl();
    ImGuiImpl(const ImGuiImpl&) = delete;
    ImGuiImpl& operator=(const ImGuiImpl&) = delete;

    bool m_initialized;
    HWND m_hwnd;
    void* m_device;
    void* m_context;
};

} // namespace cs2_mod
