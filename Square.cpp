#include "stdafx.h"
#include "Square.h"


Square::Square(float radius, Point position) 
	: RegularPolygon("Square", 4, radius, position) {}
