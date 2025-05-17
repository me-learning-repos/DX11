#pragma once
#include "pch.h"

class InputHandler 
{
public:
    InputHandler(GLFWwindow* window);
    ~InputHandler() = default;

    // Update input state
    void Update();

    // Check if a key is currently pressed
    bool IsKeyPressed(int key) const;

    // Check if a key was just pressed this frame
    bool IsKeyJustPressed(int key) const;

    // Check if a key was just released this frame
    bool IsKeyJustReleased(int key) const;

    // Get mouse position
    glm::vec2 GetMousePosition() const;

    // Check if mouse button is pressed
    bool IsMouseButtonPressed(int button) const;

    // Check if mouse button was just pressed this frame
    bool IsMouseButtonJustPressed(int button) const;

    // Check if mouse button was just released this frame
    bool IsMouseButtonJustReleased(int button) const;

private:
    GLFWwindow* m_window;
    
    // Current and previous key states
    std::unordered_map<int, bool> m_currentKeyState;
    std::unordered_map<int, bool> m_previousKeyState;
    
    // Current and previous mouse button states
    std::unordered_map<int, bool> m_currentMouseState;
    std::unordered_map<int, bool> m_previousMouseState;
    
    // Mouse position
    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
};