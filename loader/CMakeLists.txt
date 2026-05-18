cmake_minimum_required(VERSION 3.20)

# Опционально подключить Steam SDK — выключено, т.к. SDK в репо нет.
# Когда положишь его в external/steam_sdk, включи и пропиши пути.
option(KASTOL_WITH_STEAM_SDK "Линковать со Steamworks SDK" OFF)

set(LOADER_SOURCES
    src/main.cpp
    src/steam_launcher.cpp
    src/injector.cpp
    src/updater.cpp
)

set(LOADER_HEADERS
    include/steam_launcher.h
    include/injector.h
    include/updater.h
    include/logger.h
)

# Для логгера loader-а нужен один .cpp с реализацией Logger.
# В исходниках нет logger.cpp для loader — используем header-only fallback,
# складываем минимальную реализацию рядом.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp")
    message(STATUS "loader: logger.cpp отсутствует — будет создан минимальный stub")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/loader_logger_stub.cpp" "
#include \"logger.h\"
#include <iomanip>
#include <sstream>
#include <windows.h>
namespace cs2_mod { namespace loader {
Logger& Logger::Instance() { static Logger i; return i; }
Logger::Logger() : m_initialized(false) {}
Logger::~Logger() { if (m_logFile.is_open()) m_logFile.close(); }
bool Logger::Initialize(const std::string& p) {
    std::lock_guard<std::mutex> lg(m_mutex);
    m_logFile.open(p, std::ios::out | std::ios::app);
    m_initialized = m_logFile.is_open();
    return m_initialized;
}
void Logger::Log(LogLevel l, const std::string& m) {
    std::lock_guard<std::mutex> lg(m_mutex);
    if (!m_initialized) return;
    m_logFile << \"[\" << GetTimestamp() << \"] [\" << LevelToString(l) << \"] \" << m << \"\\n\";
    m_logFile.flush();
}
void Logger::Debug(const std::string& m)   { Log(LogLevel::Debug, m); }
void Logger::Info(const std::string& m)    { Log(LogLevel::Info, m); }
void Logger::Warning(const std::string& m) { Log(LogLevel::Warning, m); }
void Logger::Error(const std::string& m)   { Log(LogLevel::Error, m); }
std::string Logger::LevelToString(LogLevel l) const {
    switch (l) {
        case LogLevel::Debug: return \"DEBUG\";
        case LogLevel::Info: return \"INFO\";
        case LogLevel::Warning: return \"WARNING\";
        case LogLevel::Error: return \"ERROR\";
    }
    return \"?\";
}
std::string Logger::GetTimestamp() const {
    SYSTEMTIME st; GetLocalTime(&st);
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << st.wDay << '.'
       << std::setw(2) << st.wMonth << '.'
       << std::setw(4) << st.wYear << ' '
       << std::setw(2) << st.wHour << ':'
       << std::setw(2) << st.wMinute << ':'
       << std::setw(2) << st.wSecond;
    return ss.str();
}
} }
")
    list(APPEND LOADER_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/loader_logger_stub.cpp")
endif()

add_executable(kastol_loader WIN32 ${LOADER_SOURCES} ${LOADER_HEADERS})

target_include_directories(kastol_loader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(kastol_loader PRIVATE
    Psapi
    version
    Shell32
)

if(KASTOL_WITH_STEAM_SDK)
    # Положи Steamworks SDK в external/steam_sdk и раскомментируй:
    # target_include_directories(kastol_loader PRIVATE ${CMAKE_SOURCE_DIR}/external/steam_sdk/public)
    # target_link_directories(kastol_loader PRIVATE ${CMAKE_SOURCE_DIR}/external/steam_sdk/redistributable_bin/win64)
    target_link_libraries(kastol_loader PRIVATE steam_api64)
    target_compile_definitions(kastol_loader PRIVATE KASTOL_WITH_STEAM_SDK=1)
endif()

target_compile_definitions(kastol_loader PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

set_target_properties(kastol_loader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
