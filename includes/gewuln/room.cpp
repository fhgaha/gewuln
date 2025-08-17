#include "room.h"

Character* Room::add_character(std::string name, Model * model, std::string path, glm::vec3 pos, glm::vec3 dir)
{
	try
	{
		characters[name] = Character(model, path, pos, dir);
		return &characters[name];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return nullptr;
}