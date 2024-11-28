cmake_minimum_required(VERSION 3.5)

project(VoxelGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(ReactPhysics3D REQUIRED)
find_package(EnTT REQUIRED)

add_executable(VoxelGame src/main.cpp
    src/Window/Window.h src/Window/Window.cpp

    src/Utils/NoCopyable.h
    src/Utils/Singleton.h
    src/Engine.h src/Engine.cpp
    src/Graphics/Common/Shader.h src/Graphics/Common/Shader.cpp
    src/Graphics/Common/Texture.h src/Graphics/Common/Texture.cpp
    src/Utils/Files.h src/Utils/Files.cpp
    src/3rd/stb_image.h
    src/Window/Keyboard.h src/Window/Keyboard.cpp
    src/Window/Mouse.h src/Window/Mouse.cpp
    src/Graphics/3D/Camera.h src/Graphics/3D/Camera.cpp
    src/EngineObject.h

    src/Graphics/Common/Vertex.h

    src/Graphics/3D/Sprite3D.h src/Graphics/3D/Sprite3D.cpp

    src/Graphics/Common/Transformable.h src/Graphics/Common/Transformable.cpp
    src/System/Time.h src/System/Time.cpp
    src/System/Clock.h src/System/Clock.cpp
    src/Eb.h
    src/Voxel/Voxel.h
    src/Voxel/Chunk.h src/Voxel/Chunk.cpp
    src/Graphics/3D/ChunkMesh.h src/Graphics/3D/ChunkMesh.cpp
    src/Voxel/Chunks.h src/Voxel/Chunks.cpp

    src/Graphics/Common/RenderTarget.h src/Graphics/Common/RenderTarget.cpp
    src/Graphics/Common/DefaultShaders.h src/Graphics/Common/DefaultShaders.cpp
    src/Graphics/3D/LinesBatch.h src/Graphics/3D/LinesBatch.cpp

    src/VoxelLigtning/Lightmap.h src/VoxelLigtning/Lightmap.cpp
    src/VoxelLigtning/LightSolver.h src/VoxelLigtning/LightSolver.cpp
    src/Utils/VecUtils.h
    src/Graphics/3D/MeshInstance.h src/Graphics/3D/MeshInstance.cpp
    src/Graphics/3D/ModelInstance.h src/Graphics/3D/ModelInstance.cpp


    src/Graphics/3D/Lights.h src/Graphics/3D/Lights.cpp
    src/Utils/IdStorage.h src/Utils/IdStorage.cpp
    src/Utils/SparseSet.h
    src/Graphics/Common/GlBuffer.h src/Graphics/Common/GlBuffer.cpp
    src/Graphics/Common/VertexArray.h src/Graphics/Common/VertexArray.cpp
    src/Graphics/Common/Enums.h
    src/Graphics/3D/Material.h


    src/Graphics/3D/Vertex3D.h



    src/Graphics/2D/Vertex2D.h
    src/Graphics/2D/Sprite2D.h src/Graphics/2D/Sprite2D.cpp
    src/Graphics/3D/Shape3D.h src/Graphics/3D/Shape3D.cpp
    src/Graphics/Common/TextureFBO.h src/Graphics/Common/TextureFBO.cpp
    src/Components/DrawableComponent.h
    src/Scene3D.h src/Scene3D.cpp
    src/Graphics/Common/RenderState.h
    src/Graphics/Common/Drawable.h
    src/Scene2D.h src/Scene2D.cpp
    src/Assets/Assets.h src/Assets/Assets.cpp
    src/Assets/AssetLoader.h
    src/Assets/TextureLoader.h
    src/Graphics/Common/Mesh.h src/Graphics/Common/Mesh.cpp
    src/Graphics/Common/Model.h src/Graphics/Common/Model.cpp
    src/Assets/ModelLoader.h
    src/Graphics/3D/VertexArrayInstance.h src/Graphics/3D/VertexArrayInstance.cpp


)

target_link_libraries(VoxelGame PRIVATE
    glfw GLEW::glew OpenGL::OpenGL glm::glm spdlog::spdlog assimp::assimp ReactPhysics3D::ReactPhysics3D EnTT::EnTT)

include(GNUInstallDirs)
install(TARGETS VoxelGame
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
