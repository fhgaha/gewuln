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


		void ProcessInput(const bool keys[], Game *game, const float dt) {

			if (keys[GLFW_KEY_E]){

				assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

				//TODO should iterate through not just all loaded models but only through all the currently instanced models
				std::map<std::string, Model> *models = &ResourceManager::Models;
				for (const auto &[name, mdl] : *models)
				{
					// check if this character is colliding with an interactable
					if (mdl.interactable_mesh.has_value()) {

						const std::vector<Vertex> &interactable_verts = mdl.interactable_mesh.value().vertices;

						std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
						for (size_t i = 0; i < transformed_verts.size(); i++){
							transformed_verts[i].Position += this->position;
						}

						bool collider_intersects_an_interactable = Geometry3d::intersect(
							transformed_verts,
							interactable_verts
						);

						// if (collider_intersects_an_interactable){
						// 	std::cout << "it intersects!\n";
						// 	game->PlayCameraThing();
						// }
						// else {
						// 	std::cout << "it DOES NOT intersect!\n";
						// }
						return;
					}
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
				if (game->current_room->walkable_area->has_value()) {
					inside = inside_walkable_area(
						this->model->collider_mesh.value(),
						game->current_room->walkable_area->value(),
						this->position + velocity
					);
				}
				
			} else {
				velocity = glm::vec3(0.0f);
			}
			if (inside) {
				this->position += velocity;
			} 

		}


		bool inside_walkable_area(const Mesh &character_collider, const Mesh &walkable_area, const glm::vec3 position_to_test)
		{
			//1. get character's 4 lowest pts of collider

			std::array<glm::vec2, 4> small_square;

			//there are 4*6 small_verts (4 per side, 6 sides of a cube)
			const std::vector<Vertex> &small_verts = character_collider.vertices;
			for (size_t i = 0; i < small_verts.size(); i+=4)
			{
				Vertex copy = small_verts[i];

				// get the edge with normal = (0, -1, 0)
				//a == b when `std::fabsf(a - b) < 1e-6`
				bool copy_normal_is_minus_one = std::fabsf(copy.Normal.y - (-1.0f)) < 1e-6;
				if (copy_normal_is_minus_one){
					glm::vec3 p0 = small_verts[i+0].Position + position_to_test;
					glm::vec3 p1 = small_verts[i+1].Position + position_to_test;
					glm::vec3 p2 = small_verts[i+2].Position + position_to_test;
					glm::vec3 p3 = small_verts[i+3].Position + position_to_test;

					small_square = {
						glm::vec2(p0.x, p0.z),
						glm::vec2(p1.x, p1.z),
						glm::vec2(p2.x, p2.z),
						glm::vec2(p3.x, p3.z),
					};

					break;
				}
			}


			//2. get all walkable areas triangles and their points

			std::vector<std::array<glm::vec2, 3>> walkable_area_tris;
			
			const std::vector<Vertex> 		&walkable_verts 	= walkable_area.vertices;
			const std::vector<unsigned int> &walkable_indeces 	= walkable_area.indices;

			//0 1 2, 0 2 3
			for (size_t i = 0; i < walkable_indeces.size(); i+=3)
			{
				glm::vec3 p0 = walkable_verts[walkable_indeces[i+0]].Position;
				glm::vec3 p1 = walkable_verts[walkable_indeces[i+1]].Position;
				glm::vec3 p2 = walkable_verts[walkable_indeces[i+2]].Position;

				std::array<glm::vec2, 3> tri = {glm::vec2(p0.x, p0.z), glm::vec2(p1.x, p1.z), glm::vec2(p2.x, p2.z)};
				walkable_area_tris.push_back(tri);
			}


			//3. check the intersection with walkable area

			bool inside = Geometry2d::rect_inside_area_of_tris(
				small_square,
				walkable_area_tris
			);

			// std::cout << "small poly: \n";
			// for (auto &pt_d : small_square)
			// {
			// 	std::cout << "\t{" << pt_d << "},";
			// }
			// std::cout << "\n";

			// std::cout << "walkable_pts:\n";
			//...
			// std::cout << "inside area: " << inside << "\n";
			// std::cout << "========================\n";

			return inside;
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