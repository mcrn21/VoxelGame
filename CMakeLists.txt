cmake_minimum_required(VERSION 3.5)

project(VoxelGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)

add_executable(VoxelGame src/main.cpp
    src/Window/Window.h src/Window/Window.cpp

    src/Utils/NoCopyable.h
    src/Utils/Singleton.h
    src/Engine.h src/Engine.cpp
    src/Graphics/Shader.h src/Graphics/Shader.cpp
    src/Graphics/Texture.h src/Graphics/Texture.cpp
    src/Utils/Files.h src/Utils/Files.cpp
    src/3rd/stb_image.h
    src/Window/Keyboard.h src/Window/Keyboard.cpp
    src/Window/Mouse.h src/Window/Mouse.cpp
    src/Graphics/Camera.h src/Graphics/Camera.cpp
    src/EngineObject.h
    src/Graphics/RenderState.h
    src/Graphics/Vertex.h
    src/Graphics/VertexBuffer.h
    src/Graphics/Sprite.h src/Graphics/Sprite.cpp
    src/Graphics/Drawable.h
    src/Graphics/Transformable.h src/Graphics/Transformable.cpp
    src/System/Time.h src/System/Time.cpp
    src/System/Clock.h src/System/Clock.cpp
    src/Eb.h
    src/Voxel/Voxel.h
    src/Voxel/Chunk.h src/Voxel/Chunk.cpp
    src/Graphics/ChunkMesh.h src/Graphics/ChunkMesh.cpp
    src/Voxel/Chunks.h src/Voxel/Chunks.cpp
    src/Graphics/VoxelVertex.h
    src/Graphics/RenderTarget.h src/Graphics/RenderTarget.cpp
    src/Graphics/DefaultShaders.h src/Graphics/DefaultShaders.cpp
    src/Graphics/LinesBatch.h src/Graphics/LinesBatch.cpp
    src/Graphics/LineVertex.h

)

target_link_libraries(VoxelGame PRIVATE glfw GLEW::glew OpenGL::OpenGL glm::glm spdlog::spdlog)

include(GNUInstallDirs)
install(TARGETS VoxelGame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
