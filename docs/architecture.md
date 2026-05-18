# Архитектура KASTOL

## Обзор

**KASTOL** - это профессиональная клиентская модификация для Counter-Strike 2, состоящая из двух основных компонентов:

### 1. Loader (Загрузчик) - EXE

**Файл:** `kastol_loader.exe`

Отвечает за:
- Инициализацию Steam API
- Запуск CS2 через Steam (App ID: 730)
- Инжектирование `kastol.dll` в процесс игры
- Проверку обновлений мода

### 2. Mod (Модификация) - DLL

**Файл:** `kastol.dll`

Отвечает за:
- Создание overlay-интерфейса поверх игры
- Рендеринг UI (ImGui + DirectX 11)
- Обработку пользовательского ввода
- Управление конфигурацией

## Структура проекта

```
KASTOL/
├── loader/                    # Загрузчик (EXE)
│   ├── src/
│   │   ├── main.cpp          # Точка входа
│   │   ├── steam_launcher.cpp# Запуск CS2
│   │   ├── injector.cpp      # Инжектирование DLL
│   │   └── updater.cpp       # Проверка обновлений
│   └── include/
│       ├── steam_launcher.h
│       ├── injector.h
│       ├── updater.h
│       └── logger.h
│
├── mod/                       # Модификация (DLL)
│   ├── src/
│   │   ├── dll_main.cpp      # Точка входа DLL
│   │   ├── core/
│   │   │   ├── mod.cpp       # Основной класс KASTOL
│   │   │   └── config.cpp    # Система конфигурации
│   │   ├── overlay/
│   │   │   ├── overlay.cpp   # Overlay-интерфейс
│   │   │   └── input_handler.cpp
│   │   ├── renderer/
│   │   │   ├── dx11_renderer.cpp
│   │   │   └── imgui_impl.cpp
│   │   ├── steam/
│   │   │   └── steam_integration.cpp
│   │   └── utils/
│   │       ├── memory.cpp
│   │       ├── utils.cpp
│   │       └── logger.cpp
│   └── include/mod/
│       ├── core.h
│       ├── config.h
│       ├── overlay.h
│       ├── input_handler.h
│       ├── dx11_renderer.h
│       ├── imgui_impl.h
│       ├── steam_integration.h
│       ├── memory.h
│       ├── utils.h
│       └── logger.h
│
├── config/                    # Конфигурационные файлы
│   └── config.json
│
├── docs/                      # Документация
│   ├── architecture.md
│   ├── build.md
│   ├── contributing.md
│   └── api.md
│
├── resources/                 # Ресурсы
│
├── CMakeLists.txt            
├── README.md                 
└── LICENSE                   
```

## Поток выполнения

1. **Запуск загрузчика**
   - Инициализация логгера
   - Проверка обновлений
   - Инициализация Steam API

2. **Запуск CS2**
   - Проверка, запущена ли игра
   - Если нет - запуск через Steam protocol
   - Ожидание полного запуска процесса

3. **Инжектирование DLL**
   - Открытие процесса CS2
   - Выделение памяти под путь к DLL
   - Создание удаленного потока с LoadLibrary
   - Ожидание завершения инжекции

4. **Инициализация модификации**
   - Вызов InitializeMod() из DLL
   - Инициализация подсистем
   - Установка DirectX хуков
   - Создание overlay

5. **Работа модификации**
   - Перехват Present() для рендеринга
   - Обработка ввода пользователя
   - Отрисовка overlay-меню

## Безопасность

⚠️ **Важно**: Данная модификация предназначена исключительно для образовательных целей. Использование подобных модификаций в сетевых играх может привести к блокировке аккаунта.

## Лицензия

MIT License
