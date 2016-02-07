#include "stdafx.h"
#include "RegularPolygon.h"
#include <iostream>


RegularPolygon::RegularPolygon(std::string name, int numEdges, float radius, Point position)
	: Shape(name, position), numEdges(numEdges), radius(radius) {
	create();
}

RegularPolygon::RegularPolygon(Shape* shape) : Shape(shape) {}


void RegularPolygon::create() {
	// Angle between vertices in radians
	float rot = 3.142f * ((360.f / numEdges) / 180.f);
	// Add the initial vertex directly up from the centre
	vertices.push_back(Point(0, radius));
	std::cout << rot << std::endl;
	// For each remaining vertex
	for (int i = 1; i <= numEdges; i++) {
		// Create a vertex rotated counter-clockwise from the initial vertex by 
		// the angle between each vertex multiplied by the vertex index
		vertices.push_back(Point(
			vertices[0].x * cos(rot * i) - vertices[0].y * sin(rot * i),
			vertices[0].x * sin(rot * i) + vertices[0].y * cos(rot * i)
		));
	}
}