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
	static bool IsPolygonCompletelyInside(const Clipper2Lib::PathsD& small_polygon, const Clipper2Lib::PathsD& large_polygon) 
	{
		Clipper2Lib::ClipperD clipper;
		clipper.AddSubject(small_polygon);
		clipper.AddClip(large_polygon);
		Clipper2Lib::PathsD solution;
		clipper.Execute(
			Clipper2Lib::ClipType::Difference, 
			Clipper2Lib::FillRule::EvenOdd, 
			solution
		);
		return solution.empty();
	}
	
};
