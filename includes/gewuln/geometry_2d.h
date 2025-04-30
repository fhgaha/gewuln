#pragma once

#include "clipper2/clipper.h"
#include <iomanip> 


class Geometry2d
{
public:
	static void foo() 
	{
		Clipper2Lib::PathsD subject = {{{100,50},{9.5,79.4},{65.5,2.4},{65.5,97.6},{9.5,20.7}}};
		Clipper2Lib::PathsD clip = {{{20,20},{80,20},{80,80},{20,80}}};
		Clipper2Lib::PathsD solution = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);
		std::cout << std::setprecision(3) << solution << std::endl;
	}

	
	// using PathsD = std::vector< PathD>;
	// using PathD = Path<double>;
	// using Path = std::vector<Point<T>>;
	// struct Point {T x; T y;}
	static bool is_polygon_inside(const Clipper2Lib::PathsD& small_polygon, const Clipper2Lib::PathsD& large_polygon) 
	{
		// Clipper2Lib::PathsD solution = Clipper2Lib::Difference(
		// 	large_polygon, 
		// 	small_polygon, 
		// 	Clipper2Lib::FillRule::NonZero
		// );
		// return solution.empty();
		
		Clipper2Lib::PathsD solution = Clipper2Lib::Intersect(
			{large_polygon}, 
			{small_polygon}, 
			Clipper2Lib::FillRule::NonZero
		);
		bool inside = !solution.empty() && std::abs(Clipper2Lib::Area(small_polygon)) == std::abs(Clipper2Lib::Area(solution));
		
		// bool all_vertices_inside = true;
		// auto small = small_polygon[0];
		// for (const auto& pt : small) {
		// 	if (Clipper2Lib::PointInPolygon(pt, large_polygon[0]) != Clipper2Lib::PointInPolygonResult::IsInside) {
		// 		all_vertices_inside = false;
		// 		break;
		// 	}
		// }
		
		return inside;
	}
	
	static bool small_poly_inside_all_large_polys(const Clipper2Lib::PathD &small_polygon, const Clipper2Lib::PathsD &large_polygons) {
		return std::ranges::all_of(
			small_polygon, 
			[large_polygons](const auto &pt){
				return std::ranges::any_of(
					large_polygons,
					[pt](const auto &large_path) {
						Clipper2Lib::PointInPolygonResult res = Clipper2Lib::PointInPolygon(pt, large_path);		
						return res == Clipper2Lib::PointInPolygonResult::IsInside;
					}
				);
			}
		);
	}
	
};
