#include "Eb.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>

using namespace glm;

const int32_t WIDTH = 1280;
const int32_t HEIGHT = 720;

// TextureLoader(Assets *assets)::loadFromFile

// MeshInstance
// ModelInstance

class EngineImpl : public eb::Engine
{
public:
    EngineImpl()
        : eb::Engine{{WIDTH, HEIGHT}, "Voxel game"}
    // , m_target_block_visible{false}
    {}
    ~EngineImpl() = default;

    bool init() { return true; }

protected:
    void onWindowResize(const i32vec2 &window_size)
    {
        Engine::onWindowResize(window_size);

        m_target_sprite->setPosition(
            {static_cast<float>(window_size.x) / 2, static_cast<float>(window_size.y) / 2, 0});
    }

    void onInit()
    {
        auto &window = getWindow();

        // === Load assets ===
        // Textures
        getAssets().loadFromFile<eb::Texture>({}, "test.png");
        getAssets().loadFromFile<eb::Texture>({}, "54.png");
        getAssets().loadFromFile<eb::Texture>({}, "atlas.png");
        getAssets().loadFromFile<eb::Texture>({}, "target.png");

        // Models
        getAssets()
            .loadFromFile<eb::Model>("level",
                                     "model/cs/counter-strike_italy_-_classic_cs_map_in_3d.glb");
        getAssets().loadFromFile<eb::Model>("girl", "model/airpods_girl/scene.gltf");
        getAssets().loadFromFile<eb::Model>(
            "shrek", "model/volumen-pequeno-final-daniela-romar-taz/source/Shrek Final.fbx");
        getAssets().loadFromFile<eb::Model>("11", "model/11/11.glb");

        // ===================

        // === Camera ===
        m_scene_camera.camera = getScene3D()->getCamera();
        m_scene_camera.camera->move(glm::vec3{0.0f, 20.0f, 30.0f});
        m_scene_camera.front = glm::vec3(m_scene_camera.camera->getRotationMat()
                                         * glm::vec4(0, 0, -1, 1));
        m_scene_camera.right = glm::vec3(m_scene_camera.camera->getRotationMat()
                                         * glm::vec4(1, 0, 0, 1));

        // === Sprites ===

        // m_sptite_entity_1 = getScene3D()->getRegistry().create();

        // auto sprite1 = std::make_shared<eb::Sprite3D>();
        // sprite1->setMaterial({getAssets().get<eb::Texture>("test")},
        //                      glm::vec4{180.0f, 0.0f, 180.0f, 150.0f});
        // sprite1->setOrigin(
        //     glm::vec3{sprite1->getTextureRect().z / 2, sprite1->getTextureRect().w / 2, 0.0f});
        // sprite1->rotate(glm::vec3{glm::radians(0.0f), glm::radians(0.0f), glm::radians(45.0f)});

        // getScene3D()->getRegistry().emplace<eb::DrawableComponent>(m_sptite_entity_1, sprite1);

        // m_sptite_entity_2 = getScene3D()->getRegistry().create();

        // auto sprite2 = std::make_shared<eb::Sprite3D>();
        // sprite2->setMaterial({getAssets().get<eb::Texture>("54")});
        // sprite2->move(glm::vec3{200.0f, 0.0f, 0.0f});
        // sprite2->setOrigin(
        //     glm::vec3{sprite2->getTextureRect().z / 2, sprite2->getTextureRect().w / 2, 0.0f});

        // getScene3D()->getRegistry().emplace<eb::DrawableComponent>(m_sptite_entity_2, sprite2);

        // ===============

        // m_chunks = std::make_unique<eb::Chunks>(glm::i32vec3{4, 1, 4},
        //                                         glm::i32vec3{16, 32, 16},
        //                                         1.0f,
        //                                         16.0f,
        //                                         getAssets().get<eb::Texture>("atlas"),
        //                                         this);

        // === Target block ==

        m_target_sprite = std::make_unique<eb::Sprite2D>();
        m_target_sprite->setTexture(getAssets().get<eb::Texture>("target"));
        m_target_sprite->setOrigin({m_target_sprite->getTextureRect().z / 2,
                                    m_target_sprite->getTextureRect().w / 2,
                                    0.0f});

        // window.setClearColor(glm::vec4{0.45f, 0.72f, 1.0f, 1.0f});

        // m_target_entity = getScene3D()->getRegistry().create();
        // auto target_lines_batch = std::make_shared<eb::LinesBatch>();
        // target_lines_batch->cube({-0.005f, -0.005f, -0.005f},
        //                          {1.005f, 1.005f, 1.005f},
        //                          glm::vec4{0.0f, 0.0f, 0.0f, 0.7f});
        // target_lines_batch->setLineWidth(3.0f);

        // auto &target_dc = getScene3D()->getRegistry().emplace<eb::DrawableComponent>(
        //     m_target_entity);
        // target_dc.drawable = target_lines_batch;

        // // ====================

        // // Light
        // m_r_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 0);
        // m_g_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 1);
        // m_b_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 2);
        // m_s_solver = std::make_unique<eb::LightSolver>(m_chunks.get(), 3);

        // m_chunks->forEachVoxels(
        //     [this](const glm::i32vec3 &voxel_coords_in_chunk, const glm::i32vec3 &voxel_coords) {
        //         auto *voxel = m_chunks->getVoxel(voxel_coords);
        //         if (voxel->id == 3) {
        //             m_r_solver->add(voxel_coords, 15);
        //             m_g_solver->add(voxel_coords, 15);
        //             m_b_solver->add(voxel_coords, 15);
        //         }
        //     });

        // glm::i32vec3 voxels_size = m_chunks->getChunkSize() * m_chunks->getChunksSize();
        // glm::i32vec3 voxel_coords;
        // for (voxel_coords.z = 0; voxel_coords.z < voxels_size.z; ++voxel_coords.z) {
        //     for (voxel_coords.x = 0; voxel_coords.x < voxels_size.x; ++voxel_coords.x) {
        //         for (voxel_coords.y = voxels_size.y - 1; voxel_coords.y >= 0; --voxel_coords.y) {
        //             auto *voxel = m_chunks->getVoxel(voxel_coords);
        //             if (voxel->id != 0)
        //                 break;

        //             m_chunks->getChunkByVoxel(voxel_coords)
        //                 ->getLightmap()
        //                 .setS(voxel_coords % m_chunks->getChunkSize(), 0xF);
        //         }
        //     }
        // }

        // for (voxel_coords.z = 0; voxel_coords.z < voxels_size.z; ++voxel_coords.z) {
        //     for (voxel_coords.x = 0; voxel_coords.x < voxels_size.x; ++voxel_coords.x) {
        //         for (voxel_coords.y = voxels_size.y - 1; voxel_coords.y >= 0; --voxel_coords.y) {
        //             auto *voxel = m_chunks->getVoxel(voxel_coords);
        //             if (voxel->id != 0)
        //                 break;

        //             if (m_chunks->getLight(voxel_coords + glm::i32vec3{-1, 0, 0}, 3) == 0
        //                 || m_chunks->getLight(voxel_coords + glm::i32vec3{1, 0, 0}, 3) == 0
        //                 || m_chunks->getLight(voxel_coords + glm::i32vec3{0, -1, 0}, 3) == 0
        //                 || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 1, 0}, 3) == 0
        //                 || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 0, -1}, 3) == 0
        //                 || m_chunks->getLight(voxel_coords + glm::i32vec3{0, 0, 1}, 3) == 0) {
        //                 m_s_solver->add(voxel_coords);
        //             }

        //             m_chunks->getChunkByVoxel(voxel_coords)
        //                 ->getLightmap()
        //                 .setS(voxel_coords % m_chunks->getChunkSize(), 0xF);
        //         }
        //     }
        // }

        // m_r_solver->solve();
        // m_g_solver->solve();
        // m_b_solver->solve();
        // m_s_solver->solve();

        // === Models ===
        createModelEntity(m_test_entity, "girl", {0.0f, 10.0f, 0.0f}, vec3{0.0f}, {0.3f, 0.3f, 0.3f});
        createModelEntity(m_test_entity_2,
                          "shrek",
                          {10.0f, 10.0f, 0.0f},
                          vec3{0.0f},
                          {0.05f, 0.05f, 0.05f});

        createModelEntity(m_level_entity, "level", vec3{0.0f}, vec3{0.0f}, {0.03f, 0.03f, 0.03f});

        // ==============

        // === Lights ===
        getScene3D()->getLights()->setWorldLightAmbient({0.3f, 0.3f, 0.3f});
        getScene3D()->getLights()->setWorldLightDiffuse({0.5f, 0.5f, 0.5f});
        getScene3D()->getLights()->setWorldLightDirection({-2.0f, -5.0f, -2.0f});

        // m_point_light = getScene3D()->getLights()->createLight();
        // m_point_light.setPosition({20.0f, 20.0f, 20.0f});
        // m_point_light.setAmbient({1.0f, 1.0f, 1.0f});
        // m_point_light.setDiffuse({1.0f, 1.0f, 1.0f});
        // m_point_light.setSpecular({1.0f, 1.0f, 1.0f});
        // m_point_light.setConstant(1.0f);
        // m_point_light.setLinear(0.022f);
        // m_point_light.setQuadratic(0.0019f);

        // m_light = getScene3D()->getLights()->createLight();
        // m_light.setPosition({1.0f, 0.0f, 1.0f});
        // m_light.setAmbient({1.0f, 1.0f, 1.0f});
        // m_light.setDiffuse({1.0f, 1.0f, 1.0f});
        // m_light.setSpecular({1.0f, 1.0f, 1.0f});
        // m_light.setConstant(1.0f);
        // m_light.setLinear(0.022f);
        // m_light.setQuadratic(0.0019f);

        // m_light.setCutOff(0.9978f);
        // m_light.setOuterCutOff(0.953f);

        // ==============

        // === Physics ===
        createCubeEntity(m_static_block,
                         getAssets().get<eb::Texture>("test"),
                         {0, 0, 0},
                         {20, 10, 20});
        createCubeEntity(m_dynamic_block,
                         getAssets().get<eb::Texture>("test"),
                         {0, 0, 0},
                         {5, 5, 5});

        reactphysics3d::PhysicsWorld::WorldSettings settings;
        settings.defaultVelocitySolverNbIterations = 20;
        settings.isSleepingEnabled = false;
        settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);

