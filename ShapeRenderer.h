#pragma once
#include <vector>
#include <memory>
#include "Shape.h"



class ShapeRenderer {

public:
	ShapeRenderer();
	~ShapeRenderer();

	/**
	* Render a Shapes
	* Parameter: Shape& shape  Shape to render
	*/
	void render(Shape& shape);
	/**
	* Render a vector of Shapes in order, i.e. Shape at element 0 will be rendered first (at the back)
	* Parameter: vector<unique_ptr<Shape>>* shapes  Shapes to render
	*/
	void render(const std::vector<std::unique_ptr<Shape>>& shapes);

private:
	/**
	* Draws the vertices of a shape
	* Parameter: Shape& shape  Shape reference to draw
	*/
	void drawVertices(Shape& shape);

};

