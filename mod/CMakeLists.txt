cmake_minimum_required(VERSION 3.20)

# Это вложенный CMakeLists — родительский уже сделал project()
# Не делаем второй project(), это путало бы конфигурацию.

# ---- Опции ----
# Эти опции пока выключены: ImGui и MinHook не используются в коде каркаса
# (все вызовы закомментированы). Включай их позже, когда будем интегрировать.
option(KASTOL_WITH_IMGUI   "Подключить ImGui (рендер UI оверлея)" OFF)
option(KASTOL_WITH_MINHOOK "Подключить MinHook (хуки DXGI Present)" OFF)

# ---- Исходники ----
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

# ---- DLL ----
add_library(kastol SHARED ${MOD_SOURCES} ${MOD_HEADERS})

# Имя файла без префикса lib (важно на MinGW), и итоговое имя — kastol.dll.
set_target_properties(kastol PROPERTIES
    PREFIX ""
    OUTPUT_NAME "kastol"
)

target_include_directories(kastol PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Системные библиотеки Windows — без них не слинкуется.
target_link_libraries(kastol PRIVATE
    d3d11
    dxgi
    Psapi
    version
)

target_compile_definitions(kastol PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    KASTOL_EXPORTS  # для __declspec(dllexport) в core.h
)

# ---- Опциональные зависимости (через FetchContent) ----
if(KASTOL_WITH_IMGUI)
    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.90.4
    )
    FetchContent_MakeAvailable(imgui)

    # Сами файлы ImGui — добавим вручную, потому что upstream не даёт CMake-таргет.
    target_sources(kastol PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
    target_include_directories(kastol PRIVATE
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )
    target_compile_definitions(kastol PRIVATE KASTOL_WITH_IMGUI=1)
endif()

if(KASTOL_WITH_MINHOOK)
    include(FetchContent)
    FetchContent_Declare(
        minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(minhook)
    target_link_libraries(kastol PRIVATE minhook)
    target_compile_definitions(kastol PRIVATE KASTOL_WITH_MINHOOK=1)
endif()

# ---- Output ----
set_target_properties(kastol PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
