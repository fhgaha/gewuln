#pragma once

#include <iomanip> 


class Geometry2d
{
public:

	static bool rect_inside_area_of_tris(
		const std::array<glm::vec2, 4> 				&small_rect, 
		const std::vector<std::array<glm::vec2, 3>> &tris
	) {
		return std::ranges::all_of(
			small_rect, 
			[tris](const auto &pt){
				return std::ranges::any_of(
					tris,
					[pt](const auto &tri) {
						bool inside = point_in_triangle(pt, tri[0], tri[1], tri[2]);
						return inside;
					}
				);
			}
		);
	}
	
	static bool point_in_triangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
	{
		float s = (p0.x - p2.x) * (p.y - p2.y) - (p0.y - p2.y) * (p.x - p2.x);
		float t = (p1.x - p0.x) * (p.y - p0.y) - (p1.y - p0.y) * (p.x - p0.x);

		if ((s < 0) != (t < 0) && s != 0 && t != 0){
			return false;
		}
		float d = (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x);
		return d == 0 || (d < 0) == (s + t <= 0);
	}
};
