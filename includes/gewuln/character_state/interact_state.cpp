#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

void InteractState::enter(Character& character)
{
	printf("entered interact state\n");

	character.interactable_intersecting->action();
	// PlayCameraThing();

}


CharacterState* InteractState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	

	return new IdleState();

	return nullptr;
}

void InteractState::update(Character& character, const float dt)
{

}

void InteractState::exit(Character& character)
{
}