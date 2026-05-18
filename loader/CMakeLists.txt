cmake_minimum_required(VERSION 3.20)
project(KASTOLLoader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка для Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

# Исходные файлы загрузчика
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

# Создание исполняемого файла
add_executable(kastol_loader WIN32 ${LOADER_SOURCES} ${LOADER_HEADERS})

# Включение директорий
target_include_directories(kastol_loader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Линковка с необходимыми библиотеками
target_link_libraries(kastol_loader PRIVATE
    steam_api64
    Psapi
    version
)

# Определения
target_compile_definitions(kastol_loader PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Установка output directory
set_target_properties(kastol_loader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)
