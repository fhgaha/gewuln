#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

CharacterState* IdleState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)  
{
	printf("entered idle state\n");
	
	auto w = Keys[GLFW_KEY_W];
	auto st = new WalkState();
	auto v = character.velocity;
	
	return nullptr;
}

void IdleState::update(Character& character, const float dt)
{
	
}