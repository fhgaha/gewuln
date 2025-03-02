#include "game.h"
#include <camera.h>

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 
	
}

Game::~Game()
{
    
}

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
int SCR_WIDTH = 800; int SCR_HEIGHT = 600;
// ModelRenderer *Renderer;
    
void Game::Init()
{
    ResourceManager::LoadShader(
        "src/shaders/tex/vertex.vert", 
        "src/shaders/tex/fragment.frag", 
        nullptr, 
        "shader"
    );
    
    //Renderer = new ModelRenderer(ResourceManager::GetShader("shader"));
    auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/gltf/mona.gltf";
    ResourceManager::LoadModel(mona_path, true, "mona");
}

void Game::Update(float dt)
{
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), 
        (float)SCR_WIDTH/(float)SCR_HEIGHT, 
        0.1f, 
        100.0f
    );
    ResourceManager::GetShader("shader").Use().SetMatrix4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    ResourceManager::GetShader("shader").SetMatrix4("view", view);
    
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    
    //Renderer->DrawModel(
    //     ResourceManager::GetModel("mona"),
    //     ...
    // );    
}