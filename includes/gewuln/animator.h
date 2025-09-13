#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <gewuln/animation.h>
#include <gewuln/bone.h>
#include <gewuln/animator_observer.h>


class Animator
{
public:
	static const float			NECK_ANGLE_AROUND_X_LIMIT_DEG;
	static const float			NECK_ANGLE_AROUND_Y_LIMIT_DEG;
	static const float			NECK_ROTATION_SPEED_AROUND_Y;
	static const float			NECK_ROTATION_SPEED_AROUND_X;
	
	glm::vec3 target 		= glm::vec3(1.0f);
	glm::vec3 char_forward 	= glm::vec3(1.0f);
	glm::vec3 char_pos 		= glm::vec3(0.0f);
	 
	Animator(){};
	Animator(const std::string &animationPath, Model &model);
	
	void 					play_animation(std::string anim_name);
	void 					play_animation_once(std::string anim_name);
	void 					update_animation(float dt);
	void 					calculate_bone_transform(const AssimpNodeData* node, glm::mat4 parentTransform);
	void 					update_animation_with_look_at(float dt);
	void 					calculate_bone_transform_with_look_at(const AssimpNodeData* node, glm::mat4 parentTransform);	
	void 					add_observer(I_AnimatorObserver* observer);
	void 					remove_observer(I_AnimatorObserver* observer);
	std::vector<glm::mat4> 	get_final_bone_matrices() const;
	Animation*				get_current_animation() const;

protected:
	void notify_animation_ended(const Animator& sender, AnimatorData data);

private:
	unsigned int 								MAX_BONES_AMOUNT = 100;
	std::vector<glm::mat4> 						final_bone_matrices;
	std::unordered_map<std::string, Animation> 	animations;
	Animation* 									current_animation;
	float 										current_time;
	float 										delta_time;
	float 										angle_around_x_rad = 0.0f;
	float 										angle_around_y_rad = 0.0f;
	bool 										cur_anim_should_be_played_once;
	std::vector<I_AnimatorObserver*> 			observers;
	
	bool reached_animation_end();
	bool check_has_animations();
	void play_animation_common(std::string anim_name);
};