        m_world = m_physics_common.createPhysicsWorld(settings);

        reactphysics3d::Vector3 position(0.0, 0.0, 0.0);
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
        reactphysics3d::Transform transform(position, orientation);

        m_body_1 = m_world->createRigidBody(transform);
        m_body_1->setType(reactphysics3d::BodyType::STATIC);

        reactphysics3d::Vector3 position2(5.0, 50.0, 5.0);
        reactphysics3d::Quaternion orientation2 = reactphysics3d::Quaternion::identity();
        reactphysics3d::Transform transform2(position2, orientation2);
        m_body_2 = m_world->createRigidBody(transform2);

        const reactphysics3d::Vector3 halfExtents(10.0, 5.0, 10.0);
        reactphysics3d::BoxShape *boxShape1 = m_physics_common.createBoxShape(halfExtents);
        reactphysics3d::Collider *collider;
        collider = m_body_1->addCollider(boxShape1, reactphysics3d::Transform::identity());

        const reactphysics3d::Vector3 halfExtents2(2.5, 2.5, 2.5);
        reactphysics3d::BoxShape *boxShape2 = m_physics_common.createBoxShape(halfExtents2);
        reactphysics3d::Collider *collider2;
        collider2 = m_body_2->addCollider(boxShape2, reactphysics3d::Transform::identity());

