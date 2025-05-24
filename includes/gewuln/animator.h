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

	void UpdateAnimation(float dt)
	{
		if (!check_has_animations()) return;

		deltaTime = dt;
		if (currentAnimation)
		{
			currentTime += currentAnimation->GetTicksPerSecond() * dt;
			currentTime = fmod(currentTime, currentAnimation->GetDuration());
			// calculate_bone_transform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
			calculate_bone_transform_dbg(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
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
	
	
	glm::vec3 target = glm::vec3(1.0f);
	glm::vec3 forward_tmp = glm::vec3(1.0f);
	
	void calculate_bone_transform_dbg(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string desired_name = "mixamorig:Head";
		std::string nodeName = node->name;
		glm::mat4 globalTransformation;
		
		if (nodeName == desired_name){
			
			glm::mat4 nodeTransform = node->transformation;
			Bone* bone = currentAnimation->FindBone(nodeName);
			if (bone)
			{
				glm::vec3 up(0.0f, 1.0f, 0.0f);
				glm::mat4 pos_mat = parentTransform * bone->GetLocalTransform();
				auto pos = glm::vec3(pos_mat[3]);
				// auto pos = glm::vec3((float)pos_mat[3][0], (float)pos_mat[3][1], (float)pos_mat[3][2]);
				std::cout << pos << "\n";				
				glm::vec3 direction = glm::normalize(target - pos);

				//need to compensate character forward direction
				float angle_rad = glm::angle(direction, glm::normalize(forward_tmp));
				direction = glm::rotateY(direction, angle_rad);
				
				glm::quat rot_quat = glm::quatLookAt(direction, up);
				rot_quat = rot_quat;
				
				glm::mat4 rotation = glm::mat4(1.0f);
				// rotation = glm::rotate(
				// 	glm::radians(45.0f), 
					
				// 	// glm::normalize( glm::vec3(0.0f, 1.0f, 0.0f))
					
				// 	// glm::normalize(glm::vec3(
				// 	// 	sinf((float)glfwGetTime() + glm::half_pi<float>()), 
				// 	// 	sinf((float)glfwGetTime()), 
				// 	// 	0.0f
				// 	// ))
					
				// 	glm::normalize( glm::vec3(1.0f, 0.0f, 0.0f))
				// );
				
				rotation = glm::mat4_cast(rot_quat);
				
				
				
				bone->Update_with_rotation(currentTime, rotation);
				nodeTransform = bone->GetLocalTransform();
			}

			globalTransformation = parentTransform * nodeTransform;

			auto boneInfoMap = currentAnimation->GetBoneIDMap();
			if (boneInfoMap.find(nodeName) != boneInfoMap.end())
			{
				int index = boneInfoMap[nodeName].id;
				glm::mat4 offset = boneInfoMap[nodeName].offset;
				finalBoneMatrices[index] = globalTransformation * offset;
			}
		} else {
			glm::mat4 nodeTransform = node->transformation;
			Bone* bone = currentAnimation->FindBone(nodeName);

			if (bone){
				bone->Update(currentTime);
				nodeTransform = bone->GetLocalTransform();
			}

			globalTransformation = parentTransform * nodeTransform;

			auto boneInfoMap = currentAnimation->GetBoneIDMap();
			if (boneInfoMap.find(nodeName) != boneInfoMap.end()){
				int index = boneInfoMap[nodeName].id;
				glm::mat4 offset = boneInfoMap[nodeName].offset;
				finalBoneMatrices[index] = globalTransformation * offset;
			}
		}

		for (int i = 0; i < node->childrenCount; i++){
			calculate_bone_transform_dbg(&node->children[i], globalTransformation);
		}
	}


	std::vector<glm::mat4> 	GetFinalBoneMatrices() const {return finalBoneMatrices;}
	Animation*				GetCurrentAnimation() const {return currentAnimation;}

private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;

	bool check_has_animations() {
		if (animations.empty()) {
			std::cerr << "ERROR: can't animate a model with no animations!" << std::endl;
			return false;
		}

		return true;
	}
};
