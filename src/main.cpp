#include "Eb.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <spdlog/spdlog.h>

const int32_t WIDTH = 1280;
const int32_t HEIGHT = 720;

class EngineImpl : public eb::Engine
{
public:
    EngineImpl()
        : m_camera_cam_x{0.0f}
        , m_camera_cam_y{0.0f}
        , m_camera_speed{20.0f}
        , m_chunk{{0, 0, 0}, {16, 16, 16}, 16.0f, 16.0f}
        , m_chunks{{6, 1, 6}, {16, 16, 16}, 1.0f, 16.0f, this}
        , m_target_block_visible{false}
    {}
    ~EngineImpl() = default;

    bool init() { return true; }

protected:
    void onWindowResize(int32_t width, int32_t height)
    {
        getWindow().setViewport({0, 0, width, height});
        getWindow().getCamera()->setRatio(static_cast<float>(width) / height);

        m_target_sprite->setPosition(
            {static_cast<float>(width) / 2, static_cast<float>(height) / 2, 0});
    }

    void onInit()
    {
        auto &window = getWindow();
        auto camera = window.getCamera(); //getCamera();

        window.create(WIDTH, HEIGHT, "Voxel game");

        m_texture1 = std::make_shared<eb::Texture>();
        if (!m_texture1->loadFromFile("test.png")) {
            spdlog::error("Error loading texture");
            exit();
        }

        m_texture2 = std::make_shared<eb::Texture>();
        if (!m_texture2->loadFromFile("54.png")) {
            spdlog::error("Error loading texture");
            exit();
        }

        m_sprite1 = std::make_unique<eb::Sprite>(this);
        m_sprite1->setTexture(m_texture1, glm::vec4{180.0f, 0.0f, 180.0f, 150.0f});
        m_sprite1->setOrigin(
            glm::vec3{m_sprite1->getTextureRect().z / 2, m_sprite1->getTextureRect().w / 2, 0.0f});
        m_sprite1->rotate(glm::vec3{glm::radians(0.0f), glm::radians(0.0f), glm::radians(45.0f)});

        m_sprite2 = std::make_unique<eb::Sprite>(this);
        m_sprite2->setTexture(m_texture2);
        m_sprite2->move(glm::vec3{200.0f, 0.0f, 0.0f});
        m_sprite2->setOrigin(
            glm::vec3{m_sprite2->getTextureRect().z / 2, m_sprite2->getTextureRect().w / 2, 0.0f});

        camera->move(glm::vec3{0.0f, 0.0f, 100.0f});
        m_camera_front = glm::vec3(camera->getRotationMat() * glm::vec4(0, 0, -1, 1));
        m_camera_right = glm::vec3(camera->getRotationMat() * glm::vec4(1, 0, 0, 1));

        m_atlas = std::make_shared<eb::Texture>();
        if (!m_atlas->loadFromFile("atlas.png")) {
            spdlog::error("Error loading texture");
            exit();
        }

        auto &chunks_size = m_chunks.getChunksSize();
        auto &chunk_size = m_chunks.getChunkSize();
        float voxel_size = m_chunks.getVoxelSize();
        m_chunks_mesh.resize(chunks_size.y * chunks_size.x * chunks_size.z);

        int32_t i = 0;
        for (int32_t y = 0; y < chunks_size.y; ++y) {
            for (int32_t z = 0; z < chunks_size.z; ++z) {
                for (int32_t x = 0; x < chunks_size.x; ++x) {
                    m_chunks_mesh[i] = std::make_unique<eb::ChunkMesh>(this, m_atlas);
                    m_chunks_mesh[i]->create(&m_chunks, {x, y, z});
                    m_chunks_mesh[i]->setPosition(glm::vec3{x, y, z} * glm::vec3{chunk_size}
                                                  * voxel_size);
                    ++i;
                }
            }
        }

        m_target_texture = std::make_shared<eb::Texture>();
        if (!m_target_texture->loadFromFile("target.png")) {
            spdlog::error("Error loading texture");
            exit();
        }

        m_target_sprite = std::make_unique<eb::Sprite>(this);
        m_target_sprite->setTexture(m_target_texture);
        m_target_sprite->setDrawType(eb::Sprite::DRAW_2D);
        // m_target_sprite->setShader(&eb::Shader::getDefault2D());
        // m_target_sprite->setScale({0.005f, 0.005f, 0.005f});
        // m_target_sprite->setScale({0.05f, 0.05f, 0.05f});
        m_target_sprite->setOrigin({m_target_sprite->getTextureRect().z / 2,
                                    m_target_sprite->getTextureRect().w / 2,
                                    0.0f});

        window.setClearColor(glm::vec4{0.45f, 0.72f, 1.0f, 1.0f});

        m_target_block = std::make_unique<eb::LinesBatch>(this);
        m_target_block->cube({-0.005f, -0.005f, -0.005f},
                             {1.005f, 1.005f, 1.005f},
                             glm::vec4{0.0f, 0.0f, 0.0f, 0.7f});
        m_target_block->setLineWidth(3.0f);
    }

