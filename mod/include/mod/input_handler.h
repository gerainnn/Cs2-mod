#pragma once

/**
 * @file input_handler.h
 * @brief Обработчик ввода для overlay модификации
 * 
 * Перехватывает и обрабатывает ввод пользователя
 * для взаимодействия с overlay-меню.
 */

#include <functional>
#include <map>
#include <windows.h>

namespace cs2_mod {

/**
 * @brief Структура для хранения состояния клавиши
 */
struct KeyState {
    bool pressed = false;
    bool released = false;
    bool held = false;
    float holdTime = 0.0f;
};

/**
 * @brief Структура для хранения состояния мыши
 */
struct MouseState {
    int x = 0;
    int y = 0;
    int deltaX = 0;
    int deltaY = 0;
    float wheelDelta = 0.0f;
    
    bool leftPressed = false;
    bool leftReleased = false;
    bool leftHeld = false;
    
    bool rightPressed = false;
    bool rightReleased = false;
    bool rightHeld = false;
    
    bool middlePressed = false;
    bool middleReleased = false;
    bool middleHeld = false;
};

/**
 * @brief Класс обработки ввода
 */
class InputHandler {
public:
    static InputHandler& Instance();

    /**
     * @brief Инициализировать обработчик
     * @return true если успешно
     */
    bool Initialize();

    /**
     * @brief Обновить состояние ввода
     * @param deltaTime Время между кадрами
     */
    void Update(float deltaTime);

    /**
     * @brief Обработать сообщение Windows
     * @param msg Сообщение
     * @param wParam Параметр wParam
     * @param lParam Параметр lParam
     * @return true если сообщение обработано
     */
    bool HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Установить callback на нажатие клавиши
     */
    void SetKeyPressCallback(int key, std::function<void()> callback);

    /**
     * @brief Удалить callback клавиши
     */
    void RemoveKeyPressCallback(int key);

    /**
     * @brief Получить состояние клавиши
     */
    KeyState GetKeyState(int key) const;

    /**
     * @brief Проверить, нажата ли клавиша
     */
    bool IsKeyPressed(int key) const;

    /**
     * @brief Проверить, удерживается ли клавиша
     */
    bool IsKeyHeld(int key) const;

    /**
     * @brief Получить состояние мыши
     */
    MouseState GetMouseState() const;

    /**
     * @brief Получить позицию курсора
     */
    void GetMousePosition(int& x, int& y) const;

    /**
     * @brief Блокировать/разблокировать ввод в игру
     */
    void SetInputBlocked(bool blocked);

    /**
     * @brief Проверить, заблокирован ли ввод
     */
    bool IsInputBlocked() const;

    /**
     * @brief Установить горячую клавишу для переключения меню
     */
    void SetMenuToggleKey(int key);

private:
    InputHandler();
    ~InputHandler();
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    bool m_initialized;
    bool m_inputBlocked;
    int m_menuToggleKey;

    KeyState m_keyStates[256];
    MouseState m_mouseState;
    
    std::map<int, std::function<void()>> m_keyPressCallbacks;

    /**
     * @brief Обновить состояние клавиш
     */
    void UpdateKeyboardState(float deltaTime);

    /**
     * @brief Обновить состояние мыши
     */
    void UpdateMouseState();

    /**
     * @brief Сбросить состояния на этот кадр
     */
    void ResetFrameStates();

    /**
     * @brief Вызвать callback нажатия клавиши
     */
    void InvokeKeyPressCallback(int key);
};

} // namespace cs2_mod
