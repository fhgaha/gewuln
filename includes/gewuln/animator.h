#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <gewuln/animation.h>
#include <gewuln/bone.h>

const unsigned int MAX_BONES_AMOUNT = 100;

class Animator
{
public:
	std::unordered_map<std::string, Animation> animations;

	Animator(){};

	Animator(const std::string &animationPath, Model &model): animations()
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		if (!scene->HasAnimations()) {
			std::cerr << "ERROR: No animations found in file: " << animationPath << std::endl;
			return;
    	}

		// import animations
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			auto anim_name = scene->mAnimations[i]->mName.C_Str();
			auto animation = Animation(scene->mAnimations[i], scene, &model);
			animations[anim_name] = animation;
		}

		currentTime = 0.0;
		currentAnimation = &animations["idle"];

		finalBoneMatrices.reserve(MAX_BONES_AMOUNT);

		for (int i = 0; i < 100; i++){
			finalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void PlayAnimation(std::string anim_name)
	{
		if (!check_has_animations()) return;

		Animation* anim_to_play;
		try{
			anim_to_play = &animations.at(anim_name);
		}
		catch(const std::exception& e){
			std::cerr << "\nNo such animation as \"" << anim_name <<"\"\n";
			return;
		}

		if (currentAnimation == anim_to_play) {
			return;
		}

		currentAnimation = anim_to_play;
		currentTime = 0.0f;
	}

	void update_animation(float dt)
	{
		if (!check_has_animations()) return;

		deltaTime = dt;
		if (currentAnimation)
		{
			currentTime += currentAnimation->GetTicksPerSecond() * dt;
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
			calculate_bone_transform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void calculate_bone_transform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;
		Bone* bone = currentAnimation->FindBone(nodeName);
		if (bone){
			bone->Update(currentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++){
			calculate_bone_transform(&node->children[i], globalTransformation);
		}
	}
	
	
	void update_animation_with_look_at(float dt)
	{
		if (!check_has_animations()) return;

		deltaTime = dt;
		if (currentAnimation)
		{
			currentTime += currentAnimation->GetTicksPerSecond() * dt;
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
			// calculate_bone_transform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
			calculate_bone_transform_with_look_at(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}
	
	glm::vec3 target 		= glm::vec3(1.0f);
	glm::vec3 char_forward 	= glm::vec3(1.0f);
	glm::vec3 char_pos 		= glm::vec3(0.0f);
	
	void calculate_bone_transform_with_look_at(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string desired_name = "mixamorig:Neck";
		std::string nodeName = node->name;
		glm::mat4 globalTransformation;
		
		glm::mat4 nodeTransform = node->transformation;
		Bone* bone = currentAnimation->FindBone(nodeName);

		if (nodeName == desired_name){
			if (bone)
			{
				glm::vec3 char_to_trg_dir = glm::normalize(target - char_pos);
				float angle_around_y = -glm::orientedAngle(
					glm::normalize(glm::vec2(char_forward.x, char_forward.z)), 
					glm::normalize(glm::vec2(char_to_trg_dir.x, char_to_trg_dir.z))
				);
				
				glm::mat4 neck_pos_mat = parentTransform * bone->GetLocalTransform();
				glm::vec3 neck_pos = glm::vec3(neck_pos_mat[3]) + char_pos;
				glm::vec3 neck_to_trg_dir = glm::normalize(target - neck_pos);
				float angle_around_x = glm::acos(neck_to_trg_dir.y) - glm::half_pi<float>();	//-П/2 to get the direction of the face
				
				bool too_large_angle_around_x = glm::degrees(angle_around_x) > 70.0f || glm::degrees(angle_around_x) < -70.0f;
				bool too_large_angle_around_y = glm::degrees(angle_around_y) > 85.0f || glm::degrees(angle_around_y) < -85.0f;
				if (too_large_angle_around_x || too_large_angle_around_y){
					angle_around_x = 0.0f;
					angle_around_y = 0.0f;
				}
				
				// angle_around_x_rad = glm::lerp(angle_around_x_rad, angle_around_x,  deltaTime);
				// angle_around_y_rad = glm::lerp(angle_around_y_rad, angle_around_y,  deltaTime);
				
				// angle_around_x_rad = angle_around_x;
				// angle_around_y_rad = angle_around_y;
				
				bool big_difference  = glm::length2(angle_around_x_rad - angle_around_x) > 1e-3 
									|| glm::length2(angle_around_y_rad - angle_around_y) > 1e-3;
				if (big_difference) {
					angle_around_x_rad += (angle_around_x_rad > angle_around_x ? -1 : 1) * deltaTime;
					angle_around_y_rad += (angle_around_y_rad > angle_around_y ? -1 : 1) * deltaTime;
				}
				
				// instead of this should slerp old rotation and new rotation i guess
				glm::mat4 rotation = glm::mat4(1.0f);
				rotation = glm::rotate(rotation, angle_around_y_rad, glm::vec3(0.0f, 1.0f, 0.0f));
				rotation = glm::rotate(rotation, angle_around_x_rad, glm::vec3(1.0f, 0.0f, 0.0f));
		
				bone->Update_with_rotation(currentTime, rotation);
				// bone->Update(currentTime);
				nodeTransform = bone->GetLocalTransform();
			} 
		} else {
			if (bone){
				bone->Update(currentTime);
				nodeTransform = bone->GetLocalTransform();
			}
		}
		
		globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++){
			calculate_bone_transform_with_look_at(&node->children[i], globalTransformation);
		}
	}


	std::vector<glm::mat4> 	GetFinalBoneMatrices() const {return finalBoneMatrices;}
	Animation*				GetCurrentAnimation() const {return currentAnimation;}

private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;
	float angle_around_x_rad = 0.0f;
	float angle_around_y_rad = 0.0f;
	float NECK_ROTATION_SPEED_AROUND_X = 10.0f;
	float NECK_ROTATION_SPEED_AROUND_Y = 5.0f;

	bool check_has_animations() {
		if (animations.empty()) {
			std::cerr << "ERROR: can't animate a model with no animations!" << std::endl;
			return false;
		}

		return true;
	}
};
