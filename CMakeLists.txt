cmake_minimum_required(VERSION 3.20)
project(KASTOL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Определения для Windows
if(WIN32)
    add_definitions(-D_WIN32)
    add_definitions(-DWIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

# Подпроекты
add_subdirectory(loader)
add_subdirectory(mod)

# Документация
message(STATUS "")
message(STATUS "=== KASTOL Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "=============================")
message(STATUS "")
