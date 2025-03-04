#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <gewuln/animation.h>
#include <gewuln/bone.h>

class Animator
{
public:
	std::unordered_map<std::string, Animation> animations;
	
	Animator(){};
	
	Animator(const std::string& animationPath, Model* model): animations()
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		// if (!scene->mAnimations || scene->mNumAnimations == 0) {
		if (!scene->HasAnimations()) {
			std::cerr << "ERROR: No animations found in file: " << animationPath << std::endl;
			return;	//??
    	} else {
			
			// import animations
			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				auto anim_name = scene->mAnimations[i]->mName.C_Str();
				auto animation = Animation(scene->mAnimations[i], scene, model);
				animations[anim_name] = animation;
			}
			
			currentTime = 0.0;
			currentAnimation = &animations["idle"];

			finalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
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
			CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}
	
	void PlayAnimation(std::string anim_name)
	{
		if (!check_has_animations()) return;
		
		Animation* try_play;
		try{
			try_play = &animations.at(anim_name);
		}
		catch(const std::exception& e){
			std::cerr << "\nNo such animation as \"" << anim_name <<"\"\n";
			return;
		}
		
		if (currentAnimation == try_play) {
			return;
		}
		
		currentAnimation = try_play;
		currentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = currentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(currentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices() { return finalBoneMatrices;	}

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