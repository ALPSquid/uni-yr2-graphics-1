#include "stdafx.h"
#include "Keyboard.h"
#include <iostream>


Keyboard::Keyboard() {
}


Keyboard::~Keyboard() {
}

void Keyboard::keyboardFunc(unsigned char key, int x, int y) {
	keyStates[key] = true;
}
void Keyboard::keyboardUpFunc(unsigned char key, int x, int y) {
	keyStates[key] = false;
}
void Keyboard::specialFunc(int key, int x, int y) {
	specialStates[key] = true;
}
void Keyboard::specialUpFunc(int key, int x, int y) {
	specialStates[key] = false;
}
