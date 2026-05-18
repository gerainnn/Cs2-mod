# HANDOFF — состояние проекта на момент передачи

> Этот документ читает в первую очередь любой новый разработчик (или AI-агент в IDE),
> который продолжает работу над KASTOL. Здесь — что уже сделано, какие были ловушки,
> и с чего начинать. Технический план — в [ROADMAP.md](ROADMAP.md). Визуальная
> спецификация — в [DESIGN.md](DESIGN.md).

## TL;DR — где мы сейчас

- ✅ Каркас собирается на Windows x64 (MSVC / VS 2022) через CMake.
- ✅ CI на GitHub Actions, артефакты `kastol_loader.exe` + `kastol.dll` доступны.
- ✅ Loader корректно ищет CS2, инжектит DLL.
- ⚠️ DLL после инжекции **ничего не отображает** — это ожидаемо: ImGui и MinHook
  ещё не интегрированы (опции в CMake выключены, код заглушки закомментирован).
- ⚠️ Цель проекта — **визуально красивый оверлей-меню без читерских функций**.
  Никакого aim/wall/trigger/etc. Это не пожелание, а ограничение по дизайну
  проекта. Любые такие фичи отвергаются на code review.

## Что было фундаментально сломано до коммита `a0909c2`

Если кто-то будет смотреть историю — первый коммит `fix:` чинил кучу всего.
Не надо это всё повторять, но полезно знать про ловушки:

1. **Хаос namespace** — `core.h` был в `kastol::`, остальное в `cs2_mod::`.
   Линкер не находил символы. Сейчас всё в `kastol::`.
2. **Несуществующий макрос `MOD_API`** в `dll_main.cpp`. Использовался
   `KASTOL_API` из `core.h`. Имена экспортов: `InitializeKastol`, `ShutdownKastol`,
   `GetKastolVersion` (а не `*Mod` как было раньше).
3. **Логический deadlock в InputHandler** — toggle-клавиша обрабатывалась только
   когда меню открыто. Меню → не открыть → клавиша не обрабатывается. Починено:
   состояния клавиш обновляются всегда, блокировка только «съедает» событие для игры.
4. **ImGui/MinHook линковались как готовые таргеты, но физически их в репо не было**.
   Сейчас они подключаются через `FetchContent` под опциями
   `KASTOL_WITH_IMGUI=ON` / `KASTOL_WITH_MINHOOK=ON` (по умолчанию OFF).
5. **`<shellapi.h>` не подключался** — `WIN32_LEAN_AND_MEAN` отрезает его, а
   `ShellExecuteW` нужен в loader для `steam://rungameid/730`.
6. **`<windowsx.h>` не подключался** — нужен для `GET_X_LPARAM`/`GET_Y_LPARAM`.
7. **Класс `Mod` был `__declspec(dllexport)`** — это давало шумные C4251 на
   `std::string` полях через DLL-границу. Снято: класс живёт **внутри** DLL,
   наружу торчат только три `extern "C"` функции. Этого достаточно.
8. **Save() в деструкторе синглтона `Config`** — деструктор вызывается во время
   `DLL_PROCESS_DETACH`, а I/O там опасно. Сейчас Save делается явно.
9. **`GetModuleDirectory()` брал путь EXE процесса (cs2.exe)**, а не DLL.
   После инжекта это вело к попыткам писать лог в каталог CS2.
   Сейчас используется handle самой DLL.

## Архитектура на текущий момент

```
┌─────────────────────┐
│ kastol_loader.exe   │  cs2_mod::loader::*  (другой namespace, изолированно)
│  ├ SteamLauncher    │  ← steam://rungameid/730 + ожидание cs2.exe
│  ├ Injector         │  ← VirtualAllocEx + WriteProcessMemory + CreateRemoteThread
│  └ Updater          │  ← заглушка (HTTP не реализован)
└──────────┬──────────┘
           │ инжектит
           ▼
┌─────────────────────┐
│ kastol.dll          │  kastol::*
│  ├ DllMain          │  → создаёт KastolInitThread, дальше всё в нём
│  ├ Mod (синглтон)   │  → жизненный цикл всех подсистем
│  ├ Config           │  → JSON в файл рядом с DLL
│  ├ Logger           │  → kastol.log рядом с DLL
│  ├ InputHandler     │  → обработка ввода (готово, ждёт WndProc-хук)
│  ├ Overlay          │  → рендер меню (заглушка, ждёт ImGui)
│  ├ DX11Renderer     │  → ВНИМАНИЕ: сейчас он создаёт СВОЮ swapchain.
│  │                     В Phase 1 переписать на использование чужой
│  │                     swapchain через Present-хук.
│  ├ ImGuiImpl        │  → заглушка
│  └ Memory / Utils   │  → паттерн-сканер, helpers
└─────────────────────┘
```

## Куда смотреть в первую очередь

| Файл | Зачем |
|---|---|
| `mod/src/core/mod.cpp` | Точка инициализации мода. Сюда добавляем установку Present-хука в Phase 1. |
| `mod/src/renderer/dx11_renderer.cpp` | **Будет переписан** в Phase 1 — текущая реализация неправильная (своя swapchain). |
| `mod/src/renderer/imgui_impl.cpp` | Сейчас заглушка с закомментированным кодом ImGui — расскомментировать в Phase 1. |
| `mod/src/overlay/overlay.cpp` | Все `Render*` методы — здесь будет реальный UI по [DESIGN.md](DESIGN.md). |
| `.github/workflows/build.yml` | Если CI красный — смотри сюда. |

## Чек-лист перед началом работы дома

1. [ ] `git pull` ветки `fix/initial-build` (или после мерджа в main — `main`).
2. [ ] Установить **VS 2022** (Community ок) с workload "Desktop development with C++".
3. [ ] Установить **CMake 3.20+** (обычно идёт с VS).
4. [ ] Открыть папку проекта в **VS Code** или **Visual Studio**.
   - VS: File → Open → CMake → выбрать корневой `CMakeLists.txt`.
   - VS Code: ставим расширение CMake Tools, открываем папку.
5. [ ] Прочитать [ROADMAP.md](ROADMAP.md).
6. [ ] Прочитать [DESIGN.md](DESIGN.md) — это **обязательно** перед написанием
   любого UI кода. Всё меню должно соответствовать спеке до пикселя.

## Где запускать сборку локально

```powershell
# Минимальный билд (как в CI):
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Полный билд с зависимостями (тянет ImGui+MinHook через FetchContent):
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DKASTOL_WITH_IMGUI=ON -DKASTOL_WITH_MINHOOK=ON
cmake --build build --config Release
```

Артефакты в `build/bin/Release/`.

## Не делать

- ❌ Не возвращать `cs2_mod::` namespace в код мода — это будет регрессия.
- ❌ Не помечать `Mod` через `__declspec(dllexport)`. Класс приватный.
- ❌ Не вызывать `Save()`/любой I/O из деструктора синглтона.
- ❌ Не делать свою `IDXGISwapChain` — использовать ту, что у игры (через хук).
- ❌ Не писать читерские функции, чувствительные к VAC. Проект про **визуал**.
- ❌ Не коммитить `external/` и `build/` — `.gitignore` их и так исключает.

## Контекст по приоритетам (в одной фразе)

> Главное — чтобы оверлей в CS2 **появлялся**, **выглядел красиво и минималистично**
> по [DESIGN.md](DESIGN.md), и **ничего больше не делал**. Все остальные фичи —
> только если они визуальные (тема, шрифт, watermark, FPS-счётчик).
