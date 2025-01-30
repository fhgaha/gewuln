/*
Create two shader programs where the second program uses a different fragment shader
that outputs the color yellow; draw both triangles again where one outputs the color yellow.
Solution: /src/1.getting_started/2.5.hello_triangle_exercise3/.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
void single_triangle(GLFWwindow * window, unsigned int shaderProgram);
void two_triangles_EBO(GLFWwindow * window, unsigned int shaderProgram);
void two_triangles_no_EBO(GLFWwindow * window, unsigned int shaderProgram);
void two_triangles_orange_yellow(GLFWwindow * window, unsigned int shaderProgram, unsigned int vertexShader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderOrangeSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
const char *fragmentShaderYellowSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n\0";

int main()
{
    std::cout << "helloo from ex 5 8 3" << std::endl;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "gewuln", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    // orange
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShaderOrangeSource, NULL);
    glCompileShader(fragmentShaderOrange);
    // check for shader compile errors
    glGetShaderiv(fragmentShaderOrange, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderOrange, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShaderOrange);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderOrange);

    // single_triangle(window, shaderProgram);
    // two_triangles_EBO(window, shaderProgram);
    // two_triangles_no_EBO(window, shaderProgram);
    two_triangles_orange_yellow(window, shaderProgram, vertexShader);

    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void single_triangle(GLFWwindow * window, unsigned int shaderProgram)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void two_triangles_EBO(GLFWwindow * window, unsigned int shaderProgram)
{
    /*
       1         2
        *       *

            +

        *       *
       0         3
    */

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // left bot
        -0.5f,  0.5f, 0.0f,  // left top 
         0.5f,  0.5f, 0.0f,  // right top 
         0.5f, -0.5f, 0.0f,  // right bot
    }; 
    unsigned int indeces[] = {
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int VBO, VAO, EBO;

    {   // generation
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    {   // VAO bindings
        glBindVertexArray(VAO);
        
        {   // VBO bindings
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            
            // VBO specific stuff
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        {   // EBO bindings
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);
        }

    }

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        
        { //either of two:
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    //EBO
            // glDrawArrays(GL_TRIANGLES, 0, 3);    // VBO
        }
        
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void two_triangles_no_EBO(GLFWwindow * window, unsigned int shaderProgram)
{
    /*
       1         2
        *       *

            +

        *       *
       0         3
    */

    float vertices1[] = {
        -0.5f, -0.5f, 0.0f,  // left bot
        -0.5f,  0.5f, 0.0f,  // left top
         0.5f,  0.5f, 0.0f,  // right top 
    }; 

    float vertices2[] = {
         0.5f,  0.5f, 0.0f,  // right top 
         0.5f, -0.5f, 0.0f,  // right bot
        -0.5f, -0.5f, 0.0f,  // left bot
    };

    unsigned int VBO1;
    {
        glGenBuffers(1, &VBO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    }

    unsigned int VAO1;
    {   
        glGenVertexArrays(1, &VAO1);
        glBindVertexArray(VAO1);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    unsigned int VBO2;
    {
        glGenBuffers(2, &VBO2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    }

    unsigned int VAO2;
    {   
        glGenVertexArrays(2, &VAO2);
        glBindVertexArray(VAO2);
        glEnableVertexAttribArray(0); // should be not zero?
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // should be not zero?
    }

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 6);    

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);    
        
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO1); glDeleteVertexArrays(2, &VAO2);
    glDeleteBuffers(1, &VBO1);      glDeleteBuffers(2, &VBO2);
}

void two_triangles_orange_yellow(GLFWwindow * window, unsigned int shaderProgram, unsigned int vertexShader)
{
    /*
       1         2
        *       *

            +

        *       *
       0         3
    */

    float vertices1[] = {
        -0.5f, -0.5f, 0.0f,  // left bot
        -0.5f,  0.5f, 0.0f,  // left top
         0.5f,  0.5f, 0.0f,  // right top 
    }; 

    float vertices2[] = {
         0.5f,  0.5f, 0.0f,  // right top 
         0.5f, -0.5f, 0.0f,  // right bot
        -0.5f, -0.5f, 0.0f,  // left bot
    };

    unsigned int VBO1;
    {
        glGenBuffers(1, &VBO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    }

    unsigned int VAO1;
    {   
        glGenVertexArrays(1, &VAO1);
        glBindVertexArray(VAO1);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    unsigned int VBO2;
    {
        glGenBuffers(2, &VBO2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    }

    unsigned int VAO2;
    {   
        glGenVertexArrays(2, &VAO2);
        glBindVertexArray(VAO2);
        glEnableVertexAttribArray(0); 
        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // should be not zero?
    }

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        glCheckError();
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);    

        {//yellow
                int success;
                char infoLog[512];
                unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShaderYellow, 1, &fragmentShaderYellowSource, NULL);
                glCompileShader(fragmentShaderYellow);
                // check for shader compile errors
                glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(fragmentShaderYellow, 512, NULL, infoLog);
                    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                }

                // link shaders
                GLuint shaderProgramYellow = glCreateProgram();
                glAttachShader(shaderProgramYellow, vertexShader);
                glAttachShader(shaderProgramYellow, fragmentShaderYellow);
                glLinkProgram(shaderProgramYellow);
                // check for linking errors
                glGetProgramiv(shaderProgramYellow, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shaderProgramYellow, 512, NULL, infoLog);
                    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
                }
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShaderYellow);
        
            glUseProgram(shaderProgramYellow);

            glBindVertexArray(VAO2);
            glDrawArrays(GL_TRIANGLES, 0, 3);    
        }

        //second triangle
        
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO1); glDeleteVertexArrays(2, &VAO2);
    glDeleteBuffers(1, &VBO1);      glDeleteBuffers(2, &VBO2);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
            case GL_INVALID_ENUM: error         = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error        = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error    = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW: error    = "STACK_OVERFLOW"; break;
            // case GL_STACK_UNDERFLOW: error   = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY: error        = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "--!!--" << error << " | " << file << " (" << line << ")" <<
        std::endl;
    }
    return errorCode;
}
