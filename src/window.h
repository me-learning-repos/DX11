#pragma once
#include "pch.h"

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // Initialize the window
    bool Init();

    // Get the GLFW window pointer
    GLFWwindow* GetGLFWWindow() const { return m_Window; }

    // Get the native HWND handle
    HWND GetNativeHandle() const;

    // Get window dimensions
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    // Check if window should close
    bool ShouldClose() const;

    // Poll window events
    void PollEvents() const;

    // Set window title
    void SetTitle(const std::string& title);

    // Window resize callback
    void SetResizeCallback(std::function<void(int, int)> callback) { m_ResizeCallback = callback; }

private:
    // Window properties
    int m_Width;
    int m_Height;
    std::string m_Title;
    GLFWwindow* m_Window = nullptr;

    // Resize callback
    std::function<void(int, int)> m_ResizeCallback;

    // Static callback for GLFW resize events
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};

// Implementation

inline Window::Window(int width, int height, const std::string& title)
    : m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr)
{
}

inline Window::~Window()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

inline bool Window::Init()
{
    // Initialize GLFW if it hasn't been initialized yet
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Tell GLFW to not create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Create the window
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Set user pointer for callbacks
    glfwSetWindowUserPointer(m_Window, this);

    // Set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);

    return true;
}

inline HWND Window::GetNativeHandle() const
{
    return glfwGetWin32Window(m_Window);
}

inline bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

inline void Window::PollEvents() const
{
    glfwPollEvents();
}

inline void Window::SetTitle(const std::string& title)
{
    m_Title = title;
    glfwSetWindowTitle(m_Window, m_Title.c_str());
}

inline void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    // Get the window instance from GLFW
    auto* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    
    // Update window dimensions
    windowInstance->m_Width = width;
    windowInstance->m_Height = height;

    // Call the resize callback if set
    if (windowInstance->m_ResizeCallback)
    {
        windowInstance->m_ResizeCallback(width, height);
    }
}