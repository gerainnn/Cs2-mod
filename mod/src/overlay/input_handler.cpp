/**
 * @file input_handler.cpp
 * @brief Реализация обработчика ввода
 */

#include "mod/input_handler.h"
#include "mod/config.h"
#include "mod/logger.h"
#include <windows.h>

namespace cs2_mod {

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
    
    // Проверка клавиши переключения меню
    if (m_keyStates[m_menuToggleKey].pressed) {
        // Вызываем toggle overlay
        // Overlay::Instance().Toggle();
    }
}

bool InputHandler::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!m_initialized) {
        return false;
    }
    
    // Если ввод заблокирован, перехватываем сообщения
    switch (msg) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            if (m_inputBlocked) {
                // Обрабатываем клавишу
                int key = static_cast<int>(wParam);
                if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
                    if (!m_keyStates[key].held) {
                        m_keyStates[key].pressed = true;
                        InvokeKeyPressCallback(key);
                    }
                    m_keyStates[key].held = true;
                } else {
                    m_keyStates[key].released = true;
                    m_keyStates[key].held = false;
                }
                return true;
            }
            break;
            
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            if (m_inputBlocked) {
                m_mouseState.leftPressed = (msg == WM_LBUTTONDOWN);
                m_mouseState.leftReleased = (msg == WM_LBUTTONUP);
                m_mouseState.leftHeld = (msg == WM_LBUTTONDOWN);
                return true;
            }
            break;
            
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            if (m_inputBlocked) {
                m_mouseState.rightPressed = (msg == WM_RBUTTONDOWN);
                m_mouseState.rightReleased = (msg == WM_RBUTTONUP);
                m_mouseState.rightHeld = (msg == WM_RBUTTONDOWN);
                return true;
            }
            break;
            
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            if (m_inputBlocked) {
                m_mouseState.middlePressed = (msg == WM_MBUTTONDOWN);
                m_mouseState.middleReleased = (msg == WM_MBUTTONUP);
                m_mouseState.middleHeld = (msg == WM_MBUTTONDOWN);
                return true;
            }
            break;
            
        case WM_MOUSEMOVE:
            if (m_inputBlocked) {
                m_mouseState.x = GET_X_LPARAM(lParam);
                m_mouseState.y = GET_Y_LPARAM(lParam);
                return true;
            }
            break;
            
        case WM_MOUSEWHEEL:
            if (m_inputBlocked) {
                m_mouseState.wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
                return true;
            }
            break;
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
    // Получаем позицию курсора
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        m_mouseState.deltaX = cursorPos.x - m_mouseState.x;
        m_mouseState.deltaY = cursorPos.y - m_mouseState.y;
        m_mouseState.x = cursorPos.x;
        m_mouseState.y = cursorPos.y;
    }
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

} // namespace cs2_mod
