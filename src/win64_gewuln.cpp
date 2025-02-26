#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gewuln/shader.h>
#include <gewuln/camera.h>
#include <gewuln/model.h>

#include <iostream>
#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/resource_manager.h>
// #include <gewuln/texture.cpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;

// timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Animator mona_animator;

int main()
{
    std::cout << "Started main program" << std::endl;
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "gewuln", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);    //limits FPS to monitor's refresh rate

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    glEnable(GL_DEPTH_TEST);
    
    { // process alpha channel of textures
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // stbi_set_flip_vertically_on_load(true);
    

    Shader shader = ResourceManager::LoadShader(
        "D:/MyProjects/cpp/gewuln/src/shaders/vertex.vert", 
        "D:/MyProjects/cpp/gewuln/src/shaders/fragment.frag", 
        nullptr, "test");
    
    // animations
    auto mona_path = "D:/MyProjects/cpp/gewuln/assets/models/mona_sax/gltf/mona.gltf";
    Model mona = ResourceManager::LoadModel(mona_path, true, "mona_model");
    mona_animator = Animator(mona_path, &mona);
    
    Model floor = ResourceManager::LoadModel(
        "D:/MyProjects/cpp/gewuln/assets/models/test_rooms/test_floor/gltf/test_floor.gltf",
        false, 
        "floor"
    );

    // Model room = ResourceManager::LoadModel(
    //     "D:/MyProjects/cpp/gewuln/assets/models/room/gltf/room.gltf", 
    //     false, 
    //     "room"
    // );
    
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        processInput(window);
        mona_animator.UpdateAnimation(deltaTime);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(
              GL_COLOR_BUFFER_BIT 
            | GL_DEPTH_BUFFER_BIT
        );

        shader.Use();
        
        //mona
        {
            // scale -> rotate -> translate. with matrises multiplications it should be reversed. model mat is doing that.
            // Vclip = Mprojection * Mview * Mmodel * Vlocal
            
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
            shader.SetMatrix4("projection", projection);
            
            glm::mat4 view = camera.GetViewMatrix();
            shader.SetMatrix4("view", view);

            // animation stuff
            auto transforms = mona_animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                auto name = "finalBonesMatrices[" + std::to_string(i) + "]";
                shader.SetMatrix4(name.c_str(), transforms[i]);
            }
            
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(0, 0, 0));
            // model = glm::rotate(model, (float)glfwGetTime() *  glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            
            shader.SetMatrix4("model", model);
            
            mona.Draw(shader);
        }
        
        // floor
        {
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
            shader.SetMatrix4("projection", projection);
            
            glm::mat4 view = camera.GetViewMatrix();
            shader.SetMatrix4("view", view);

            // we ignore finalBonesMatrices here
            
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(0, -1.0f, 0));
            // model = glm::rotate(model, (float)glfwGetTime() *  glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            
            shader.SetMatrix4("model", model);
            
            floor.Draw(shader);
        }
 
        glCheckError();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // cam movement    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // animations
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        mona_animator.PlayAnimation("idle");
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        mona_animator.PlayAnimation("walk");
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        mona_animator.PlayAnimation("interact");
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:         error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:        error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:    error = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW:    error = "STACK_OVERFLOW"; break;  //not working
            // case GL_STACK_UNDERFLOW:   error = "STACK_UNDERFLOW"; break; //not working
            case GL_OUT_OF_MEMORY:        error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "--!!--" << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}