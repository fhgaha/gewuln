#include "room.h"

Room::~Room()
{
	delete initial_cam;
	delete current_cam;
	delete walkable_area;
	delete model;
}

void Room::init(Model *model)
{
	this->model = model;
	this->walkable_area = &model->walkable_area;
}

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

bool Room::remove_character(std::string name)
{
	size_t amount_removed = characters.erase(name);
	return amount_removed > 0;
}

bool Room::remove_character(int idx)
{
	auto it = characters.begin();
	for (int i = 0; i < idx; i++)
	{
		it++;
	}
	auto next_after_removed = characters.erase(it);
	return next_after_removed != characters.begin();
}

void Room::init_interactable(const char* name, const RoomObjects::Interactable interactable)
{
	interactables[name] = interactable;
}

void Room::init_exit(const char* name, const RoomObjects::Exit exit)
{
	exits[name] = exit;
}

bool Room::inside_walkable_area(const Mesh &character_collider, const glm::vec3 position_to_test)
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
