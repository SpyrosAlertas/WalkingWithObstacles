/* MIT License
*
* Copyright(c) 2016 Spyros Alertas
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* (copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


# ifndef __GRAPHICS__
# define __GRAPHICS__

# include <list>														// List

# include "gl\glut.h"													// OpenGL Utility Toolkit

// -------------------- Class: Camera --------------------
class Camera {															// Camera - Controls the view of the scene
public:
	Camera();															// Initialize Cameras values

	void moveLeft();													// Move camera left (X-Axis)
	void moveRight();													// Move camera right (X-Axis)
	void moveUp();														// Move camera up (Y-Axis)
	void moveDown();													// Move camera up (Y-Axis)
	void zoomIn();														// Move camera closer to the scene
	void zoomOut();														// Move camera away from the scene
	void lookAt(GLfloat centerX, GLfloat centerY, GLfloat centerZ);		// Set cameras point of interest, where camera looks at

	void place() const;													// Place camera in the scene

private:
	const GLfloat angleChange;											// Value of cameras movement speed
	GLfloat angleX, angleY, distance;									// Current rotation around X/Y Axis and distance from the scene
	GLfloat centerX, centerY, centerZ;									// Current center of the scene
};
// -------------------- Class: Camera --------------------

// -------------------- Class: Character --------------------
class Character {														// Abstract class - Classes Implementing this are in Graphics.cpp
public:
	virtual ~Character() = 0 {};

	virtual void display() const = 0;									// Display character
	virtual void animate() = 0;											// Animate character

	enum kindOf { HUMAN_RUNNER, BANANA_RUNNER, SKELETON_RUNNER };
};

// -------------------- !! BEGIN: ALL IMPLEMENTATIONS OF CLASS CHARACTER !! --------------------
// -------------------- Class: Runner  --------------------
class Runner : public Character {
public:
	Runner();
	~Runner();

	virtual void animate();

	void speedUp();
	void slowDown();
	void jump();
	GLfloat getSpeed() const;
	virtual int getKindOf() const = 0;

	GLfloat getHeight() const;

protected:
	GLfloat rotationH, rotationL;										// Rotation for hight & low part of hands & legs
	GLfloat height;														// Human's distance from ground (when jumping-falling)

private:
	GLfloat speed;														// Runners current moving speed
	const GLfloat speedChange, minSpeed, maxSpeed;						// Value of speed change each time user speeds up/slows down, and minimum/maximum speed
	GLfloat jumpHeight;													// How hight runner jumps, depends on the speed at that time

	int move;															// runners current move
	enum movement														// Enumeration with all possible movements the runner can perform
	{
		LEFT_ARM_FRONT, RIGHT_ARM_FRONT,
		JUMP_UP_LEFT_ARM_FRONT, JUMP_UP_RIGHT_ARM_FRONT, JUMP_DOWN_LEFT_ARM_FRONT, JUMP_DOWN_RIGHT_ARM_FRONT,
	};
};
// -------------------- Class: Runner  --------------------

// -------------------- !! END: ALL IMPLEMENTATIONS OF CLASS CHARACTER !! --------------------

// -------------------- Class: Character --------------------

// -------------------- Class: Object --------------------
class Object {															// Abstract class Object
public:
	Object(GLfloat posX = 0.0, GLfloat posY = 0.0, GLfloat posZ = 0.0) { this->posX = posX; this->posY = posY; this->posZ = posZ; }
	Object(Object& object, GLfloat posX = 0.0, GLfloat posY = 0.0, GLfloat posZ = 0.0) { this->posX = posX; this->posY = posY; this->posZ = posZ; }
	virtual ~Object() = 0 {}

	virtual void display() const = 0;									// Displays object
	virtual void animate(GLfloat speed = 0.0) = 0;						// Animates object

	virtual GLfloat getPositionX() const { return posX; }				// Get position of object (on the x-Axis to see if it's out of view)
	virtual GLfloat getPositionY() const { return posY; }
	virtual GLfloat getPositionZ() const { return posZ; }

	virtual void setPositionX(GLfloat posX) { this->posX = posX; }
	virtual void setPositionY(GLfloat posY) { this->posY = posY; }
	virtual void setPositionZ(GLfloat posZ) { this->posZ = posZ; }

private:
	GLfloat posX, posY, posZ;
};
// -------------------- Class: Object --------------------

// -------------------- Class: Text --------------------
class Text {
public:
	Text(const char* _str, int _style, GLfloat _startposx, GLfloat _startposy);

	void display() const;
	void animate();

	void setStyle(int style);
	void setColor(GLfloat _r, GLfloat _g, GLfloat _b);
	void setFade(bool fade);

	const char* getText() const;

	static void* font;

	enum Styles { NORMAL, APPEAR, APPEAR_ROTATE_Y, MOVE_RIGHT, MOVE_LEFT, BLINKING_FADE, BLINKING_APPEAR };

private:
	const char* str;
	GLfloat startposx, startposy, curposx, alpha, rotationY;
	GLfloat r, g, b;
	int style;
	bool fade;
};
// -------------------- Class: Text --------------------

// -------------------- Namespace: Graphics --------------------
namespace Graphics {
	// -------------------- Basic functions used by glut when each specific event occurs --------------------
	void Init();														// Initialize Graphics Library
	void Exit();														// Clean up Graphics Library

	void reshape(int width, int height);								// Main reshape callback function
	void display();														// Main display callback function
	void animate();														// Main idle callback function

	void keyboard(unsigned char key, int x, int y);						// Main keyboard callback function
																		// Is called each time a key press generates an ASCII character (AT THE END OF THIS FILE IS THE TABLE OF ASCII CODES)
																		// unsigned char key;	: The generated ASCII character
																		// int x; int y;		: Indicates the mouse location in window relative coordinates
	void specialKeyboard(int key, int x, int y);						// Main special keyboard callback function
																		// Is called each time keyboard function or directional keys are pressed
																		// int key;				: Is a GLUT_KEY_* constant for the special key pressed (AT THE END OF THIS FILE IS THE LIST OF THE AVAILABLE KEYS)
																		// int x; int y;		: Indicates the mouse location in window relative coordinates
																		// -------------------- Basic functions used by glut when each specific event occurs --------------------

																		// -------------------- Class: Graphics --------------------
	class Graphics {
	public:
		virtual ~Graphics() = 0 {}

		virtual void display() const = 0 {}
		virtual void animate() = 0 {}

		virtual void keyboard(unsigned char key, int x, int y) {}
		virtual void specialKeyboard(int key, int x, int y) {}
	};
	// -------------------- Class: Graphics --------------------

	// -------------------- Class: Intro --------------------
	class Intro : public Graphics {
	public:
		Intro();
		~Intro();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);

	private:
		std::list<Text> strs;
		double elapsedTime;
	};
	// -------------------- Class: Intro --------------------

	// -------------------- Class: MainMenu --------------------
	class MainMenu : public Graphics {
	public:
		MainMenu();
		~MainMenu();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);
		virtual void specialKeyboard(int key, int x, int y);

	private:
		std::list<Text> strs;
		enum Selects { NEW_GAME, CONTINUE, OPTIONS, REPLAY_INTRO, QUIT, NONE };
		double elapsedTime;
		int selected;
		static bool changed;
	};
	// -------------------- Class: MainMenu --------------------

	// -------------------- Class: NewGameMenu --------------------
	class NewGameMenu : public Graphics {
	public:
		NewGameMenu();
		~NewGameMenu();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);
		virtual void specialKeyboard(int key, int x, int y);

	private:
		std::list<Text> strs;
		enum Selects { MAIN_MENU, SELECT_CHARACTER, START_GAME, CHOOSING_CHARACTER };
		int selected;
		std::list<Runner*> characters;
	};
	// -------------------- Class: NewGameMenu --------------------

	// -------------------- Class: OptionsMenu --------------------
	class OptionsMenu : public Graphics {
	public:
		OptionsMenu();
		~OptionsMenu();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);
		virtual void specialKeyboard(int key, int x, int y);

	private:
		std::list<Text> strs;
		enum selects { SOUND, KEYBOARD, MAIN_MENU };
		int selected;
	};
	// -------------------- Class: OptionsMenu --------------------

	// -------------------- Class: SoundMenu --------------------
	class SoundMenu : public Graphics {
	public:
		SoundMenu();
		~SoundMenu();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);
		virtual void specialKeyboard(int key, int x, int y);

	private:
		enum selects { INCREASE_VOLUME, DECREASE_VOLUME, OPTIONS };
		int selected;

		std::list<Text> strs;
	};
	// -------------------- Class: SoundMenu --------------------

	// -------------------- Class: KeyboardMenu --------------------
	class KeyboardMenu : public Graphics {
	public:
		KeyboardMenu();
		~KeyboardMenu();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int x, int y);

	private:
		std::list<Text> strs;
	};
	// -------------------- Class: KeyboardMenu --------------------

	// -------------------- Class: Quit --------------------
	class Quit : public MainMenu {
	public:
		Quit();

		virtual void display() const;
		virtual void animate();

		virtual void keyboard(unsigned char key, int, int y);
		virtual void specialKeyboard(int key, int x, int y);

	private:
		std::list<Text> strs;
		enum Selects { YES, NO };
		int selected;
	};
	// -------------------- Class: Quit --------------------

	// -------------------- Class: World --------------------
	class World : public Graphics {
	public:
		World(int character);											// Create the World
		~World();														// Destroy the Wolrd

		virtual void display() const;									// Display World
		virtual void animate();											// Animate World

		virtual void keyboard(unsigned char key, int x, int y);
		virtual void specialKeyboard(int key, int x, int y);

		void continueWalk();											// When user continues his last walk - stops main menu & continues walks
		void pauseWalk();												// When user presses escape - pause walk & returns to main menu

	private:
		Camera camera;													// Worlds camera
		Runner* character;												// User's character (a runner)
		std::list<Object*> contentG;									// Worlds content - general
		std::list<Object*> contentO;									// Worlds content - obstacles
		Object *path;
		Text *text, *textB;

		bool paused;													// true: World paused, false: otherwise
		int elapsedTime;
		bool hit;
		bool noHit;
	};
	// -------------------- Class: World --------------------
}
// -------------------- Namespace: Graphics --------------------

# endif  // # ifndef __GRAPHICS__


/* Some Material Properties *
---------- VRML Defaults ----------
specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
---------- Black Rubber ----------
specref[0] = 0.02; specref[1] = 0.02; specref[2] = 0.02; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.01; specref[1] = 0.01; specref[2] = 0.01; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.4; specref[1] = 0.4; specref[2] = 0.4; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 10.0);
---------- Brass ----------
specref[0] = 0.329412; specref[1] = 0.223529; specref[2] = 0.027451; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.780392; specref[1] = 0.568627; specref[2] = 0.113725; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.992157; specref[1] = 0.941176; specref[2] = 0.807843; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 27.8974);
---------- Chrome ----------
specref[0] = 0.25; specref[1] = 0.25; specref[2] = 0.25; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.4; specref[1] = 0.4; specref[2] = 0.4; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.774597; specref[1] = 0.774597; specref[2] = 0.774597; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 76.8);
---------- Polished Silver ----------
specref[0] = 0.23125; specref[1] = 0.23125; specref[2] = 0.23125; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.2775; specref[1] = 0.2775; specref[2] = 0.2775; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.773911; specref[1] = 0.773911; specref[2] = 0.773911; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 89.6);
---------- Polished Copper ----------
specref[0] = 0.2295; specref[1] = 0.08825; specref[2] = 0.0275; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.5508; specref[1] = 0.2118; specref[2] = 0.066; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.580594; specref[1] = 0.223257; specref[2] = 0.0695701; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
---------- Polished Bronze ----------
specref[0] = 0.25; specref[1] = 0.148; specref[2] = 0.06475; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.4; specref[1] = 0.2368; specref[2] = 0.1036; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.774597; specref[1] = 0.458561; specref[2] = 0.200621; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 76.8);
---------- Pewter ----------
specref[0] = 0.105882; specref[1] = 0.058824; specref[2] = 0.113725; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.427451; specref[1] = 0.470588; specref[2] = 0.541176; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.333333; specref[1] = 0.333333; specref[2] = 0.521569; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 9.84615);
---------- Ruby ----------
specref[0] = 0.1745; specref[1] = 0.01175; specref[2] = 0.01175; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.61424; specref[1] = 0.04136; specref[2] = 0.04136; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.727811; specref[1] = 0.626959; specref[2] = 0.626959; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 76.8);
---------- Jade ----------
specref[0] = 0.135; specref[1] = 0.2225; specref[2] = 0.1575; specref[3] = 0.95;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.54; specref[1] = 0.89; specref[2] = 0.63; specref[3] = 0.95;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.316228; specref[1] = 0.316228; specref[2] = 0.316228; specref[3] = 0.95;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
---------- Pearl ----------
specref[0] = 0.25; specref[1] = 0.20725; specref[2] = 0.20725; specref[3] = 0.922;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 1.0; specref[1] = 0.829; specref[2] = 0.829; specref[3] = 0.922;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.296648; specref[1] = 0.296648; specref[2] = 0.296648; specref[3] = 0.922;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 11.264);
---------- Gold ----------
specref[0] = 0.24725; specref[1] = 0.1995; specref[2] = 0.0745; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.75164; specref[1] = 0.60648; specref[2] = 0.22648; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.628281; specref[1] = 0.555802; specref[2] = 0.366065; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
---------- Polished Gold ----------
specref[0] = 0.24725; specref[1] = 0.2245; specref[2] = 0.0645; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.34615; specref[1] = 0.3143; specref[2] = 0.0903; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.797357; specref[1] = 0.723991; specref[2] = 0.208006; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 83.2);
---------- Emerald ----------
specref[0] = 0.0215; specref[1] = 0.1745; specref[2] = 0.0215; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.07568; specref[1] = 0.61424; specref[2] = 0.07568; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.633; specref[1] = 0.727811; specref[2] = 0.633; specref[3] = 0.55;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 76.8);
---------- Silver ----------
specref[0] = 0.19225; specref[1] = 0.19225; specref[2] = 0.19225; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.50754; specref[1] = 0.50754; specref[2] = 0.50754; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.508273; specref[1] = 0.508273; specref[2] = 0.508273; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
---------- Bronze ----------
specref[0] = 0.2125; specref[1] = 0.1275; specref[2] = 0.054; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.714; specref[1] = 0.4284; specref[2] = 0.18144; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.393548; specref[1] = 0.271906; specref[2] = 0.166721; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
---------- Copper ----------
specref[0] = 0.19125; specref[1] = 0.0735; specref[2] = 0.0225; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.7038; specref[1] = 0.27048; specref[2] = 0.0828; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.256777; specref[1] = 0.137622; specref[2] = 0.086014; specref[3] = 0.82;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
---------- Obsidian ----------
specref[0] = 0.05375; specref[1] = 0.05; specref[2] = 0.06625; specref[3] = 0.82;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.18275; specref[1] = 0.17; specref[2] = 0.22525; specref[3] = 0.82;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.332741; specref[1] = 0.328634; specref[2] = 0.346435; specref[3] = 0.82;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 38.4);
---------- Glass ----------
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.588235; specref[1] = 0.670588; specref[2] = 0.729412; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.9; specref[1] = 0.9; specref[2] = 0.9; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 96.0);
---------- Black Plastic ----------
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.01; specref[1] = 0.01; specref[2] = 0.01; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.5; specref[1] = 0.5; specref[2] = 0.5; specref[3] = 1.0;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 32.0);
---------- Turquoise ----------
specref[0] = 0.1; specref[1] = 0.18725; specref[2] = 0.1745; specref[3] = 0.7;
glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
specref[0] = 0.396; specref[1] = 0.74151; specref[2] = 0.69102; specref[3] = 0.7;
glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
specref[0] = 0.297254; specref[1] = 0.30829; specref[2] = 0.306678; specref[3] = 0.7;
glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
glMaterialfv(GL_FRONT, GL_EMISSION, specref);
glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
---------- ----------
* Some Material Properties */


