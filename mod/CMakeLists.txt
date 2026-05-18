cmake_minimum_required(VERSION 3.20)
project(KASTOL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка для Windows DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

# Исходные файлы модификации
set(MOD_SOURCES
    src/dll_main.cpp
    src/core/mod.cpp
    src/core/config.cpp
    src/overlay/overlay.cpp
    src/overlay/input_handler.cpp
    src/renderer/dx11_renderer.cpp
    src/renderer/imgui_impl.cpp
    src/steam/steam_integration.cpp
    src/utils/memory.cpp
    src/utils/utils.cpp
    src/utils/logger.cpp
)

set(MOD_HEADERS
    include/mod/core.h
    include/mod/config.h
    include/mod/overlay.h
    include/mod/input_handler.h
    include/mod/dx11_renderer.h
    include/mod/imgui_impl.h
    include/mod/steam_integration.h
    include/mod/memory.h
    include/mod/utils.h
    include/mod/logger.h
)

# Создание DLL
add_library(kastol SHARED ${MOD_SOURCES} ${MOD_HEADERS})

# Включение директорий
target_include_directories(kastol PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/minhook
)

# Линковка с необходимыми библиотеками
target_link_libraries(kastol PRIVATE
    d3d11
    dxgi
    Psapi
    version
    imgui
    minhook
)

# Определения
target_compile_definitions(kastol PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    KASTOL_EXPORTS
)

# Установка output directory
set_target_properties(kastol PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)
