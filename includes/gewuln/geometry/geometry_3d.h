#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <gewuln/geometry/gjk_algo.h>
#include <gewuln/utils/gewuln_std.h>


class Bone;
class Animator;


class Geometry3d
{
public:
    
    static glm::vec3    mat4_to_translation(glm::mat4 mat);        
    static bool         intersect(const std::vector<Vertex>& box_lhs, const std::vector<Vertex>& box_rhs);
    static glm::vec3    compute_box_center(const std::vector<Vertex>& box);
    static glm::vec2    calc_angles_around_x_y_rad(glm::mat4 from_mat, glm::vec3 char_pos, glm::vec3 char_forward, glm::vec3 target);
};
