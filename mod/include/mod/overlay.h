#pragma once

/**
 * @file overlay.h
 * @brief Overlay-интерфейс модификации
 * 
 * Управляет отображением меню модификации поверх игры.
 * Обеспечивает рендеринг UI и обработку ввода.
 */

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <windows.h>

namespace kastol {

/**
 * @brief Структура для хранения информации об окне
 */
struct OverlayWindow {
    std::string name;
    float x, y;
    float width, height;
    bool isOpen;
    bool hasCloseButton;
    std::function<void()> renderCallback;
};

/**
 * @brief Класс overlay-интерфейса
 */
class Overlay {
public:
    static Overlay& Instance();

    /**
     * @brief Инициализировать overlay
     * @param hwnd Дескриптор окна игры
     * @return true если успешно
     */
    bool Initialize(HWND hwnd);

    /**
     * @brief Завершить работу overlay
     */
    void Shutdown();

    /**
     * @brief Начать новый кадр
     */
    void BeginFrame();

    /**
     * @brief Отобразить overlay
     */
    void Render();

    /**
     * @brief Завершить кадр
     */
    void EndFrame();

    /**
     * @brief Установить видимость overlay
     */
    void SetVisible(bool visible);

    /**
     * @brief Проверить видимость overlay
     */
    bool IsVisible() const;

    /**
     * @brief Переключить видимость overlay
     */
    void Toggle();

    /**
     * @brief Обновить размеры overlay
     */
    void UpdateSize(int width, int height);

    /**
     * @brief Получить размеры overlay
     */
    void GetSize(int& width, int& height) const;

    /**
     * @brief Добавить окно в overlay
     */
    void AddWindow(const OverlayWindow& window);

    /**
     * @brief Удалить окно из overlay
     */
    void RemoveWindow(const std::string& name);

    /**
     * @brief Проверить, наведен ли курсор на overlay
     */
    bool IsMouseOver() const;

    /**
     * @brief Проверить, перехватывает ли overlay ввод
     */
    bool IsCapturingInput() const;

private:
    Overlay();
    ~Overlay();
    Overlay(const Overlay&) = delete;
    Overlay& operator=(const Overlay&) = delete;

    HWND m_hwnd;
    bool m_initialized;
    bool m_visible;
    int m_width, m_height;
    
    std::vector<OverlayWindow> m_windows;
    
    // Состояние ввода
    bool m_mouseOver;
    bool m_capturingInput;

    /**
     * @brief Рендеринг главного меню
     */
    void RenderMainMenu();

    /**
     * @brief Рендеринг панели настроек
     */
    void RenderSettingsPanel();

    /**
     * @brief Рендеринг информационной панели
     */
    void RenderInfoPanel();

    /**
     * @brief Рендеринг вкладок меню
     */
    void RenderTabs();

    /**
     * @brief Обновить состояние ввода
     */
    void UpdateInputState();
};

} // namespace kastol
