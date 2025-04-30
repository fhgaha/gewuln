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

			if (keys[GLFW_KEY_W]){
			    velocity = forward * WALK_SPEED * dt;
				
				{// check if this character is inside of walkable area
				
					std::map<std::string, Model> *models = &ResourceManager::Models;
					for (const auto &[name, mdl] : *models)
					{
						if (mdl.walkable_area.has_value()) {
							const std::vector<Vertex> &walkable_verts = mdl.walkable_area.value().vertices;
							
							//1. get characters 4 lowest pts of collider
							
							std::vector<Clipper2Lib::PointD> walkable_pts;
							
							std::ranges::transform(
								walkable_verts,
								std::back_inserter(walkable_pts),
								// [this](const Vertex &v) {
								[this](const auto &v) {
									Vertex copy = v;
									return Clipper2Lib::PointD(copy.Position.x, copy.Position.z);
								}
							);
							
							std::vector<glm::vec3> bottom_side_pts;
							bottom_side_pts.resize(4);
							
							//there are 4*6 verts (4 per side, 6 sides of a cube)
							auto &verts = this->model->collider_mesh.value().vertices;
							for (size_t i = 0; i < verts.size(); i+=4)
							{
								Vertex copy = verts[i];
								
								// get the edge with normal = (0, -1, 0)
								//a == b when `std::fabsf(a - b) < 1e-6`
								bool copy_normal_is_minus_one = std::fabsf(copy.Normal.y - (-1.0f)) < 1e-6;
								if (copy_normal_is_minus_one){
									bottom_side_pts[0] = verts[i+0].Position + this->position;
									bottom_side_pts[1] = verts[i+1].Position + this->position;
									bottom_side_pts[2] = verts[i+2].Position + this->position;
									bottom_side_pts[3] = verts[i+3].Position + this->position;
									break;
								}
							}
							
							// for (auto &p : bottom_side_pts)
							// {
							// 	std::cout << p << "\t";
							// }
							// std::cout << "\n";
							
							
							std::vector<Clipper2Lib::PointD> small_poly_path_d;
							
							std::ranges::transform(
								bottom_side_pts,
								std::back_inserter(small_poly_path_d),
								[](const auto &v) {
									glm::vec3 copy = v;
									return Clipper2Lib::PointD(copy.x, copy.z);
								}
							);
							
							//2. check the intersection with walkable area

							// bool inside = Geometry2d::is_polygon_inside(
							// 	{small_poly_path_d}, 
							// 	{walkable_pts}
							// );
							
							Clipper2Lib::PathsD large_paths;
							
							for (size_t i = 0; i < walkable_pts.size(); i+=4)
							{
								auto p0 = walkable_pts[i+0];
								auto p1 = walkable_pts[i+1];
								auto p2 = walkable_pts[i+2];
								auto p3 = walkable_pts[i+3];
								
								large_paths.push_back({p0, p1, p2, p3});
							}
							
							bool inside = Geometry2d::small_poly_inside_all_large_polys(
								small_poly_path_d, 
								large_paths
							);
							
							
							std::cout << "small poly: \n";
							for (auto &pt_d : small_poly_path_d)
							{
								std::cout << "\t{" << pt_d << "},";
							}
							std::cout << "\n";
							
							std::cout << "walkable_pts:\n";
							for (auto &pt_d : walkable_pts)
							{
								std::cout << "\t{" << pt_d << "},";
							}
							std::cout << "\ninside area: " << inside << "\n"; 
							std::cout << "========================\n";
							
							
							
							if (!inside){
								
							}
							
						}
					}	
				
					
				}
				
				
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