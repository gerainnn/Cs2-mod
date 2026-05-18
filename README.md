# KASTOL - Модификация для Counter-Strike 2

<div align="center">

![KASTOL Logo](resources/logo.png)

**KASTOL** - современная клиентская модификация для Counter-Strike 2

[Возможности](#возможности) • [Установка](#установка) • [Сборка](#сборка) • [Документация](docs/architecture.md)

</div>

---

## 📋 Описание проекта

**KASTOL** - это профессиональная клиентская модификация для Counter-Strike 2, предоставляющая расширенные возможности через интуитивный overlay-интерфейс. Мод полностью написан на C++20 с использованием современных технологий и паттернов проектирования.

### 🎯 Назначение

KASTOL создан для:
- Предоставления удобного overlay-интерфейса для взаимодействия с игрой
- Демонстрации современных техник программирования модификаций
- Обучения работе с DirectX 11, ImGui и Steam API
- Создания расширяемой платформы для разработки модов

## ✨ Возможности

- 🎮 **Автоматический запуск CS2** через Steam API
- 🖼️ **Overlay-интерфейс** поверх игры на базе ImGui
- 🖱️ **Полная обработка ввода** - клики, нажатия клавиш
- ⚙️ **Гибкая система конфигурации** с сохранением настроек
- 🔄 **Автоматическое обновление** модификации
- 📊 **Система логирования** всех действий
- 🎨 **DirectX 11 рендеринг** с поддержкой VSync

---

## 🗂️ Структура проекта

```
KASTOL/
├── loader/                 # 🚀 Загрузчик (EXE)
│   ├── src/               
│   │   ├── main.cpp           # Точка входа
│   │   ├── steam_launcher.cpp # Запуск игры через Steam
│   │   ├── injector.cpp       # Инжектирование мода
│   │   └── updater.cpp        # Система обновлений
│   ├── include/           
│   │   ├── steam_launcher.h
│   │   ├── injector.h
│   │   ├── updater.h
│   │   └── logger.h
│   └── resources/         
│
├── mod/                    # 🎨 Модификация (DLL)
│   ├── src/               
│   │   ├── dll_main.cpp       # Точка входа DLL
│   │   ├── core/              # Ядро мода
│   │   │   ├── mod.cpp        # Основной класс KASTOL
│   │   │   └── config.cpp     # Система конфигурации
│   │   ├── overlay/           # Overlay-интерфейс
│   │   │   ├── overlay.cpp    # Главное меню
│   │   │   └── input_handler.cpp
│   │   ├── renderer/          # Рендеринг
│   │   │   ├── dx11_renderer.cpp
│   │   │   └── imgui_impl.cpp
│   │   ├── steam/             # Steam интеграция
│   │   └── utils/             # Утилиты
│   ├── include/mod/       
│   │   ├── core.h
│   │   ├── config.h
│   │   ├── overlay.h
│   │   ├── input_handler.h
│   │   ├── dx11_renderer.h
│   │   ├── imgui_impl.h
│   │   ├── steam_integration.h
│   │   ├── memory.h
│   │   ├── utils.h
│   │   └── logger.h
│   └── resources/         
│
├── config/                 # ⚙️ Конфигурация
│   └── config.json
│
├── docs/                   # 📚 Документация
│   ├── architecture.md
│   ├── build.md
│   ├── contributing.md     # Руководство для разработчиков
│   └── api.md             # API документация
│
├── resources/              # 📦 Ресурсы
│   ├── fonts/
│   └── images/
│
├── CMakeLists.txt         
├── README.md              
└── LICENSE                
```

---

## 🏗️ Компоненты

### Loader (Загрузчик KASTOL)

**Назначение:** Обеспечивает запуск CS2 и загрузку мода в процесс игры.

| Компонент | Описание |
|-----------|----------|
| `SteamLauncher` | Запуск CS2 через Steam API (App ID: 730) |
| `Injector` | Инжектирование `kastol.dll` в процесс игры |
| `Updater` | Проверка и загрузка обновлений мода |
| `Logger` | Система логирования всех событий |

### Mod (Модификация KASTOL)

**Назначение:** Создаёт overlay-интерфейс и обеспечивает взаимодействие с пользователем.

| Компонент | Описание |
|-----------|----------|
| `Mod` | Основной класс, управляющий жизненным циклом мода |
| `Overlay` | Overlay-меню поверх игры |
| `DX11Renderer` | DirectX 11 рендерер для отрисовки UI |
| `ImGuiImpl` | Интеграция ImGui для интерфейса |
| `InputHandler` | Обработка кликов и нажатий клавиш |
| `Config` | Система конфигурации и настроек |
| `Memory` | Утилиты для работы с памятью процесса |

---

## 🚀 Установка

### Предварительные требования

- ✅ Windows 10/11 (64-bit)
- ✅ Counter-Strike 2 (установлен в Steam)
- ✅ Steam Client (запущен)
- ✅ Visual C++ Redistributable 2022

### Быстрая установка

1. Скачайте последнюю версию KASTOL из [Releases](https://github.com/gerainnn/Cs2-mod/releases)
2. Распакуйте архив в любую папку
3. Убедитесь, что Steam запущен
4. Запустите `kastol_loader.exe`
5. Дождитесь запуска CS2 и загрузки мода
6. Нажмите **INSERT** для открытия/закрытия меню

### Сборка из исходного кода

См. [Инструкцию по сборке](docs/build.md)

---

## 🔧 Сборка

### Требования для сборки

| Инструмент | Версия |
|------------|--------|
| Visual Studio | 2022 (17.0+) |
| CMake | 3.20+ |
| Windows SDK | 10.0 |
| C++ Standard | C++20 |

### Зависимости

Мод использует следующие библиотеки:

| Библиотека | Назначение | Ссылка |
|------------|------------|--------|
| **ImGui** | UI библиотека для overlay | [github.com/ocornut/imgui](https://github.com/ocornut/imgui) |
| **MinHook** | Библиотека для хуков | [github.com/TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook) |
| **Steam API** | Интеграция со Steam | [partner.steamgames.com](https://partner.steamgames.com/) |

### Инструкция по сборке

```bash
# 1. Клонируйте репозиторий
git clone https://github.com/gerainnn/Cs2-mod.git
cd Cs2-mod

# 2. Создайте директорию external и скачайте зависимости
mkdir external
cd external

# ImGui
git clone https://github.com/ocornut/imgui.git

# MinHook
git clone https://github.com/TsudaKageyu/minhook.git

cd ..

# 3. Создайте директорию сборки
mkdir build
cd build

# 4. Конфигурация проекта
cmake .. -G "Visual Studio 17 2022" -A x64

# 5. Сборка
cmake --build . --config Release

# 6. Результаты сборки
# build/bin/Release/kastol_loader.exe
# build/bin/Release/kastol.dll
```

Подробная инструкция: [docs/build.md](docs/build.md)

---

## 📖 Использование

### Запуск мода

```
┌─────────────────────────────────────────┐
│  1. Запустите Steam                      │
│  2. Запустите kastol_loader.exe          │
│  3. Дождитесь запуска CS2                │
│  4. Мод автоматически загрузится         │
│  5. Нажмите INSERT для открытия меню     │
└─────────────────────────────────────────┘
```

### Горячие клавиши

| Клавиша | Действие |
|---------|----------|
| `INSERT` | Открыть/закрыть меню |
| `END` | Выгрузить мод из памяти |

### Конфигурация

Файл конфигурации: `config/config.json`

```json
{
    "mod": {
        "name": "KASTOL",
        "version": "1.0.0"
    },
    "menu": {
        "toggleKey": 45,
        "transparency": 0.95
    }
}
```

---

## 📚 Документация

| Документ | Описание |
|----------|----------|
| [Архитектура](docs/architecture.md) | Подробное описание архитектуры KASTOL |
| [Сборка](docs/build.md) | Инструкция по сборке проекта |
| [Contributing](docs/contributing.md) | Руководство для разработчиков |
| [API Reference](docs/api.md) | Документация API |

---

## 🤝 Вклад в проект

Мы приветствуем вклад в развитие KASTOL! 

### Как внести вклад

1. Форкните репозиторий
2. Создайте ветку для вашей функции (`git checkout -b feature/amazing-feature`)
3. Закоммитьте изменения (`git commit -m 'Add amazing feature'`)
4. Запушьте ветку (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

См. [CONTRIBUTING.md](docs/contributing.md) для подробностей.

---

## 📝 Лицензия

Этот проект распространяется под лицензией MIT. См. [LICENSE](LICENSE) для подробностей.

---

## ⚠️ Disclaimer

**Важно:** Данный проект создан исключительно в образовательных целях. 

- 🚫 Использование модификаций в сетевых играх может нарушать правила игры
- 🚫 Может привести к блокировке игрового аккаунта
- ✅ Используйте только на свой страх и риск
- ✅ Авторы не несут ответственности за последствия использования

---

## 👥 Авторы

- **Разработчик** - [gerainnn](https://github.com/gerainnn)

---

## 📞 Поддержка

- 📧 Email: support@kastol.mod
- 💬 Discord: [KASTOL Community](https://discord.gg/kastol)
- 📝 Issues: [GitHub Issues](https://github.com/gerainnn/Cs2-mod/issues)

---

<div align="center">

**KASTOL** - Professional CS2 Modification Framework

Made with ❤️ by the KASTOL Team

</div>