#ifndef CHARACTER_H
#define CHARACTER_H

#include <gewuln/animator.h>
#include <gewuln/model.h>


class Character {
	public:
		Model		*model;
		Animator	animator;

		Character(){};

		Character(Model *model, Animator animator){
			this->model = model;
			this->animator = animator;

			this->animator.PlayAnimation("idle");
		}
};

#endif