        spdlog::info("OK");
    }

    void onProcessInput(const eb::Time &elapsed)
    {
        eb::Engine::onProcessInput(elapsed);

        auto &keyboard = getKeyboard();
        auto &mouse = getMouse();
        auto &window = getWindow();

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

        if (keyboard.isKeyJustPressed(GLFW_KEY_TAB))
            window.setMouseEnable(!window.isMouseEnable());

        // === Camera ===

        if (!window.isMouseEnable()) {
            m_scene_camera.dir_y += -static_cast<float>(mouse.getDeltaY()) / window.getSize().x
                                    * 1.5f;
            m_scene_camera.dir_x += -static_cast<float>(mouse.getDeltaX()) / window.getSize().y
                                    * 1.5f;

            if (m_scene_camera.dir_y < -glm::radians(89.0f))
                m_scene_camera.dir_y = -glm::radians(89.0f);

            if (m_scene_camera.dir_y > glm::radians(89.0f)) {
                m_scene_camera.dir_y = glm::radians(89.0f);
            }

            m_scene_camera.camera->setRotation(
                glm::vec3{m_scene_camera.dir_y, m_scene_camera.dir_x, 0.0f});

            m_scene_camera.front = glm::vec3(m_scene_camera.camera->getRotationMat()
                                             * glm::vec4(0, 0, -1, 1));
            m_scene_camera.right = glm::vec3(m_scene_camera.camera->getRotationMat()
                                             * glm::vec4(1, 0, 0, 1));
        }

        if (keyboard.isKeyPressed(GLFW_KEY_W))
            m_scene_camera.camera->move(m_scene_camera.front * elapsed.asSeconds()
                                        * m_scene_camera.speed);

        if (keyboard.isKeyPressed(GLFW_KEY_S))
            m_scene_camera.camera->move(-m_scene_camera.front * elapsed.asSeconds()
                                        * m_scene_camera.speed);

        if (keyboard.isKeyPressed(GLFW_KEY_A))
            m_scene_camera.camera->move(-m_scene_camera.right * elapsed.asSeconds()
                                        * m_scene_camera.speed);

        if (keyboard.isKeyPressed(GLFW_KEY_D))
            m_scene_camera.camera->move(m_scene_camera.right * elapsed.asSeconds()
                                        * m_scene_camera.speed);

        // ==============

        m_light.setPosition(m_scene_camera.camera->getPosition());
        m_light.setDirection(m_scene_camera.camera->getFront());

        // ray cast
        // {
        //     m_target_block_visible = false;

        //     glm::vec3 end;
        //     glm::vec3 norm;
        //     glm::vec3 iend;
        //     const eb::Voxel *voxel = m_chunks->rayCast(m_scene_camera.camera->getPosition(),
        //                                                m_scene_camera.camera->getFront(),
        //                                                10.0f,
        //                                                end,
        //                                                norm,
        //                                                iend);

        //     if (voxel) {
        //         m_target_block_visible = true;
        //         auto &target_dc = getScene3D()->getRegistry().get<eb::DrawableComponent>(
        //             m_target_entity);
        //         target_dc.drawable->setPosition(iend);

        //         if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_1)) {
        //             m_chunks->setVoxelByGlobal(iend, eb::Voxel{0});

        //             glm::i32vec3 voxel_coords = iend / m_chunks->getVoxelSize();

        //             m_r_solver->remove(voxel_coords);
        //             m_g_solver->remove(voxel_coords);
        //             m_b_solver->remove(voxel_coords);

        //             m_r_solver->solve();
        //             m_g_solver->solve();
        //             m_b_solver->solve();

        //             if (m_chunks->getLight(voxel_coords + glm::i32vec3{0, 1, 0}, 3) == 0xF) {
        //                 for (int32_t i = voxel_coords.y; i >= 0; i--) {
        //                     if (m_chunks->getVoxel(glm::i32vec3{voxel_coords.x, i, voxel_coords.z})->id
        //                         != 0)
        //                         break;
        //                     m_s_solver->add(glm::i32vec3{voxel_coords.x, i, voxel_coords.z}, 0xF);
        //                 }
        //             }

        //             static const std::vector<glm::i32vec3> neighbors = {glm::i32vec3{0, 0, 1},
        //                                                                 glm::i32vec3{0, 0, -1},
        //                                                                 glm::i32vec3{0, 1, 0},
        //                                                                 glm::i32vec3{0, -1, 0},
        //                                                                 glm::i32vec3{1, 0, 0},
        //                                                                 glm::i32vec3{-1, 0, 0}};

        //             for (const auto &n : neighbors) {
        //                 m_r_solver->add(voxel_coords + n);
        //                 m_g_solver->add(voxel_coords + n);
        //                 m_b_solver->add(voxel_coords + n);
        //                 m_s_solver->add(voxel_coords + n);
        //             }

        //             m_r_solver->solve();
        //             m_g_solver->solve();
        //             m_b_solver->solve();
        //             m_s_solver->solve();
        //         }

        //         if (mouse.isButtonJustPressed(GLFW_MOUSE_BUTTON_2)) {
        //             int32_t id = 3;

        //             m_chunks->setVoxelByGlobal(iend + norm, eb::Voxel{id});

        //             glm::i32vec3 voxel_coords = (iend + norm) / m_chunks->getVoxelSize();

        //             m_r_solver->remove(voxel_coords);
        //             m_g_solver->remove(voxel_coords);
        //             m_b_solver->remove(voxel_coords);
        //             m_s_solver->remove(voxel_coords);

        //             for (int32_t i = voxel_coords.y - 1; i >= 0; i--) {
        //                 m_s_solver->remove(glm::i32vec3{voxel_coords.x, i, voxel_coords.z});
        //                 if (i == 0
        //                     || m_chunks->getVoxel(glm::i32vec3{voxel_coords.x, i, voxel_coords.z})->id
        //                            != 0) {
        //                     break;
        //                 }
        //             }
        //             m_r_solver->solve();
        //             m_g_solver->solve();
        //             m_b_solver->solve();
        //             m_s_solver->solve();

        //             if (id == 3) {
        //                 m_r_solver->add(voxel_coords, 15);
        //                 m_g_solver->add(voxel_coords, 15);
        //                 m_b_solver->add(voxel_coords, 15);

        //                 m_r_solver->solve();
        //                 m_g_solver->solve();
        //                 m_b_solver->solve();
        //             }
        //         }
        //     }
        // }
    }

    void onTick(const eb::Time &elapsed)
    {
        auto &window = getWindow();
        auto &keyboard = getKeyboard();
        auto &mouse = getMouse();

        // auto &sprite_entity_2_dc = getScene3D()->getRegistry().get<eb::DrawableComponent>(
        //     m_sptite_entity_2);

        // sprite_entity_2_dc.drawable->rotate(glm::vec3{glm::radians(0.0f),
        //                                               glm::radians(0.0f),
        //                                               glm::radians(elapsed.asSeconds() * 20)});

        // m_chunks->update();

        m_world->update(elapsed.asSeconds());

        const reactphysics3d::Transform &transform = m_body_1->getTransform();
        const reactphysics3d::Vector3 &position = transform.getPosition();

        auto &static_dc = getScene3D()->getRegistry().get<eb::DrawableComponent>(m_static_block);
        static_dc.drawable->setPosition({position.x, position.y, position.z});

        const reactphysics3d::Transform &transform2 = m_body_2->getTransform();
        const reactphysics3d::Vector3 &position2 = transform2.getPosition();

        const quat glm_quat(transform2.getOrientation().w,
                            transform2.getOrientation().x,
                            transform2.getOrientation().y,
                            transform2.getOrientation().z);
        const vec3 euler = glm::eulerAngles(glm_quat);

        auto &dynamic_dc = getScene3D()->getRegistry().get<eb::DrawableComponent>(m_dynamic_block);
        dynamic_dc.drawable->setPosition({position2.x, position2.y, position2.z});
        dynamic_dc.drawable->setRotation({euler.x, euler.y, euler.z});
    }

    void onDraw(const eb::Time &elapsed)
    {
        eb::Engine::onDraw(elapsed);

        getScene2D()->draw(*m_target_sprite);

        // m_chunks->draw(getWindow());
    }

