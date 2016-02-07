#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "glut.h"

/** 
* Header only static utilities class
*/
class Utils {

public:
	/**
	* Draws a string using glut
	* Parameter: std::string str  String to draw
	*/
	static inline void renderString(float x, float y, std::string str) {
		glColor3f(0, 0, 0);
		glRasterPos2f(x, y);
		for (unsigned i = 0; i < str.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}
	}

	/**
	* Based on http://stackoverflow.com/a/236803
	* Parameter: const std::string& str  String to split
	* Parameter: char delimiter  Delimiter to split at
	* Parameter: std::vector<std::string>& out  Vector of strings to put split strings in
	*/
	static inline void splitString(const std::string& str, char delimiter, std::vector<std::string>& out) {
		std::stringstream stream(str);
		std::string item;
		// Iterate through each part of the string, split at the passed delimiter
		while (std::getline(stream, item, delimiter))
			out.push_back(item);
	}
	static inline std::vector<std::string> splitString(const std::string& str, char delimiter) {
		std::vector<std::string> out;
		splitString(str, delimiter, out);
		return out;
	}

	/**
	* Removes the specified character from the string
	* Parameter: std::string& str  String to edit
	* Parameter: char c  Character to remove
	*/
	static inline void removeFromString(std::string& str, char c) {
		str.erase(std::remove(str.begin(), str.end(), c), str.end());
	}
	/**
	* Removes the specified characters from the string
	* Parameter: std::string& str String to edit
	* Parameter: char c[]  Array of characters to remove, these are processed individually
	*/
	static inline void removeFromString(std::string& str, char c[]) {
		int size = sizeof(c) / sizeof(char);
		for (int i = 0; i < size; i++) removeFromString(str, c[i]);
	}

	/**
	* From http://stackoverflow.com/a/23790392
	* Moves the item at itemIndex to the back of the vector. This does not trigger reallocation
	* Parameter: std::vector<T>& v  Vector reference to manipulate
	* Parameter: size_t itemIndex  Index of the item in vector to move to the back
	*/
	template <typename T>
	static void moveToBack(std::vector<T>& v, size_t itemIndex) {
		auto it = v.begin() + itemIndex;
		std::rotate(it, it + 1, v.end());
	}
};


/**
* Basic struct representing a 2D coordinate
*/
struct Point {
	float x = 0;
	float y = 0;

	Point(float x = 0, float y = 0) {
		this->x = x;
		this->y = y; 
	}

	// Creates a point object from a Point string output in the format: (x, y)
	Point(std::string pointOutput) {
		char chars[] = {'(', ')', ' '};
		Utils::removeFromString(pointOutput, chars);
		std::vector<std::string> split = Utils::splitString(pointOutput, ',');
		x = std::stof(split[0]);
		y = std::stof(split[1]);
	}

	friend std::ostream& operator<<(std::ostream& out, const Point& point) {
		out << "(" << point.x << ", " << point.y << ")";
		return out;
	}

	Point& operator+=(const Point& a) {
		x += a.x;
		y += a.y;
		return *this;
	}
	
	friend Point operator+(Point a, const Point& b) {
		return a += b;
	}


	Point& operator*=(const Point& a) {
		x *= a.x;
		y *= a.y;
		return *this;
	}
	Point& operator*=(const float& a) {
		x *= a;
		y *= a;
		return *this;
	}

	friend Point operator*(Point a, const Point& b) {
		return a *= b;
	}
	friend Point operator*(Point a, const float& b) {
		return a *= b;
	}
};

/** 
* Basic struct representing a colour with a red, green and blue component between 0 and 1
*/
struct Colour{
	float r;
	float g;
	float b;

	Colour(float r = 0, float g = 0, float b = 0) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	// Creates a colour object from a Colour string output in the format: (r, g, b)
	Colour(std::string colourOutput) {
		char chars[] = {'(', ')', ' '};
		Utils::removeFromString(colourOutput, chars);
		std::vector<std::string>& split = Utils::splitString(colourOutput, ',');
		r = std::stof(split[0]);
		g = std::stof(split[1]);
		b = std::stof(split[2]);
	}

	friend std::ostream& operator<<(std::ostream& out, const Colour& colour) {
		out << "(" << colour.r << ", " << colour.g << ", " << colour.b << ")";
		return out;
	}
};
