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


# include "utilities.h"
# include "graphics.h"

# include "gl\glut.h"

// -------------------- Namespace: Application --------------------
namespace Application {
	void Init(int* argc, char** argv) {									// Initialize Application
		glutInit(argc, argv);											// Initialize the glut library
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);		// Set the display mode
		glutCreateWindow("Graphics 1 - Animation Project 2012-2013");	// Create window with title Graphics 1 Project 2012-2013...
		glutFullScreen();												// Enter full screen mode
		glutSetCursor(GLUT_CURSOR_NONE);								// Hide cursor (is totally useless within the application)
		Utilities::Init();												// Initialize Utilities Library (has to be initialized first because others depend on this one)
		Graphics::Init();												// Initialize Graphics Library
		glutReshapeFunc(Graphics::reshape);								// Is called when window is resized
		glutDisplayFunc(Graphics::display);								// Is called when window content needs to be redisplayed
		glutIdleFunc(Graphics::animate);								// Set as our idle function - makes the world move
		glutKeyboardFunc(Graphics::keyboard);							// Is called each time a key press generates an ASCII character
		glutSpecialFunc(Graphics::specialKeyboard);						// Is called each time keyboard function or directional keys are pressed
	}

	void Exit() {														// Clean up Application
		Graphics::Exit();												// Clean up Graphics Library
		Utilities::Exit();												// Clean up Utilities Library (has to be cleaned last)
		exit(1);
	}

	void Start() {														// Enter the event processing loop
		glutMainLoop();													// Enter the GLUT Event processing loop
	}
};
// -------------------- Namespace: Application --------------------