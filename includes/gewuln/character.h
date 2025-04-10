#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/model.h>
#include <gewuln/gewuln_math.h>
#include <iostream>
#include <map>

class Character {
	public:
		float WALK_SPEED = 1.2f;
		float ROT_SPEED  = 4.0f;

		Model		*model;
		Animator	animator;

		glm::vec3	position;
		float		rot_rad;
		glm::vec3	forward  = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3	velocity = glm::vec3(0.0f);

		Character(){};

		Character(Model *model, Animator animator, glm::vec3 position){
			this->model = model;
			this->animator = animator;
			this->position = position;

			this->animator.PlayAnimation("idle");
		}


		void ProcessInput(const bool keys[], const Game *game, const float dt) {

			if (keys[GLFW_KEY_E]){
				// get all the interactables
				
				//TODO should iterate through not just all loaded models but only through all the currently instanced models
				std::map<std::string, Model> *models = &ResourceManager::Models;
				for (const auto &[name, mdl] : *models)
				{
					if (mdl.interactable_mesh.has_value()) {
						const Mesh *intrcbl_mesh = &mdl.interactable_mesh.value();
						

						//collider mesh is drawn properly, with updated position, but it still has zero based positions in log. why? how?
						auto cldr_verts = this->model->collider_mesh.value().vertices;
						// std::cout << "collider:\n";
						for (size_t i = 0; i < cldr_verts.size(); i++)
						{
							cldr_verts[i].Position += this->position;
						// 	std::cout << "\t" << i <<":\t" << cldr_verts[i].Position << "\n";
						}
						
						// std::cout << "interactable:\n";
						// for (size_t i = 0; i < intrcbl_mesh->vertices.size(); i++)
						// {
						// 	std::cout << "\t" << i <<":\t" << intrcbl_mesh->vertices[i].Position << "\n";
						// }
						// std::cout << "=======================\n";
						
						
						
						bool collider_intersects_an_interactable = GewulnMath::intersect(
							cldr_verts, 
							intrcbl_mesh->vertices
						);
						
						if (collider_intersects_an_interactable){
							std::cout << "it intersects!\n";
							game->PlayCameraThing();
						}
						else 
						{
							std::cout << "it DOES NOT intersect!\n";
						}
						return;
					}
				}
				

				// check if collider intersects one of the interactables

				//if does - run the cutscene
				//if not - do nothing
			}


			if (keys[GLFW_KEY_A]){
				rot_rad += ROT_SPEED * dt;
				forward = glm::rotate(forward, ROT_SPEED * dt, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (keys[GLFW_KEY_D]){
				rot_rad -= ROT_SPEED * dt;
				forward = glm::rotate(forward, -ROT_SPEED * dt, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			if (rot_rad > glm::pi<float>()) {
				rot_rad -= 2.0f * glm::pi<float>();
			}
			if (rot_rad < -glm::pi<float>()) {
				rot_rad += 2.0f * glm::pi<float>();
			}

			if (keys[GLFW_KEY_W]){
			    velocity = forward * WALK_SPEED * dt;
			} else {
				velocity = glm::vec3(0.0f);
			}
		    position += velocity;

		}

		void Update(const float dt) {
			if (glm::length2(velocity) > 0) {
				animator.PlayAnimation("walk");
			}
			else {
				animator.PlayAnimation("idle");
			}
			animator.UpdateAnimation(dt);
		}
};

#endif