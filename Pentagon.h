#pragma once

#include "RegularPolygon.h"


class Pentagon : public RegularPolygon {

public:
	/**
	* Create a new pentagon shape
	* Parameter: float radius  Distance, in pixels, from center to vertex
	*/
	Pentagon(float radius, Point position);
};

