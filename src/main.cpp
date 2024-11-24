#include "Eb.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <spdlog/spdlog.h>

using namespace glm;

const int32_t WIDTH = 1280;
const int32_t HEIGHT = 720;

class EngineImpl : public eb::Engine
{
public:
    EngineImpl()
        : m_camera_cam_x{0.0f}
        , m_camera_cam_y{0.0f}
        , m_camera_speed{20.0f}
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

        m_chunks = std::make_unique<eb::Chunks>(glm::i32vec3{4, 1, 4},
                                                glm::i32vec3{16, 32, 16},
                                                1.0f,
                                                16.0f,
                                                m_atlas,
                                                this);

        m_target_texture = std::make_shared<eb::Texture>();
        if (!m_target_texture->loadFromFile("target.png")) {
            spdlog::error("Error loading texture");
            exit();
        }

        m_target_sprite = std::make_unique<eb::Sprite>(this);
        m_target_sprite->setTexture(m_target_texture);
        m_target_sprite->setDrawType(eb::Sprite::DRAW_2D);
        m_target_sprite->setOrigin({m_target_sprite->getTextureRect().z / 2,
                                    m_target_sprite->getTextureRect().w / 2,
                                    0.0f});

        // window.setClearColor(glm::vec4{0.45f, 0.72f, 1.0f, 1.0f});

        m_target_block = std::make_unique<eb::LinesBatch>(this);
        m_target_block->cube({-0.005f, -0.005f, -0.005f},
                             {1.005f, 1.005f, 1.005f},
                             glm::vec4{0.0f, 0.0f, 0.0f, 0.7f});
        m_target_block->setLineWidth(3.0f);

