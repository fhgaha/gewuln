#include "game.h"
#include <unordered_map>
#include <memory>
#include <utility>
#include <gewuln/model_renderer.h>
#include <gewuln/character.h>
#include <gewuln/text_renderer.h>
#include <gewuln/geometry_2d.h>


// settings
bool show_fps = true;


ModelRenderer                                               *model_renderer;
TextRenderer                                                *text_renderer;

std::unordered_map<std::string, Character>                  characters;
std::unordered_map<std::string, std::unique_ptr<Room>>      rooms;


Character                                                   *active_character;

bool                                                        show_granny_text;


Game::Game(unsigned int width, unsigned int height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height)/*, free_look_camera(glm::vec3(0.0f, 0.0f, 3.0f))*/{}

Game::~Game()
{
    delete model_renderer;
    delete text_renderer;
    delete start_room;
    delete current_room;
    delete active_character;
}

void Game::Init()
{
    ResourceManager::LoadShader(
        "D:/MyProjects/cpp/gewuln/src/shaders/default/default.vert",
        "D:/MyProjects/cpp/gewuln/src/shaders/default/default.frag",
        nullptr,
        "model_shader"
    );

    model_renderer = new ModelRenderer(ResourceManager::GetShader("model_shader"));
    model_renderer->draw_gizmos = true;

    {
        auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/gltf_3_cube_collider/mona.gltf";
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
        // "D:/MyProjects/cpp/gewuln/assets/models/room/gltf_3_walkable_area/room.gltf",
        "D:/MyProjects/cpp/gewuln/assets/models/room/gltf_4_walkable_area_merged_by_distance_removed_overlapping/room.gltf",
        // "D:/MyProjects/cpp/gewuln/assets/models/room/gltf_5_walkable_area_simple_square/room.gltf",
        false,
        "room"
    );


    {//text renderer
        text_renderer = new TextRenderer(this->Width, this->Height);
        text_renderer->Load("D:/MyProjects/cpp/gewuln/assets/fonts/arial/arial.ttf", 24);
    }

    
    {//setting up rooms
        rooms["start_room"] = std::make_unique<Room>();
        start_room = rooms["start_room"].get();

        {//start room cameras
            start_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                glm::vec3(-3.228f, 3.582f, 4.333f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                -39.0f,
                41.0f
            );

            start_room->cameras["look_at_camera_corridor"] = std::make_unique<CameraLookAt>(
                glm::vec3(-3.228f, 3.582f, 4.333f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                -39.0f,
                41.0f
            );

            start_room->cameras["look_at_camera_kitchen_start"] = std::make_unique<CameraLookAt>(
                glm::vec3(1.367f, 2.045f, 3.924f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                -86.3601f,
                16.0f
            );

            start_room->cameras["look_at_camera_kitchen_end"] = std::make_unique<CameraLookAt>(
                glm::vec3(0.673f, 2.138f, 4.030f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                -263.88f,
                19.7599f
            );

            start_room->initial_cam = start_room->cameras["look_at_camera_corridor"].get();
            // start_room->active_cam = start_room->initial_cam;
            start_room->active_cam = start_room->cameras["cam_fly"].get();
        }

        start_room->Init(&ResourceManager::GetModel("room"));
        
        current_room = start_room;
        
    }
}


void Game::Update(float dt)
{
    this->dt = dt;

    if (active_character) {
        active_character->Update(dt);

        //tmp
        glm::vec3 trg = active_character->position + glm::vec3(0.0f, 1.5f, 0.0f);
        current_room->active_cam->LookAt(&trg);
    }

}


void Game::ProcessInput()
{
    // if (Keys[GLFW_KEY_W])
    if (Keys[GLFW_KEY_UP])
        current_room->active_cam->ProcessKeyboard(FORWARD, dt);
    // if (Keys[GLFW_KEY_S])
    if (Keys[GLFW_KEY_DOWN])
        current_room->active_cam->ProcessKeyboard(BACKWARD, dt);
    // if (Keys[GLFW_KEY_A])
    if (Keys[GLFW_KEY_LEFT])
        current_room->active_cam->ProcessKeyboard(LEFT, dt);
    // if (Keys[GLFW_KEY_D])
    if (Keys[GLFW_KEY_RIGHT])
        current_room->active_cam->ProcessKeyboard(RIGHT, dt);


    if (active_character) {
        active_character->ProcessInput(Keys, this, dt);
    }

}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    current_room->active_cam->ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    current_room->active_cam->ProcessMouseScroll(yoffset);
}

void Game::Render()
{

    // std::cout << "camera pos " << active_cam->Position
    // << " yaw " << active_cam->Yaw
    // << " pitch " << active_cam->Pitch
    // << " zoom " << active_cam->Zoom
    // << "\n";

    model_renderer->DrawCharacter(
        active_character,
        current_room->active_cam,
        (float)Width/(float)Height
    );

    model_renderer->DrawSimpleModel(
        ResourceManager::GetModel("room"),
        current_room->active_cam,
        (float)Width/(float)Height
    );


    //fps
    if (show_fps) {
        text_renderer->Draw("FPS: " + std::to_string(1/this->dt), this->Width * 0.05f, this->Height * 0.05f, 1.0f);
    }

    //subtitles
    if (show_granny_text)
    {
        std::string line_1 = "Probably some granny's kitchen,";
        std::string line_2 = "though I'm not sure";
        std::string line_3 = "since there's no granny around";

        //                                                                    font height, scale, line height
        text_renderer->Draw(line_1, this->Width * 0.10f, this->Height - 24.0f * 2.0f * 4.0f, 1.5f);
        text_renderer->Draw(line_2, this->Width * 0.10f, this->Height - 24.0f * 2.0f * 3.0f, 1.5f);
        text_renderer->Draw(line_3, this->Width * 0.10f, this->Height - 24.0f * 2.0f * 2.0f, 1.5f);
    }
}

void Game::PlayCameraThing()
{
    //TODO camera is still targeting mona so it snips to her immidiatelly
    rooms["start_room"]->active_cam = rooms["start_room"]->cameras["look_at_camera_kitchen_start"].get();

    // start cam movement
    // final kitchen cam state: pos [    0.673,    2.138,    4.030] yaw -169.84 pitch 22.1999 zoom 35.8083

    show_granny_text = true;

}