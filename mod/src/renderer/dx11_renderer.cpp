/**
 * @file dx11_renderer.cpp
 * @brief Реализация DirectX 11 рендерера
 */

#include "mod/dx11_renderer.h"
#include "mod/logger.h"

namespace cs2_mod {

DX11Renderer& DX11Renderer::Instance() {
    static DX11Renderer instance;
    return instance;
}

DX11Renderer::DX11Renderer()
    : m_hwnd(nullptr)
    , m_initialized(false)
    , m_device(nullptr)
    , m_context(nullptr)
    , m_swapChain(nullptr)
    , m_renderTargetView(nullptr)
    , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
    , m_width(0)
    , m_height(0) {
}

DX11Renderer::~DX11Renderer() {
    Shutdown();
}

bool DX11Renderer::Initialize(HWND hwnd) {
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Инициализация DirectX 11 рендерера...");
    
    m_hwnd = hwnd;
    
    if (!CreateDeviceAndSwapChain()) {
        LOG_ERROR("Не удалось создать устройство D3D11");
        return false;
    }
    
    if (!CreateRenderTargetView()) {
        LOG_ERROR("Не удалось создать render target");
        CleanupResources();
        return false;
    }
    
    m_initialized = true;
    LOG_INFO("DirectX 11 рендерер инициализирован");
    return true;
}

void DX11Renderer::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Завершение работы DirectX 11 рендерера...");
    
    CleanupResources();
    
    m_initialized = false;
    LOG_INFO("DirectX 11 рендерер остановлен");
}

void DX11Renderer::BeginFrame() {
    if (!m_initialized) return;
    
    // Очистка back buffer
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void DX11Renderer::EndFrame() {
    if (!m_initialized) return;
    
    // Презентация
    m_swapChain->Present(1, 0);
}

bool DX11Renderer::CreateDeviceAndSwapChain() {
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;
    
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferCount = 2;
    scDesc.BufferDesc.Width = m_width;
    scDesc.BufferDesc.Height = m_height;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = m_hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Windowed = TRUE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &scDesc,
        &m_swapChain,
        &m_device,
        &m_featureLevel,
        &m_context
    );
    
    if (FAILED(hr)) {
        LOG_ERROR(("D3D11CreateDeviceAndSwapChain failed: 0x" + std::to_string(hr)).c_str());
        return false;
    }
    
    return true;
}

bool DX11Renderer::CreateRenderTargetView() {
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr)) {
        return false;
    }
    
    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();
    
    if (FAILED(hr)) {
        return false;
    }
    
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    return true;
}

void DX11Renderer::CleanupResources() {
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
    if (m_context) {
        m_context->Release();
        m_context = nullptr;
    }
    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
}

ID3D11Device* DX11Renderer::GetDevice() const {
    return m_device;
}

ID3D11DeviceContext* DX11Renderer::GetContext() const {
    return m_context;
}

IDXGISwapChain* DX11Renderer::GetSwapChain() const {
    return m_swapChain;
}

void DX11Renderer::OnResize(int width, int height) {
    if (!m_initialized) return;
    
    m_width = width;
    m_height = height;
    
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
    
    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTargetView();
}

void DX11Renderer::GetScreenSize(int& width, int& height) const {
    width = m_width;
    height = m_height;
}

} // namespace cs2_mod
