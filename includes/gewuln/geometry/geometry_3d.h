#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <gewuln/geometry/gjk_algo.h>
#include <gewuln/utils/gewuln_std.h>


class Geometry3d
{
public:
    
    static glm::vec3 mat4_to_translation(glm::mat4 mat) {
        return glm::vec3((float)mat[3][0], (float)mat[3][1], (float)mat[3][2]);
    }
        
    static bool intersect(const std::vector<Vertex>& box_lhs, const std::vector<Vertex>& box_rhs) {
        return GJK::intersect(box_lhs, box_rhs);
    }
       
    static glm::vec3 compute_box_center(const std::vector<Vertex>& box) {
        glm::vec3 center(0.0f);
        for (const auto& v : box) {
            center += v.Position;
        }
        return center / static_cast<float>(box.size());
    }
    
};
