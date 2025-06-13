#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "game.h"

// Use NVIDIA Gpu for NVIDIA Optimus laptops/GPUs
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void APIENTRY glDebugOutput(
    GLenum source, GLenum type, unsigned int id,
    GLenum severity, GLsizei length,
    const char *message, const void *userParam
);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

const bool debug = true;

bool firstMouse = true;
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;

// timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Game game(SCR_WIDTH, SCR_HEIGHT);

int main()
{
    std::cout << "Started main program" << std::endl;

    // glfw: initialize and configure
    glfwInit();
    // hints should be configured before we create a window:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (debug) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "gewuln", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetWindowAspectRatio(window, 4, 3);
    glfwSwapInterval(1);    //limits FPS to monitor's refresh rate

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    if (debug) {
        int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            std::cout << "Successfully initialized Open GL debug context\n";
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(
                  GL_DEBUG_SOURCE_API
                , GL_DEBUG_TYPE_ERROR
                , GL_DEBUG_SEVERITY_HIGH
                , 0, nullptr, GL_TRUE
            );
        }
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // draw faces of front sides only

    { // process alpha channel of textures
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // stbi_set_flip_vertically_on_load(true);


    game.init();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        game.update(deltaTime);
        game.process_input();

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
        );
        game.render();

        // glCheckError();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (!(key >= 0 && key < 1024)) {
        return;
    }

    switch (action)
    {
		// https://www.glfw.org/docs/3.3/input_guide.html
        case GLFW_PRESS:
            game.Keys[key] = true;
            break;
        case GLFW_RELEASE:
            game.Keys[key] = false;
            game.KeysProcessed[key] = false;
            break;
        default:
            break;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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

    game.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    game.process_mouse_scroll(static_cast<float>(yoffset));
}


// GLenum glCheckError_(const char *file, int line)
// {
    // GLenum errorCode;
    // while ((errorCode = glGetError()) != GL_NO_ERROR)
    // {
    //     std::string error;
    //     switch (errorCode)
    //     {
    //         case GL_INVALID_ENUM:         error = "INVALID_ENUM"; break;
    //         case GL_INVALID_VALUE:        error = "INVALID_VALUE"; break;
    //         case GL_INVALID_OPERATION:    error = "INVALID_OPERATION"; break;
    //         // case GL_STACK_OVERFLOW:    error = "STACK_OVERFLOW"; break;  //not working
    //         // case GL_STACK_UNDERFLOW:   error = "STACK_UNDERFLOW"; break; //not working
    //         case GL_OUT_OF_MEMORY:        error = "OUT_OF_MEMORY"; break;
    //         case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
    //     }
    //     std::cout << "--!!--" << error << " | " << file << " (" << line << ")" << std::endl;
    // }
//     return errorCode;
// }

void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam
){
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:               std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:       std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:             std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:            std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:          std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:             std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:    std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}