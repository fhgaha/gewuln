#ifndef CHARACTER_H
#define CHARACTER_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <gewuln/room.h>
#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/model.h>
#include <gewuln/geometry_3d.h>
#include <gewuln/geometry_2d.h>
#include <gewuln/character_state/character_state.h>
#include <iostream>
#include <map>

class Character: public I_AnimatorObserver {
public:
	float 						WALK_SPEED = 1.2f;
	float 						ROT_SPEED  = 4.0f;
		
	Model*						model;
	Animator					animator;
			
	glm::vec3					position;
	float						rot_rad;
	glm::vec3					velocity = glm::vec3(0.0f);
	glm::vec3					forward  = glm::vec3(0.0f, 0.0f, -1.0f);
			
	Room*						current_room;
	RoomObjects::Interactable*	interactable_intersecting;
	bool						controlled_by_player;

	Character(){};
	Character(Model *model, std::string path, glm::vec3 pos, glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f));
	~Character();
	
	void 						ProcessInput(bool *Keys, bool *KeysProcessed, const float dt);
	void 						Update(const float dt);
	void 						turn_left(const float dt);
	void 						turn_right(const float dt);
	RoomObjects::Interactable* 	collider_intersects_an_interactable();
	void 						switch_rooms();
	void 						walk_if_possible(const float dt);
	void 						on_notify(const Animator& sender, AnimatorData data);

private:
	CharacterState*	state;
	
	void 						direction_to_yaw_pitch(const glm::vec3& direction, float& yaw, float& pitch);
};

#endif
