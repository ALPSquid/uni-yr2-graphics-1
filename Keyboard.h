#pragma once

#include <unordered_map>

class Keyboard {

protected:
	std::unordered_map<unsigned char, bool> keyStates;
	std::unordered_map<int, bool> specialStates;

public:
	Keyboard();
	~Keyboard();

	/**
	* Glut keyboard call back
	* Parameter: unsigned char key  Key that was pressed 
	* Parameter: int x  Mouse X position when the key pressed
	* Parameter: int y  Mouse Y position when the key pressed
	*/
	void keyboardFunc(unsigned char key, int x, int y);
	/**
	* Glut keyboard up call back
	* Parameter: unsigned char key  Key that was pressed
	* Parameter: int x  Mouse X position when the key released
	* Parameter: int y  Mouse Y position when the key released
	*/
	void keyboardUpFunc(unsigned char key, int x, int y);
	/**
	* Glut special key call back
	* Parameter: int key  Key (GLUT_KEY_X) that was pressed
	* Parameter: int x  Mouse X position when the key pressed
	* Parameter: int y  Mouse Y position when the key pressed
	*/
	void specialFunc(int key, int x, int y);
	/**
	* Glut special key up call back
	* Parameter: int key  Key (GLUT_KEY_X) that was released
	* Parameter: int x  Mouse X position when the key released
	* Parameter: int y  Mouse Y position when the key released
	*/
	void specialUpFunc(int key, int x, int y);

	/**
	* Parameter: unsigned char key  Key to query
	* Returns: bool  True if the key is currently pressed down
	*/
	inline bool isKeyDown(unsigned char key) { return keyStates[key]; }
	/**
	* Parameter: int key  GLUT_KEY_X key to query
	* Returns: bool  True if the key is currently pressed down
	*/
	inline bool isSpecialDown(int key) { return specialStates[key]; }
};

