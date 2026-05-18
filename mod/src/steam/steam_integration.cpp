/**
 * @file steam_integration.cpp
 * @brief Реализация интеграции со Steam
 */

#include "mod/steam_integration.h"
#include "mod/logger.h"

namespace cs2_mod {

SteamIntegration& SteamIntegration::Instance() {
    static SteamIntegration instance;
    return instance;
}

SteamIntegration::SteamIntegration()
    : m_initialized(false)
    , m_steamId(0) {
}

SteamIntegration::~SteamIntegration() {
    Shutdown();
}

bool SteamIntegration::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Инициализация Steam интеграции...");
    
    // В реальной реализации здесь используется Steam API
    // if (!SteamAPI_Init()) {
    //     LOG_ERROR("Не удалось инициализировать Steam API");
    //     return false;
    // }
    
    // m_steamId = SteamUser()->GetSteamID().ConvertToUint64();
    // m_username = SteamFriends()->GetPersonaName();
    
    m_initialized = true;
    LOG_INFO(("Steam интеграция инициализирована. Пользователь: " + m_username).c_str());
    return true;
}

void SteamIntegration::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Завершение Steam интеграции...");
    
    // SteamAPI_Shutdown();
    
    m_initialized = false;
    LOG_INFO("Steam интеграция остановлена");
}

void SteamIntegration::Update() {
    if (!m_initialized) {
        return;
    }
    
    // SteamAPI_RunCallbacks();
}

uint64_t SteamIntegration::GetSteamId() const {
    return m_steamId;
}

std::string SteamIntegration::GetUsername() const {
    return m_username;
}

uint32_t SteamIntegration::GetAppId() const {
    return 730; // CS2 App ID
}

bool SteamIntegration::IsSteamRunning() const {
    // В реальной реализации проверяем статус Steam
    // return SteamAPI_IsSteamRunning();
    return true;
}

std::string SteamIntegration::GetSteamInstallPath() const {
    // Получение пути к Steam через реестр или Steam API
    return "";
}

} // namespace cs2_mod