// Details:
// LIST OF THE AVAILABLE GLUT_KEY_* CONSTANTS ARE:
// * * * * * * * * * * * * * * * * * * * * * * * * * *	*
// < GLUT_KEY_F1 >			: F1  function key			*
// < GLUT_KEY_F2 >			: F2  function key			*
// < GLUT_KEY_F3 >			: F3  function key			*
// < GLUT_KEY_F4 >			: F4  function key			*
// < GLUT_KEY_F5 >			: F5  function key			*
// < GLUT_KEY_F6 >			: F6  function key			*
// < GLUT_KEY_F7 >			: F7  function key			*
// < GLUT_KEY_F8 >			: F8  function key			*
// < GLUT_KEY_F9 >			: F9  function key			*
// < GLUT_KEY_F10 >			: F10 function key			*
// < GLUT_KEY_F11 >			: F11 function key			*
// < GLUT_KEY_F12 >			: F12 function key			*
//														*
// < GLUT_KEY_LEFT >		: Left  directional key		*
// < GLUT_KEY_UP >			: Up    directional key		*
// < GLUT_KEY_RIGHT >		: Right directional key		*
// < GLUT_KEY_DOWN >		: Down  directional key		*
//														*
// < GLUT_KEY_PAGE_UP >		: Page up   directional key	*
// < GLUT_KEY_PAGE_DOWN >	: Page down directional key	*
// < GLUT_KEY_HOME >		: Home      directional key	*
// < GLUT_KEY_END >			: End       directional key	*
// < GLUT_KEY_INSERT >		: Insert    directional key	*
// * * * * * * * * * * * * * * * * * * * * * * * * * *	*
// Note that the escape, backspace, and delete keys are generated as an ASCII character

