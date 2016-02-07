#include "stdafx.h"
#include "Pentagon.h"
#include <iostream>

using std::vector;


Pentagon::Pentagon(float radius, Point position) 
	: RegularPolygon("Pentagon", 5, radius, position) {}