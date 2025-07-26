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
#include <gewuln/character_state/character_state.h>
#include <iostream>
#include <map>
#include <gewuln/room.h>

class Room;

class Character {
public:
	float 				WALK_SPEED = 1.2f;
	float 				ROT_SPEED  = 4.0f;

	Model*				model;
	Animator			animator;
	
	glm::vec3			position;
	float				rot_rad;
	glm::vec3			velocity = glm::vec3(0.0f);
	glm::vec3			forward  = glm::vec3(0.0f, 0.0f, -1.0f);
	
	Room*				current_room;
	Room::Interactable*	interactable_intersecting;
	bool				controlled_by_player;


	Character(){};

	Character(Model *model, Animator animator, glm::vec3 pos, glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f))
	{
		this->model = model;
		this->animator = animator;
		this->position = pos;

		float yaw, _pitch;
		direction_to_yaw_pitch(dir, yaw, _pitch);
		this->rot_rad = yaw;

		this->animator.PlayAnimation("idle");
		this->state = new IdleState();
	}

	void ProcessInput(bool *Keys, bool *KeysProcessed, const float dt);
	void Update(const float dt);
	void turn_left(const float dt);
	void turn_right(const float dt);
	Room::Interactable* collider_intersects_an_interactable();
	void switch_rooms();

private:
	CharacterState*	state;

    void direction_to_yaw_pitch(const glm::vec3& direction, float& yaw, float& pitch)
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

};

#endif