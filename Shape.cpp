#include "stdafx.h"
#include "Shape.h"

using std::string;


Shape::Shape(string name, Point position) : name(name), position(position) {}

Shape::Shape(string name, Point position, std::vector<Point> vertices) : name(name), position(position), vertices(vertices) {}

Shape::Shape(Shape* shape) {
	copy(shape);
}

void Shape::rotateBy(float angle) {
	updateRotation(rotation + angle);
}
void Shape::setRotation(float angle, bool updateVerts) {
	if (updateVerts) updateRotation(angle);
	else rotation = angle;
}

void Shape::updateRotation(float newRotation) {
	// Wrap values above +-360 to make text output more logical
	if (abs(newRotation) > 360) newRotation = (int)newRotation % 360;
	// Amount to rotate by to reach target rotation, in radians
	float deltaRot = 3.142f * ((rotation - newRotation) / 180);
	for (auto& vert : vertices) {
		vert = Point(
			// Rotate point clockwise
			vert.x * cos(deltaRot) + vert.y * sin(deltaRot),
			-vert.x * sin(deltaRot) + vert.y * cos(deltaRot)
		);
	}
	rotation = newRotation;
}


void Shape::scaleBy(float scaleFactor) {
	updateScaling(scale * scaleFactor);
}
void Shape::increaseScale(float scaleAmount) {
	updateScaling(scale + scaleAmount);
}
void Shape::setScale(float scaleFactor) {
	updateScaling(scaleFactor);
}

void Shape::updateScaling(float newScale) {
	// Amount to scale by to reach target scale
	float deltaScale = newScale - scale;
	// To avoid floating point rounding errors, vertices are scaled when used, 
	// as opposed to constantly being scaled on update
	scale += deltaScale;
}


void Shape::translate(float x, float y) {
	position.x += x;
	position.y += y;
}

void Shape::setPosition(float x, float y) {
	position.x = x;
	position.y = y;
}


void Shape::setColour(float r, float g, float b) {
	colour.r = r;
	colour.g = g;
	colour.b = b;
}
void Shape::setColour(Colour& newColour) {
	colour.r = newColour.r;
	colour.g = newColour.g;
	colour.b = newColour.b;
}

void Shape::setOutlineColour(float r, float g, float b) {
	outlineColour.r = r;
	outlineColour.g = g;
	outlineColour.b = b;
}
void Shape::setOutlineColour(Colour& newColour) {
	outlineColour.r = newColour.r;
	outlineColour.g = newColour.g;
	outlineColour.b = newColour.b;
}


bool Shape::pointInBounds(float x, float y) {
	// Convert test point to local coordinates
	x -= position.x;
	y -= position.y;
	// Smallest and largest x and y points
	float xMin = vertices[0].x;
	float xMax = vertices[0].x;
	float yMin = vertices[0].y;
	float yMax = vertices[0].y;
	for (auto& vert : vertices) {
		if (vert.x < xMin) xMin = vert.x;
		if (vert.x > xMax) xMax = vert.x;
		if (vert.y < yMin) yMin = vert.y;
		if (vert.y > yMax) yMax = vert.y;
	}
	// If the target x is within xMin and xMax and target y is within yMin and yMax, it's within the shape's bounds
	return (x > xMin * scale && x < xMax * scale) && (y > yMin * scale && y < yMax * scale);
}

bool Shape::pointInShape(float x, float y) {
	// Quick check to see if point is in the shape bounding box, if it is, perform a slower, more accurate test
	if (pointInBounds(x, y)) {
		// Convert test point to local coordinates
		x -= position.x;
		y -= position.y;
		// Based on PNPOLY by Wm. Randolph Franklin (https://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html)
		// Cast a horizontal ray to the right from the test point and for each edge the it crosses, flips the inShape boolean 
		// If number of crossed edges is odd, the point is in the shape and the boolean will be true
		int numVerts = vertices.size();
		bool inShape = false; // Whether the point is in the shape
		std::vector<float> vertx; // Vector of x vertices
		std::vector<float> verty; // Vector of y vertices
		for (auto& vert : vertices) {
			// Multiply each vertex by the scale and add to appropriate vector to improve readability of the if statement
			vertx.push_back(vert.x * scale);
			verty.push_back(vert.y * scale);
		}
		int i, j;
		for (i = 0, j = numVerts - 1; i < numVerts; j = i++) {
			// j and i are the indices of the 2 vertices that make up the current edge
			// Check if test y is within the upper and lower y bound of the edge
			if (((verty[i] > y) != (verty[j] > y)) &&
				// If it is, check if the test x is to the left of the x point of the edge, given the test y
				(x < (vertx[j] - vertx[i]) * (y - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
				// If if its, an edge has been crossed, flip inShape boolean
				inShape = !inShape;
		}
		return inShape;
		//
	}
	return false;
}

void Shape::morph(Shape* shape) {
	if (shape) {
		// Set this shape's vertices to the target shape's vertices
		vertices = shape->getVertices();
		// Since the target shape could have a different scale and rotation which will have been applied to the vertices
		// cache this shape's current scale and rotation for transforming to after
		float currentScale = scale;
		float currentRotation = rotation;
		// Update this shape's scale and rotation to match the target shape's scale and rotation
		scale = shape->getScale();
		rotation = shape->getRotation();
		name = shape->getName();
		// Update this shape's scale and rotation to the original (cached) values
		setScale(currentScale);
		setRotation(currentRotation);
	}
}

void Shape::copy(Shape* shape) {
	if (shape) {
		rotation = shape->getRotation();
		scale = shape->getScale();
		position = shape->getPosition();
		colour = shape->getColour();
		outlineColour = shape->getOutlineColour();
		morph(shape);
	}
}
