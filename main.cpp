#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "glut.h"
#include "Utils.h"
#include "ShapeManager.h"
#include "Pentagon.h"
#include "Triangle.h"
#include "Square.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "SaveManager.h"

// Verbose to avoid potentially conflicting namespaces
using std::cout;			using std::endl;
using std::vector;			using std::unique_ptr;
using std::map;				using std::string;


struct SceneSettings {
	float zoom = 1; // View zoom modifier
	int panX, panY = 0; // View pan location
};

const int CAMERA_WIDTH = 1000;
const int CAMERA_HEIGHT = CAMERA_WIDTH / (16.0 / 9); // Optimise for 16:9 resolutions
const int DEFAULT_RADIUS = 25; // Used when adding and morphing shapes

// Actions used for key and mouse mappings
enum Action {
	A_ADD, A_DUPLICATE, A_DELETE, A_ZOOM, A_PAN, A_TRANSLATE, A_SCALE, A_ROTATE, 
	A_COLOUR_RED, A_COLOUR_GREEN, A_COLOUR_BLUE, A_MORPH_UP, A_MORPH_DOWN, A_MODIFIER, A_CLEAR
};
map<Action, char> keyMappings;
map<Action, int> mouseMappings;

SceneSettings sceneSettings;
SaveManager saveManager;
ShapeManager shapeManager;
Mouse mouse;
Keyboard keyboard;
Shape* selectedShape;
Shape* lastSelectedShape;
// Vector of different shape types used for cycling through different shapes
vector<unique_ptr<Shape>> shapeTypes;

