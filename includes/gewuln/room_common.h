#ifndef ROOM_COMMON_H
#define ROOM_COMMON_H

#include <functional>
#include <string>
#include <unordered_map>

struct Mesh;
class Room;

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

#endif