private:
    void createModelEntity(entt::entity &entity,
                           const std::string &model_name,
                           const vec3 &position = vec3{0.0f},
                           const vec3 &rotation = vec3{0.0f},
                           const vec3 &scale = vec3{1.0f})
    {
        auto &registry = getScene3D()->getRegistry();

        entity = registry.create();
        auto &drawable_comp = registry.emplace<eb::DrawableComponent>(entity);

        auto model = std::make_shared<eb::ModelInstance>(getAssets().get<eb::Model>(model_name));
        model->setPosition(position);
        model->setRotation(rotation);
        model->setScale(scale);
        drawable_comp.drawable = model;
    }

    void createCubeEntity(entt::entity &entity,
                          const std::shared_ptr<eb::Texture> &texture,
                          const vec3 &from,
                          const vec3 &to)
    {
        auto &registry = getScene3D()->getRegistry();

        entity = registry.create();
        auto &drawable_comp = registry.emplace<eb::DrawableComponent>(entity);

        auto shape = std::make_shared<eb::Shape3D>();
        shape->setMaterial({texture});
        shape->createCube(from, to);
        shape->setOrigin((to - from) / 2.0f);
        drawable_comp.drawable = shape;
    }

private:
    entt::entity m_sptite_entity_1;
    entt::entity m_sptite_entity_2;

    struct SceneCamera
    {
        std::shared_ptr<eb::Camera> camera;
        glm::vec3 front;
        glm::vec3 right;
        float dir_x{0.0f};
        float dir_y{0.0f};
        float speed{20.0f};
    } m_scene_camera;

    // std::unique_ptr<eb::Chunks> m_chunks;

    std::unique_ptr<eb::Sprite2D> m_target_sprite;

    // std::unique_ptr<eb::LinesBatch> m_target_block;
    // entt::entity m_target_entity;
    // bool m_target_block_visible;

    // std::unique_ptr<eb::LightSolver> m_r_solver;
    // std::unique_ptr<eb::LightSolver> m_g_solver;
    // std::unique_ptr<eb::LightSolver> m_b_solver;
    // std::unique_ptr<eb::LightSolver> m_s_solver;

    entt::entity m_test_entity;
    entt::entity m_test_entity_2;
    entt::entity m_level_entity;

    eb::Light m_point_light;
    eb::Light m_light;

    entt::entity m_static_block;
    entt::entity m_dynamic_block;

    reactphysics3d::PhysicsCommon m_physics_common;
    reactphysics3d::PhysicsWorld *m_world;
    reactphysics3d::RigidBody *m_body_1;
    reactphysics3d::RigidBody *m_body_2;
};

int main()
{
    spdlog::set_level(spdlog::level::debug);

    spdlog::debug("Voxel game");

    EngineImpl{}.mainLoop();

    return 0;
}
