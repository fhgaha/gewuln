#include <gewuln/animator.h>
#include <glm/gtx/vector_angle.hpp>

const float Animator::NECK_ANGLE_AROUND_X_LIMIT_DEG = 70.0f;
const float Animator::NECK_ANGLE_AROUND_Y_LIMIT_DEG = 85.0f;
const float Animator::NECK_ROTATION_SPEED_AROUND_Y = 5.0f;
const float Animator::NECK_ROTATION_SPEED_AROUND_X = 10.0f;

Animator::Animator(const std::string &animationPath, Model &model): animations()
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

	current_time = 0.0;
	current_animation = &animations["idle"];

	final_bone_matrices.reserve(MAX_BONES_AMOUNT);

	for (int i = 0; i < 100; i++){
		final_bone_matrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::play_animation(std::string anim_name)
{
	cur_anim_should_be_played_once = false;
	play_animation_common(anim_name);
}

void Animator::play_animation_once(std::string anim_name)
{
	cur_anim_should_be_played_once = true;
	play_animation_common(anim_name);
}

void Animator::play_animation_common(std::string anim_name)
{
	Animation* anim_to_play;
	try{
		anim_to_play = &animations.at(anim_name);
	}
	catch(const std::exception& e){
		std::cerr << "No such animation as \"" << anim_name <<"\"\n";
		return;
	}

	if (current_animation == anim_to_play) {
		return;
	}

	current_animation = anim_to_play;
	current_time = 0.0f;
}

void Animator::update_animation(float dt)
{
	// if (!check_has_animations()) return;

	delta_time = dt;
	if (current_animation)
	{
		current_time += current_animation->GetTicksPerSecond() * dt;
		current_time = fmod(current_time, current_animation->GetDuration());
		calculate_bone_transform(&current_animation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::calculate_bone_transform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;
	Bone* bone = current_animation->FindBone(nodeName);
	if (bone){
		bone->Update(current_time);
		nodeTransform = bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = current_animation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		final_bone_matrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++){
		calculate_bone_transform(&node->children[i], globalTransformation);
	}
}

void Animator::update_animation_with_look_at(float dt)
{
	// if (!check_has_animations()) return;

	delta_time = dt;
	if (current_animation)
	{
		current_time += current_animation->GetTicksPerSecond() * dt;
		if (reached_animation_end()){
			notify_animation_ended(*this, AnimatorData{});
		}
		current_time = fmod(current_time, current_animation->GetDuration());
		calculate_bone_transform_with_look_at(&current_animation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::calculate_bone_transform_with_look_at(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	const std::string desired_name = "mixamorig:Neck";
	const std::string nodeName = node->name;
	glm::mat4 globalTransformation;

	glm::mat4 nodeTransform = node->transformation;
	Bone* bone = current_animation->FindBone(nodeName);

	if (bone){
		if (nodeName == desired_name){

			glm::vec2 values = Geometry3d::calc_angles_around_x_y_rad(
				parentTransform * bone->GetLocalTransform(), 
				char_pos, 
				char_forward, 
				target
			);
			float new_angle_around_x_rad = values.x; 
			float new_angle_around_y_rad = values.y;
			
			bool big_enough_difference  = glm::length2(angle_around_x_rad - new_angle_around_x_rad) > 1e-3
									   || glm::length2(angle_around_y_rad - new_angle_around_y_rad) > 1e-3;
			if (big_enough_difference) {
				angle_around_x_rad += (angle_around_x_rad > new_angle_around_x_rad ? -1 : 1) * delta_time;
				angle_around_y_rad += (angle_around_y_rad > new_angle_around_y_rad ? -1 : 1) * delta_time;
			}
			
			// instead of this should slerp old rotation and new rotation i guess
			glm::mat4 rotation = glm::mat4(1.0f);
			rotation = glm::rotate(rotation, angle_around_y_rad, glm::vec3(0.0f, 1.0f, 0.0f));
			rotation = glm::rotate(rotation, angle_around_x_rad, glm::vec3(1.0f, 0.0f, 0.0f));

			bone->Update_with_rotation(current_time, rotation);
		} else {
			bone->Update(current_time);
		}

		nodeTransform = bone->GetLocalTransform();
	}

	globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = current_animation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		final_bone_matrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++){
		calculate_bone_transform_with_look_at(&node->children[i], globalTransformation);
	}
}

void Animator::add_observer(I_AnimatorObserver* observer)
{
	observers.push_back(observer);
}

void Animator::remove_observer(I_AnimatorObserver* observer)
{
	for (std::vector<I_AnimatorObserver*>::iterator it = observers.begin(); it != observers.end();)
	{
		if (*it == observer){
			it = observers.erase(it);
		}
	}
}

std::vector<glm::mat4> 	Animator::get_final_bone_matrices() const {return final_bone_matrices;}
Animation*				Animator::get_current_animation() const {return current_animation;}

void Animator::notify_animation_ended(const Animator& sender, AnimatorData data)
{
	for (int i = 0; i < observers.size(); i++)
	{
		observers[i]->on_notify(sender, data);
	}
}

bool Animator::reached_animation_end(){
	assert(current_animation && "Current animation is not set");
	return current_time > current_animation->GetDuration();
}

bool Animator::check_has_animations() {
	if (animations.empty()) {
		std::cerr << "ERROR: can't animate a model with no animations! " << "{this animator parent name}" << std::endl;
		return false;
	}

	return true;
}
