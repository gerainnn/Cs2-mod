# ROADMAP — план работ по KASTOL

> Этот документ — пошаговый технический план. Идёт строго по фазам.
> Каждая фаза заканчивается зелёным CI и видимым результатом.
> Прежде чем перейти к следующей — текущая должна быть полностью завершена.

## Принципы работы

1. **Одна фаза = один Pull Request.** Не смешиваем.
2. **CI должен быть зелёный** перед мерджем.
3. **Никаких читерских функций.** Цель — визуально красивый оверлей без
   геймплейного влияния. См. `HANDOFF.md`.
4. **Сначала работает, потом красиво.** Phase 1 = «меню появляется» (даже
   уродливое). Phase 2 = «меню красивое» строго по `DESIGN.md`.

---

## ✅ Phase 0 — Каркас и CI (готово)

- [x] Унификация namespace, фиксы экспорта, includes.
- [x] CMake под Windows x64 + MSVC.
- [x] FetchContent для ImGui/MinHook (опционально).
- [x] GitHub Actions: сборка + артефакты.
- [x] Loader инжектит DLL в `cs2.exe`.
- [x] DLL пишет лог рядом с собой.

**Артефакт:** `kastol_loader.exe` + `kastol.dll`, оба собираются в CI.

---

## 🔥 Phase 1 — Оживление: меню реально появляется в игре

Цель: после инжекта по `INSERT` появляется **любое** ImGui окно поверх CS2.
Внешний вид пока неважен — важно, что технически всё работает.

### 1.1. Включить зависимости
- [ ] В `CMakeLists.txt` (root или mod): сделать `KASTOL_WITH_IMGUI=ON` и
  `KASTOL_WITH_MINHOOK=ON` дефолтным значением, либо добавить в CI вторую
  matrix-конфигурацию с этими опциями.
- [ ] Убедиться что `FetchContent` тянет ImGui v1.90+ и MinHook master.
- [ ] CI зелёный с включёнными опциями.

### 1.2. DXGI Present-хук через MinHook
- [ ] Создать **временный dummy-windows + dummy-swapchain** один раз при
  инициализации мода. Цель — получить vtable-указатели на `Present`
  и `ResizeBuffers` реального DXGI.
- [ ] Удалить dummy-объекты сразу после извлечения указателей.
- [ ] Установить хук на `IDXGISwapChain::Present` (vtable index 8) через MinHook.
- [ ] Установить хук на `IDXGISwapChain::ResizeBuffers` (vtable index 13)
  для корректного ресайза RTV.
- [ ] В хуке `Present` при первом вызове:
  - Получить `ID3D11Device` из `pSwapChain->GetDevice()`.
  - Получить `ID3D11DeviceContext` из `device->GetImmediateContext()`.
  - Извлечь `HWND` из `DXGI_SWAP_CHAIN_DESC`.
  - Сохранить всё в `DX11Renderer::Instance()` (через новые сеттеры).
- [ ] Удалить старый код `DX11Renderer::CreateDeviceAndSwapChain` —
  свою swapchain создавать **нельзя**, мы используем игровую.

### 1.3. ImGui интеграция
- [ ] В `ImGuiImpl::Initialize()`:
  - `ImGui::CreateContext()`.
  - `ImGui_ImplWin32_Init(hwnd)`.
  - `ImGui_ImplDX11_Init(device, context)`.
  - Конфигурация `io.ConfigFlags |= NavEnableKeyboard | DockingEnable`.
- [ ] В хуке `Present` (после получения device/context):
  - `ImGui_ImplDX11_NewFrame()` + `ImGui_ImplWin32_NewFrame()` + `ImGui::NewFrame()`.
  - Вызов `Overlay::Instance().Render()`.
  - `ImGui::Render()` + `ImGui_ImplDX11_RenderDrawData(...)`.
- [ ] В хуке `ResizeBuffers`:
  - Уничтожить RTV перед `ResizeBuffers`.
  - Создать новый RTV после.

### 1.4. WndProc-хук для ввода
- [ ] При старте сохранить оригинальный WndProc через
  `SetWindowLongPtrW(hwnd, GWLP_WNDPROC, &MyWndProc)`.
- [ ] В `MyWndProc`:
  - Сначала `ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)`.
  - Затем `InputHandler::Instance().HandleMessage(...)`.
  - Если меню открыто и ImGui хочет ввод (`io.WantCaptureKeyboard/Mouse`)
    — **не пробрасывать** в оригинальный WndProc.
  - Иначе → `CallWindowProcW(originalWndProc, ...)`.
- [ ] При выгрузке DLL восстановить оригинальный WndProc.

### 1.5. Проверка
- [ ] Минимальное окно: `ImGui::Begin("KASTOL Test"); ImGui::Text("Hello"); ImGui::End();`
- [ ] При нажатии INSERT окно появляется/исчезает.
- [ ] Игра не падает при alt-tab, при ресайзе, при выходе из CS2.
- [ ] Лог `kastol.log` показывает все этапы инициализации хуков.

