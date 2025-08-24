#ifndef ROOM_H
#define ROOM_H

#include <glm/glm.hpp>
#include <gewuln/room_objects.h>
#include <gewuln/camera/camera.h>
#include <gewuln/camera/camera_look_at.h>
#include <gewuln/camera/camera_fly.h>
#include <gewuln/geometry_3d.h>
#include <gewuln/geometry_2d.h>
#include <gewuln/character.h>
#include <gewuln/model.h>
#include <unordered_map>
#include <memory>
#include <utility>
#include <array>
#include <functional>

class Character;
class Animator;

class Room
{
public:
	std::unordered_map<std::string, std::unique_ptr<Camera>>	cameras;
	std::unordered_map<std::string, RoomObjects::Interactable>	interactables;
	std::unordered_map<std::string, RoomObjects::Exit>			exits;
    
	Model														*model;
	Camera														*initial_cam;
	Camera														*current_cam;
	std::optional<Mesh>											*walkable_area;

    std::unordered_map<std::string, Character>                  characters;
	Character               									*active_character;

	//TODO
	//initial_character_position
	//cameras

	~Room();

	void 		init(Model *model);
	Character* 	add_character(std::string name, Model *model, std::string path, glm::vec3 pos, glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f));
	bool		remove_character(std::string name);
	bool		remove_character(int idx);
	void 		init_interactable(const char* name, const RoomObjects::Interactable interactable);
	void 		init_exit(const char* name, const RoomObjects::Exit exit);
	bool 		inside_walkable_area(const Mesh &character_collider, const glm::vec3 position_to_test);
};

#endif
