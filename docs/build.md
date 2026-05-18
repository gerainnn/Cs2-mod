# Сборка KASTOL

## Требования

- Windows 10/11 (x64)
- Visual Studio 2022 (17.0+) c рабочей нагрузкой "Desktop development with C++"
- CMake 3.20+
- Windows SDK 10.0

> Linux/Mac не поддерживаются: проект использует DXGI/D3D11/CreateRemoteThread,
> которые есть только на Windows.

## Быстрая сборка (минимальный билд, без ImGui/MinHook)

```powershell
git clone https://github.com/gerainnn/Cs2-mod.git
cd Cs2-mod

cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

После сборки в `build\bin\Release\` появятся:

- `kastol_loader.exe` — загрузчик
- `kastol.dll`        — сама модификация

В таком билде ImGui и MinHook не подключены — мод соберётся, инжектируется,
напишет лог `kastol.log` рядом с DLL, но реально ничего не отрисует.
Это намеренная стартовая точка.

## Сборка с ImGui и MinHook (когда будем интегрировать рендер)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 ^
  -DKASTOL_WITH_IMGUI=ON ^
  -DKASTOL_WITH_MINHOOK=ON
cmake --build build --config Release
```

CMake сам скачает зависимости через `FetchContent` — ничего вручную в `external/`
класть не нужно.

## Сборка со Steamworks SDK (опционально)

1. Скачай Steamworks SDK с partner.steamgames.com.
2. Распакуй в `external/steam_sdk/`.
3. Сконфигурируй с `-DKASTOL_WITH_STEAM_SDK=ON`.
4. Скопируй `steam_api64.dll` рядом с `kastol_loader.exe`.

## Отладка

```powershell
cmake --build build --config Debug
```

## Известные ограничения текущего каркаса

- Реальных хуков `IDXGISwapChain::Present` пока нет.
- ImGui-меню не рендерится (вызовы закомментированы).
- JSON-конфиг сохраняется/загружается, но парсер — заглушка.
- Updater не делает реальных HTTP-запросов.

См. `docs/architecture.md` и план в README.
