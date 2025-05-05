#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/model.h>
#include <gewuln/geometry_3d.h>
#include <gewuln/geometry_2d.h>
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


		void ProcessInput(const bool keys[], Game *game, const float dt) 
		{

			if (keys[GLFW_KEY_E]){

				assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

				{//interactable
					for (auto &room_interactable : game->current_room->interactables)
					{
						std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
						for (size_t i = 0; i < transformed_verts.size(); i++){
							transformed_verts[i].Position += this->position;
						}
						
						const std::vector<Vertex> &interactable_verts = room_interactable.second.mesh->vertices;
						
						bool collider_intersects_an_interactable = Geometry3d::intersect(
							transformed_verts,
							interactable_verts
						);

						std::cout << "collider_intersects_an_interactable: " << collider_intersects_an_interactable << "\n";
						if (collider_intersects_an_interactable){
							//TODO should be configurable action
							room_interactable.second.action();
							// game->PlayCameraThing();
						}
						
					}
					
				}
				
				{//switch rooms

					// //TODO should iterate through not just all loaded models but only through all the currently instanced models
					// std::map<std::string, Model> *models = &ResourceManager::Models;
					// for (const auto &[name, mdl] : *models)
					// {
					// 	// check if this character is colliding with an interactable
					// 	if (mdl.room_exit.has_value()) {

					// 		const std::vector<Vertex> &room_exit_verts = mdl.room_exit.value().vertices;

					// 		std::vector<Vertex> transformed_verts = this->model->room_exit.value().vertices;
					// 		for (size_t i = 0; i < transformed_verts.size(); i++){
					// 			transformed_verts[i].Position += this->position;
					// 		}

					// 		bool collider_intersects_room_exit = Geometry3d::intersect(
					// 			transformed_verts,
					// 			room_exit_verts
					// 		);
							
					// 		std::cout << "collider transformed verts\n";
					// 		for (auto &v : transformed_verts){
					// 			std::cout << "\t" << v.Position;
					// 		}
					// 		std::cout << "\n===========\n";
							
					// 		std::cout << "room exit verts\n";
					// 		for (auto &v : room_exit_verts){
					// 			std::cout << "\t" << v.Position;
					// 		}
					// 		std::cout << "\n===========\n";
							
							
					// 		if (collider_intersects_room_exit){
					// 			std::cout << "collider_intersects_room_exit: " << collider_intersects_room_exit <<"\n";
					// 			game->switch_rooms();
					// 		}
					// 		return;
					// 	}
					// }
				}

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

			bool inside = false;
			if (keys[GLFW_KEY_W]){
			    velocity = forward * WALK_SPEED * dt;

				//check that the character won't leave walkable area on the next frame
				inside = game->current_room->inside_walkable_area(
					this->model->collider_mesh.value(),
					this->position + velocity
				);
				
			} else {
				velocity = glm::vec3(0.0f);
			}
			if (inside) {
				this->position += velocity;
			} 

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