void display();
void reshape(int w, int h);
void idle();
void init();
void mouseFunc(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void keyboardUpFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void specialUpFunc(int key, int x, int y);
void save();
void load();


int main(int argc, char* argv[]) {
	// Init glut
	glutInit(&argc, argv);

	// Center window
	glutInitWindowPosition(1280/2, 720/2);
	// Set window size (16:9 ratio)
	glutInitWindowSize(1280, 720);

	// Set display mode: RGBA, Double buffered, depth buffer
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// Enable depth testing 
	glEnable(GL_DEPTH_TEST);

	// Create window
	glutCreateWindow("2D Graphics App - Andrew Palmer 14416354");

	// Register glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutSpecialFunc(specialFunc);
	glutSpecialUpFunc(specialUpFunc);

	// Init program specific settings
	init();

	// Save on exit
	atexit(save);

	// Enter glut main loop
	glutMainLoop();
	
	return 0;
}

/**
* Init program specific settings
*/
void init() {
	// Load save
	load();

	// Key mappings
	keyMappings[Action::A_MODIFIER] = ' '; // Held to switch from shape to view manipulation
	keyMappings[Action::A_ADD] = 'a';
	keyMappings[Action::A_DUPLICATE] = 'd';
	keyMappings[Action::A_DELETE] = 'x';
	keyMappings[Action::A_MORPH_UP] = 'w';
	keyMappings[Action::A_MORPH_DOWN] = 's';
	keyMappings[Action::A_COLOUR_RED] = 'r';
	keyMappings[Action::A_COLOUR_GREEN] = 'g';
	keyMappings[Action::A_COLOUR_BLUE] = 'b';
	keyMappings[Action::A_CLEAR] = '\b';

	mouseMappings[Action::A_PAN] = GLUT_RIGHT_BUTTON;
	mouseMappings[Action::A_TRANSLATE] = GLUT_RIGHT_BUTTON;
	mouseMappings[Action::A_ROTATE] = GLUT_LEFT_BUTTON;
	mouseMappings[Action::A_SCALE] = GLUT_MIDDLE_BUTTON;
	mouseMappings[Action::A_ZOOM] = GLUT_MIDDLE_BUTTON;

	// Create shape types, from triangle to decagon, for cycling through when the up and down arrow keys are pressed
	unique_ptr<Shape> shape;
	// Examples of RegularPolygon child classes
	shape.reset(new Triangle(DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new Square(DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new Pentagon(DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	// Examples of instantiating a RegularPolygon directly
	shape.reset(new RegularPolygon("Hexagon", 6, DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new RegularPolygon("Heptagon", 7, DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new RegularPolygon("Octagon", 8, DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new RegularPolygon("Nonagon", 9, DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));

	shape.reset(new RegularPolygon("Decagon", 10, DEFAULT_RADIUS, Point(0, 0)));
	shapeTypes.push_back(std::move(shape));
}


/**
* GLUT display callback. Triggered depending on the window redisplay state
*/
void display() {
	// Set the background colour to white
	glClearColor(1, 1, 1, 1);
	// Clear the colour and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Switch to model matrix for drawing
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		// Scale view by zoom amount
		glScalef(sceneSettings.zoom, sceneSettings.zoom, 1);
		// Translate the view by the pan amount
		glTranslatef(sceneSettings.panX, sceneSettings.panY, 0);
		// Update and render Shapes
		shapeManager.update();
		// Save translated model matrix for mouse mapping before resetting 
		mouse.updateModelMatrix();
	glPopMatrix();

	// Draw UI
	glPushMatrix();
		// Render text in top left of the screen
		int lineHeight = 15;
		float x = -CAMERA_WIDTH/2;
		float y = -CAMERA_HEIGHT / 2 + lineHeight;
		// Calculate zoom percentage
		int zoom = ((CAMERA_WIDTH * sceneSettings.zoom) / CAMERA_WIDTH) * 100;
		// Draw zoom and pan text
		string strZoom = "Zoom: " + std::to_string(zoom) + "%";
		string strPan = "Pan: " + std::to_string(sceneSettings.panX) + ", " + std::to_string(sceneSettings.panY);
		Utils::renderString(x, y, strZoom);
		Utils::renderString(x, y += lineHeight, strPan);
		// Draw selected shape settings
		if (selectedShape) {
			Utils::renderString(x, y += lineHeight * 2, "Name: " + selectedShape->getName());
			Utils::renderString(x, y += lineHeight, "Scale: " + std::to_string(selectedShape->getScale()));
			Utils::renderString(x, y += lineHeight, "Rotation: " + std::to_string(selectedShape->getRotation()));
			Utils::renderString(x, y += lineHeight, "Position: " + std::to_string(selectedShape->getPosition().x) + ", " + std::to_string(selectedShape->getPosition().y));
		}
	glPopMatrix();

	// Swap buffers 
	// (move contents of the back buffer to front buffer and clear back buffer)
	glutSwapBuffers();
}

/**
* GLUT reshape callback, called when window is reshaped
* Parameter: int w  New window width
* Parameter: int h  New window height
*/
void reshape(int w, int h) {
	// Window height cannot be 0, also prevents dividing by 0 when calculating the ratio
	if (h == 0) h = 1;
	float ratio = 1.0 * w/h;

	// Switch to projection matrix for camera manipulation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Fit viewport to window
	glViewport(0, 0, w, h);
	// Orthographic camera for 2D with 0,0 at center of screen so zooming is done from the center of the view
	glOrtho(-CAMERA_WIDTH / 2, CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2, -CAMERA_HEIGHT / 2, 0, 2);
	// Save updated projection matrix for mouse mapping
	mouse.updateProjectionMatrix();

	// Switch back to model view matrix (default)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
* GLUT idle callback continuously called when no window events are being processed
*/
void idle() {
	// Set the window redisplay state so display will be called
	glutPostRedisplay();
}

void save() {
	saveManager.startSave("save.txt");
	saveManager.startSection("scene");
	saveManager.addValue("zoom", sceneSettings.zoom);
	saveManager.addValue("pan_x", sceneSettings.panX);
	saveManager.addValue("pan_y", sceneSettings.panY);
	saveManager.endSection();

	shapeManager.save(saveManager);

	saveManager.stopSave();
}

void load() {
	if (saveManager.load("save.txt")) {
		//saveManager.prettyPrint();
		map<string, string>& vals = saveManager.getSectionValues("scene");
		sceneSettings.zoom = std::stof(vals["zoom"]);
		sceneSettings.panX = std::stof(vals["pan_x"]);
		sceneSettings.panY = std::stof(vals["pan_y"]);

		shapeManager.load(saveManager);
	}
}

void mouseFunc(int button, int state, int x, int y) {
	// Delegate to Mouse instance
	mouse.onClick(button, state, x, y);
	// When releasing a button, selected shape should be set to null, otherwise get the shape under the mouse
	lastSelectedShape = selectedShape;
	selectedShape = (state == GLUT_UP)? nullptr : shapeManager.getShapeAt(mouse.getPosition().x, mouse.getPosition().y);
	if (selectedShape) {
		// Bring the selected shape to the front and show it's outline
		shapeManager.bringToFront(selectedShape);
		selectedShape->setOutlineVisible(true);
	} else if (lastSelectedShape) {
		// Hide the outline of the last shape
		lastSelectedShape->setOutlineVisible(false);
	}
}
void mouseMotion(int x, int y) {
	// Delegate to Mouse instance
	mouse.onMove(x, y);

	// Using mouse screen position for input, rather than object position provides, a much smoother input experience 
	// and avoids potentially large floating point number arithmetic
	if (selectedShape) {
		if (mouse.isButtonPressed(mouseMappings[Action::A_TRANSLATE]) 
			&& !keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
			// Use mouse object position for placing shapes
			selectedShape->setPosition(mouse.getPosition().x, mouse.getPosition().y);
		}
		else if (mouse.isButtonPressed(mouseMappings[Action::A_ROTATE])
				 && !keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
			selectedShape->rotateBy(mouse.getScreenPosition().x - mouse.getPrevScreenPosition().x);
		}
		else if (mouse.isButtonPressed(mouseMappings[Action::A_SCALE])
				 && !keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
			selectedShape->increaseScale((mouse.getScreenPosition().x - mouse.getPrevScreenPosition().x) * 0.01);
		}
	}
	if (mouse.isButtonPressed(mouseMappings[Action::A_PAN])
			 && keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
		// Pan when the pan button and modifier key are pressed
		sceneSettings.panX += mouse.getScreenPosition().x - mouse.getPrevScreenPosition().x;
		sceneSettings.panY += mouse.getScreenPosition().y - mouse.getPrevScreenPosition().y;
	}
	else if (mouse.isButtonPressed(mouseMappings[Action::A_ZOOM])
		&& keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
		// Zoom when the zoom button and modifier key are pressed
		sceneSettings.zoom += (mouse.getPrevScreenPosition().y - mouse.getScreenPosition().y) * 0.002;
		// Clamp max zoom at 1000% and min zoom at 1%
		if (sceneSettings.zoom < 0.01) sceneSettings.zoom = 0.01;
		else if (sceneSettings.zoom > 10) sceneSettings.zoom = 10;
	}
}

void keyboardFunc(unsigned char key, int x, int y) {
	keyboard.keyboardFunc(key, x, y);
	// Since key presses get repeated when held, the mouse position shouldn't be updated when the modifier key is pressed
	// as it causes the position delta fluctuate too much making view manipulation less smooth
	if (key != keyMappings[Action::A_MODIFIER]) mouse.onMove(x, y);

	if (keyboard.isKeyDown(keyMappings[Action::A_ADD])) {
		// Add a pentagon at the mouse location
		std::cout << "Adding Shape" << std::endl;
		shapeManager.add(new Pentagon(DEFAULT_RADIUS, Point(mouse.getPosition().x, mouse.getPosition().y)));
	}
	else if (keyboard.isKeyDown(keyMappings[Action::A_CLEAR])) {
		// Clear shapes
		shapeManager.clear();
		selectedShape = nullptr;
	}
	else if (keyboard.isKeyDown(keyMappings[Action::A_DUPLICATE]) && selectedShape) {
		// Duplicate shape
		Shape* s = new RegularPolygon(selectedShape);
		// Randomly offset (between -30 and 30) slightly to visualise the duplication
		int randRange = 30 - -30 + 1;
		s->translate(rand() % randRange + -30, rand() % randRange + -30);
		shapeManager.add(s);
	}
	else if (keyboard.isKeyDown(keyMappings[Action::A_DELETE]) && selectedShape) {
		shapeManager.remove(selectedShape);
		selectedShape = nullptr;
	}
	// Colour change keys
	else if (keyboard.isKeyDown(keyMappings[Action::A_COLOUR_RED]) && selectedShape) {
		// Red
		selectedShape->setColour(0.9, 0.12, 0.25);
	}
	else if (keyboard.isKeyDown(keyMappings[Action::A_COLOUR_GREEN]) && selectedShape) {
		// Green
		selectedShape->setColour(0.64, 0.91, 0.12);
	}
	else if (keyboard.isKeyDown(keyMappings[Action::A_COLOUR_BLUE]) && selectedShape) {
		// Blue
		selectedShape->setColour(0.12, 0.64, 0.9);
	}
	// Cycle through shapes when w or s key is pressed
	else if (selectedShape && (keyboard.isKeyDown(keyMappings[Action::A_MORPH_UP]) || keyboard.isKeyDown(keyMappings[Action::A_MORPH_DOWN]))
		     && !keyboard.isKeyDown(keyMappings[Action::A_MODIFIER])) {
		// Whether the shape type should be cycled in reverse
		bool reverse = false;
		if (keyboard.isKeyDown(keyMappings[Action::A_MORPH_DOWN])) reverse = true;

		// Iterate over map of shape types
		for (vector<unique_ptr<Shape>>::iterator it = shapeTypes.begin(); it != shapeTypes.end(); it++) {
			// If the selected shape is the same as the current shape type
			if (selectedShape->getName() == (*it)->getName()) {
				if (reverse) {
					// If cycling in reverse, and the iterator is at the beginning of the map, wrap around to the end
					if (it == shapeTypes.begin()) it = shapeTypes.end();
					// Minus one from the iterator to get the shapeType in front of the current shape in the map
					it--;
				}
				// If the next iteration is the end of the map, wrap around to the beginning of the map
				else if (++it == shapeTypes.end()) it = shapeTypes.begin();
				// Morph the shape to the next shape type
				selectedShape->morph((*it).get());
				break;
			}
		}
	}
}
void keyboardUpFunc(unsigned char key, int x, int y) {
	keyboard.keyboardUpFunc(key, x, y);
	mouse.onMove(x, y);
}

void specialFunc(int key, int x, int y) {
	keyboard.specialFunc(key, x, y);
	mouse.onMove(x, y);
	
}
void specialUpFunc(int key, int x, int y) {
	keyboard.specialUpFunc(key, x, y);
	mouse.onMove(x, y);
}