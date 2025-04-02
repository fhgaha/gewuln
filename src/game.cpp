#include "game.h"
#include <gewuln/model_renderer.h>
#include <gewuln/character.h>
#include <map>

ModelRenderer *renderer;

std::unordered_map<std::string, Character>  characters;
Character                                   *active_character;


Game::Game(unsigned int width, unsigned int height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height), free_look_cam(glm::vec3(0.0f, 0.0f, 3.0f)){}

Game::~Game()
{
    delete renderer;
}

void Game::Init()
{
    free_look_cam.Position = glm::vec3(-1.0f, 1.0f, 3.0f);

    ResourceManager::LoadShader(
        "D:/MyProjects/cpp/gewuln/src/shaders/vertex.vert",
        "D:/MyProjects/cpp/gewuln/src/shaders/fragment.frag",
        nullptr,
        "shader"
    );

    renderer = new ModelRenderer(ResourceManager::GetShader("shader"));

    {
        auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/gltf_2/mona.gltf";
        ResourceManager::LoadModel(mona_path, true, "mona");

        characters["mona"] = Character(
            &ResourceManager::GetModel("mona"),
            Animator(
                mona_path,
                ResourceManager::GetModel("mona")
            ),
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
        active_character = &characters["mona"];
    }



    ResourceManager::LoadModel(
        "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/test_floor/gltf/test_floor.gltf",
        false,
        "floor"
    );

    ResourceManager::LoadModel(
        "D:/MyProjects/cpp/gewuln/assets/models/room/gltf/applyed_transforms/room.gltf",
        false,
        "room"
    );
}

void Game::Update(float dt)
{
    if (active_character) {
        active_character->Update(dt);
    }
}

void Game::ProcessInput(float dt)
{
    // cam movement
    // if (Keys[GLFW_KEY_W])
    if (Keys[GLFW_KEY_UP])
        free_look_cam.ProcessKeyboard(FORWARD, dt);
    // if (Keys[GLFW_KEY_S])
    if (Keys[GLFW_KEY_DOWN])
        free_look_cam.ProcessKeyboard(BACKWARD, dt);
    // if (Keys[GLFW_KEY_A])
    if (Keys[GLFW_KEY_LEFT])
        free_look_cam.ProcessKeyboard(LEFT, dt);
    // if (Keys[GLFW_KEY_D])
    if (Keys[GLFW_KEY_RIGHT])
        free_look_cam.ProcessKeyboard(RIGHT, dt);

    // animations
    if (active_character) {
        active_character->ProcessInput(Keys, dt);
    }

}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    free_look_cam.ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    free_look_cam.ProcessMouseScroll(yoffset);
}

void Game::Render()
{
    renderer->DrawCharacter(
        active_character,
        free_look_cam,
        (float)Width/(float)Height
        // glm::vec3(-1, 0, 0)
    );

    renderer->DrawSimpleModel(
        ResourceManager::GetModel("room"),
        free_look_cam,
        (float)Width/(float)Height
    );

    
}