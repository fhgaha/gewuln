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
bool show_granny_text;

ModelRenderer                                               *model_renderer;
TextRenderer                                                *text_renderer;
Character                                                   *active_character;

std::unordered_map<std::string, Character>                  characters;


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
        auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/export/gltf_3_cube_collider/mona.gltf";
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

    {//text renderer
        text_renderer = new TextRenderer(this->Width, this->Height);
        text_renderer->Load("D:/MyProjects/cpp/gewuln/assets/fonts/arial/arial.ttf", 24);
    }
    
    
    {//setting up rooms
        {//first room
            ResourceManager::LoadModel(
                "D:/MyProjects/cpp/gewuln/assets/models/room/export/gltf_6_room_exit/room.gltf",
                false,
                "room"
            );
            
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
            
            start_room->init_interactable(
                "kitchen_inter", 
                Room::Interactable{
                    //TODO hardcoded garbage shit
                    .mesh = &start_room->model->interactiable_meshes[0],
                    .glfw_key = GLFW_KEY_E,
                    .action = []{
                        std::cout << "hello action\n";
                    }
                }
            );
            
        }

        
        {//second room
            ResourceManager::LoadModel(
                "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/export/test_floor/gltf_2_tiling_texture/test_rooms.gltf",
                false,
                "another_room"
            );
            
            rooms["another_room"] = std::make_unique<Room>();
            auto another_room = rooms["another_room"].get();
            another_room->Init(&ResourceManager::GetModel("another_room"));
            another_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                glm::vec3(-3.228f, 3.582f, 4.333f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                -39.0f,
                41.0f
            );
            another_room->initial_cam = another_room->cameras["cam_fly"].get();
            another_room->active_cam = another_room->initial_cam;
        }

        //TODO forced to do this shit after creating "another room"
        {//start room exits
            
            start_room->init_exit(
                "exit",
                Room::Exit {
                    //TODO hardcoded garbage code
                    .mesh           = &rooms["start_room"].get()->model->room_exit_meshes[0],
                    .glfw_key       = GLFW_KEY_E,
                    .action = [this]{
                        this->current_room = this->rooms["another_room"].get();
                    },
                    .this_room      = rooms["start_room"].get(),
                    .go_to_room     = rooms["another_room"].get(),
                    .on_room_enter  = []{
                        std::cout << "start room: on room enter\n";
                    },
                    .on_room_exit   = []{
                        std::cout << "start room: on room exit\n";
                    }
                }
            );
        
        }        
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
    if (Keys[GLFW_KEY_UP])
        current_room->active_cam->ProcessKeyboard(FORWARD, dt);
    if (Keys[GLFW_KEY_DOWN])
        current_room->active_cam->ProcessKeyboard(BACKWARD, dt);
    if (Keys[GLFW_KEY_LEFT])
        current_room->active_cam->ProcessKeyboard(LEFT, dt);
    if (Keys[GLFW_KEY_RIGHT])
        current_room->active_cam->ProcessKeyboard(RIGHT, dt);

    if (active_character) {
        active_character->ProcessInput( this, dt);
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
    model_renderer->DrawCharacter(
        active_character,
        current_room->active_cam,
        (float)Width/(float)Height
    );

    model_renderer->DrawSimpleModel(
        *current_room->model,
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
