#include "pch.h"
#include "window.h"
#include "Graphics/TextureRenderer.h"
#include "InputHandler.h"

// DX11 related global variables
ComPtr<ID3D11Device> g_device;
ComPtr<ID3D11DeviceContext> g_deviceContext;
ComPtr<IDXGISwapChain> g_swapChain;
ComPtr<ID3D11RenderTargetView> g_renderTargetView;
std::unique_ptr<TextureRenderer> g_textureRenderer;
std::unique_ptr<InputHandler> g_inputHandler;

// Game state variables
glm::vec2 g_imagePosition = glm::vec2(0.0f, 0.0f);
float g_imageSpeed = 300.0f; // Pixels per second
std::chrono::steady_clock::time_point g_lastFrameTime;

// Function to initialize DirectX 11
bool InitializeDX11(const Window& window)
{
    // Get the HWND from the Window class
    HWND hwnd = window.GetNativeHandle();

    // Setup swap chain parameters
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = window.GetWidth();
    swapChainDesc.BufferDesc.Height = window.GetHeight();
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Create device, device context, and swap chain
    D3D_FEATURE_LEVEL featureLevel;
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // Use default adapter
        D3D_DRIVER_TYPE_HARDWARE,   // Use hardware rendering
        nullptr,                    // No software renderer
        flags,                      // Optional flags
        nullptr,                    // Default feature levels
        0,                          // Number of feature levels
        D3D11_SDK_VERSION,          // SDK version
        &swapChainDesc,             // Swap chain description
        &g_swapChain,               // Swap chain
        &g_device,                  // Device
        &featureLevel,              // Supported feature level
        &g_deviceContext            // Device context
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create device and swap chain" << std::endl;
        return false;
    }

    // Create a render target view for the back buffer
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to get back buffer" << std::endl;
        return false;
    }

    hr = g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, g_renderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create render target view" << std::endl;
        return false;
    }

    // Set the render target
    g_deviceContext->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), nullptr);

    // Setup viewport
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(window.GetWidth());
    viewport.Height = static_cast<float>(window.GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_deviceContext->RSSetViewports(1, &viewport);

    // Initialize texture renderer
    g_textureRenderer = std::make_unique<TextureRenderer>();
    if (!g_textureRenderer->Init(g_device.Get(), g_deviceContext.Get()))
    {
        std::cerr << "Failed to initialize texture renderer" << std::endl;
        return false;
    }

    // Load the texture
    if (!g_textureRenderer->LoadTexture("image.jpg"))
    {
        std::cerr << "Failed to load image.jpg" << std::endl;
        return false;
    }

    return true;
}

// Clean up DirectX resources
void CleanupDX11()
{
    g_textureRenderer.reset();
    if (g_deviceContext) g_deviceContext->ClearState();
    g_renderTargetView.Reset();
    g_swapChain.Reset();
    g_deviceContext.Reset();
    g_device.Reset();
}

// Handles window resize events to update the DirectX 11 resources
void HandleResize(int width, int height)
{
    if (width == 0 || height == 0 || !g_device || !g_swapChain)
        return;

    // Release render target view
    g_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    g_renderTargetView.Reset();

    // Resize the swap chain
    HRESULT hr = g_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
    {
        std::cerr << "Failed to resize swap chain buffers" << std::endl;
        return;
    }

    // Recreate render target view
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to get back buffer after resize" << std::endl;
        return;
    }

    hr = g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, g_renderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create render target view after resize" << std::endl;
        return;
    }

    // Set the render target
    g_deviceContext->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), nullptr);

    // Update viewport
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_deviceContext->RSSetViewports(1, &viewport);
}

// Update game state
void UpdateGame(float deltaTime, const Window& window)
{
    // Get window dimensions for boundary checking
    int windowWidth = window.GetWidth();
    int windowHeight = window.GetHeight();
    
    // Handle input for image movement
    if (g_inputHandler->IsKeyPressed(GLFW_KEY_UP))
    {
        g_imagePosition.y -= g_imageSpeed * deltaTime;
    }
    if (g_inputHandler->IsKeyPressed(GLFW_KEY_DOWN))
    {
        g_imagePosition.y += g_imageSpeed * deltaTime;
    }
    if (g_inputHandler->IsKeyPressed(GLFW_KEY_LEFT))
    {
        g_imagePosition.x -= g_imageSpeed * deltaTime;
    }
    if (g_inputHandler->IsKeyPressed(GLFW_KEY_RIGHT))
    {
        g_imagePosition.x += g_imageSpeed * deltaTime;
    }

    // Clamp the image position to keep it visible in the window
    // Assuming image size is 256x256
    const float imageHalfWidth = 128.0f;
    const float imageHalfHeight = 128.0f;
    
    g_imagePosition.x = std::max(-windowWidth/2.0f + imageHalfWidth, 
                        std::min(g_imagePosition.x, windowWidth/2.0f - imageHalfWidth));
    g_imagePosition.y = std::max(-windowHeight/2.0f + imageHalfHeight, 
                        std::min(g_imagePosition.y, windowHeight/2.0f - imageHalfHeight));
    
    // Update the texture renderer with new position
    g_textureRenderer->SetPosition(g_imagePosition);
}

int main()
{
    Window window(1280, 720, "Trumpet");
    if (!window.Init())
    {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }

    // Set resize callback
    window.SetResizeCallback(HandleResize);

    // Initialize input handler
    g_inputHandler = std::make_unique<InputHandler>(window.GetGLFWWindow());

    // Initialize DirectX 11
    if (!InitializeDX11(window))
    {
        return -1;
    }

    // Print DirectX 11 device info
    ComPtr<IDXGIDevice> dxgiDevice;
    ComPtr<IDXGIAdapter> adapter;
    DXGI_ADAPTER_DESC adapterDesc;

    if (SUCCEEDED(g_device.As(&dxgiDevice)) &&
        SUCCEEDED(dxgiDevice->GetAdapter(adapter.GetAddressOf())) &&
        SUCCEEDED(adapter->GetDesc(&adapterDesc)))
    {
        std::wcout << L"DirectX 11 Device: " << adapterDesc.Description << std::endl;
    }

    // Initialize timing variables
    g_lastFrameTime = std::chrono::steady_clock::now();

    // Main game loop
    while (!window.ShouldClose())
    {
        // Measure frame time for smooth movement
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - g_lastFrameTime).count();
        g_lastFrameTime = currentTime;

        // Update game state
        UpdateGame(deltaTime, window);

        // Clear the back buffer
        float clearColor[4] = { 0.2f, 0.2f, 0.4f, 1.0f }; // Dark blue background
        g_deviceContext->ClearRenderTargetView(g_renderTargetView.Get(), clearColor);

        // Render the texture
        g_textureRenderer->Render();

        // Present the back buffer to the screen
        g_swapChain->Present(1, 0);

        // Process events
        window.PollEvents();
        g_inputHandler->Update();
    }

    // Cleanup
    CleanupDX11();

    return 0;
}