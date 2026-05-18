# Сборка CS2 Mod

## Требования

- Windows 10/11
- Visual Studio 2022 (17.0 или новее)
- CMake 3.20 или новее
- Windows SDK 10.0

## Зависимости

Для полноценной сборки понадобятся внешние библиотеки:

1. **ImGui** - UI библиотека
2. **MinHook** - библиотека для хуков
3. **Steam API SDK** - для интеграции со Steam

### Установка зависимостей

```bash
# Создайте директорию external
mkdir external
cd external

# Клонируйте ImGui
git clone https://github.com/ocornut/imgui.git

# Клонируйте MinHook
git clone https://github.com/TsudaKageyu/minhook.git
```

## Сборка с помощью CMake

```bash
# Создайте директорию сборки
mkdir build
cd build

# Конфигурация
cmake .. -G "Visual Studio 17 2022" -A x64

# Сборка
cmake --build . --config Release
```

## Сборка в Visual Studio

1. Откройте `build/CS2ModProject.sln`
2. Выберите конфигурацию Release
3. Соберите проект (Build -> Build Solution)

## Выходные файлы

После сборки в `build/bin/Release/` будут находиться:
- `cs2_mod_loader.exe` - Загрузчик
- `cs2_mod.dll` - Модификация

## Структура зависимостей

```
Cs2-mod/
├── external/
│   ├── imgui/
│   │   ├── imgui.h
│   │   ├── imgui.cpp
│   │   ├── imgui_impl_dx11.h
│   │   ├── imgui_impl_dx11.cpp
│   │   ├── imgui_impl_win32.h
│   │   └── imgui_impl_win32.cpp
│   ├── minhook/
│   │   ├── MinHook.h
│   │   └── ...
│   └── steam/
│       ├── steam_api.h
│       ├── steam_api64.dll
│       └── ...
```

## Отладка

Для отладки установите конфигурацию Debug:

```bash
cmake --build . --config Debug
```

## Замечания

- Убедитесь, что Steam запущен перед использованием загрузчика
- CS2 должна быть установлена через Steam
- Для работы нужен Steam SDK (steam_api64.dll)
