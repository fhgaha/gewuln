#ifndef GIZMO_RENDERER_H
#define GIZMO_RENDERER_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gewuln/shader.h>

class GizmoRenderer
{
public:
	GizmoRenderer(Shader &shader)
	{
		this->shader = shader;
		
        float vertices[] = {
            // X-axis (red)
            0.0f, 0.0f, 0.0f,  // Origin
            1.0f, 0.0f, 0.0f,  // X-direction
            
            // Y-axis (green)
            0.0f, 0.0f, 0.0f,  // Origin
            0.0f, 1.0f, 0.0f,  // Y-direction
            
            // Z-axis (blue)
            0.0f, 0.0f, 0.0f,  // Origin
            0.0f, 0.0f, 1.0f   // Z-direction
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // Vertex positions (matches vertex shader layout location 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

	void Draw(Camera *cam, float aspect)
	{
        shader.Use(); // CRITICAL: Must activate shader first
        
        // Prepare transformations
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), aspect, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix

        shader.SetMatrix4("projection", projection);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("model", model);

        // Draw line
        glBindVertexArray(VAO);
		glDisable(GL_DEPTH_TEST);
        
        // red
        shader.SetVector3f("color", glm::vec3(1.0f, 0.0f, 0.0f));
		glDrawArrays(GL_LINES, 0, 2);

        // green
        shader.SetVector3f("color", glm::vec3(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_LINES, 2, 2);
        
        // blue
        shader.SetVector3f("color", glm::vec3(0.0f, 0.0f, 1.0f));
        glDrawArrays(GL_LINES, 4, 2);

		glEnable(GL_DEPTH_TEST);
        glBindVertexArray(0);
	}

private:
	Shader shader;
	unsigned int VAO, VBO;
};

#endif