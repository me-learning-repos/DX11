#include "pch.h"
#include "InputHandler.h"

InputHandler::InputHandler(GLFWwindow* window)
    : m_window(window)
{
    // Initialize key state maps
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
    {
        m_currentKeyState[key] = false;
        m_previousKeyState[key] = false;
    }
    
    // Initialize mouse button state maps
    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
    {
        m_currentMouseState[button] = false;
        m_previousMouseState[button] = false;
    }
}

void InputHandler::Update()
{
    // Update key states
    for (auto& pair : m_currentKeyState)
    {
        int key = pair.first;
        m_previousKeyState[key] = m_currentKeyState[key];
        m_currentKeyState[key] = glfwGetKey(m_window, key) == GLFW_PRESS;
    }
    
    // Update mouse button states
    for (auto& pair : m_currentMouseState)
    {
        int button = pair.first;
        m_previousMouseState[button] = m_currentMouseState[button];
        m_currentMouseState[button] = glfwGetMouseButton(m_window, button) == GLFW_PRESS;
    }
    
    // Update mouse position
    glfwGetCursorPos(m_window, &m_mouseX, &m_mouseY);
}

bool InputHandler::IsKeyPressed(int key) const
{
    auto it = m_currentKeyState.find(key);
    if (it != m_currentKeyState.end())
    {
        return it->second;
    }
    return false;
}

bool InputHandler::IsKeyJustPressed(int key) const
{
    auto currentIt = m_currentKeyState.find(key);
    auto previousIt = m_previousKeyState.find(key);
    
    if (currentIt != m_currentKeyState.end() && previousIt != m_previousKeyState.end())
    {
        return currentIt->second && !previousIt->second;
    }
    return false;
}

bool InputHandler::IsKeyJustReleased(int key) const
{
    auto currentIt = m_currentKeyState.find(key);
    auto previousIt = m_previousKeyState.find(key);
    
    if (currentIt != m_currentKeyState.end() && previousIt != m_previousKeyState.end())
    {
        return !currentIt->second && previousIt->second;
    }
    return false;
}

glm::vec2 InputHandler::GetMousePosition() const
{
    return glm::vec2(static_cast<float>(m_mouseX), static_cast<float>(m_mouseY));
}

bool InputHandler::IsMouseButtonPressed(int button) const
{
    auto it = m_currentMouseState.find(button);
    if (it != m_currentMouseState.end())
    {
        return it->second;
    }
    return false;
}

bool InputHandler::IsMouseButtonJustPressed(int button) const
{
    auto currentIt = m_currentMouseState.find(button);
    auto previousIt = m_previousMouseState.find(button);
    
    if (currentIt != m_currentMouseState.end() && previousIt != m_previousMouseState.end())
    {
        return currentIt->second && !previousIt->second;
    }
    return false;
}

bool InputHandler::IsMouseButtonJustReleased(int button) const
{
    auto currentIt = m_currentMouseState.find(button);
    auto previousIt = m_previousMouseState.find(button);
    
    if (currentIt != m_currentMouseState.end() && previousIt != m_previousMouseState.end())
    {
        return !currentIt->second && previousIt->second;
    }
    return false;
}