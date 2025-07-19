#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

CharacterState* WalkState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	printf("entered walk state\n");
	
	auto w = Keys[GLFW_KEY_W];
	auto st = new IdleState();
	
	return nullptr;
	
	
}

void WalkState::update(Character& character, const float dt)
{
	
}