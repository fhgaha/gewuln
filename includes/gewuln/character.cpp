#include <gewuln/character.h>
#include <GLFW/glfw3.h>
#include <gewuln/room.h>
#include <gewuln/geometry_3d.h>
// #include "character.h"


void Character::ProcessInput(bool *Keys, bool *KeysProcessed, const float dt)
{
	if (!controlled_by_player) {
		return;
	}

	// if (Keys[GLFW_KEY_E] && !KeysProcessed[GLFW_KEY_E]){

		// assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

		// {//interactables
		// 	for (auto &[room_name, interactable] : current_room->interactables)
		// 	{
		// 		std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
		// 		for (size_t i = 0; i < transformed_verts.size(); i++){
		// 			transformed_verts[i].Position += this->position;
		// 		}

		// 		bool collider_intersects_an_interactable = Geometry3d::intersect(
		// 			transformed_verts,
		// 			interactable.mesh->vertices
		// 		);

		// 		std::cout << "collider_intersects_an_interactable: " << collider_intersects_an_interactable << "\n";
		// 		if (collider_intersects_an_interactable){
		// 			//TODO should be configurable action
		// 			interactable.action();
		// 			// PlayCameraThing();
		// 		}

		// 	}

		// }

		// {//switch rooms

		// 	for (auto &[room_name, room_exit] : current_room->exits)
		// 	{
		// 		std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
		// 		for (size_t i = 0; i < transformed_verts.size(); i++){
		// 			transformed_verts[i].Position += this->position;
		// 		}

		// 		bool collider_intersects_room_exit = Geometry3d::intersect(
		// 			transformed_verts,
		// 			room_exit.mesh->vertices
		// 		);

		// 		std::cout << "collider_intersects_room_exit: " << collider_intersects_room_exit <<"\n";
		// 		if (collider_intersects_room_exit){
		// 			room_exit.on_room_exit();
		// 			room_exit.action();
		// 		}
		// 	}
		// }

		// KeysProcessed[GLFW_KEY_E] = true;
	// }

	CharacterState* new_state = state_->process_input(*this, Keys, KeysProcessed, dt);
	if (new_state){
		state_->exit(*this);
		delete state_;
		state_ = new_state;
		state_->enter(*this);
	}

}

void Character::Update(const float dt)
{
	if (glm::length2(velocity) > 0) {
		animator.PlayAnimation("walk");
	}
	else {
		animator.PlayAnimation("idle");
	}

	{ //move
		assert(current_room && "Should have current room");
		bool inside = current_room->inside_walkable_area(
			this->model->collider_mesh.value(),
			this->position + velocity
		);
		if (inside) {
			this->position += velocity;
		} else {
			// move and slide
			// https://gamedev.stackexchange.com/questions/200354/how-to-slide-along-a-wall-at-full-speed

			glm::vec3 left(velocity.z, velocity.y, -velocity.x);	//rotated 90 deg counter clockwise
			glm::vec3 right(-velocity.z, velocity.y, velocity.x);	//rotated 90 deg clockwise

			glm::vec3 small_left  = glm::normalize(left) * dt;
			glm::vec3 small_right = glm::normalize(right) * dt;

			float vel_len = glm::length(velocity);
			glm::vec3 eight_right = glm::normalize(velocity + right) * vel_len;
			glm::vec3 eight_left  = glm::normalize(velocity + left)  * vel_len;

			//if one of them not inside after movement, move the other way. if both not inside - dont move.
			bool eight_right_is_inside = current_room->inside_walkable_area(
				this->model->collider_mesh.value(),
				this->position + eight_right
			);
			bool eight_left_is_inside  = current_room->inside_walkable_area(
				this->model->collider_mesh.value(),
				this->position + eight_left
			);
			if (eight_left_is_inside && eight_right_is_inside){
			} else if (eight_left_is_inside) {
				this->position += small_left;
			} else if (eight_right_is_inside) {
				this->position += small_right;
			} else {
				//cant move anywhere
			}
		}
	}

	{ //look at center of interactable cube
		//TODO use events like on enter, on exit or something. check a stack of active interactables maybe
		bool collider_intersects_an_interactable = false;
		Room::Interactable *interacting_with = nullptr;
		for (auto &[room_name, interactable] : current_room->interactables)
		{
			std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
			for (size_t i = 0; i < transformed_verts.size(); i++){
				transformed_verts[i].Position += this->position;
			}

			collider_intersects_an_interactable = Geometry3d::intersect(
				transformed_verts,
				interactable.mesh->vertices
			);
			if (collider_intersects_an_interactable) {
				interacting_with = &interactable;
				break;
			}
		}

		if (collider_intersects_an_interactable) {
			animator.target = Geometry3d::compute_box_center(interacting_with->mesh->vertices) /*+ interacting_with->mesh->Position*/;
			animator.char_pos = this->position;
			animator.char_forward = this->forward;
			animator.update_animation_with_look_at(dt);
		} else {
			animator.update_animation(dt);
		}
	}
}

void Character::turn_left(const float dt)
{
	forward = glm::rotateY(forward, ROT_SPEED * dt);
	rot_rad += ROT_SPEED * dt;
	if (rot_rad > glm::pi<float>()) {
		rot_rad -= glm::two_pi<float>();
	}
}

void Character::turn_right(const float dt)
{
	forward = glm::rotateY(forward, -ROT_SPEED * dt);
	rot_rad -= ROT_SPEED * dt;
	if (rot_rad < -glm::pi<float>()) {
		rot_rad += glm::two_pi<float>();
	}
}

Room::Interactable* Character::collider_intersects_an_interactable()
{
	assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

	for (auto &[room_name, interactable] : current_room->interactables)
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
			// interactable.action();
			// PlayCameraThing();
			return &interactable;
		}

	}

	return nullptr;
}


void Character::switch_rooms()
{//switch rooms

	for (auto &[room_name, room_exit] : current_room->exits)
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
