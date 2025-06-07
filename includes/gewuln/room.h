#ifndef ROOM_H
#define ROOM_H

#include <glm/glm.hpp>
#include <unordered_map>
#include <gewuln/camera/camera.h>
#include <gewuln/camera/camera_look_at.h>
#include <gewuln/camera/camera_fly.h>
#include <gewuln/geometry_3d.h>
#include <gewuln/geometry_2d.h>
#include <memory>
#include <utility>
#include <array>
#include <functional>


class Room
{
public:

	struct Interactable
	{
		Mesh 					*mesh;
		int 					glfw_key;
		std::function<void()> 	action;
	};

	struct Exit
	{
		Mesh					*mesh;
		int						glfw_key;
		std::function<void()> 	action;
		Room					*this_room;
		Room					*go_to_room;
		std::function<void()> 	on_room_enter;
		std::function<void()> 	on_room_exit;
	};

	std::unordered_map<std::string, std::unique_ptr<Camera>>	cameras;
	std::unordered_map<std::string, Interactable>				interactables;
	std::unordered_map<std::string, Exit>						exits;


	Model														*model;
	Camera														*initial_cam;
	Camera														*current_cam;

	std::optional<Mesh>											*walkable_area;

	//TODO
	//initial_character_position
	//cameras

	~Room() {
		delete initial_cam;
		delete current_cam;
		delete walkable_area;
		delete model;
	}


	void Init(Model *model)
	{
		this->model = model;
		this->walkable_area = &model->walkable_area;
	}

	void init_interactable(const char* name, const Interactable interactable)
	{
		interactables[name] = interactable;
	}

	void init_exit(const char* name, const Exit exit)
	{
		exits[name] = exit;
	}


	bool inside_walkable_area(const Mesh &character_collider, const glm::vec3 position_to_test)
	{
		assert(walkable_area->has_value() && "Walkable area has no value!");

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

		const std::vector<Vertex> 		&walkable_verts 	= this->walkable_area->value().vertices;
		const std::vector<unsigned int> &walkable_indeces 	= this->walkable_area->value().indices;

		//for a square indeces are: 0 1 2, 0 2 3
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

		return inside;
	}
};

#endif