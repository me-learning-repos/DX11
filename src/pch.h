#pragma once

// GLFW (keep this, but without Vulkan)
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// DirectX 11 headers
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <WICTextureLoader.h>

// GLM Math Library
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Library
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <string>
#include <array>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>

// Link the DirectX libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib") // Add DirectXTK library

// DirectX smart pointer typedefs using Windows Runtime Library
using Microsoft::WRL::ComPtr;