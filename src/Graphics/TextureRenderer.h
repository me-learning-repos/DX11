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
    
    // Shaders
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;

    // Create vertex and pixel shaders
    bool CreateShaders();
    
    // Create vertex buffer for quad rendering
    bool CreateVertexBuffer();
    
    // Create sampler state for texture sampling
    bool CreateSamplerState();
};