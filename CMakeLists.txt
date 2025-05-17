cmake_minimum_required(VERSION 3.20)

project(DirectX11Learning)

# Enable C++20 and PCH support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add 3rdparty dependencies (assumes glfw and glm are added via add_subdirectory)
add_subdirectory(3rdparty)

# Get source files
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")

# Create the executable
add_executable(${PROJECT_NAME} ${SRC})

# Include project directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw glm DirectXTK)

# Include DirectX 11 libraries
if(WIN32)
    # Link the DirectX libraries
    target_link_libraries(${PROJECT_NAME} d3d11 dxgi d3dcompiler)
else()
    message(FATAL_ERROR "DirectX11 is only available on Windows platforms")
endif()

# Precompiled header (assuming pch.h is in src/)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)