        // Light
        m_r_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 0);
        m_g_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 1);
        m_b_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 2);
        m_s_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 3);

        m_chunks->forEachVoxels(
            [this](const glm::i32vec3 &voxel_coords_in_chunk, const glm::i32vec3 &voxel_coords) {
                auto *voxel = m_chunks->getVoxel(voxel_coords);
                if (voxel->id == 3) {
                    m_r_solver->add(voxel_coords, 15);
                    m_g_solver->add(voxel_coords, 15);
                    m_b_solver->add(voxel_coords, 15);
                }
            });

        glm::i32vec3 voxels_size = m_chunks->getChunkSize() * m_chunks->getChunksSize();
        glm::i32vec3 voxel_coords;
        for (voxel_coords.z = 0; voxel_coords.z < voxels_size.z; ++voxel_coords.z) {
            for (voxel_coords.x = 0; voxel_coords.x < voxels_size.x; ++voxel_coords.x) {
                for (voxel_coords.y = voxels_size.y - 1; voxel_coords.y >= 0; --voxel_coords.y) {
                    auto *voxel = m_chunks->getVoxel(voxel_coords);
                    if (voxel->id != 0)
                        break;

                    m_chunks->getChunkByVoxel(voxel_coords)
                        ->getLightmap()
                        .setS(voxel_coords % m_chunks->getChunkSize(), 0xF);
                }
            }
        }

        for (voxel_coords.z = 0; voxel_coords.z < voxels_size.z; ++voxel_coords.z) {
            for (voxel_coords.x = 0; voxel_coords.x < voxels_size.x; ++voxel_coords.x) {
                for (voxel_coords.y = voxels_size.y - 1; voxel_coords.y >= 0; --voxel_coords.y) {
                    auto *voxel = m_chunks->getVoxel(voxel_coords);
                    if (voxel->id != 0)
                        break;

                    if (m_chunks->getLight(voxel_coords + glm::i32vec3{-1, 0, 0}, 3) == 0
                        || m_chunks->getLight(voxel_coords + glm::i32vec3{1, 0, 0}, 3) == 0
                        || m_chunks->getLight(voxel_coords + glm::i32vec3{0, -1, 0}, 3) == 0
                        || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 1, 0}, 3) == 0
                        || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 0, -1}, 3) == 0
                        || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 0, 1}, 3) == 0) {
                        m_s_solver->add(voxel_coords);
                    }

                    m_chunks->getChunkByVoxel(voxel_coords)
                        ->getLightmap()
                        .setS(voxel_coords % m_chunks->getChunkSize(), 0xF);
                }
            }
        }

        m_r_solver->solve();
        m_g_solver->solve();
        m_b_solver->solve();
        m_s_solver->solve();

        std::vector<eb::MeshVertex> mesh_verticies = {{{0, 0, 0}},
                                                      {{10, 0, 0}},
                                                      {{10, 10, 0}},
                                                      {{0, 10, 0}}};

        std::vector<uint32_t> mesh_indices = {0, 1, 3, 1, 2, 3};

        m_test_mesh = std::make_unique<eb::Mesh>(mesh_verticies,
                                                 mesh_indices,
                                                 std::vector<std::shared_ptr<eb::Texture>>{});

        // Model
        m_test_model = std::make_unique<eb::Model>();
        m_test_model->loadFromFile("model/airpods_girl/scene.gltf");
        // m_test_model->setScale({1.1f, 1.1f, 1.1f});
        m_test_model->setRotation({glm::radians(-90.0f), 0.0f, 0.0f});
        m_test_model->setPosition({0.0f, 20.0f, 0.0f});

        m_test_model2 = std::make_unique<eb::Model>();

        m_test_model2->loadFromFile(
            "model/volumen-pequeno-final-daniela-romar-taz/source/Shrek Final.fbx");

        m_test_model2->setScale({10.3f, 10.3f, 10.3f});
        m_test_model2->setRotation({glm::radians(-90.0f), 0.0f, 0.0f});
        m_test_model2->setPosition({40.0f, 40.0f, 0.0f});

        window.getLights()->setWorldLightAmbient({0.5f, 0.5f, 0.5f});
        window.getLights()->setWorldLightDiffuse({0.0f, 0.5f, 0.5f});
        window.getLights()->setWorldLightDirection({-2.0f, -5.0f, -2.0f});

        // window.getLights()->setWorldLightAmbient({0.0f, 0.0f, 0.0f});
        // window.getLights()->setWorldLightDiffuse({0.0f, 0.0f, 0.0f});
        // window.getLights()->setWorldLightDirection({0.0f, 0.0f, 0.0f});

        m_point_light = window.getLights()->createLight();
        m_point_light.setPosition({20.0f, 20.0f, 20.0f});
        m_point_light.setAmbient({1.0f, 1.0f, 1.0f});
        m_point_light.setDiffuse({1.0f, 1.0f, 1.0f});
        m_point_light.setSpecular({1.0f, 1.0f, 1.0f});
        m_point_light.setConstant(1.0f);
        m_point_light.setLinear(0.022f);
        m_point_light.setQuadratic(0.0019f);
        m_point_light.setEnable(true);

        m_light = window.getLights()->createLight();
        m_light.setPosition({1.0f, 0.0f, 1.0f});
        m_light.setAmbient({1.0f, 1.0f, 1.0f});
        m_light.setDiffuse({1.0f, 1.0f, 1.0f});
        m_light.setSpecular({1.0f, 1.0f, 1.0f});
        m_light.setConstant(1.0f);
        m_light.setLinear(0.022f);
        m_light.setQuadratic(0.0019f);

        m_light.setCutOff(0.9978f);
        m_light.setOuterCutOff(0.953f);

        // m_light.setCutOff(0.0);
        // m_light.setOuterCutOff(0.0);

        m_light.setEnable(true);

        spdlog::info("OK");
    }

    void onProcessInput(const eb::Time &elapsed)
    {
        auto &keyboard = getKeyboard();
        auto &mouse = getMouse();
        auto &window = getWindow();
        auto camera = window.getCamera(); //getCamera();

        // rotate light
        // auto direct_light = window.getDirectLight();
        // auto &world_light = window.getLights().getWorldLight();
        // static eb::Time sum_e;
        // sum_e += elapsed;

        // auto dir = world_light.getDirection();
        // dir.y = sin(sum_e.asSeconds()) * 20;

        // world_light.setDirection(dir);

        // spdlog::debug("{}", direct_light->direction.y);

        // if (keyboard.isKeyJustPressed(GLFW_KEY_2)) {
        //     m_point_light.setEnable(!m_point_light.isEnable());
        // }

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

        m_light.setPosition(camera->getPosition());
        m_light.setDirection(camera->getFront());
        // m_point_light.setPosition(camera->getPosition());

        // ray cast
        {
            m_target_block_visible = false;

            glm::vec3 end;
            glm::vec3 norm;
            glm::vec3 iend;
            const eb::Voxel *voxel = m_chunks->rayCast(camera->getPosition(),
                                                       camera->getFront(),
                                                       10.0f,
                                                       end,
                                                       norm,
                                                       iend);

            if (voxel) {
                m_target_block_visible = true;
                m_target_block->setPosition(iend);

                if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
                    m_chunks->setVoxelByGlobal(iend, eb::Voxel{0});

                    glm::i32vec3 voxel_coords = iend / m_chunks->getVoxelSize();

                    m_r_solver->remove(voxel_coords);
                    m_g_solver->remove(voxel_coords);
                    m_b_solver->remove(voxel_coords);

                    m_r_solver->solve();
                    m_g_solver->solve();
                    m_b_solver->solve();

                    if (m_chunks->getLight(voxel_coords + glm::i32vec3{0, 1, 0}, 3) == 0xF) {
                        for (int32_t i = voxel_coords.y; i >= 0; i--) {
                            if (m_chunks->getVoxel(glm::i32vec3{voxel_coords.x, i, voxel_coords.z})->id
                                != 0)
                                break;
                            m_s_solver->add(glm::i32vec3{voxel_coords.x, i, voxel_coords.z}, 0xF);
                        }
                    }

                    static const std::vector<glm::i32vec3> neighbors = {glm::i32vec3{0, 0, 1},
                                                                        glm::i32vec3{0, 0, -1},
                                                                        glm::i32vec3{0, 1, 0},
                                                                        glm::i32vec3{0, -1, 0},
                                                                        glm::i32vec3{1, 0, 0},
                                                                        glm::i32vec3{-1, 0, 0}};

                    for (const auto &n : neighbors) {
                        m_r_solver->add(voxel_coords + n);
                        m_g_solver->add(voxel_coords + n);
                        m_b_solver->add(voxel_coords + n);
                        m_s_solver->add(voxel_coords + n);
                    }

                    m_r_solver->solve();
                    m_g_solver->solve();
                    m_b_solver->solve();
                    m_s_solver->solve();
                }

                if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_2)) {
                    int32_t id = 3;

                    m_chunks->setVoxelByGlobal(iend + norm, eb::Voxel{id});

                    glm::i32vec3 voxel_coords = (iend + norm) / m_chunks->getVoxelSize();

                    m_r_solver->remove(voxel_coords);
                    m_g_solver->remove(voxel_coords);
                    m_b_solver->remove(voxel_coords);
                    m_s_solver->remove(voxel_coords);

                    for (int32_t i = voxel_coords.y - 1; i >= 0; i--) {
                        m_s_solver->remove(glm::i32vec3{voxel_coords.x, i, voxel_coords.z});
                        if (i == 0
                            || m_chunks->getVoxel(glm::i32vec3{voxel_coords.x, i, voxel_coords.z})->id
                                   != 0) {
                            break;
                        }
                    }
                    m_r_solver->solve();
                    m_g_solver->solve();
                    m_b_solver->solve();
                    m_s_solver->solve();

                    if (id == 3) {
                        m_r_solver->add(voxel_coords, 15);
                        m_g_solver->add(voxel_coords, 15);
                        m_b_solver->add(voxel_coords, 15);

                        m_r_solver->solve();
                        m_g_solver->solve();
                        m_b_solver->solve();
                    }
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

        m_chunks->update();
    }

    void onDraw(const eb::Time &elapsed)
    {
        // getWindow().draw(*m_sprite1);
        getWindow().draw(*m_sprite2);

        // for (const auto &mesh : m_chunks_mesh) {
        //     getWindow().draw(*mesh);
        // }

        m_chunks->draw(getWindow());

        if (m_target_block_visible)
            getWindow().draw(*m_target_block);

        // getWindow().draw(*m_test_mesh);
        getWindow().draw(*m_test_model);
        getWindow().draw(*m_test_model2);

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

    std::shared_ptr<eb::Texture> m_atlas;
    std::unique_ptr<eb::Chunks> m_chunks;

    std::shared_ptr<eb::Texture> m_target_texture;
    std::unique_ptr<eb::Sprite> m_target_sprite;

    std::unique_ptr<eb::LinesBatch> m_target_block;
    bool m_target_block_visible;

    std::unique_ptr<eb::LightSolver> m_r_solver;
    std::unique_ptr<eb::LightSolver> m_g_solver;
    std::unique_ptr<eb::LightSolver> m_b_solver;
    std::unique_ptr<eb::LightSolver> m_s_solver;

    std::unique_ptr<eb::Mesh> m_test_mesh;
    std::unique_ptr<eb::Model> m_test_model;

    std::unique_ptr<eb::Model> m_test_model2;

    eb::Light m_point_light;
    eb::Light m_light;
};

int main()
{
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("Voxel game");

    EngineImpl{}.mainLoop();

    return 0;
}
