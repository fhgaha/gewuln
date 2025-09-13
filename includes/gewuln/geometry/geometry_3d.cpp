#include <gewuln/geometry/geometry_3d.h>
#include <gewuln/bone.h>
#include <gewuln/animator.h>

glm::vec3 Geometry3d::mat4_to_translation(glm::mat4 mat) {
    return glm::vec3((float)mat[3][0], (float)mat[3][1], (float)mat[3][2]);
}
    
bool Geometry3d::intersect(const std::vector<Vertex>& box_lhs, const std::vector<Vertex>& box_rhs) {
    return GJK::intersect(box_lhs, box_rhs);
}
    
glm::vec3 Geometry3d::compute_box_center(const std::vector<Vertex>& box) {
    glm::vec3 center(0.0f);
    for (const auto& v : box) {
        center += v.Position;
    }
    return center / static_cast<float>(box.size());
}

void Geometry3d::foo(glm::mat4 parentTransform, Bone* bone, glm::vec3 char_pos, glm::vec3 char_forward, glm::vec3 target)
{
    float new_angle_around_x_rad, new_angle_around_y_rad; 

    glm::vec3 char_to_trg_dir = glm::normalize(target - char_pos);
    {
        glm::mat4 neck_pos_mat = parentTransform * bone->GetLocalTransform();
        glm::vec3 neck_pos = glm::vec3(neck_pos_mat[3]) + char_pos;
        glm::vec3 neck_to_trg_dir = glm::normalize(target - neck_pos);
        new_angle_around_x_rad = glm::acos(neck_to_trg_dir.y) - glm::half_pi<float>();	//-П/2 to get the direction of the face
    }

    new_angle_around_y_rad = -glm::orientedAngle(
        glm::normalize(glm::vec2(char_forward.x, char_forward.z)),
        glm::normalize(glm::vec2(char_to_trg_dir.x, char_to_trg_dir.z))
    );
    
    bool too_large_angle_around_x = glm::degrees(new_angle_around_x_rad) >  Animator::NECK_ANGLE_AROUND_X_LIMIT_DEG 
                                || 	glm::degrees(new_angle_around_x_rad) < -Animator::NECK_ANGLE_AROUND_X_LIMIT_DEG;
    bool too_large_angle_around_y = glm::degrees(new_angle_around_y_rad) >  Animator::NECK_ANGLE_AROUND_Y_LIMIT_DEG 
                                ||	glm::degrees(new_angle_around_y_rad) < -Animator::NECK_ANGLE_AROUND_Y_LIMIT_DEG;
    if (too_large_angle_around_x || too_large_angle_around_y){
        new_angle_around_x_rad = 0.0f;
        new_angle_around_y_rad = 0.0f;
    }
}
