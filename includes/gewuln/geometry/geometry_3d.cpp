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

//wtf is happening here?
glm::vec2 Geometry3d::calc_angles_around_x_y_rad(glm::mat4 neck_pos_mat_local, glm::vec3 char_pos, glm::vec3 char_forward, glm::vec3 target)
{
    float angle_around_x_rad, angle_around_y_rad; 

    {
        glm::vec3 neck_pos_global = glm::vec3(neck_pos_mat_local[3]) + char_pos;
        glm::vec3 neck_to_trg_dir = glm::normalize(target - neck_pos_global);
        angle_around_x_rad = glm::acos(neck_to_trg_dir.y) - glm::half_pi<float>();	//-П/2 to get the direction of the face
    }

    glm::vec3 char_to_trg_dir = glm::normalize(target - char_pos);
    angle_around_y_rad = -glm::orientedAngle(
        glm::normalize(glm::vec2(char_forward.x, char_forward.z)),
        glm::normalize(glm::vec2(char_to_trg_dir.x, char_to_trg_dir.z))
    );
    
    bool too_large_angle_around_x = glm::degrees(angle_around_x_rad) >  Animator::NECK_ANGLE_AROUND_X_LIMIT_DEG 
                                || 	glm::degrees(angle_around_x_rad) < -Animator::NECK_ANGLE_AROUND_X_LIMIT_DEG;
    bool too_large_angle_around_y = glm::degrees(angle_around_y_rad) >  Animator::NECK_ANGLE_AROUND_Y_LIMIT_DEG 
                                ||	glm::degrees(angle_around_y_rad) < -Animator::NECK_ANGLE_AROUND_Y_LIMIT_DEG;
    if (too_large_angle_around_x || too_large_angle_around_y){
        angle_around_x_rad = 0.0f;
        angle_around_y_rad = 0.0f;
    }
    
    return glm::vec2(angle_around_x_rad, angle_around_y_rad);
}
