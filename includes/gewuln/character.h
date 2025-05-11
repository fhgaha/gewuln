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


		void ProcessInput(Game *game, const float dt) 
		{

			if (game->Keys[GLFW_KEY_E] && !game->KeysProcessed[GLFW_KEY_E]){

				assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

				{//interactables
					for (auto &[room_name, interactable] : game->current_room->interactables)
					{
						std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
						for (size_t i = 0; i < transformed_verts.size(); i++){
							transformed_verts[i].Position += this->position;
						}
						
						bool collider_intersects_an_interactable = Geometry3d::intersect(
							transformed_verts,
							interactable.mesh->vertices
						);

						std::cout << "collider_intersects_an_interactable: " << collider_intersects_an_interactable << "\n";
						if (collider_intersects_an_interactable){
							//TODO should be configurable action
							interactable.action();
							// game->PlayCameraThing();
						}
						
					}
					
				}
				
				{//switch rooms

					for (auto &[room_name, room_exit] : game->current_room->exits)
					{
						std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
						for (size_t i = 0; i < transformed_verts.size(); i++){
							transformed_verts[i].Position += this->position;
						}
						
						bool collider_intersects_room_exit = Geometry3d::intersect(
							transformed_verts,
							room_exit.mesh->vertices
						);
						
						std::cout << "collider_intersects_room_exit: " << collider_intersects_room_exit <<"\n";
						if (collider_intersects_room_exit){
							room_exit.on_room_exit();
							room_exit.action();
						}
					}
				}
				
				game->KeysProcessed[GLFW_KEY_E] = true;
			}


			if (game->Keys[GLFW_KEY_A]){
				rot_rad += ROT_SPEED * dt;
				forward = glm::rotate(forward, ROT_SPEED * dt, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (game->Keys[GLFW_KEY_D]){
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
			if (game->Keys[GLFW_KEY_W]){
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