#pragma once

/**
 * @file dx11_renderer.h
 * @brief DirectX 11 рендерер для overlay
 * 
 * Управляет DirectX 11 устройством и цепочкой обмена,
 * используется для отрисовки overlay-интерфейса.
 */

#include <d3d11.h>
#include <dxgi.h>
#include <memory>

namespace kastol {

/**
 * @brief Класс рендерера DirectX 11
 */
class DX11Renderer {
public:
    static DX11Renderer& Instance();

    /**
     * @brief Инициализировать рендерер
     * @param hwnd Дескриптор окна
     * @return true если успешно
     */
    bool Initialize(HWND hwnd);

    /**
     * @brief Завершить работу рендерера
     */
    void Shutdown();

    /**
     * @brief Начать кадр
     */
    void BeginFrame();

    /**
     * @brief Завершить кадр и презентовать
     */
    void EndFrame();

    /**
     * @brief Получить устройство D3D11
     */
    ID3D11Device* GetDevice() const;

    /**
     * @brief Получить контекст устройства
     */
    ID3D11DeviceContext* GetContext() const;

    /**
     * @brief Получить цепочку обмена
     */
    IDXGISwapChain* GetSwapChain() const;

    /**
     * @brief Обновить размеры при изменении окна
     */
    void OnResize(int width, int height);

    /**
     * @brief Создать render target view
     */
    bool CreateRenderTargetView();

    /**
     * @brief Получить размеры экрана
     */
    void GetScreenSize(int& width, int& height) const;

private:
    DX11Renderer();
    ~DX11Renderer();
    DX11Renderer(const DX11Renderer&) = delete;
    DX11Renderer& operator=(const DX11Renderer&) = delete;

    HWND m_hwnd;
    bool m_initialized;
    
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    
    D3D_FEATURE_LEVEL m_featureLevel;
    int m_width;
    int m_height;

    /**
     * @brief Инициализировать устройство D3D11
     */
    bool CreateDeviceAndSwapChain();

    /**
     * @brief Освободить ресурсы
     */
    void CleanupResources();
};

} // namespace kastol
