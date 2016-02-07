#pragma once

#include "ShapeRenderer.h"
#include "Shape.h"
#include "Pentagon.h"
#include "SaveManager.h"
#include <vector>
#include <memory>

/*
* Manages Shape objects in a scene, including rendering and updating
*/
class ShapeManager {


protected:
	// Use a (smart) pointer for polymorphism
	std::vector<std::unique_ptr<Shape>> shapes;
	ShapeRenderer renderer;

public:
	ShapeManager();
	~ShapeManager();

	/**
	* Update and render added Shapes
	*/
	void update();

	/**
	* Saves the current manager settings to a save_manager section. The SaveManager must be in a saving state
	* Parameter: SaveManager& saveManager  SaveManager to use to save
	*/
	void save(SaveManager& saveManager);
	/**
	* Loads from the SaveManager instance, file must have been loaded beforehand
	* Parameter: SaveManager& saveManager  SaveManager to load from
	*/
	void load(SaveManager& saveManager);

	/**
	* Removes all shapes from the manager
	*/
	void clear();

	/**
	* Gets the first shape that touches the specified point
	* Parameter: float x  Point to find shape at
	* Parameter: float y  Point to find shape at
	* Returns: std::unique_ptr<Shape>&  Reference of unique_ptr to shape instance, or to nullptr if no shape was found
	*/
	//std::unique_ptr<Shape>& getShapeAt(float x, float y);
	Shape* getShapeAt(float x, float y);


	/**
	* Moves the shape to be rendered at the front
	* Parameter: Shape* shape  Pointer to shape to render at the front, the shape must have been added to this manager first
	*/
	void bringToFront(Shape* shape);

	/**
	* Parameter: Shape* shape  Pointer to shape to add to the manager. 
	* Note: You do not need to manage the memory of the created shape after adding!
	*/
	void add(Shape* shape);

	/**
	* Parameter: Shape* shape  Pointer to shape to remove from the manager.
	*/
	void remove(Shape* shape);


	/**
	* Returns: std::vector<std::unique_ptr<Shape>>&  Vector of unique_ptrs to shapes added to this manager.
	*/
	inline std::vector<std::unique_ptr<Shape>>& getShapes() { return shapes; }
};

