#pragma once
#include "pch.h"

class TextureRenderer
{
public:
    TextureRenderer();
    ~TextureRenderer();

    // Initialize the texture renderer
    bool Init(ID3D11Device* device, ID3D11DeviceContext* context);

    // Load a texture from a file
    bool LoadTexture(const std::string& filename);

    // Render the texture
    void Render();

    // Set the position of the texture
    void SetPosition(const glm::vec2& position) { m_position = position; }

    // Get the position of the texture
    const glm::vec2& GetPosition() const { return m_position; }

    // Set the size of the texture
    void SetSize(const glm::vec2& size) { m_size = size; }

    // Get the size of the texture
    const glm::vec2& GetSize() const { return m_size; }

private:
    // DirectX resources
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    
    // Texture resources
    ComPtr<ID3D11ShaderResourceView> m_textureView;
    ComPtr<ID3D11SamplerState> m_samplerState;
    
    // Vertex buffer and input layout
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    
    // Constant buffer for transformation
    ComPtr<ID3D11Buffer> m_constantBuffer;
    
    // Shaders
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;

    // Texture properties
    glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_size = glm::vec2(256.0f, 256.0f);

    // Create vertex and pixel shaders
    bool CreateShaders();
    
    // Create vertex buffer for quad rendering
    bool CreateVertexBuffer();
    
    // Create constant buffer for transforms
    bool CreateConstantBuffer();
    
    // Create sampler state for texture sampling
    bool CreateSamplerState();
    
    // Update the vertex buffer with current position and size
    void UpdateVertexBuffer();
};