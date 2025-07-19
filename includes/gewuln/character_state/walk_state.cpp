#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

CharacterState* WalkState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	printf("entered walk state\n");
	
	auto w = Keys[GLFW_KEY_W];
	auto st = new IdleState();
	auto v = character.velocity;
	
	bool any = Keys[GLFW_KEY_A] || Keys[GLFW_KEY_D] || Keys[GLFW_KEY_W];
	if (!any) {
		character.velocity = glm::vec3(0.0f);
		return new IdleState();
	}
	
	if (Keys[GLFW_KEY_A]){
		character.turn_left(dt);
	}
	if (Keys[GLFW_KEY_D]){
		character.turn_right(dt);
	}
	if (Keys[GLFW_KEY_W]){
		character.velocity = character.forward * character.WALK_SPEED * dt;
	} 
	
	return nullptr;
}

void WalkState::update(Character& character, const float dt)
{
	
}