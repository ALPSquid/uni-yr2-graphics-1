#include "stdafx.h"
#include "Mouse.h"
#include "glut.h"
#include <gl\GL.h>
#include <iostream>


Mouse::Mouse() {
	// Default button states
	buttonStates[GLUT_LEFT_BUTTON] = GLUT_UP;
	buttonStates[GLUT_RIGHT_BUTTON] = GLUT_UP;
	buttonStates[GLUT_MIDDLE_BUTTON] = GLUT_UP;
}

Point Mouse::screenToObject(int x, int y) {
	// Using cached model and projection matrix
	GLint viewportMatrix[4];
	GLfloat winX, winY;
	GLdouble objectX, objectY, objectZ;

	// Get viewport matrix
	glGetIntegerv(GL_VIEWPORT, viewportMatrix);

	winX = x;
	winY = viewportMatrix[3] - y; // invert y so 0,0 is top left
	// Map screen coordinates to object coordinates
	gluUnProject(winX, winY, 0, modelViewMatrix, projectionMatrix, viewportMatrix, &objectX, &objectY, &objectZ);
	return Point(objectX, objectY);
}
void Mouse::updateModelMatrix() {
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
}
void Mouse::updateProjectionMatrix() {
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
}

void Mouse::onClick(int button, int state, int x, int y) {
	buttonStates[button] = state;
	onMove(x, y);
}

void Mouse::onMove(int x, int y) {
	prevScreenPosition = screenPosition;
	screenPosition.x = x;
	screenPosition.y = y;
	// Update object coordinates
	Point objectCoords = screenToObject(x, y);
	prevPosition = position;
	position.x = objectCoords.x;
	position.y = objectCoords.y;
}

bool Mouse::isButtonPressed(int button) {
	return buttonStates[button] == GLUT_DOWN;
}

bool Mouse::leftButtonPressed() {
	return buttonStates[GLUT_LEFT_BUTTON] == GLUT_DOWN;
}

bool Mouse::rightButtonPressed() {
	return buttonStates[GLUT_RIGHT_BUTTON] == GLUT_DOWN;
}

bool Mouse::middleButtonPressed() {
	return buttonStates[GLUT_MIDDLE_BUTTON] == GLUT_DOWN;
}
