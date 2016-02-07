#pragma once

#include <string>
#include "Shape.h"


class RegularPolygon : public Shape {

protected:
	int numEdges;
	float radius;

public:
	
	/**
	* Creates a regular polygon where all sides are of equal length and vertices are rotated around it's center
	* Parameter: std::string name  Name of the polygon, such as Triangle, Square, Pentagon etc.
	* Parameter: float numEdges  Number of edges for the polygon
	* Parameter: float radius  Radius of the polygon (distance from center to a vertex)
	* Parameter: Point position  Initial position of the polygon
	*/
	RegularPolygon(std::string name, int numEdges, float radius, Point position);
	RegularPolygon(Shape* shape);

protected:
	virtual void create() override;
};

