# API Документация KASTOL

## 📚 Справочник по API

Полная документация по всем классам и методам мода KASTOL.

---

## Содержание

1. [Core API](#core-api)
2. [Overlay API](#overlay-api)
3. [Config API](#config-api)
4. [Memory API](#memory-api)
5. [Logger API](#logger-api)

---

## Core API

### Класс `Mod`

Основной класс мода KASTOL. Управляет жизненным циклом модификации.

```cpp
namespace kastol {

class Mod {
public:
    // Получить экземпляр мода (Singleton)
    static Mod& Instance();
    
    // Инициализация мода
    bool Initialize();
    
    // Завершение работы
    void Shutdown();
    
    // Обновление (каждый кадр)
    void Update(float deltaTime);
    
    // Получить состояние
    ModState GetState() const;
    
    // Проверить инициализацию
    bool IsInitialized() const;
    
    // Получить версию
    std::string GetVersion() const;
    
    // Получить имя мода
    std::string GetName() const;
};

}
```

### Пример использования

```cpp
// Инициализация
if (!Mod::Instance().Initialize()) {
    LOG_ERROR("Не удалось инициализировать KASTOL");
    return;
}

// Проверка состояния
if (Mod::Instance().IsInitialized()) {
    Mod::Instance().Update(deltaTime);
}

// Завершение
Mod::Instance().Shutdown();
```

---

## Overlay API

### Класс `Overlay`

Управляет overlay-интерфейсом поверх игры.

```cpp
namespace kastol {

class Overlay {
public:
    static Overlay& Instance();
    
    bool Initialize(HWND hwnd);
    void Shutdown();
    
    void BeginFrame();
    void Render();
    void EndFrame();
    
    void SetVisible(bool visible);
    bool IsVisible() const;
    void Toggle();
    
    void UpdateSize(int width, int height);
    void GetSize(int& width, int& height) const;
    
    bool IsMouseOver() const;
    bool IsCapturingInput() const;
};

}
```

### Пример использования

```cpp
// Инициализация
Overlay::Instance().Initialize(gameHwnd);

// Переключение видимости
Overlay::Instance().Toggle();

// Рендеринг в цикле
Overlay::Instance().BeginFrame();
Overlay::Instance().Render();
Overlay::Instance().EndFrame();
```

---

## Config API

### Класс `Config`

Система конфигурации мода.

```cpp
namespace kastol {

class Config {
public:
    static Config& Instance();
    
    bool Initialize(const std::string& configPath);
    bool Load();
    bool Save();
    void Reset();
    
    // Геттеры
    bool GetBool(const std::string& key, bool defaultValue = false) const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Сеттеры
    void SetBool(const std::string& key, bool value);
    void SetInt(const std::string& key, int value);
    void SetFloat(const std::string& key, float value);
    void SetString(const std::string& key, const std::string& value);
};

}
```

### Пример использования

```cpp
// Инициализация
Config::Instance().Initialize("config/config.json");

// Чтение значений
bool enabled = Config::Instance().GetBool("feature.enabled", false);
int value = Config::Instance().GetInt("feature.value", 100);

// Запись значений
Config::Instance().SetBool("feature.enabled", true);
Config::Instance().Save();
```

---

## Memory API

### Класс `Memory`

Утилиты для работы с памятью процесса.

```cpp
namespace kastol {

class Memory {
public:
    // Поиск паттернов
    static uintptr_t FindPattern(const char* moduleName, const char* pattern);
    
    // Чтение/запись
    static bool Read(uintptr_t address, void* buffer, size_t size);
    static bool Write(uintptr_t address, const void* data, size_t size);
    
    // Модули
    static uintptr_t GetModuleBase(const char* moduleName);
    static size_t GetModuleSize(const char* moduleName);
};

}
```

### Пример использования

```cpp
// Получить базовый адрес модуля
uintptr_t clientBase = Memory::GetModuleBase("client.dll");

// Найти паттерн
uintptr_t patternAddr = Memory::FindPattern("client.dll", "48 89 5C 24 ?? 48 89 74 24 ??");

// Чтение значения
int health;
Memory::Read(healthAddress, &health, sizeof(health));

// Запись значения
int newHealth = 100;
Memory::Write(healthAddress, &newHealth, sizeof(newHealth));
```

---

## Logger API

### Класс `Logger`

Система логирования.

```cpp
namespace kastol {

class Logger {
public:
    static Logger& Instance();
    
    bool Initialize(const std::string& logPath);
    
    void Log(LogLevel level, const std::string& message);
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
};

// Макросы
#define LOG_DEBUG(msg) Logger::Instance().Debug(msg)
#define LOG_INFO(msg) Logger::Instance().Info(msg)
#define LOG_WARNING(msg) Logger::Instance().Warning(msg)
#define LOG_ERROR(msg) Logger::Instance().Error(msg)

}
```

### Пример использования

```cpp
// Инициализация
Logger::Instance().Initialize("kastol.log");

// Использование макросов
LOG_DEBUG("Детальное сообщение для отладки");
LOG_INFO("Инициализация завершена");
LOG_WARNING("Низкий FPS");
LOG_ERROR("Не удалось загрузить файл");
```

---

## Полный пример инициализации

```cpp
#include "mod/core.h"
#include "mod/config.h"
#include "mod/overlay.h"
#include "mod/logger.h"

using namespace kastol;

bool InitializeMod() {
    // 1. Логгер
    Logger::Instance().Initialize("kastol.log");
    LOG_INFO("=== Инициализация KASTOL ===");
    
    // 2. Конфигурация
    if (!Config::Instance().Initialize("config.json")) {
        LOG_WARNING("Конфигурация не найдена");
    }
    
    // 3. Overlay
    HWND gameHwnd = FindWindowW(nullptr, L"Counter-Strike 2");
    if (!gameHwnd) {
        LOG_ERROR("Не найдено окно CS2");
        return false;
    }
    
    if (!Overlay::Instance().Initialize(gameHwnd)) {
        LOG_ERROR("Не удалось инициализировать overlay");
        return false;
    }
    
    LOG_INFO("KASTOL успешно инициализирован!");
    return true;
}
```

---

<div align="center">

**KASTOL API Documentation v1.0.0**

</div>