    void onProcessInput(const eb::Time &elapsed)
    {
        auto &keyboard = getKeyboard();
        auto &mouse = getMouse();
        auto &window = getWindow();
        auto camera = window.getCamera(); //getCamera();

        if (keyboard.isKeyJustPressed(GLFW_KEY_ESCAPE))
            window.setShouldClose(true);

        if (keyboard.isKeyJustPressed(GLFW_KEY_1)) {
            auto &window = getWindow();
            window.destroy();
            window.create(WIDTH, HEIGHT, "Voxel game");
        }

        // window.setClearColor(glm::vec4{0.8f, 0.4f, 0.2f, 1.0f});
        if (keyboard.isKeyJustPressed(GLFW_KEY_TAB))
            window.setMouseEnable(!window.isMouseEnable());

        if (!window.isMouseEnable()) {
            m_camera_cam_y += -static_cast<float>(mouse.getDeltaY()) / window.getSize().x * 1.5f;
            m_camera_cam_x += -static_cast<float>(mouse.getDeltaX()) / window.getSize().y * 1.5f;

            if (m_camera_cam_y < -glm::radians(89.0f))
                m_camera_cam_y = -glm::radians(89.0f);

            if (m_camera_cam_y > glm::radians(89.0f)) {
                m_camera_cam_y = glm::radians(89.0f);
            }

            camera->setRotation(glm::vec3{m_camera_cam_y, m_camera_cam_x, 0.0f});

            m_camera_front = glm::vec3(camera->getRotationMat() * glm::vec4(0, 0, -1, 1));
            m_camera_right = glm::vec3(camera->getRotationMat() * glm::vec4(1, 0, 0, 1));
        }

        if (keyboard.isKeyPressed(GLFW_KEY_W))
            camera->move(m_camera_front * elapsed.asSeconds() * m_camera_speed);

        if (keyboard.isKeyPressed(GLFW_KEY_S))
            camera->move(-m_camera_front * elapsed.asSeconds() * m_camera_speed);

        if (keyboard.isKeyPressed(GLFW_KEY_A))
            camera->move(-m_camera_right * elapsed.asSeconds() * m_camera_speed);

        if (keyboard.isKeyPressed(GLFW_KEY_D))
            camera->move(m_camera_right * elapsed.asSeconds() * m_camera_speed);

        // ray cast
        {
            m_target_block_visible = false;

            glm::vec3 end;
            glm::vec3 norm;
            glm::vec3 iend;
            const eb::Voxel *voxel = m_chunks.rayCast(camera->getPosition(),
                                                      camera->getFront(),
                                                      10.0f,
                                                      end,
                                                      norm,
                                                      iend);

            if (voxel) {
                m_target_block_visible = true;
                m_target_block->setPosition(iend);

                if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
                    m_chunks.setVoxelByGlobal(iend, 0);
                    m_chunks_mesh[m_chunks.getChunkIndexByGlobal(static_cast<glm::i32vec3>(iend))]
                        ->create(&m_chunks, m_chunks.getChunkCoordsByGlobal(iend));
                }

                if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_2)) {
                    m_chunks.setVoxelByGlobal(iend + norm, 2);
                    m_chunks_mesh[m_chunks.getChunkIndexByGlobal(static_cast<glm::i32vec3>(iend))]
                        ->create(&m_chunks, m_chunks.getChunkCoordsByGlobal(iend));
                }
            }
        }
    }

    void onTick(const eb::Time &elapsed)
    {
        auto &window = getWindow();
        auto &keyboard = getKeyboard();
        auto &mouse = getMouse();

        m_sprite2->rotate(glm::vec3{glm::radians(0.0f),
                                    glm::radians(0.0f),
                                    glm::radians(elapsed.asSeconds() * 20)});
    }

    void onDraw(const eb::Time &elapsed)
    {
        // getWindow().draw(*m_sprite1);
        getWindow().draw(*m_sprite2);

        for (const auto &mesh : m_chunks_mesh) {
            getWindow().draw(*mesh);
        }

        if (m_target_block_visible)
            getWindow().draw(*m_target_block);

        getWindow().draw(*m_target_sprite);

        // getWindow().draw(m_chunk_mesh);
    }

private:
    std::shared_ptr<eb::Texture> m_texture1;
    std::shared_ptr<eb::Texture> m_texture2;

    std::unique_ptr<eb::Sprite> m_sprite1;
    std::unique_ptr<eb::Sprite> m_sprite2;

    glm::vec3 m_camera_front;
    glm::vec3 m_camera_right;
    float m_camera_cam_x;
    float m_camera_cam_y;
    float m_camera_speed;

    eb::Chunk m_chunk;
    std::shared_ptr<eb::Texture> m_atlas;
    std::unique_ptr<eb::ChunkMesh> m_chunk_mesh;

    eb::Chunks m_chunks;
    std::vector<std::unique_ptr<eb::ChunkMesh>> m_chunks_mesh;

    std::shared_ptr<eb::Texture> m_target_texture;
    std::unique_ptr<eb::Sprite> m_target_sprite;

    std::unique_ptr<eb::LinesBatch> m_target_block;
    bool m_target_block_visible;
};

int main()
{
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("Voxel game");

    EngineImpl{}.mainLoop();

    return 0;
}