// Table of ASCII Codes:
//	_____________________________________________________________________________________________________________________________________
//	|	Char	Dec		Oct		Hex		|  char	Dec		Oct		Hex		|  char	Dec		Oct		Hex		|  char	Dec		Oct		Hex		|
//	|----------------------------------|------------------------------|------------------------------|-------------------------------|
//	|	(nul)	0		0000	0x00	|  (sp)	32		0040	0x20	|  @	64		0100	0x40	|  `	96		0140	0x60	|
//	|	(soh)	1		0001	0x01	|  !	33		0041	0x21	|  A	65		0101	0x41	|  a	97		0141	0x61	|
//	|	(stx)	2		0002	0x02	|  "	34		0042	0x22	|  B	66		0102	0x42	|  b	98		0142	0x62	|
//	|	(etx)	3		0003	0x03	|  #	35		0043	0x23	|  C	67		0103	0x43	|  c	99		0143	0x63	|
//	|	(eot)	4		0004	0x04	|  $	36		0044	0x24	|  D	68		0104	0x44	|  d	100		0144	0x64	|
//	|	(enq)	5		0005	0x05	|  %	37		0045	0x25	|  E	69		0105	0x45	|  e	101		0145	0x65	|
//	|	(ack)	6		0006	0x06	|  &	38		0046	0x26	|  F	70		0106	0x46	|  f	102		0146	0x66	|
//	|	(bel)	7		0007	0x07	|  '	39		0047	0x27	|  G	71		0107	0x47	|  g	103		0147	0x67	|
//	|	(bs)	8		0010	0x08	|  (	40		0050	0x28	|  H	72		0110	0x48	|  h	104		0150	0x68	|
//	|	(ht)	9		0011	0x09	|  )	41		0051	0x29	|  I	73		0111	0x49	|  i	105		0151	0x69	|
//	|	(nl)	10		0012	0x0a	|  *	42		0052	0x2a	|  J	74		0112	0x4a	|  j	106		0152	0x6a	|
//	|	(vt)	11		0013	0x0b	|  +	43		0053	0x2b	|  K	75		0113	0x4b	|  k	107		0153	0x6b	|
//	|	(np)	12		0014	0x0c	|  ,	44		0054	0x2c	|  L	76		0114	0x4c	|  l	108		0154	0x6c	|
//	|	(cr)	13		0015	0x0d	|  -	45		0055	0x2d	|  M	77		0115	0x4d	|  m	109		0155	0x6d	|
//	|	(so)	14		0016	0x0e	|  .	46		0056	0x2e	|  N	78		0116	0x4e	|  n	110		0156	0x6e	|
//	|	(si)	15		0017	0x0f	|  /	47		0057	0x2f	|  O	79		0117	0x4f	|  o	111		0157	0x6f	|
//	|	(dle)	16		0020	0x10	|  0	48		0060	0x30	|  P	80		0120	0x50	|  p	112		0160	0x70	|
//	|	(dc1)	17		0021	0x11	|  1	49		0061	0x31	|  Q	81		0121	0x51	|  q	113		0161	0x71	|
//	|	(dc2)	18		0022	0x12	|  2	50		0062	0x32	|  R	82		0122	0x52	|  r	114		0162	0x72	|
//	|	(dc3)	19		0023	0x13	|  3	51		0063	0x33	|  S	83		0123	0x53	|  s	115		0163	0x73	|
//	|	(dc4)	20		0024	0x14	|  4	52		0064	0x34	|  T	84		0124	0x54	|  t	116		0164	0x74	|
//	|	(nak)	21		0025	0x15	|  5	53		0065	0x35	|  U	85		0125	0x55	|  u	117		0165	0x75	|
//	|	(syn)	22		0026	0x16	|  6	54		0066	0x36	|  V	86		0126	0x56	|  v	118		0166	0x76	|
//	|	(etb)	23		0027	0x17	|  7	55		0067	0x37	|  W	87		0127	0x57	|  w	119		0167	0x77	|
//	|	(can)	24		0030	0x18	|  8	56		0070	0x38	|  X	88		0130	0x58	|  x	120		0170	0x78	|
//	|	(em)	25		0031	0x19	|  9	57		0071	0x39	|  Y	89		0131	0x59	|  y	121		0171	0x79	|
//	|	(sub)	26		0032	0x1a	|  :	58		0072	0x3a	|  Z	90		0132	0x5a	|  z	122		0172	0x7a	|
//	|	(esc)	27		0033	0x1b	|  ;	59		0073	0x3b	|  [	91		0133	0x5b	|  {	123		0173	0x7b	|
//	|	(fs)	28		0034	0x1c	|  <	60		0074	0x3c	|  \	92		0134	0x5c	|  |	124		0174	0x7c	|
//	|	(gs)	29		0035	0x1d	|  =	61		0075	0x3d	|  ]	93		0135	0x5d	|  }	125		0175	0x7d	|
//	|	(rs)	30		0036	0x1e	|  >	62		0076	0x3e	|  ^	94		0136	0x5e	|  ~	126		0176	0x7e	|
//	|	(us)	31		0037	0x1f	|  ?	63		0077	0x3f	|  _	95		0137	0x5f	| (del)	127		0177	0x7f	|
//	----------------------------------------------------------------------------------------------------------------------------------
