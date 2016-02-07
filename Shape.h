#pragma once

#include <vector>
#include <string>
#include "Utils.h"


class Shape {


protected:
	std::vector<Point> vertices;
	Point position;
	Colour colour;
	Colour outlineColour;
	bool outlineVisible = false;
	std::string name;
	float rotation = 0;
	float scale = 1;

public:
	/**
	* Parameter: std::string name  Name of the shape
	* Parameter: Point position  Starting position of the shape, with center origin
	*/
	Shape(std::string name, Point position);
	/**
	* Parameter: std::string name  Name of the shape
	* Parameter: Point position  Starting position of the shape, with center origin
	* Parameter: std::vector<Point> vertices  Vector of vertices to use for this shape
	*/
	Shape(std::string name, Point position, std::vector<Point> vertices);
	/**
	* Copy all properties of the input shape to the new Shape instance, effectively duplicating the input shape
	*/
	Shape(Shape* shape);
	~Shape() {}

	/**
	* Returns whether the specified point is within the shape's bounding box
	* Parameter: float x  X coordinate of the point
	* Parameter: float y  Y coordinate of the point
	* Returns: bool  True if the point is within the shape's bounding box
	*/
	bool pointInBounds(float x, float y);
	/**
	* Returns whether the specified point is within the shape's area
	* Parameter: float x  X coordinate of the point
	* Parameter: float y  Y coordinate of the point
	* Returns: bool  True if the point is within the shape's area
	*/
	virtual bool pointInShape(float x, float y);


	/**
	* Converts this shape's vertices into the target shape's vertices, current scaling and rotation are maintained
	* Parameter: Shape* shape  Shape to morph to
	*/
	void morph(Shape* shape);

	/**
	* Copies all properties of the input shape to this shape
	* Parameter: Shape* shape  Shape to mirror
	*/
	void copy(Shape* shape);

	/**
	* Rotate the shape by the provided angle, in degrees, around its centre
	* Parameter: float angle  Rotation difference in degrees
	*/
	void rotateBy(float angle);
	/**
	* Set rotation of the shape around its centre
	* Parameter: float angle  Rotation angle in degrees
	* Parameter: bool updateVerts  True if the vertices should be updated to match the new rotation, rather than just setting the rotation value
	*/
	void setRotation(float angle, bool updateVerts=true);
	/**
	* Returns: int  Current shape rotation in degrees
	*/
	inline float getRotation() { return rotation; }

	/**
	* Scale the shape by the scale factor with shape center as origin
	* Parameter: float scaleFactor  Factor to scale by
	*/
	void scaleBy(float scaleFactor);
	/**
	* Adds the value to the current scale
	* Parameter: float scaleAmount  Amount to increase scale by
	*/
	void increaseScale(float scaleAmount);
	/**
	* Set the scale of the shape with shape center as origin
	* Parameter: float scaleFactor  New shape scale factor
	*/
	void setScale(float scaleFactor);
	/**
	* Returns: float  Current shape scale factor
	*/
	inline float getScale() { return scale; }
	
	/**
	* Translate the shape along x and y axis by x and y with shape center as origin
	* Parameter: float x  translation, in pixels, along x axis
	* Parameter: float y  translation, in pixels, along y axis
	*/
	void translate(float x, float y);
	/**
	* Set the position of the shape with shape center as origin
	* Parameter: float x  X position
	* Parameter: float y  Y position
	*/
	void setPosition(float x, float y);
	/**
	* Returns: Point  Current shape position
	*/
	inline Point getPosition() { return position; }

	/**
	* Set the colour of the shape
	* Parameter: float r  Red component
	* Parameter: float g  Green component
	* Parameter: float b  Blue component
	*/
	void setColour(float r, float g, float b);
	// Note: Colour properties are copied
	void setColour(Colour& newColour);
	/**
	* Returns: Colour  Current shape colour
	*/
	inline Colour getColour() { return colour; }

	/**
	* Set the outline colour of the shape
	* Parameter: float r  Red component
	* Parameter: float g  Green component
	* Parameter: float b  Blue component
	*/
	void setOutlineColour(float r, float g, float b);
	// Note: Colour properties are copied
	void setOutlineColour(Colour& newColour);
	/**
	* Returns: Colour  Current shape outline colour
	*/
	inline Colour getOutlineColour() { return outlineColour; }

	/**
	* Sets the visibility of the shape outline
	* Parameter: bool visible  True if the outline should be visible
	*/
	inline void setOutlineVisible(bool visible) { outlineVisible = visible; }
	/**
	* Returns: bool  True if the shape outline is currently visible
	*/
	inline bool isOutlineVisible() { return outlineVisible;  }

	inline std::string getName() { return name; }

	/**
	* Returns: std::vector<Point>&  Vector of shape vertices (as Points)
	*/
	inline std::vector<Point>& getVertices() { return vertices; }

protected:
	/**
	* Called on initialisation to populate the vertices vector. Should be overridden in child classes.
	*/
	virtual void create() {};

	
	/**
	* Updates vertex coordinates to the new scale
	*/
	void updateScaling(float newScale);

	/**
	* Updates vertex coordinates to the new rotation
	* Parameter: float newRotation  New shape rotation in degrees
	*/
	void updateRotation(float newRotation);
};