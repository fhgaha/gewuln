#include "game.h"

#include <gewuln/animation.h>
#include <gewuln/animator.h>


Game::Game(unsigned int width, unsigned int height) 
: State(GAME_ACTIVE), Keys(), Width(width), Height(height), Camera(glm::vec3(0.0f, 0.0f, 3.0f))
{ 
	
}

Game::~Game()
{
    
}


// ModelRenderer *Renderer;
Animator mona_animator;

    
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
    Model mona = ResourceManager::LoadModel(mona_path, true, "mona");
    mona_animator = Animator(mona_path, &mona);
}

void Game::Update(float dt)
{
    
    mona_animator.UpdateAnimation(dt);

    ResourceManager::GetShader("shader").Use();

    // scale -> rotate -> translate. with matrises multiplications it should be reversed. model mat is doing that.
    // Vclip = Mprojection * Mview * Mmodel * Vlocal
    
    glm::mat4 projection = glm::perspective(
        glm::radians(Camera.Zoom), (float)Width/(float)Height, 0.1f, 100.0f);
    ResourceManager::GetShader("shader").SetMatrix4("projection", projection);
    
    glm::mat4 view = Camera.GetViewMatrix();
    ResourceManager::GetShader("shader").SetMatrix4("view", view);
    
    // animation stuff
    auto transforms = mona_animator.GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i) {
        auto name = "finalBonesMatrices[" + std::to_string(i) + "]";
        ResourceManager::GetShader("shader").SetMatrix4(name.c_str(), transforms[i]);
    }
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, 0));
    // model = glm::rotate(model, (float)glfwGetTime() *  glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    ResourceManager::GetShader("shader").SetMatrix4("model", model);
    
    auto shader = ResourceManager::GetShader("shader");
    ResourceManager::GetModel("mona").Draw(shader);
 
    
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
        mona_animator.PlayAnimation("idle");
    if (Keys[GLFW_KEY_2])
        mona_animator.PlayAnimation("walk");
    if (Keys[GLFW_KEY_3])
        mona_animator.PlayAnimation("interact");
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
    //Renderer->DrawModel(
    //     ResourceManager::GetModel("mona"),
    //     ...
    // );    
}