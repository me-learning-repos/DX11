#include "pch.h"
#include "Graphics/TextureRenderer.h"
#include <WICTextureLoader.h>

// Simple vertex structure
struct Vertex {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
};

// Constant buffer structure
struct ConstantBuffer {
    DirectX::XMFLOAT4X4 TransformMatrix;
};

// Updated shader code to handle transformation
const char* vertexShaderCode = R"(
cbuffer ConstantBuffer : register(b0)
{
    matrix TransformMatrix;
};

struct VS_Input {
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PS_Input {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

PS_Input main(VS_Input input) {
    PS_Input output;
    output.Position = mul(float4(input.Position, 1.0f), TransformMatrix);
    output.TexCoord = input.TexCoord;
    return output;
}
)";

const char* pixelShaderCode = R"(
Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PS_Input {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main(PS_Input input) : SV_TARGET {
    return shaderTexture.Sample(samplerState, input.TexCoord);
}
)";

TextureRenderer::TextureRenderer() = default;

TextureRenderer::~TextureRenderer() = default;

bool TextureRenderer::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;

    if (!CreateShaders())
    {
        std::cerr << "Failed to create shaders" << std::endl;
        return false;
    }

    if (!CreateVertexBuffer())
    {
        std::cerr << "Failed to create vertex buffer" << std::endl;
        return false;
    }

    if (!CreateConstantBuffer())
    {
        std::cerr << "Failed to create constant buffer" << std::endl;
        return false;
    }

    if (!CreateSamplerState())
    {
        std::cerr << "Failed to create sampler state" << std::endl;
        return false;
    }

    return true;
}

bool TextureRenderer::LoadTexture(const std::string& filename)
{
    // Convert from string to wstring
    std::wstring wFilename(filename.begin(), filename.end());

    // Load the texture
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        m_device.Get(),
        m_context.Get(),
        wFilename.c_str(),
        nullptr,
        m_textureView.GetAddressOf()
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }

    return true;
}

void TextureRenderer::Render()
{
    // Update the vertex buffer with current position/size
    UpdateVertexBuffer();

    // Set the input layout
    m_context->IASetInputLayout(m_inputLayout.Get());

    // Set the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

    // Set the primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set the shaders
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // Set the constant buffer
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Set the texture and sampler state
    m_context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
    m_context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Draw the quad (6 vertices for 2 triangles)
    m_context->Draw(6, 0);
}

bool TextureRenderer::CreateShaders()
{
    // Compile the vertex shader
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompile(
        vertexShaderCode, 
        strlen(vertexShaderCode), 
        "VS", 
        nullptr, 
        nullptr, 
        "main", 
        "vs_5_0", 
        0, 
        0, 
        vsBlob.GetAddressOf(), 
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << "Vertex shader compilation failed: " << 
                static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
        return false;
    }

    // Create the vertex shader
    hr = m_device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        m_vertexShader.GetAddressOf()
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create vertex shader" << std::endl;
        return false;
    }

    // Create the input layout
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(
        inputDesc,
        ARRAYSIZE(inputDesc),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        m_inputLayout.GetAddressOf()
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create input layout" << std::endl;
        return false;
    }

    // Compile the pixel shader
    ComPtr<ID3DBlob> psBlob;
    hr = D3DCompile(
        pixelShaderCode, 
        strlen(pixelShaderCode), 
        "PS", 
        nullptr, 
        nullptr, 
        "main", 
        "ps_5_0", 
        0, 
        0, 
        psBlob.GetAddressOf(), 
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << "Pixel shader compilation failed: " << 
                static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
        return false;
    }

    // Create the pixel shader
    hr = m_device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        m_pixelShader.GetAddressOf()
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create pixel shader" << std::endl;
        return false;
    }

    return true;
}

bool TextureRenderer::CreateVertexBuffer()
{
    // Define the vertices for a quad centered at origin
    Vertex vertices[] = {
        // First triangle
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(-0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3( 0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        
        // Second triangle
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3( 0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        { DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }
    };

    // Create the vertex buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // Changed to dynamic for updates
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allow CPU write access

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = m_device->CreateBuffer(&bufferDesc, &initData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create vertex buffer" << std::endl;
        return false;
    }

    return true;
}

bool TextureRenderer::CreateConstantBuffer()
{
    // Create the constant buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(ConstantBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create constant buffer" << std::endl;
        return false;
    }

    return true;
}

bool TextureRenderer::CreateSamplerState()
{
    // Create the sampler state
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create sampler state" << std::endl;
        return false;
    }

    return true;
}

void TextureRenderer::UpdateVertexBuffer()
{
    // Map the constant buffer to update the transform matrix
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr))
    {
        ConstantBuffer* dataPtr = static_cast<ConstantBuffer*>(mappedResource.pData);
        
        // Create a transformation matrix for the sprite
        // Convert from screen space to clip space
        DirectX::XMMATRIX transformMatrix = DirectX::XMMatrixIdentity();
        
        // Get information about the render target
        D3D11_VIEWPORT viewport;
        UINT numViewports = 1;
        m_context->RSGetViewports(&numViewports, &viewport);
        
        float scaleX = m_size.x / viewport.Width * 2.0f;
        float scaleY = m_size.y / viewport.Height * 2.0f;
        
        // Apply scaling
        transformMatrix = DirectX::XMMatrixMultiply(
            transformMatrix, 
            DirectX::XMMatrixScaling(scaleX, scaleY, 1.0f)
        );
        
        // Convert position from screen space to clip space
        float posX = (m_position.x / viewport.Width) * 2.0f;
        float posY = (m_position.y / viewport.Height) * 2.0f;
        
        // Apply translation
        transformMatrix = DirectX::XMMatrixMultiply(
            transformMatrix, 
            DirectX::XMMatrixTranslation(posX, -posY, 0.0f)
        );
        
        // Store the transformation matrix in the constant buffer
        DirectX::XMStoreFloat4x4(&dataPtr->TransformMatrix, DirectX::XMMatrixTranspose(transformMatrix));
        
        // Unmap the constant buffer
        m_context->Unmap(m_constantBuffer.Get(), 0);
    }
}