#include <gewuln/character.h>
#include <gewuln/room.h>
#include <GLFW/glfw3.h>
#include <gewuln/room.h>
#include <gewuln/geometry/geometry_3d.h>
#include "character.h"

Character::Character(Model* model, std::string path, glm::vec3 pos, glm::vec3 dir)
{
	this->model = model;
	this->animator = Animator(path, *model);

	float yaw, _pitch;
	direction_to_yaw_pitch(dir, yaw, _pitch);
	
	model->transform = Transform{.position = pos, .rot_rad = yaw};
	this->transform = &model->transform;

	this->animator.play_animation("idle");
	this->state = new IdleState();
}

Character::~Character() {
    animator.remove_observer(this);
    // delete state;
}

void Character::ProcessInput(bool *Keys, bool *KeysProcessed, const float dt)
{
	if (!controlled_by_player) {
		return;
	}

	CharacterState* new_state = state->process_input(*this, Keys, KeysProcessed, dt);
	if (new_state){
		state->exit(*this);
		delete state;
		state = new_state;
		state->enter(*this);
	}

}

void Character::Update(const float dt)
{
	state->update(*this, dt);

	{ //look at center of interactable cube
		//TODO use events like on enter, on exit or something. check a stack of active interactables maybe
		bool collider_intersects_an_interactable = false;
		RoomObjects::Interactable* interacting_with = nullptr;
		for (auto &[room_name, interactable] : current_room->interactables)
		{
			std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
			for (size_t i = 0; i < transformed_verts.size(); i++){
				transformed_verts[i].Position += this->transform->position;
			}

			collider_intersects_an_interactable = Geometry3d::intersect(
				transformed_verts,
				interactable.mesh->vertices
			);
			if (collider_intersects_an_interactable) {
				interacting_with = &interactable;
				break;
			} else {
				interacting_with = nullptr;
			}
		}

		if (collider_intersects_an_interactable) {
			animator.target = Geometry3d::compute_box_center(interacting_with->mesh->vertices) /*+ interacting_with->mesh->Position*/;
			animator.char_pos = this->transform->position;
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
	transform->rot_rad += ROT_SPEED * dt;
	if (transform->rot_rad > glm::pi<float>()) {
		transform->rot_rad -= glm::two_pi<float>();
	}
}

void Character::turn_right(const float dt)
{
	forward = glm::rotateY(forward, -ROT_SPEED * dt);
	transform->rot_rad -= ROT_SPEED * dt;
	if (transform->rot_rad < -glm::pi<float>()) {
		transform->rot_rad += glm::two_pi<float>();
	}
}

void Character::turn_to_target_instantly(glm::vec3 target)
{
	glm::vec3 char_to_trg_dir = target - transform->position;
	glm::vec2 char_to_trg_dir_vec2 = glm::normalize(glm::vec2(char_to_trg_dir.x, char_to_trg_dir.z));
	glm::vec2 forward_vec2 = glm::normalize(glm::vec2(forward.x, forward.z));

	float angle_rad = glm::orientedAngle(char_to_trg_dir_vec2, forward_vec2);
	
	forward = glm::rotateY(forward, angle_rad);
	transform->rot_rad += angle_rad;
	if (transform->rot_rad < -glm::pi<float>()) {
		transform->rot_rad += glm::two_pi<float>();
	}
	if (transform->rot_rad > glm::pi<float>()) {
		transform->rot_rad -= glm::two_pi<float>();
	}
}

RoomObjects::Interactable* Character::collider_intersects_an_interactable()
{
	assert(this->model->collider_mesh.has_value() && "Character must have collider mesh!");

	for (auto &[room_name, interactable] : current_room->interactables)
	{
		std::vector<Vertex> transformed_verts = this->model->collider_mesh.value().vertices;
		for (size_t i = 0; i < transformed_verts.size(); i++){
			transformed_verts[i].Position += this->transform->position;
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
			transformed_verts[i].Position += this->transform->position;
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

void Character::walk_if_possible(const float dt)
{
	assert(current_room && "Should have current room");
	bool inside = current_room->inside_walkable_area(
		this->model->collider_mesh.value(),
		this->transform->position + velocity
	);
	if (inside) {
		this->transform->position += velocity;
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
			this->transform->position + eight_right
		);
		bool eight_left_is_inside  = current_room->inside_walkable_area(
			this->model->collider_mesh.value(),
			this->transform->position + eight_left
		);

		if (eight_left_is_inside && eight_right_is_inside){
		} else if (eight_left_is_inside) {
			this->transform->position += small_left;
		} else if (eight_right_is_inside) {
			this->transform->position += small_right;
		} else {
			//cant move anywhere
		}
	}
}

void Character::on_notify(const Animator& sender, AnimatorData data)
{
	printf("character recieved event [%p] [%p]\n", (void*)&sender, &data);
	this->state = new IdleState();
}

void Character::direction_to_yaw_pitch(const glm::vec3& direction, float& yaw, float& pitch)
{
	const float epsilon = 1e-6f;
	glm::vec3 dir = direction;

	// Normalize vector to handle non-unit directions
	if (glm::length(dir) > epsilon) {
		dir = glm::normalize(dir);
	} else {
		yaw = 0.0f;
		pitch = 0.0f;
		return;
	}

	// Calculate pitch (vertical angle)
	pitch = glm::asin(-dir.y);  // Negative because up is positive Y

	// Calculate yaw (horizontal angle)
	if (glm::abs(dir.x) < epsilon && glm::abs(dir.z) < epsilon) {
		yaw = 0.0f;  // Straight up/down case
	} else {
		// yaw = glm::atan(dir.x, -dir.z);
		yaw = glm::atan(dir.x, dir.z);
	}
}