**Артефакт фазы:** видео/скриншот с уродливым ImGui-окном поверх CS2.

---

## 🎨 Phase 2 — Визуал по DESIGN.md

Цель: меню выглядит ровно так, как описано в [DESIGN.md](DESIGN.md).
Минималистичное, белый контрастный фон, чёрные иконки, один акцентный цвет.
**Не отступать от спеки.** Если в спеке что-то не описано — спросить, не выдумывать.

### 2.1. Стиль ImGui
- [ ] Реализовать функцию `ApplyKastolStyle()` в `imgui_impl.cpp`:
  - Все цвета `ImVec4` — из палитры `DESIGN.md §2`.
  - Радиусы скругления, паддинги, скейлы — из `DESIGN.md §3`.
  - Border radius на кнопках 6px, на окнах 12px.
- [ ] Загрузить шрифт Inter (или Segoe UI Variable) через
  `io.Fonts->AddFontFromMemoryTTF(...)` — шрифт встраиваем в DLL как resource
  (см. `DESIGN.md §4`).

### 2.2. Каркас главного окна
- [ ] Окно фиксированного размера 720×480 (опция в конфиге).
- [ ] Закруглённые углы 12px, тень/отсутствие тени по спеке.
- [ ] Sidebar слева 200px с вертикальным списком вкладок.
- [ ] Контентная область справа.
- [ ] Заголовок «KASTOL» + версия маленьким шрифтом.

### 2.3. Иконки
- [ ] Подключить **IconFontCppHeaders** + один из:
  - **Lucide** (рекомендую — чистые тонкие иконки, идеально под минимализм).
  - либо Phosphor / Tabler.
- [ ] Иконки строго чёрные, размер 16-20px.
- [ ] Сайдбар-иконки + лейблы в одной строке.

### 2.4. Вкладки
- [ ] **Home** — приветствие, watermark с никнеймом, версия.
- [ ] **Visual** — слайдер прозрачности окна, выбор акцентного цвета (один из 4-5
  заранее заданных), переключатель темы (если будет светлая+тёмная).
- [ ] **Performance** — переключатель FPS-счётчика, его позиция (4 угла).
- [ ] **About** — версия, ссылка на GitHub, дисклеймер про fair-play.

### 2.5. Анимации
- [ ] Fade-in/fade-out при toggle меню (200ms).
- [ ] Плавный hover на кнопках (150ms на цвет).

### 2.6. FPS-счётчик
- [ ] Маленький watermark в выбранном углу.
- [ ] Размер ~14px, прозрачность 0.7, акцентный цвет.

**Артефакт фазы:** скриншот в README — чтобы было сразу видно, что мод выглядит круто.

---

## 🔧 Phase 3 — Качество кода

Это техдолг, который копился. Делается одним PR после визуальной готовности.

- [ ] Подключить `nlohmann/json` через FetchContent.
- [ ] Реализовать `Config::ParseConfig` нормально.
- [ ] **Объединить** `Config::Settings` и `m_values` в один источник правды.
  Сейчас они живут параллельно и не синхронизированы.
- [ ] `Updater::FetchUrl` через WinHTTP (опционально, низкий приоритет).
- [ ] `Updater` со сравнением версий через семантический парсер, не через `string >`.
- [ ] CI: добавить вторую matrix-конфигурацию `Debug` (для CI-проверки sanitizer’а
  тоже неплохо, но это потом).

---

## 🚀 Phase 4 — Релизы и удобство

- [ ] **Auto-Release**: workflow_dispatch + tag-trigger → создание GitHub Release
  с приложенным ZIP `kastol-vX.Y.Z.zip` (loader + dll + конфиг).
- [ ] Скриншоты в README.
- [ ] Иконка приложения для loader.exe (`.rc` + `.ico`).
- [ ] Code-signing — НЕ нужен (требует платной подписи, мод не для дистрибуции).

---

## 🌟 Phase 5 — Идеи на потом (не делаем без явного запроса)

- [ ] Watermark с произвольным текстом.
- [ ] Hotkey-биндер для CS2 console команд (`bind X "say hi"`).
- [ ] Скриншот-менеджер.
- [ ] Smart-RGB интеграция (Razer/Corsair/Logitech SDK).
- [ ] Несколько готовых тем (Light/Dark/Solarized/Nord и т.п.).
- [ ] Локализация UI (ru/en).

---

## Definition of Done — общее для каждой фазы

- ✅ CI зелёный.
- ✅ Артефакт собирается и не падает при инжекте.
- ✅ Лог `kastol.log` без `ERROR`-строк при штатном сценарии.
- ✅ Документация обновлена (если изменилось поведение).
- ✅ Если фаза визуальная — скриншот приложен к PR.
