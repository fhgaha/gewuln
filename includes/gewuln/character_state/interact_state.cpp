#include <gewuln/character_state/character_state.h>
#include <GLFW/glfw3.h>

void InteractState::enter(Character& character)
{
	printf("entered interact state\n");

	character.interactable_intersecting->action();
	//turn player to the interactable object center
	
	bool ok = true;

	if (ok){
		character.turn_to_target_instantly(
			Geometry3d::compute_box_center(character.interactable_intersecting->mesh->vertices)
		);
	}
	
	
	// PlayCameraThing();

}


CharacterState* InteractState::process_input(Character& character, bool *Keys, bool *KeysProcessed, const float dt)
{
	

	// return new IdleState();

	return nullptr;
}

void InteractState::update(Character& character, const float dt)
{
	character.animator.play_animation("interact");
}

void InteractState::exit(Character& character)
{
}
