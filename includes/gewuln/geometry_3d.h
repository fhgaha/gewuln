#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <gewuln/mesh.h>
#include <limits>
#include <array>


class Geometry3d
{
public:
    
    static glm::vec3 mat4_to_translation(glm::mat4 mat)
    {
        return glm::vec3((float)mat[3][0], (float)mat[3][1], (float)mat[3][2]);
    }
    
    
    //-------------------------------------------------------------------------------
    //TODO gjk stuff below, should be separated
    //-------------------------------------------------------------------------------
    
    // Uses GJK (Gilbert–Johnson–Keerthi distance) algorithm. pass in only vertex coords of box_lhs and box_rhs.
    static bool intersect(const std::vector<Vertex>& box_lhs, const std::vector<Vertex>& box_rhs) {
        glm::vec3 dir = computeCenter(box_rhs) - computeCenter(box_lhs);
        if (glm::length(dir) < 1e-6) dir = glm::vec3(1, 0, 0); // Avoid zero direction

        std::vector<glm::vec3> simplex;
        simplex.push_back(minkowski_support(box_lhs, box_rhs, dir));
        dir = -simplex.back();

        constexpr int MAX_ITERATIONS = 32;
        for (int i = 0; i < MAX_ITERATIONS; ++i) {
            glm::vec3 new_point = minkowski_support(box_lhs, box_rhs, dir);
            if (glm::dot(new_point, dir) < 0) return false;

            simplex.push_back(new_point);
            if (handle_simplex(simplex, dir)) return true;
        }

        return false;
    }
    
private:
    static glm::vec3 computeCenter(const std::vector<Vertex>& box) {
        glm::vec3 center(0.0f);
        for (const auto& v : box) center += v.Position;
        return center / static_cast<float>(box.size());
    }

    static glm::vec3 support(const std::vector<Vertex>& shape, const glm::vec3& d) {
        glm::vec3 result = shape[0].Position;
        float max_dot = glm::dot(result, d);
        for (const auto& v : shape) {
            float dot = glm::dot(v.Position, d);
            if (dot > max_dot) {
                max_dot = dot;
                result = v.Position;
            }
        }
        return result;
    }

    static glm::vec3 minkowski_support(const std::vector<Vertex>& a, const std::vector<Vertex>& b, const glm::vec3& d) {
        return support(a, d) - support(b, -d);
    }

    static bool same_direction(const glm::vec3& a, const glm::vec3& b) {
        return glm::dot(a, b) > 1e-6; // Epsilon check
    }

    static bool handle_simplex(std::vector<glm::vec3>& simplex, glm::vec3& dir) {
        switch (simplex.size()) {
            case 2: return line_case(simplex, dir);
            case 3: return triangle_case(simplex, dir);
            case 4: return tetrahedron_case(simplex, dir);
            default: return false;
        }
    }

	static bool line_case(std::vector<glm::vec3>& simplex, glm::vec3& dir) {
        const glm::vec3 a = simplex[1];
        const glm::vec3 b = simplex[0];
        const glm::vec3 ab = b - a;
        const glm::vec3 ao = -a;

        if (same_direction(ab, ao)) {
            dir = glm::normalize(glm::cross(glm::cross(ab, ao), ab));
        } else {
            simplex = {a};
            dir = ao;
        }
        return false;
    }

    static bool triangle_case(std::vector<glm::vec3>& simplex, glm::vec3& dir) {
        const glm::vec3 a = simplex[2];
        const glm::vec3 b = simplex[1];
        const glm::vec3 c = simplex[0];
        
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ao = -a;

        const glm::vec3 abc = glm::cross(ab, ac);

        // Check perpendicular to AB
        const glm::vec3 ab_perp = glm::cross(ab, abc);
        if (same_direction(ab_perp, ao)) {
            simplex = {a, b};
            dir = glm::normalize(glm::cross(glm::cross(ab, ao), ab));
            return false;
        }

        // Check perpendicular to AC
        const glm::vec3 ac_perp = glm::cross(abc, ac);
        if (same_direction(ac_perp, ao)) {
            simplex = {a, c};
            dir = glm::normalize(glm::cross(glm::cross(ac, ao), ac));
            return false;
        }

        // Check triangle normal
        if (same_direction(abc, ao)) {
            dir = glm::normalize(abc);
        } else {
            dir = -glm::normalize(abc);
        }
        return false;
    }

    static bool tetrahedron_case(std::vector<glm::vec3>& simplex, glm::vec3& dir) {
        const glm::vec3 a = simplex[3];
        const glm::vec3 b = simplex[2];
        const glm::vec3 c = simplex[1];
        const glm::vec3 d = simplex[0];

        // Test all 4 faces of the tetrahedron
        const std::array<std::array<glm::vec3, 3>, 4> faces = {{
            {b, c, d}, // Opposite a
            {a, c, d}, // Opposite b
            {a, b, d}, // Opposite c
            {a, b, c}  // Opposite d
        }};

        const std::array<glm::vec3, 4> opposites = {a, b, c, d};

        for (int i = 0; i < 4; ++i) {
            const glm::vec3& B = faces[i][0];
            const glm::vec3& C = faces[i][1];
            const glm::vec3& D = faces[i][2];
            const glm::vec3& opposite = opposites[i];

            const glm::vec3 edge1 = C - B;
            const glm::vec3 edge2 = D - B;
            glm::vec3 normal = glm::cross(edge1, edge2);

            // Ensure normal points outward
            if (glm::dot(normal, opposite - B) > 0) {
                normal = -normal;
            }

            // Check if origin is outside this face
            if (glm::dot(normal, -B) > 1e-6) {
                simplex = {B, C, D};
                dir = glm::normalize(normal);
                return false;
            }
        }

        // Origin is inside tetrahedron
        return true;
    }
};
