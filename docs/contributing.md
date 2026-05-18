# Руководство для разработчиков KASTOL

## 🎯 Добро пожаловать в команду KASTOL!

Это руководство поможет вам быстро разобраться в архитектуре мода и начать вносить свой вклад в проект.

---

## 📋 Содержание

1. [Введение](#введение)
2. [Архитектура](#архитектура)
3. [Настройка окружения](#настройка-окружения)
4. [Стандарты кода](#стандарты-кода)
5. [Добавление новых функций](#добавление-новых-функций)
6. [Тестирование](#тестирование)
7. [Pull Request процесс](#pull-request-процесс)

---

## Введение

### Что такое KASTOL?

**KASTOL** - это модификация для Counter-Strike 2, состоящая из двух компонентов:

| Компонент | Тип | Назначение |
|-----------|-----|------------|
| `kastol_loader.exe` | EXE | Запускает CS2 и инжектирует мод |
| `kastol.dll` | DLL | Создаёт overlay-интерфейс и обрабатывает ввод |

### Ключевые технологии

- **C++20** - основной язык программирования
- **DirectX 11** - рендеринг overlay
- **ImGui** - UI библиотека
- **MinHook** - библиотека для хуков
- **Steam API** - интеграция со Steam

---

## Архитектура

### Обзор архитектуры

```
┌─────────────────────────────────────────────────────────────┐
│                    KASTOL LOADER (EXE)                       │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │    Steam     │  │   Injector   │  │   Updater    │      │
│  │   Launcher   │  │              │  │              │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                           │
                           │ Инжекция
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                    KASTOL MOD (DLL)                          │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                      Mod Core                         │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐   │  │
│  │  │  Config  │  │  Logger  │  │    Memory        │   │  │
│  │  └──────────┘  └──────────┘  └──────────────────┘   │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                    Overlay System                     │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐   │  │
│  │  │ Overlay  │  │  Input   │  │    ImGui         │   │  │
│  │  │          │  │ Handler  │  │    Impl          │   │  │
│  │  └──────────┘  └──────────┘  └──────────────────┘   │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                   Renderer (DX11)                     │  │
│  │  ┌──────────────────┐  ┌──────────────────────────┐ │  │
│  │  │  DX11 Renderer   │  │    SwapChain Hooks      │ │  │
│  │  └──────────────────┘  └──────────────────────────┘ │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Жизненный цикл мода

```
1. ЗАПУСК LOADER
   │
   ├── Инициализация логгера
   ├── Проверка обновлений
   ├── Инициализация Steam API
   │
2. ЗАПУСК CS2
   │
   ├── Проверка: CS2 уже запущена?
   │   ├── Да → Переход к шагу 3
   │   └── Нет → Запуск через steam://rungameid/730
   │
   ├── Ожидание процесса cs2.exe (таймаут 60 сек)
   │
3. ИНЖЕКЦИЯ DLL
   │
   ├── OpenProcess(PROCESS_ALL_ACCESS)
   ├── VirtualAllocEx (выделение памяти)
   ├── WriteProcessMemory (запись пути к DLL)
   ├── CreateRemoteThread(LoadLibraryW)
   └── WaitForSingleObject (ожидание загрузки)
   │
4. ИНИЦИАЛИЗАЦИЯ МОДА
   │
   ├── DllMain(DLL_PROCESS_ATTACH)
   ├── InitializeMod()
   │   ├── Инициализация подсистем
   │   │   ├── Config::Initialize()
   │   │   ├── Logger::Initialize()
   │   │   └── Overlay::Initialize()
   │   │
   │   └── Установка DirectX хуков
   │       ├── Hook: IDXGISwapChain::Present
   │       └── Hook: IDXGISwapChain::ResizeBuffers
   │
5. РАБОТА МОДА
   │
   ├── Перехват Present()
   │   ├── BeginFrame()
   │   ├── ImGui::NewFrame()
   │   ├── Overlay::Render()
   │   ├── ImGui::Render()
   │   └── EndFrame()
   │
   └── Обработка ввода
       ├── Перехват WM_*
       └── InputHandler::HandleMessage()
   │
6. ЗАВЕРШЕНИЕ
   │
   ├── DllMain(DLL_PROCESS_DETACH)
   ├── ShutdownMod()
   │   ├── Сохранение конфигурации
   │   ├── Снятие хуков
   │   └── Освобождение ресурсов
   │
   └── Выгрузка DLL (FreeLibrary)
```

---

## Настройка окружения

### Необходимые инструменты

| Инструмент | Версия | Ссылка |
|------------|--------|--------|
| Visual Studio 2022 | 17.0+ | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) |
| CMake | 3.20+ | [cmake.org](https://cmake.org/) |
| Git | latest | [git-scm.com](https://git-scm.com/) |

### Компоненты Visual Studio

Убедитесь, что установлены:
- ✅ Разработка классических приложений на C++
- ✅ Windows 10/11 SDK
- ✅ C++ CMake tools for Windows

### Клонирование и настройка

```bash
# 1. Клонирование
git clone https://github.com/gerainnn/Cs2-mod.git
cd Cs2-mod

# 2. Создание структуры external
mkdir external
cd external

# 3. Загрузка ImGui
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout docking  # рекомендуемая ветка
cd ..

# 4. Загрузка MinHook
git clone https://github.com/TsudaKageyu/minhook.git
cd ..

# 5. Steam SDK (если есть доступ)
# Распакуйте steam_api.h и steam_api64.dll в external/steam/
```

### Структура external/

```
external/
├── imgui/
│   ├── imgui.h
│   ├── imgui.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_tables.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_impl_dx11.h
│   ├── imgui_impl_dx11.cpp
│   ├── imgui_impl_win32.h
│   └── imgui_impl_win32.cpp
│
├── minhook/
│   ├── include/MinHook.h
│   └── lib/MinHook.x64.lib
│
└── steam/
    ├── steam_api.h
    └── steam_api64.dll
```

---

## Стандарты кода

### Именование

| Тип | Стиль | Пример |
|-----|-------|--------|
| Классы | PascalCase | `class OverlayManager` |
| Функции | PascalCase | `void RenderOverlay()` |
| Переменные | camelCase | `int menuWidth` |
| Константы | UPPER_SNAKE_CASE | `MAX_OVERLAY_SIZE` |
| Приватные члены | m_ префикс | `m_isInitialized` |
| Параметры | camelCase | `void SetWidth(int newWidth)` |

### Пространства имён

```cpp
namespace kastol {

namespace overlay {
    class Overlay { /* ... */ };
}

namespace renderer {
    class DX11Renderer { /* ... */ };
}

} // namespace kastol
```

### Форматирование

```cpp
// Пример правильно отформатированного кода

namespace kastol {

class ExampleClass {
public:
    ExampleClass();
    ~ExampleClass();

    // Публичные методы
    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();

    // Геттеры
    int GetValue() const { return m_value; }
    bool IsInitialized() const { return m_initialized; }

private:
    // Приватные методы
    void ProcessInternal();
    bool ValidateInput(int value);

    // Приватные члены
    bool m_initialized;
    int m_value;
    std::string m_name;
};

} // namespace kastol
```

### Документирование кода

```cpp
/**
 * @file overlay.h
 * @brief Overlay-интерфейс мода KASTOL
 * 
 * Управляет отображением меню поверх игры.
 */

namespace kastol {

/**
 * @brief Класс overlay-интерфейса
 * 
 * Отвечает за рендеринг меню и обработку ввода пользователя.
 * 
 * @example
 * @code
 * auto& overlay = Overlay::Instance();
 * overlay.Initialize(hwnd);
 * overlay.SetVisible(true);
 * @endcode
 */
class Overlay {
public:
    /**
     * @brief Инициализировать overlay
     * 
     * @param hwnd Дескриптор окна игры
     * @return true если инициализация успешна
     */
    bool Initialize(HWND hwnd);
    
    /**
     * @brief Отобразить overlay
     * 
     * Вызывается каждый кадр для отрисовки UI.
     */
    void Render();
};

} // namespace kastol
```

---

## Добавление новых функций

### Создание нового компонента

#### 1. Создание заголовочного файла

```cpp
// mod/include/mod/new_feature.h
#pragma once

#include <string>

namespace kastol {

/**
 * @brief Описание новой функции
 */
class NewFeature {
public:
    static NewFeature& Instance();
    
    bool Initialize();
    void Update(float deltaTime);
    void Shutdown();
    
private:
    NewFeature();
    ~NewFeature();
    
    bool m_initialized;
};

} // namespace kastol
```

#### 2. Создание файла реализации

```cpp
// mod/src/features/new_feature.cpp
#include "mod/new_feature.h"
#include "mod/logger.h"

namespace kastol {

NewFeature& NewFeature::Instance() {
    static NewFeature instance;
    return instance;
}

NewFeature::NewFeature()
    : m_initialized(false) {
}

bool NewFeature::Initialize() {
    if (m_initialized) return true;
    
    LOG_INFO("Инициализация NewFeature...");
    
    // Код инициализации
    
    m_initialized = true;
    return true;
}

void NewFeature::Update(float deltaTime) {
    if (!m_initialized) return;
    
    // Код обновления
}

void NewFeature::Shutdown() {
    if (!m_initialized) return;
    
    LOG_INFO("Завершение NewFeature...");
    
    // Код очистки
    
    m_initialized = false;
}

} // namespace kastol
```

#### 3. Добавление в CMakeLists.txt

```cmake
# mod/CMakeLists.txt
set(MOD_SOURCES
    # ... существующие файлы
    src/features/new_feature.cpp
)

set(MOD_HEADERS
    # ... существующие файлы
    include/mod/new_feature.h
)
```

#### 4. Интеграция в Mod::Initialize()

```cpp
// mod/src/core/mod.cpp

#include "mod/new_feature.h"

bool Mod::InitializeSubsystems() {
    // ... существующий код
    
    // Инициализация новой функции
    if (!NewFeature::Instance().Initialize()) {
        LOG_ERROR("Не удалось инициализировать NewFeature");
        return false;
    }
    
    return true;
}
```

### Добавление пункта меню

```cpp
// В overlay.cpp

void Overlay::RenderMainMenu() {
    ImGui::Begin("KASTOL Menu");
    
    // Существующие пункты
    if (ImGui::CollapsingHeader("Основные")) {
        // ...
    }
    
    // Новый пункт меню
    if (ImGui::CollapsingHeader("Новая функция")) {
        auto& feature = NewFeature::Instance();
        
        bool enabled = feature.IsEnabled();
        if (ImGui::Checkbox("Включить функцию", &enabled)) {
            feature.SetEnabled(enabled);
        }
        
        int value = feature.GetValue();
        if (ImGui::SliderInt("Значение", &value, 0, 100)) {
            feature.SetValue(value);
        }
    }
    
    ImGui::End();
}
```

---

## Тестирование

### Сборка в Debug режиме

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

### Отладка в Visual Studio

1. Откройте `build/KASTOL.sln`
2. Установите конфигурацию Debug
3. Установите `kastol_loader` как запускаемый проект
4. F5 для запуска с отладкой

### Логирование

Все логи сохраняются в:
- Loader: `kastol_loader.log`
- Mod: `kastol_mod.log`

Уровни логирования:
- `LOG_DEBUG` - детальная отладка
- `LOG_INFO` - информационные сообщения
- `LOG_WARNING` - предупреждения
- `LOG_ERROR` - ошибки

---

## Pull Request процесс

### Перед созданием PR

1. ✅ Убедитесь, что код компилируется без предупреждений
2. ✅ Протестируйте изменения в игре
3. ✅ Обновите документацию, если необходимо
4. ✅ Следуйте стандартам кода

### Шаблон PR

```markdown
## Описание
Краткое описание изменений.

## Тип изменений
- [ ] Bug fix
- [ ] New feature
- [ ] Refactoring
- [ ] Documentation

## Тестирование
Опишите, как тестировали изменения.

## Скриншоты
Если применимо, добавьте скриншоты.

## Checklist
- [ ] Код следует стандартам проекта
- [ ] Документация обновлена
- [ ] Тесты пройдены
```

### Code Review

Все PR проходят code review. Ожидайте:
- Проверку архитектуры
- Проверку стиля кода
- Проверку безопасности
- Тестирование в игре

---

## Частые вопросы

### Q: Как добавить новую вкладку в меню?

A: См. раздел "Добавление пункта меню" выше.

### Q: Как работать с памятью игры?

A: Используйте класс `Memory` из `mod/include/mod/memory.h`:

```cpp
// Найти паттерн
uintptr_t address = Memory::FindPattern("client.dll", "48 89 5C 24 ?? 48 89 74 24 ??");

// Прочитать значение
int value;
Memory::Read(address, &value, sizeof(value));

// Записать значение
int newValue = 100;
Memory::Write(address, &newValue, sizeof(newValue));
```

### Q: Как добавить конфигурацию?

A: Добавьте в `config/config.json` и используйте `Config` класс:

```cpp
// Чтение
int value = Config::Instance().GetInt("myFeature.value", 0);

// Запись
Config::Instance().SetInt("myFeature.value", 100);
Config::Instance().Save();
```

---

## Контакты

- 💬 Discord: [KASTOL Community](https://discord.gg/kastol)
- 📧 Email: dev@kastol.mod

---

<div align="center">

**Удачи в разработке! 🚀**

</div>
