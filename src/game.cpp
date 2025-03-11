#include "game.h"

#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/model_renderer.h>


ModelRenderer *renderer;
Animator *mona_animator;

Game::Game(unsigned int width, unsigned int height) 
: State(GAME_ACTIVE), Keys(), Width(width), Height(height), Camera(glm::vec3(0.0f, 0.0f, 3.0f))
{ 
}

Game::~Game()
{
    delete renderer;
    delete mona_animator;
}

void Game::Init()
{
    ResourceManager::LoadShader(
        "D:/MyProjects/cpp/gewuln/src/shaders/vertex.vert", 
        "D:/MyProjects/cpp/gewuln/src/shaders/fragment.frag", 
        nullptr, 
        "shader"
    );
    
    renderer = new ModelRenderer(ResourceManager::GetShader("shader"));
    
    auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/gltf/mona.gltf";
    ResourceManager::LoadModel(mona_path, true, "mona");
    mona_animator = new Animator(
        mona_path, 
        ResourceManager::GetModel("mona")
    );
    
    ResourceManager::LoadModel(
        "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/test_floor/gltf/test_floor.gltf",
        false,
        "floor"
    );
    
    ResourceManager::LoadModel(
        "D:/MyProjects/cpp/gewuln/assets/models/room/gltf/room.gltf",
        false,
        "room"
    );
}

void Game::Update(float dt)
{
    mona_animator->UpdateAnimation(dt);
}

void Game::ProcessInput(float dt)
{
    // cam movement    
    if (Keys[GLFW_KEY_W])
        Camera.ProcessKeyboard(FORWARD, dt);
    if (Keys[GLFW_KEY_S])
        Camera.ProcessKeyboard(BACKWARD, dt);
    if (Keys[GLFW_KEY_A])
        Camera.ProcessKeyboard(LEFT, dt);
    if (Keys[GLFW_KEY_D])
        Camera.ProcessKeyboard(RIGHT, dt);
    
    // animations
    if (Keys[GLFW_KEY_1])
        mona_animator->PlayAnimation("idle");
    if (Keys[GLFW_KEY_2])
        mona_animator->PlayAnimation("walk");
    if (Keys[GLFW_KEY_3])
        mona_animator->PlayAnimation("interact");
}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    Camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    Camera.ProcessMouseScroll(yoffset);
}

void Game::Render()
{
    renderer->DrawAnimatedModel(
        ResourceManager::GetModel("mona"), 
        Camera, 
        (float)Width/(float)Height,
        mona_animator
    );
    
    renderer->DrawSimpleModel(
        ResourceManager::GetModel("floor"),
        Camera,
        (float)Width/(float)Height, 
        glm::vec3(0, -1, 0),
        0.0f, 
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
 
}