#include "game.h"
#include <gewuln/model_renderer.h>
#include <gewuln/character.h>
#include <gewuln/text_renderer.h>
#include <map>

ModelRenderer   *model_renderer;
TextRenderer    *text_renderer;

std::unordered_map<std::string, Character>  characters;
Character                                   *active_character;


Game::Game(unsigned int width, unsigned int height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height), free_look_cam(glm::vec3(0.0f, 0.0f, 3.0f)){}

Game::~Game()
{
    delete model_renderer;
    delete text_renderer;
}

void Game::Init()
{
    free_look_cam.Position = glm::vec3(-1.0f, 1.0f, 3.0f);

    ResourceManager::LoadShader(
        "D:/MyProjects/cpp/gewuln/src/shaders/vertex.vert",
        "D:/MyProjects/cpp/gewuln/src/shaders/fragment.frag",
        nullptr,
        "model_shader"
    );

    model_renderer = new ModelRenderer(ResourceManager::GetShader("model_shader"));

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


    {
        text_renderer = new TextRenderer(this->Width, this->Height);
        text_renderer->Load("D:/MyProjects/cpp/gewuln/assets/fonts/arial/arial.ttf", 24);
    }
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
    model_renderer->DrawCharacter(
        active_character,
        free_look_cam,
        (float)Width/(float)Height
    );

    model_renderer->DrawSimpleModel(
        ResourceManager::GetModel("room"),
        free_look_cam,
        (float)Width/(float)Height
    );

    std::string line_1 = "Probably a doghouse,";
    std::string line_2 = "though I'm not sure";
    std::string line_3 = "since there's no dog around";

    //                                                        font height  scale  some height
    text_renderer->Draw(line_1, this->Width/5.0f, this->Height - 24.0f *    2.0f *   4.0f, 2.0f);
    text_renderer->Draw(line_2, this->Width/5.0f, this->Height - 24.0f *    2.0f *   3.0f, 2.0f);
    text_renderer->Draw(line_3, this->Width/5.0f, this->Height - 24.0f *    2.0f *   2.0f, 2.0f);
}