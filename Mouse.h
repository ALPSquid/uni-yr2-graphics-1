#pragma once

#include "Utils.h"
#include <unordered_map>

/**
* Stores mouse information such as position and button states. 
* Callbacks should be passed window coordinates which will be converted to object coordinates 
*/
class Mouse {

protected:
	// Last recorded object position before the current position was updated
	Point prevPosition;
	// Position in object coordinates
	Point position;
	// Last recorded screen space position before the current position was updated
	Point prevScreenPosition;
	// Position in screen coordinates
	Point screenPosition;
	std::unordered_map<int, int> buttonStates;

	// Cached matrices
	GLdouble modelViewMatrix[16];
	GLdouble projectionMatrix[16];

public:
	Mouse();

	/**
	* Maps a screen point (mouse position) to object coordinates
	* Parameter: int x  Screen x coordinate
	* Parameter: int y  Screen y coordinate
	* Returns: Point in object coordinates
	*/
	Point screenToObject(int x, int y);

	/**
	* Updates the cached model matrix used for screen to object mapping to the current model matrix.
	*/
	void updateModelMatrix();
	/**
	* Updates the cached projection matrix used for screen to object mapping to the current projection matrix.
	*/
	void updateProjectionMatrix();

	/**
	* Glut mouse callback
	* Parameter: int button  Integer corresponding to the button changed
	* Parameter: int state  Integer corresponding to the state of the button
	* Parameter: int x  Mouse x position in window coordinates 
	* Parameter: int y  Mouse y position in window coordinates
	*/
	void onClick(int button, int state, int x, int y);
	/**
	* Glut mouse motion callback
	* Parameter: int x  Mouse x position in window coordinates
	* Parameter: int y  Mouse y position in window coordinates
	*/
	void onMove(int x, int y);

	/**
	* Parameter: int button  Button to query
	* Returns: bool  True if the button is currently pressed
	*/
	bool isButtonPressed(int button);
	/**
	* Returns: bool  True if the left mouse button is currently pressed
	*/
	bool leftButtonPressed();
	/**
	* Returns: bool  True if the right mouse button is currently pressed
	*/
	bool rightButtonPressed();
	/**
	* Returns: bool  True if the middle mouse button is currently pressed
	*/
	bool middleButtonPressed();


	/**
	* Returns: Point  Mouse position in object coordinates
	*/
	inline Point getPosition() { return position; }
	/**
	* Returns: Point  Mouse position in screen coordinates
	*/
	inline Point getScreenPosition() { return screenPosition; }

	/**
	* Returns: Point  Last object position of the mouse before the current position was updated
	*/
	inline Point getPrevPosition() { return prevPosition; }
	/**
	* Returns: Point  Last screen position of the mouse before the current position was updated
	*/
	inline Point getPrevScreenPosition() { return prevScreenPosition; }

};

