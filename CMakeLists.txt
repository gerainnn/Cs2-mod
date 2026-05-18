cmake_minimum_required(VERSION 3.20)
project(KASTOL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Только Windows (DXGI/D3D11/PSAPI/CreateRemoteThread).
if(NOT WIN32)
    message(FATAL_ERROR
        "KASTOL собирается только под Windows (нужны DXGI / D3D11 / kernel32). "
        "Используй Visual Studio 2022 или MinGW-w64.")
endif()

# Для DLL — только x64 (cs2.exe тоже x64).
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "KASTOL поддерживает только x64. Перегенерируй с -A x64.")
endif()

# По умолчанию Release, чтобы не собирать дебажный жирный билд случайно.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Глобальные дефайны Windows.
add_compile_definitions(
    _WIN32
    WIN32
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

# MSVC специфика.
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    # /MP — параллельная компиляция; /utf-8 — корректные русские строки в исходниках.
    add_compile_options(/MP /utf-8)
endif()

add_subdirectory(loader)
add_subdirectory(mod)

message(STATUS "")
message(STATUS "=== KASTOL Configuration ===")
message(STATUS "Version       : ${PROJECT_VERSION}")
message(STATUS "Build Type    : ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard  : ${CMAKE_CXX_STANDARD}")
message(STATUS "Generator     : ${CMAKE_GENERATOR}")
message(STATUS "Compiler      : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "============================")
message(STATUS "")
