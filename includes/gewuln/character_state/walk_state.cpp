#include <gewuln/character_state/character_state.h>

CharacterState* WalkState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	printf("entered walk state\n");
	
	// if (Keys[GLFW_KEY_W]){
	// 	velocity = forward * WALK_SPEED * dt;
	// } else {
	// 	velocity = glm::vec3(0.0f);
	// }
	
	// if (true) {
	// 	return new IdleState();
	// }
	
	return nullptr;
	
	
}

void WalkState::update(Character& character, const float dt)
{
	
}