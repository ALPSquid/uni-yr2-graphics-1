#include "stdafx.h"
#include "ShapeManager.h"
#include "Shape.h"
#include "ShapeRenderer.h"
#include <iostream>

using std::unique_ptr;
using std::vector;
using std::string;
using std::map;


ShapeManager::ShapeManager() {
	renderer = ShapeRenderer();
}


ShapeManager::~ShapeManager() {
	// Vectors and the unique pointers are destroyed automatically
}

void ShapeManager::update() {
	renderer.render(shapes);
}

void ShapeManager::save(SaveManager& saveManager) {
	saveManager.startSection("shape_manager");
	for (auto& shape : shapes) {
		saveManager.startKey("shape");
		saveManager.addValue("name", shape->getName());
		saveManager.addValue("rotation", shape->getRotation());
		saveManager.addValue("scale", shape->getScale());
		saveManager.addValue("position", shape->getPosition());
		//saveManager.addValue("outline_visible", shape->isOutlineVisible());
		saveManager.addValue("vertices", shape->getVertices());
		saveManager.addValue("colour", shape->getColour());
		saveManager.addValue("outline_colour", shape->getOutlineColour());
		saveManager.endKey();
	}
	saveManager.endSection();
}

void ShapeManager::load(SaveManager& saveManager) {
	vector<map<string, string>>& shapeVals = saveManager.getSectionKeyValues("shape_manager", "shape");
	vector<map<string, vector<string>>>& shapeArrays = saveManager.getSectionKeyArrays("shape_manager", "shape");
	Shape* shape;
	vector<Point> vertices;
	for (unsigned i = 0; i < shapeVals.size(); i++) {
		vertices.clear();
		// Convert point strings to Point objects
		for (const auto& pointStr : shapeArrays[i]["vertices"]) {
			vertices.push_back(Point(pointStr));
		}
		shape = new Shape(shapeVals[i]["name"], Point(shapeVals[i]["position"]), vertices);
		shape->setRotation(stof(shapeVals[i]["rotation"]), false);
		shape->setScale(stof(shapeVals[i]["scale"]));
		//shape->setOutlineVisible((shapeVals[i]["scale"] == "1")? true : false);
		shape->setColour(Colour(shapeVals[i]["colour"]));
		shape->setOutlineColour(Colour(shapeVals[i]["outline_colour"]));
		add(shape);
	}
}

void ShapeManager::clear() {
	shapes.clear();
}

Shape* ShapeManager::getShapeAt(float x, float y) {
	// Since the shape at the back is the shape rendered on top, iterate in reverse
	for (vector<unique_ptr<Shape>>::reverse_iterator it = shapes.rbegin(); it != shapes.rend(); it++) {
		if ((*it)->pointInShape(x, y)) {
			return (*it).get();
		}
	}
	return nullptr;
}

void ShapeManager::bringToFront(Shape* shape) {
	if (shape) {
		for (vector<unique_ptr<Shape>>::iterator it = shapes.begin(); it != shapes.end(); it++) {
			// If the memory address of the test shape is equal to the memory address of the current shape
			if (&(*shape) == &(**it)) {
				Utils::moveToBack(shapes, it - shapes.begin());
			}
		}
	}
}

void ShapeManager::add(Shape* shape) {
	if (shape) {
		// Create smart pointer
		unique_ptr<Shape> pShape;
		// Set to existing shape pointer
		pShape.reset(shape);
		// Move the pointer into the vector
		shapes.push_back(std::move(pShape));
	}
}

void ShapeManager::remove(Shape* shape) {
	if (shape) {
		for (vector<unique_ptr<Shape>>::iterator it = shapes.begin(); it != shapes.end(); it++) {
			// If the memory address of the test shape is equal to the memory address of the current shape
			if (&(*shape) == &(**it)) {
				shapes.erase(it);
				break;
			}
		}
	}
}
