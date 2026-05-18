/**
 * @file mod.cpp
 * @brief Реализация ядра модификации
 */

#include "mod/core.h"
#include "mod/config.h"
#include "mod/overlay.h"
#include "mod/logger.h"
#include <windows.h>

namespace cs2_mod {

Mod& Mod::Instance() {
    static Mod instance;
    return instance;
}

Mod::Mod()
    : m_state(ModState::Uninitialized)
    , m_version("1.0.0")
    , m_name("CS2 Mod")
    , m_module(nullptr) {
}

Mod::~Mod() {
    if (m_state == ModState::Running) {
        Shutdown();
    }
}

bool Mod::Initialize() {
    if (m_state != ModState::Uninitialized) {
        LOG_WARNING("Модификация уже инициализирована");
        return true;
    }
    
    m_state = ModState::Initializing;
    LOG_INFO(("Инициализация " + m_name + " v" + m_version).c_str());

    // Инициализация подсистем
    if (!InitializeSubsystems()) {
        m_state = ModState::Error;
        LOG_ERROR("Не удалось инициализировать подсистемы");
        return false;
    }

    // Установка DirectX хуков
    if (!InstallDirectXHooks()) {
        m_state = ModState::Error;
        LOG_ERROR("Не удалось установить DirectX хуки");
        return false;
    }

    m_state = ModState::Running;
    LOG_INFO("Модификация успешно запущена");
    return true;
}

void Mod::Shutdown() {
    if (m_state != ModState::Running) {
        return;
    }
    
    m_state = ModState::ShuttingDown;
    LOG_INFO("Завершение работы модификации...");

    // Завершение работы подсистем
    ShutdownSubsystems();

    m_state = ModState::Uninitialized;
    LOG_INFO("Модификация остановлена");
}

void Mod::Update(float deltaTime) {
    if (m_state != ModState::Running) {
        return;
    }

    // Обновление конфигурации
    Config::Instance();
    
    // Обновление overlay
    Overlay::Instance();
}

ModState Mod::GetState() const {
    return m_state;
}

bool Mod::IsInitialized() const {
    return m_state == ModState::Running;
}

std::string Mod::GetVersion() const {
    return m_version;
}

std::string Mod::GetName() const {
    return m_name;
}

bool Mod::InitializeSubsystems() {
    LOG_INFO("Инициализация подсистем...");

    // Инициализация конфигурации
    if (!Config::Instance().Initialize("config.json")) {
        LOG_WARNING("Не удалось загрузить конфигурацию, используются значения по умолчанию");
    }

    // Инициализация overlay
    // Overlay будет инициализирован после получения HWND игры
    
    LOG_INFO("Подсистемы инициализированы");
    return true;
}

void Mod::ShutdownSubsystems() {
    LOG_INFO("Завершение работы подсистем...");

    // Сохранение конфигурации
    if (Config::Instance().settings.saveOnExit) {
        Config::Instance().Save();
    }

    // Завершение overlay
    Overlay::Instance().Shutdown();

    LOG_INFO("Подсистемы остановлены");
}

bool Mod::CreateRenderThread() {
    LOG_INFO("Создание потока рендеринга...");
    
    // В реальной реализации создается отдельный поток
    // для рендеринга overlay
    
    return true;
}

bool Mod::InstallDirectXHooks() {
    LOG_INFO("Установка DirectX хуков...");
    
    // В реальной реализации здесь устанавливаются хуки
    // на DirectX функции для перехвата рендеринга
    
    // Используем MinHook для установки хуков на:
    // - IDXGISwapChain::Present
    // - IDXGISwapChain::ResizeBuffers
    // - ID3D11DeviceContext::DrawIndexed
    
    LOG_INFO("DirectX хуки установлены");
    return true;
}

} // namespace cs2_mod
