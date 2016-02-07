#include "stdafx.h"
#include <Windows.h>
#include <gl\GL.h>
#include "ShapeRenderer.h"
#include "Shape.h"
#include <iostream>
#include <iterator>

using std::vector;
using std::unique_ptr;


ShapeRenderer::ShapeRenderer() {

}


ShapeRenderer::~ShapeRenderer() {
}

void ShapeRenderer::drawVertices(Shape& shape) {
	for (const Point& vertex : shape.getVertices()) {
		glVertex2f(
			shape.getPosition().x + (vertex.x * shape.getScale()), 
			shape.getPosition().y + (vertex.y* shape.getScale())
		);
	}
}

void ShapeRenderer::render(Shape& shape) {
	// Draw filled polygon
	glBegin(GL_POLYGON);
	glColor3f(shape.getColour().r, shape.getColour().g, shape.getColour().b);
	drawVertices(shape);
	glEnd();
	// Draw outline if it's set
	if (shape.isOutlineVisible()) {
		glBegin(GL_LINE_LOOP);
		glColor3f(shape.getOutlineColour().r, shape.getOutlineColour().g, shape.getOutlineColour().b);
		drawVertices(shape);
		glEnd();
	}
}

void ShapeRenderer::render(const vector<unique_ptr<Shape>>& shapes) {
	for (const auto& shape : shapes) {
		render(*shape);
	}
}