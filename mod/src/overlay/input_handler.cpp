/**
 * @file input_handler.cpp
 * @brief Реализация обработчика ввода
 */

#include "mod/input_handler.h"
#include "mod/config.h"
#include "mod/logger.h"
#include "mod/overlay.h"
#include <windows.h>
#include <windowsx.h> // для GET_X_LPARAM / GET_Y_LPARAM

namespace kastol {

InputHandler& InputHandler::Instance() {
    static InputHandler instance;
    return instance;
}

InputHandler::InputHandler()
    : m_initialized(false)
    , m_inputBlocked(false)
    , m_menuToggleKey(VK_INSERT) {
}

InputHandler::~InputHandler() {
}

bool InputHandler::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Инициализация обработчика ввода...");
    
    // Сброс состояний
    memset(m_keyStates, 0, sizeof(m_keyStates));
    m_mouseState = MouseState();
    
    m_initialized = true;
    LOG_INFO("Обработчик ввода инициализирован");
    return true;
}

void InputHandler::Update(float deltaTime) {
    if (!m_initialized) {
        return;
    }
    
    ResetFrameStates();
    UpdateKeyboardState(deltaTime);
    UpdateMouseState();
    
    // Проверка клавиши переключения меню — toggle оверлея
    if (m_keyStates[m_menuToggleKey].pressed) {
        Overlay::Instance().Toggle();
    }
}

bool InputHandler::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!m_initialized) {
        return false;
    }

    // ВАЖНО: toggle-клавиша должна обрабатываться ВСЕГДА, иначе меню никогда не откроется.
    // Также сами состояния клавиш мы обновляем независимо от блокировки —
    // блокировка влияет только на то, "поглощаем" ли мы сообщение для игры.
    switch (msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            int key = static_cast<int>(wParam);
            if (key >= 0 && key < 256) {
                if (!m_keyStates[key].held) {
                    m_keyStates[key].pressed = true;
                    InvokeKeyPressCallback(key);
                }
                m_keyStates[key].held = true;
            }
            return m_inputBlocked;
        }

        case WM_KEYUP:
        case WM_SYSKEYUP: {
            int key = static_cast<int>(wParam);
            if (key >= 0 && key < 256) {
                m_keyStates[key].released = true;
                m_keyStates[key].held = false;
            }
            return m_inputBlocked;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            m_mouseState.leftPressed  = (msg == WM_LBUTTONDOWN);
            m_mouseState.leftReleased = (msg == WM_LBUTTONUP);
            m_mouseState.leftHeld     = (msg == WM_LBUTTONDOWN);
            return m_inputBlocked;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            m_mouseState.rightPressed  = (msg == WM_RBUTTONDOWN);
            m_mouseState.rightReleased = (msg == WM_RBUTTONUP);
            m_mouseState.rightHeld     = (msg == WM_RBUTTONDOWN);
            return m_inputBlocked;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            m_mouseState.middlePressed  = (msg == WM_MBUTTONDOWN);
            m_mouseState.middleReleased = (msg == WM_MBUTTONUP);
            m_mouseState.middleHeld     = (msg == WM_MBUTTONDOWN);
            return m_inputBlocked;

        case WM_MOUSEMOVE: {
            // Только клиентские координаты окна, без смешения с GetCursorPos.
            const int newX = GET_X_LPARAM(lParam);
            const int newY = GET_Y_LPARAM(lParam);
            m_mouseState.deltaX = newX - m_mouseState.x;
            m_mouseState.deltaY = newY - m_mouseState.y;
            m_mouseState.x = newX;
            m_mouseState.y = newY;
            return m_inputBlocked;
        }

        case WM_MOUSEWHEEL:
            m_mouseState.wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
            return m_inputBlocked;
    }

    return false;
}

void InputHandler::SetKeyPressCallback(int key, std::function<void()> callback) {
    m_keyPressCallbacks[key] = callback;
}

void InputHandler::RemoveKeyPressCallback(int key) {
    m_keyPressCallbacks.erase(key);
}

KeyState InputHandler::GetKeyState(int key) const {
    if (key >= 0 && key < 256) {
        return m_keyStates[key];
    }
    return KeyState();
}

bool InputHandler::IsKeyPressed(int key) const {
    if (key >= 0 && key < 256) {
        return m_keyStates[key].pressed;
    }
    return false;
}

bool InputHandler::IsKeyHeld(int key) const {
    if (key >= 0 && key < 256) {
        return m_keyStates[key].held;
    }
    return false;
}

MouseState InputHandler::GetMouseState() const {
    return m_mouseState;
}

void InputHandler::GetMousePosition(int& x, int& y) const {
    x = m_mouseState.x;
    y = m_mouseState.y;
}

void InputHandler::SetInputBlocked(bool blocked) {
    m_inputBlocked = blocked;
    LOG_DEBUG(blocked ? "Ввод заблокирован" : "Ввод разблокирован");
}

bool InputHandler::IsInputBlocked() const {
    return m_inputBlocked;
}

void InputHandler::SetMenuToggleKey(int key) {
    m_menuToggleKey = key;
}

void InputHandler::UpdateKeyboardState(float deltaTime) {
    // Обновляем время удержания клавиш
    for (int i = 0; i < 256; i++) {
        if (m_keyStates[i].held) {
            m_keyStates[i].holdTime += deltaTime;
        } else {
            m_keyStates[i].holdTime = 0.0f;
        }
    }
}

void InputHandler::UpdateMouseState() {
    // Координаты и delta теперь обновляются исключительно через WM_MOUSEMOVE
    // (см. HandleMessage). Здесь оставлено пустым на случай будущих расширений
    // (например, raw-input или fallback на GetCursorPos при свернутом окне).
}

void InputHandler::ResetFrameStates() {
    // Сброс состояний "нажата" и "отпущена" для нового кадра
    for (int i = 0; i < 256; i++) {
        m_keyStates[i].pressed = false;
        m_keyStates[i].released = false;
    }
    
    m_mouseState.leftPressed = false;
    m_mouseState.leftReleased = false;
    m_mouseState.rightPressed = false;
    m_mouseState.rightReleased = false;
    m_mouseState.middlePressed = false;
    m_mouseState.middleReleased = false;
    m_mouseState.wheelDelta = 0.0f;
}

void InputHandler::InvokeKeyPressCallback(int key) {
    auto it = m_keyPressCallbacks.find(key);
    if (it != m_keyPressCallbacks.end() && it->second) {
        it->second();
    }
}

} // namespace kastol
