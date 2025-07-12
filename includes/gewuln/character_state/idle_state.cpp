#include <gewuln/character_state/character_state.h>

CharacterState* IdleState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)  
{
	printf("entered idle state\n");
	
	if (true) {
		return new WalkState();
	}
	
	return nullptr;
}

void IdleState::update(Character& character, const float dt)
{
	
}