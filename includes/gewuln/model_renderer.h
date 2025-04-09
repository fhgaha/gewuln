#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <vector>
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "character.h"
#include "stdio.h"
#include "glm/ext.hpp"

class ModelRenderer
{

public:
	ModelRenderer(Shader &shader): shader(shader){}

	void DrawCharacter(
		Character *character,
		Camera cam,
		float aspect,
		float rot_deg = 0.0f,
		glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	){
		DrawAnimatedModel(
			*character->model,
			cam,
			aspect,
			&character->animator,
			character->position,
			glm::degrees(character->rot_rad),
			rot_axis,
			scale
		);
	}

	void DrawAnimatedModel(
		Model& loaded_model,
		Camera cam, 
		float aspect,
		Animator *animator,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		float rot_deg = 0.0f, 
		glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	){
        shader.Use();

        // scale -> rotate -> translate. with matrises multiplications it should be reversed. model mat is doing that.
        // Vclip = Mprojection * Mview * Mmodel * Vlocal

        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), aspect, 0.1f, 100.0f);
        shader.SetMatrix4("projection", projection);

		glm::mat4 view = cam.GetViewMatrix();
        shader.SetMatrix4("view", view);

		auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            shader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
        }

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(rot_deg), rot_axis);
        model = glm::scale(model, scale);
        shader.SetMatrix4("model", model);

		// for meshes Draw() calls are frag shader calls
		for(unsigned int i = 0; i < loaded_model.meshes.size(); i++){
			loaded_model.meshes[i].Draw(shader);
		}
		
		
		//draw collider wireframe
		{
			{	//set wireframe settings
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				shader.SetBool("drawing_wireframe", true);
				shader.SetVector3f("wireframe_color", 1.0f, 0.0f, 0.0f);
			}
			
			// for (size_t i = 0; i < loaded_model.collider_mesh.vertices.size(); i++)
			// {
			// 	loaded_model.collider_mesh.vertices[i].Position += glm::vec3(0.0f, 0.0f, 0.01f);
			// 	glm::vec3 pos = loaded_model.collider_mesh.vertices[i].Position;
			// 	std::cout << "vert " << i << ", pos " << pos << "\n";
			// }
			// std::cout << "=======================\n";
			
			std::vector<Vertex> verts = loaded_model.collider_mesh.vertices;
			for (size_t i = 0; i < verts.size(); i++)
			{
				verts[i].Position = glm::vec3(0.0f, 0.0f, -10.f) + verts[i].Position;
			}
			
			Mesh(
				verts,
				loaded_model.collider_mesh.indices,
				loaded_model.collider_mesh.textures,
				false
			).Draw(shader);
			
			// loaded_model.collider_mesh.Draw(shader);

			{	//reset wireframe to textures
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				shader.SetBool("drawing_wireframe", false);
			}
		}
		
	}

	// not animated model. we ignore finalBonesMatrices here
	void DrawSimpleModel(
		Model& loaded_model,
		Camera cam, 
		float aspect,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		float rot_deg = 0.0f, 
		glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	){
        shader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), aspect, 0.1f, 100.0f);
        shader.SetMatrix4("projection", projection);

        glm::mat4 view = cam.GetViewMatrix();
        shader.SetMatrix4("view", view);

	 	// we dont call set_bone_matrices here

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(rot_deg), rot_axis);
        model = glm::scale(model, scale);

        shader.SetMatrix4("model", model);
		

		for(unsigned int i = 0; i < loaded_model.meshes.size(); i++){
			loaded_model.meshes[i].Draw(shader);
		}
		
		//draw interactable wireframe
		{
			{	//set wireframe settings
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				shader.SetBool("drawing_wireframe", true);
				shader.SetVector3f("wireframe_color", 0.0f, 1.0f, 1.0f);
			}
			loaded_model.interactable_mesh.Draw(shader);
			{	//reset
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				shader.SetBool("drawing_wireframe", false);
			}
		}
	}

private:
	Shader &shader;
};

#endif
