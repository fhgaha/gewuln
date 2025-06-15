#include "game.h"
#include <unordered_map>
#include <memory>
#include <utility>
#include <stdio.h>
#include <gewuln/rendering/gizmo_renderer.h>
#include <gewuln/rendering/model_renderer.h>
#include <gewuln/rendering/text_renderer.h>
#include <gewuln/geometry_2d.h>

// settings
bool show_fps = true;
bool show_granny_text = true;

GizmoRenderer   *gizmo_renderer;
ModelRenderer   *model_renderer;
TextRenderer    *text_renderer;


Game::Game(unsigned int width, unsigned int height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height)/*, free_look_camera(glm::vec3(0.0f, 0.0f, 3.0f))*/{}

Game::~Game()
{
    delete gizmo_renderer;
    delete model_renderer;
    delete text_renderer;
    delete current_room;
    delete active_character;
}

void Game::init()
{
    { //renderers
        model_renderer = new ModelRenderer(
            ResourceManager::LoadShader(
                "D:/MyProjects/cpp/gewuln/src/shaders/default/default.vert",
                "D:/MyProjects/cpp/gewuln/src/shaders/default/default.frag",
                nullptr,
                "model_shader"
            )
        );
        Global::draw_gizmos = false;
        
        
        text_renderer = new TextRenderer(
            ResourceManager::LoadShader(
                "D:/MyProjects/cpp/gewuln/src/shaders/text_shaders/text_2d.vert",
                "D:/MyProjects/cpp/gewuln/src/shaders/text_shaders/text_2d.frag",
                nullptr,
                "text_shader"
            ),
            this->Width, 
            this->Height
        );
        text_renderer->Load("D:/MyProjects/cpp/gewuln/assets/fonts/arial/arial.ttf", 24);
        
        
        gizmo_renderer = new GizmoRenderer(
            ResourceManager::LoadShader(
                "D:/MyProjects/cpp/gewuln/src/shaders/origin_gizmo/origin_gizmo.vert",
                "D:/MyProjects/cpp/gewuln/src/shaders/origin_gizmo/origin_gizmo.frag",
                nullptr,
                "gizmo_shader"
            )
        );
    }

    

    {//characters
        {//main character
            auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/export/gltf_3_cube_collider/mona.gltf";
            ResourceManager::LoadModel(mona_path, true, "mona");

            characters["mona"] = Character(
                &ResourceManager::GetModel("mona"),
                Animator(
                    mona_path,
                    ResourceManager::GetModel("mona")
                ),
                glm::vec3(0.0f, 0.0f, 0.0f)
            );
            active_character = &characters["mona"];
        }
    }


    {//setting up rooms
        {//kitchen room
            ResourceManager::LoadModel(
                "D:/MyProjects/cpp/gewuln/assets/models/room/export/gltf_6_room_exit/room.gltf",
                false,
                "test_kitchen_room_model"
            );

            rooms["test_kitchen_room"] = std::make_unique<Room>();
            auto kitchen_room = rooms["test_kitchen_room"].get();

            {//start room cameras

                // //kitchen corner
                // start_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                //     glm::vec3(-3.228f, 3.582f, 4.333f),
                //     glm::vec3(0.0f, 1.0f, 0.0f),
                //     -39.0f,
                //     41.0f
                // );

                //close to mona
                kitchen_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                    CameraFly(
                        glm::vec3(-1.152526f, 1.611192f, 1.471875f),
                        glm::vec3(0.0f, 1.0f, 0.0f), //this garbage thing should be always up on start
                        -80.7201f,
                        13.36f
                    )
                );

                kitchen_room->cameras["look_at_camera_corridor"] = std::make_unique<CameraLookAt>(
                    glm::vec3(-3.228f, 3.582f, 4.333f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -39.0f,
                    41.0f
                );

                kitchen_room->cameras["look_at_camera_kitchen_start"] = std::make_unique<CameraLookAt>(
                    glm::vec3(1.367f, 2.045f, 3.924f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -86.3601f,
                    16.0f
                );

                kitchen_room->cameras["look_at_camera_kitchen_end"] = std::make_unique<CameraLookAt>(
                    glm::vec3(0.673f, 2.138f, 4.030f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -263.88f,
                    19.7599f
                );

                kitchen_room->initial_cam = kitchen_room->cameras["look_at_camera_corridor"].get();
                // start_room->active_cam = start_room->initial_cam;
                kitchen_room->current_cam = kitchen_room->cameras["cam_fly"].get();
            }

            kitchen_room->Init(&ResourceManager::GetModel("test_kitchen_room_model"));

            kitchen_room->init_interactable(
                "kitchen_inter",
                Room::Interactable{
                    //TODO hardcoded garbage shit
                    .mesh = &kitchen_room->model->interactiable_meshes[0],
                    .glfw_key = GLFW_KEY_E,
                    .action = []{
                        std::cout << "hello action\n";
                    }
                }
            );
            
            current_room = kitchen_room;
        }


        {//orange room
            ResourceManager::LoadModel(
                // "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/export/test_floor/gltf_2_tiling_texture/test_rooms.gltf",
                // "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/export/test_floor/gltf_3_added_interactable/test_rooms.gltf",
                "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/export/test_floor/gltf_4_two_interactables_one_with_center_below_another_with_center_above/test_rooms.gltf",
                false,
                "test_room_model"
            );

            rooms["test_room"] = std::make_unique<Room>();
            auto test_room = rooms["test_room"].get();
            test_room->Init(&ResourceManager::GetModel("test_room_model"));
            test_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                CameraFly(
                    glm::vec3(1.674590f, 4.121703f, 3.214545f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -90.0f,
                    50.0f
                )
            );

            { //interactables
                test_room->init_interactable(
                    "interactable_with_center_at_hight_half_meter",
                    Room::Interactable{
                        //TODO hardcoded garbage shit
                        .mesh = &test_room->model->interactiable_meshes[0],
                        .glfw_key = GLFW_KEY_E,
                        .action = []{
                            std::cout << "hello action\n";
                        }
                    }
                );

                test_room->init_interactable(
                    "interactable_with_center_at_hight_2_meters",
                    Room::Interactable{
                        //TODO hardcoded garbage shit
                        .mesh = &test_room->model->interactiable_meshes[1],
                        .glfw_key = GLFW_KEY_E,
                        .action = []{
                            std::cout << "hello action\n";
                        }
                    }
                );
            }

            test_room->initial_cam = test_room->cameras["cam_fly"].get();
            test_room->current_cam = test_room->initial_cam;
            current_room = test_room;
        }

        //TODO forced to do this shit after creating "another room"
        {//kitchen room exits

            rooms["test_kitchen_room"]->init_exit(
                "exit",
                Room::Exit {
                    //TODO hardcoded garbage code
                    .mesh           = &rooms["test_kitchen_room"].get()->model->room_exit_meshes[0],
                    .glfw_key       = GLFW_KEY_E,
                    .action = [this]{
                        this->current_room = this->rooms["test_room"].get();
                    },
                    .this_room      = rooms["test_kitchen_room"].get(),
                    .go_to_room     = rooms["test_room"].get(),
                    .on_room_enter  = []{
                        std::cout << "start room: on room enter\n";
                    },
                    .on_room_exit   = []{
                        std::cout << "start room: on room exit\n";
                    }
                }
            );
        }

        { //town
            ResourceManager::LoadModel(
                "D:/MyProjects/cpp/gewuln/assets/models/town/export/glfw_5_fixed_faces/town.gltf",
                false,
                "town_model"
            );

            rooms["town_room"] = std::make_unique<Room>();
            auto town_room = rooms["town_room"].get();
            town_room->Init(&ResourceManager::GetModel("town_model"));
            town_room->cameras["cam_fly"] = std::make_unique<CameraFly>(
                CameraFly(
                    glm::vec3(2.191804, 6.516104, -7.915638),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -249.56f,
                    25.0f
                )
            );

            town_room->initial_cam = town_room->cameras["cam_fly"].get();
            town_room->current_cam = town_room->initial_cam;
            current_room = town_room;
        }
        
        
        
        { //check_everything_inited
            assert(this->current_room               && "!Current room is not set");
            assert(this->current_room->current_cam  && "!Current camera is not set");
        }
    }
}


void Game::update(float dt)
{
    this->dt = dt;

    if (active_character) {
        // active_character->room_interactables_tmp = &current_room->interactables;
        active_character->current_room_tmp = current_room;
        active_character->Update(dt);

        //look at cam looks at character, fly cam does nothing
        glm::vec3 trg = active_character->position + glm::vec3(0.0f, 1.5f, 0.0f);
        current_room->current_cam->LookAt(&trg);
    }

}

void Game::process_input()
{
    if (Keys[GLFW_KEY_UP])
        current_room->current_cam->ProcessKeyboard(FORWARD, dt);
    if (Keys[GLFW_KEY_DOWN])
        current_room->current_cam->ProcessKeyboard(BACKWARD, dt);
    if (Keys[GLFW_KEY_LEFT])
        current_room->current_cam->ProcessKeyboard(LEFT, dt);
    if (Keys[GLFW_KEY_RIGHT])
        current_room->current_cam->ProcessKeyboard(RIGHT, dt);

    if (active_character) {
        active_character->current_room_tmp = this->current_room;
        active_character->ProcessInput(Keys, KeysProcessed, dt);
    }

    if (Keys[GLFW_KEY_GRAVE_ACCENT] && !KeysProcessed[GLFW_KEY_GRAVE_ACCENT]) { /* ` */
        Global::draw_gizmos = !Global::draw_gizmos;
        KeysProcessed[GLFW_KEY_GRAVE_ACCENT] = true;
    }

    if (Keys[GLFW_KEY_1] && !KeysProcessed[GLFW_KEY_1]) {
        current_room->current_cam->print_state();
        KeysProcessed[GLFW_KEY_1] = true;
    }


}

void Game::process_mouse_movement(float xoffset, float yoffset)
{
    current_room->current_cam->ProcessMouseMovement(xoffset, yoffset);
}

void Game::process_mouse_scroll(float yoffset)
{
    current_room->current_cam->ProcessMouseScroll(yoffset);
}

void Game::render()
{
    model_renderer->DrawCharacter(
        active_character,
        current_room->current_cam,
        (float)Width/(float)Height
    );

    model_renderer->DrawSimpleModel(
        *current_room->model,
        current_room->current_cam,
        (float)Width/(float)Height
    );

    //fps
    //TODO no need to pass it there every frame
    if (show_fps) {
        text_renderer->Draw(std::format("FPS: {}", (int)(1/this->dt)), this->Width * 0.05f, this->Height * 0.05f, 1.0f);
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
    

    if (Global::draw_gizmos) {
        //TODO no need to pass it there every frame
        gizmo_renderer->Draw(current_room->current_cam, (float)Width/(float)Height);
    }        
    
}
