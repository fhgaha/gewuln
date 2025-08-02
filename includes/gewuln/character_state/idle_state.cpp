#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

void IdleState::enter(Character& character)
{
	printf("entered idle state\n");

}

CharacterState* IdleState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	if (Keys[GLFW_KEY_E] && !KeysProcessed[GLFW_KEY_E]){
		auto intr = character.collider_intersects_an_interactable();
		if (intr) {
			character.interactable_intersecting = intr;
			KeysProcessed[GLFW_KEY_E] = true;
			return new InteractState();
		}
		KeysProcessed[GLFW_KEY_E] = true;
	}

	bool wasd = Keys[GLFW_KEY_A] || Keys[GLFW_KEY_D] || Keys[GLFW_KEY_W];
	if (wasd) {
		return new WalkState();
	}

	return nullptr;
}

void IdleState::update(Character& character, const float dt)
{

}

void IdleState::exit(Character& character)
{
}