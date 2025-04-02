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
		Camera cam, float aspect,
		Animator *animator,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		float rot_deg = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	){
        shader.Use();

        // scale -> rotate -> translate. with matrises multiplications it should be reversed. model mat is doing that.
        // Vclip = Mprojection * Mview * Mmodel * Vlocal

        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), aspect, 0.1f, 100.0f);
        shader.SetMatrix4("projection", projection);

		glm::mat4 view = cam.GetViewMatrix();
        shader.SetMatrix4("view", view);

        set_bone_matrices(animator);

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
		model = glm::rotate(model, glm::radians(rot_deg), rot_axis);
        model = glm::scale(model, scale);
        shader.SetMatrix4("model", model);


		for(unsigned int i = 0; i < loaded_model.meshes.size(); i++){
			loaded_model.meshes[i].Draw(shader);
		}
	}

	// not animated model. we ignore finalBonesMatrices here
	void DrawSimpleModel(
		Model& loaded_model,
		Camera cam, float aspect,
		glm::vec3 pos = glm::vec3(0, 0, 0),
		float rot_deg = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
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
	}

private:
	Shader &shader;

	void set_bone_matrices(Animator *animator) {
        auto transforms = animator->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            auto name = "finalBonesMatrices[" + std::to_string(i) + "]";
            shader.SetMatrix4(name.c_str(), transforms[i]);
        }
	}
};

#endif
