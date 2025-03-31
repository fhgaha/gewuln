#ifndef CHARACTER_H
#define CHARACTER_H

#include <gewuln/animation.h>
#include <gewuln/animator.h>
#include <gewuln/model.h>


class Character {
	public:
		float SPEED = 0.5f;

		Model		*model;
		Animator	animator;

		glm::vec3	position;
		glm::vec3	direction = glm::vec3(0.0f, 0.0f, -1.0f);	//forward
		glm::vec3	velocity  = glm::vec3(0.0f);

		Character(){};

		Character(Model *model, Animator animator, glm::vec3 position){
			this->model = model;
			this->animator = animator;
			this->position = position;

			this->animator.PlayAnimation("idle");
		}

		void ProcessInput(const bool keys[]) {
			if (keys[GLFW_KEY_1]) {
            	animator.PlayAnimation("idle");
            	velocity = glm::vec3(0.0f);
	        }
	        if (keys[GLFW_KEY_2]){
	            animator.PlayAnimation("walk");
	            velocity = glm::vec3(0.0f, 0.0f, 1.0f) * SPEED;
	        }
	        if (keys[GLFW_KEY_3]){
	            animator.PlayAnimation("interact");
	            velocity = glm::vec3(0.0f);
	        }
		}

		void Update(const float dt) {
			// update position
			position += velocity * dt;

			animator.UpdateAnimation(dt);
		}

};

#endif