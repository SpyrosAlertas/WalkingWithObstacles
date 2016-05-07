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


# include <ctime>
# include <sstream>

# include "graphics.h"
# include "utilities.h"
# include "application.h"

// -------------------- Static content - to be used only in this file -------------------

using namespace Graphics;

static GLfloat zFar = 5000.0;

static GLUquadric* quadric;
static class Graphics* graphics;
static class World* lastRace = NULL;

static GLfloat specref[4];

// -------------------- Static content - to be used only in this file -------------------

// -------------------- Class: Camera --------------------
Camera::Camera()
	: angleChange(2.5), angleX(45.0), angleY(0.0), distance(-200.0), centerX(0.0), centerY(0.0), centerZ(0.0) {
}

void Camera::moveLeft() {												// Move camera left (Y-Axis)
	angleY += angleChange;
	if (angleY >= 360.0)												// Camera fully rotated around Y Axis
		angleY = 0.0;
}

void Camera::moveRight() {												// Move camera right (Y-Axis)
	angleY -= angleChange;
	if (angleY <= 0.0)													// Camera fully rotated around Y Axis
		angleY = 360.0;
}

void Camera::moveUp() {													// Move camera up (X-Axis)
	angleX += angleChange;
	if (angleX > 179.0)													// Camera fully rotated around X Axis
		angleX = 179.0;
}

void Camera::moveDown() {												// Move camera down (X-Axis)
	angleX -= angleChange;
	if (angleX < 1.0)													// Camera fully rotated around X Axis
		angleX = 1.0;
}

void Camera::zoomIn() {													// Move camera closer to the scene
	distance += angleChange*3.0;
	if (distance > -200.0)												// Camera cannot go closer
		distance = -200.0;
}

void Camera::zoomOut() {												// Move camera away from the scene
	distance -= angleChange*3.0;
	if (distance < -1000)												// Camera cannot go any further
		distance = -1000;
}

void Camera::lookAt(GLfloat centerX, GLfloat centerY, GLfloat centerZ) {// Set cameras point of interest, where camera looks at
	this->centerX = centerX;
	this->centerY = centerY;
	this->centerZ = centerZ;
}

void Camera::place() const {											// Place camera in the scene
	glTranslatef(0.0, 0.0, distance);									// Push back scene as needed
	glRotatef(angleX, 1.0, 0.0, 0.0);									// Rotate scene around the X axis
	glRotatef(angleY, 0.0, 1.0, 0.0);									// Rotate scene around the Y axis
	glTranslatef(centerX, centerY, centerZ);
}
// -------------------- Class: Camera --------------------

// -------------------- !! BEGIN: ALL IMPLEMENTATIONS OF CLASS CHARACTER !! --------------------

// -------------------- Class: Runner --------------------
Runner::Runner() : speed(15.0), speedChange(2.0), minSpeed(8.0), maxSpeed(25.0), move(RIGHT_ARM_FRONT),
height(0.0), rotationH(0.0), rotationL(0.0) {
	Audio::Load("step");
	Audio::SetVolume("step", 0.6);
	Audio::Load("fall");
}

Runner::~Runner() {
	Audio::Unload("step");
	Audio::Unload("fall");
}

void Runner::animate() {												// Animate Runner
	static double speedFactorR = 35.0, speedFactorJ = speedFactorR*4.0;
	if (move == RIGHT_ARM_FRONT) {
		rotationH += speed * Timer::GetElapsedTime() / speedFactorR;
		if (rotationL > 0.0)
			rotationL -= speed * Timer::GetElapsedTime() / speedFactorR;
		if (rotationH >= 45.0 + speed) {
			move = LEFT_ARM_FRONT;
			Audio::Play("step");
		}
	}
	else if (move == LEFT_ARM_FRONT) {
		rotationH -= speed * Timer::GetElapsedTime() / speedFactorR;
		if (rotationL < 45.0)
			rotationL += speed * Timer::GetElapsedTime() / speedFactorR;
		if (rotationH <= -45.0 - speed) {
			move = RIGHT_ARM_FRONT;
			Audio::Play("step");
		}
	}
	else if (move == JUMP_UP_RIGHT_ARM_FRONT) {
		height += speed * Timer::GetElapsedTime() / (speedFactorJ);
		rotationH += speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationL > 0.0)
			rotationL -= speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationH >= 45.0 + speed)
			move = JUMP_UP_LEFT_ARM_FRONT;
		if (height >= jumpHeight)
			if (move == JUMP_UP_LEFT_ARM_FRONT)
				move = JUMP_DOWN_LEFT_ARM_FRONT;
			else
				move = JUMP_DOWN_RIGHT_ARM_FRONT;
	}
	else if (move == JUMP_UP_LEFT_ARM_FRONT) {
		height += speed * Timer::GetElapsedTime() / (speedFactorJ);
		rotationH -= speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationL < 45.0)
			rotationL += speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationH <= -45.0 - speed)
			move = JUMP_UP_RIGHT_ARM_FRONT;
		if (height >= jumpHeight)
			if (move == JUMP_UP_LEFT_ARM_FRONT)
				move = JUMP_DOWN_LEFT_ARM_FRONT;
			else
				move = JUMP_DOWN_RIGHT_ARM_FRONT;
	}
	else if (move == JUMP_DOWN_RIGHT_ARM_FRONT) {
		height -= speed * Timer::GetElapsedTime() / (speedFactorJ);
		rotationH += speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationL > 0.0)
			rotationL -= speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationH >= 45.0 + speed)
			move = JUMP_DOWN_LEFT_ARM_FRONT;
		if (height < 0.0) {
			if (move == JUMP_DOWN_LEFT_ARM_FRONT)
				move = LEFT_ARM_FRONT;
			else
				move = RIGHT_ARM_FRONT;
			height = 0.0;
			Audio::Play("fall");
		}
	}
	else if (move == JUMP_DOWN_LEFT_ARM_FRONT) {
		height -= speed * Timer::GetElapsedTime() / (speedFactorJ);
		rotationH -= speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationL < 45.0)
			rotationL += speed * Timer::GetElapsedTime() / speedFactorJ;
		if (rotationH <= -45.0 - speed)
			move = JUMP_DOWN_RIGHT_ARM_FRONT;
		if (height < 0.0) {
			if (move == JUMP_DOWN_LEFT_ARM_FRONT)
				move = LEFT_ARM_FRONT;
			else
				move = RIGHT_ARM_FRONT;
			height = 0.0;
			Audio::Play("fall");
		}
	}
}

void Runner::speedUp() {												// Increase runners speed
	if (move == LEFT_ARM_FRONT || move == RIGHT_ARM_FRONT) {
		speed += speedChange;
		if (speed > maxSpeed)
			speed = maxSpeed;
	}
}

void Runner::slowDown() {												// Decrease runners speed
	if (move == LEFT_ARM_FRONT || move == RIGHT_ARM_FRONT) {
		speed -= speedChange;
		if (speed < minSpeed)
			speed = minSpeed;
	}
}

void Runner::jump() {													// Makes runner jump
	if (move == LEFT_ARM_FRONT) {
		move = JUMP_UP_LEFT_ARM_FRONT;
		jumpHeight = 5.0*speed;
	}
	else if (move == RIGHT_ARM_FRONT) {
		jumpHeight = 5.0*speed;
		move = JUMP_UP_RIGHT_ARM_FRONT;
	}
	if (jumpHeight > 80.0)
		jumpHeight = 80.0;
}

GLfloat Runner::getSpeed() const {
	return speed;
}

GLfloat Runner::getHeight() const {
	return height;
}
// -------------------- Class: Runner --------------------

// -------------------- !! BEGIN: ALL IMPLEMENTATIONS OF CLASS RUNNER !! --------------------

// -------------------- Class: HumanRunner --------------------
class HumanRunner : public Runner {
public:
	HumanRunner() {
		Texture::Load("skin");
		Texture::Load("black");
	}

	~HumanRunner() {
		Texture::Unload("skin");
		Texture::Unload("black");
	}

	virtual void display() const {										// Display Human
		GLint detail = 30;
		Texture::Use("skin");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		glPushMatrix();
		glTranslatef(0.0, height + 37.0, -50.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// Display Body
		glPushMatrix();
		glScalef(1.0, 1.2, 1.0);
		gluSphere(quadric, 10.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, -10.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutSolidTorus(2.0, 4.0, detail, detail);
		gluCylinder(quadric, 4.0, 8.0, 6.0, detail, detail);
		glPopMatrix();
		// Display head
		glPushMatrix();
		glTranslatef(0.0, 16.0, 0.0);
		glScalef(1.0, 2.0, 1.0);
		gluSphere(quadric, 10.0, detail, detail);
		glPopMatrix();
		Texture::Use("black");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// right eye
		glPushMatrix();
		glTranslatef(-3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// left eye
		glPushMatrix();
		glTranslatef(3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// mouth
		glPushMatrix();
		glTranslatef(0.0, 14.0, 8.0);
		glScalef(3.0, 2.0, 1.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		Texture::Use("skin");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// nose
		glPushMatrix();
		glTranslatef(0.0, 21.0, 10.0);
		glScalef(1.0, 1.0, 1.5);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// Display right leg/foot half-hight part
		glPushMatrix();
		glTranslatef(-3.5, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display right leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display right leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		// Display left leg/foot half-hight part
		glPushMatrix();
		glTranslatef(3.5, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display left leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(-rotationL + 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display left leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display right arm/hand half-hight part
		glTranslatef(12.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display right arm/hand half-low part
		glRotatef(rotationL - 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display left arm/hand half-hight part
		glTranslatef(-12.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display left arm/hand half-low part
		glRotatef(-rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

	static void display(int which_hand) {
		GLint detail = 30;
		Texture::Load("gold");
		Texture::Use("gold");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		glPushMatrix();
		glTranslatef(0.0, 90.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// Display Body
		glPushMatrix();
		glScalef(1.0, 1.2, 1.0);
		gluSphere(quadric, 10.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, -10.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutSolidTorus(2.0, 4.0, detail, detail);
		gluCylinder(quadric, 4.0, 8.0, 6.0, detail, detail);
		glPopMatrix();
		// Display head
		glPushMatrix();
		glTranslatef(0.0, 16.0, 0.0);
		glScalef(1.0, 2.0, 1.0);
		gluSphere(quadric, 10.0, detail, detail);
		glPopMatrix();
		Texture::Use("black");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// right eye
		glPushMatrix();
		glTranslatef(-3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// left eye
		glPushMatrix();
		glTranslatef(3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// mouth
		glPushMatrix();
		glTranslatef(0.0, 14.0, 8.0);
		glScalef(3.0, 2.0, 1.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		Texture::Use("gold");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// nose
		glPushMatrix();
		glTranslatef(0.0, 21.0, 10.0);
		glScalef(1.0, 1.0, 1.5);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// Display right leg/foot half-hight part
		glPushMatrix();
		glTranslatef(-3.5, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display right leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display right leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		// Display left leg/foot half-hight part
		glPushMatrix();
		glTranslatef(3.5, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display left leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display left leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display right arm/hand half-hight part
		glTranslatef(12.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display right arm/hand half-low part
		if (which_hand == STATUE_RIGHT_HAND) {
			glRotatef(-90.0, 1.0, 0.0, 0.0);
		}
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display left arm/hand half-hight part
		glTranslatef(-12.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display left arm/hand half-low part
		if (which_hand == STATUE_LEFT_HAND) {
			glRotatef(-90.0, 1.0, 0.0, 0.0);
		}
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

	int getKindOf() const {
		return HUMAN_RUNNER;
	}

	enum states { STATUE_LEFT_HAND, STATUE_RIGHT_HAND };
};
// -------------------- Class: HumanRunner --------------------

// -------------------- Class: BananaRunner --------------------
class BananaRunner : public Runner {
public:
	BananaRunner() {
		Texture::Load("banana_skin");
		Texture::Load("black");
	}

	~BananaRunner() {
		Texture::Unload("banana_skin");
		Texture::Unload("black");
	}

	virtual void display() const {
		GLint detail = 30;
		Texture::Use("banana_skin");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		glPushMatrix();
		glTranslatef(0.0, height + 37.0, -50.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// Display body-head
		glPushMatrix();
		glTranslatef(0.0, 9.0, 0.0);
		glScalef(1.0, 3.0, 1.0);
		gluSphere(quadric, 10.0, detail, detail);
		glPopMatrix();
		Texture::Use("black");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// right eye
		glPushMatrix();
		glTranslatef(-3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// left eye
		glPushMatrix();
		glTranslatef(3.0, 27.0, 6.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// mouth
		glPushMatrix();
		glTranslatef(0.0, 14.0, 8.0);
		glScalef(3.0, 2.0, 1.0);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		Texture::Use("banana_skin");
		specref[0] = 0.2; specref[1] = 0.2; specref[2] = 0.2; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.8; specref[1] = 0.8; specref[2] = 0.8; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		// nose
		glPushMatrix();
		glTranslatef(0.0, 21.0, 10.0);
		glScalef(1.0, 1.0, 1.5);
		gluSphere(quadric, 2.0, detail, detail);
		glPopMatrix();
		// Display right leg/foot half-hight part
		glPushMatrix();
		glTranslatef(-4.0, -15.0, 0.0);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display right leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display right leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		// Display left leg/foot half-hight part
		glPushMatrix();
		glTranslatef(4.0, -15.0, 0.0);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display left leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.0, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(-rotationL + 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		// Display left leg/foot sole
		glTranslatef(0.0, 2.0, -1.5);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display right arm/hand half-hight part
		glTranslatef(11.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display right arm/hand half-low part
		glRotatef(rotationL - 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display left arm/hand half-hight part
		glTranslatef(-11.0, 5.0, 0.0);
		gluSphere(quadric, 3.0, detail, detail);
		glTranslatef(0.0, -1.0, 0.0);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.1, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display left arm/hand half-low part
		glRotatef(-rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

	int getKindOf() const {
		return BANANA_RUNNER;
	}
};
// -------------------- Class: BananaRunner --------------------

// -------------------- Class: SkeletonRunner --------------------
class SkeletonRunner : public Runner {
public:
	SkeletonRunner() {
		Texture::Load("gold");
	}

	~SkeletonRunner() {
		Texture::Unload("gold");
	}

	virtual void display() const {
		GLint detail = 30;
		Texture::Use("gold");
		specref[0] = 0.105882; specref[1] = 0.058824; specref[2] = 0.113725; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.427451; specref[1] = 0.470588; specref[2] = 0.541176; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.333333; specref[1] = 0.333333; specref[2] = 0.521569; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 9.84615);
		glPushMatrix();
		glTranslatef(0.0, height + 38.0, -50.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		// Display body-head
		// head
		glPushMatrix();
		glTranslatef(0.0, 14.5, 0.0);
		glScalef(1.0, 1.4, 1.0);
		gluSphere(quadric, 4.0, 25.0, 25.0);
		glPopMatrix();
		// neck
		glPushMatrix();
		glTranslatef(0.0, 10.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 3.0, 25.0, 25.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, 6.5, 0.0);
		glPushMatrix();
		glTranslatef(-9.0, 0.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 18.0, 25.0, 25.0);
		glPopMatrix();
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 20.0, 25.0, 25.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glTranslatef(-6.0, 0.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 12.0, 25.0, 25.0);
		glPopMatrix();
		// Display right leg/foot half-hight part
		glPushMatrix();
		glTranslatef(-6.0, -15.0, 0.0);
		gluSphere(quadric, 2.5, 25.0, 25.0);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display right leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.5);
		gluSphere(quadric, 2.2, detail, detail);
		// Display right leg/foot sole
		glTranslatef(0.0, 2.0, -1.0);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		// Display left leg/foot half-hight part
		glPushMatrix();
		glTranslatef(6.0, -15.0, 0.0);
		gluSphere(quadric, 2.5, detail, detail);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		// Display left leg/foot half-low part
		glTranslatef(0.0, 0.3, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glTranslatef(0.0, -0.3, 0.0);
		glRotatef(-rotationL + 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.5);
		gluSphere(quadric, 2.2, detail, detail);
		// Display left leg/foot sole
		glTranslatef(0.0, 2.0, -1.0);
		glPushMatrix();
		glScalef(1.0, 2.0, 1.0);
		gluCylinder(quadric, 0.0, 3.0, 3.5, detail, detail);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 3.5);
		glScalef(1.0, 2.0, 1.0);
		gluDisk(quadric, 0.0, 3.0, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display right arm/hand half-hight part
		glTranslatef(10.0, 6.5, 0.0);
		gluSphere(quadric, 2.2, detail, detail);
		glRotatef(90.0 - rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display right arm/hand half-low part
		glRotatef(rotationL - 45, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPushMatrix();
		// Display left arm/hand half-hight part
		glTranslatef(-10.0, 6.5, 0.0);
		gluSphere(quadric, 2.2, detail, detail);
		glRotatef(90.0 + rotationH, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, -0.2, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glTranslatef(0.0, 0.2, 0.0);
		// Display left arm/hand half-low part
		glRotatef(-rotationL, 1.0, 0.0, 0.0);
		gluCylinder(quadric, 2.0, 2.0, 10.0, detail, detail);
		glTranslatef(0.0, 0.0, 10.0);
		gluSphere(quadric, 2.2, detail, detail);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}

	int getKindOf() const {
		return SKELETON_RUNNER;
	}
};
// -------------------- Class: SkeletonRunner --------------------

// -------------------- !! END: ALL IMPLEMENTATIONS OF CLASS RUNNER !! --------------------

// -------------------- !! END: ALL IMPLEMENTATIONS OF CLASS CHARACTER !! --------------------

// -------------------- !! BEGIN: ALL IMPLEMENTATIONS OF CLASS OBJECT !! --------------------

// -------------------- Class: Path --------------------
class Path : public Object {
public:
	Path() {
		Texture::Load("path");
		for (int i = -zFar / 1000.0 - 1; i < zFar / 1000.0 + 1; i++)
			roadParts.push_back(i*500.0 + 250.0);
	}

	~Path() {
		Texture::Unload("path");
		roadParts.clear();
	}

	virtual void Path::display() const {								// Display Road/Path
		Texture::Use("path");
		specref[0] = 0.19225; specref[1] = 0.19225; specref[2] = 0.19225; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.50754; specref[1] = 0.50754; specref[2] = 0.50754; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.508273; specref[1] = 0.508273; specref[2] = 0.508273; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
		std::list <GLfloat>::const_iterator it;
		glPushMatrix();
		glTranslatef(0.0, -25.0, 0.0);
		for (it = roadParts.begin(); it != roadParts.end(); it++) {
			glPushMatrix();
			glTranslatef((*it), 0.0, 0.0);
			glScalef(10.0, 1.0, 10.0);
			Cube(50.0);
			glPopMatrix();
		}
		glPopMatrix();
	}

	virtual void animate(GLfloat speed) {								// Animate Road/Path
		if (roadParts.front() <= -zFar / 2.0 - 750.0) {
			roadParts.push_back(roadParts.back() + 500.0);
			roadParts.pop_front();
		}
		for (std::list <GLfloat>::iterator it = roadParts.begin(); it != roadParts.end(); it++)
			(*it) -= speed / 5.0;
	}

private:
	std::list<GLfloat> roadParts;
};
// -------------------- Class: Path --------------------

// -------------------- Class: Tree --------------------
class Tree : public Object {
public:
	Tree(GLfloat posX, GLfloat posY, GLfloat posZ) : Object(posX, posY, posZ) {
	}

	void display() const {
		glPushMatrix();
		glTranslatef(getPositionX(), getPositionY(), getPositionZ());
		glScalef(600.0, 300.0, 600.0);
		specref[0] = 0.19225; specref[1] = 0.19225; specref[2] = 0.19225; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.50754; specref[1] = 0.50754; specref[2] = 0.50754; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.508273; specref[1] = 0.508273; specref[2] = 0.508273; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
		Texture::Use("green");
		Wavefront::Display("tree");
		glPopMatrix();
	}

	void animate(GLfloat speed) {
		setPositionX(getPositionX() - speed / 5.0);
	}

	static void Load() {
		Texture::Load("green");
		Wavefront::Load("tree");
	}

	static void Unload() {
		Texture::Unload("green");
		Wavefront::Unload("tree");
	}
};
// -------------------- Class: Tree --------------------

// -------------------- Class: Container --------------------
class Container : public Object {
public:
	Container(GLfloat posX, GLfloat posY, GLfloat posZ) : Object(posX, posY, posZ) {
		Texture::Load("wood");
	}

	~Container() {
	}

	void display() const {
		specref[0] = 0.19225; specref[1] = 0.19225; specref[2] = 0.19225; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.50754; specref[1] = 0.50754; specref[2] = 0.50754; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.508273; specref[1] = 0.508273; specref[2] = 0.508273; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
		Texture::Use("wood");
		glPushMatrix();
		glTranslatef(getPositionX(), getPositionY(), getPositionZ());
		Cube(50.0);
		glPopMatrix();
	}

	void animate(GLfloat speed) {
		setPositionX(getPositionX() - speed / 5.0);
	}
};
// -------------------- Class: Container --------------------

// -------------------- Class: Obstacle --------------------
class Obstacle : public Object {
public:
	Obstacle(GLfloat posX, GLfloat posY, GLfloat posZ) : Object(posX, posY, posZ) {
		Texture::Load("metal");
	}

	~Obstacle() {
	}

	void display() const {
		specref[0] = 0.19225; specref[1] = 0.19225; specref[2] = 0.19225; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.50754; specref[1] = 0.50754; specref[2] = 0.50754; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.508273; specref[1] = 0.508273; specref[2] = 0.508273; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 51.2);
		Texture::Use("metal");
		glPushMatrix();
		glTranslatef(getPositionX(), getPositionY(), getPositionZ() + 50.0);
		glScalef(1.5, 5.0, 1.5);
		Cube(10.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(getPositionX(), getPositionY(), getPositionZ() - 50.0);
		glScalef(1.5, 5.0, 1.5);
		Cube(10.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(getPositionX(), getPositionY(), getPositionZ());
		glScalef(1.0, 1.0, 10.0);
		Cube(10.0);
		glPopMatrix();
	}

	void animate(GLfloat speed) {
		setPositionX(getPositionX() - speed / 5.0);
	}
};
// -------------------- Class: Obstacle --------------------

// -------------------- !! END: ALL IMPLEMENTATIONS OF CLASS OBJECT !! --------------------

// -------------------- Class: Text --------------------
void* Text::font = GLUT_STROKE_MONO_ROMAN;

Text::Text(const char* _str, int _style, GLfloat _startposx, GLfloat _startposy)
	: str(_str), style(_style), startposx(_startposx), startposy(_startposy), fade(false), r(0.0), g(1.0), b(0.0) {
	int t = strlen(str) * 175;
	rotationY = 0.0;
	switch (style) {
	case NORMAL:
		alpha = 1.0;
		break;
	case APPEAR:
		alpha = 0.0;
		break;
	case APPEAR_ROTATE_Y:
		alpha = 0.0;
		break;
	case MOVE_RIGHT:
		alpha = 1.0;
		curposx = 5500;
		break;
	case MOVE_LEFT:
		alpha = 1.0;
		curposx = -5500 - t;
		break;
	case BLINKING_FADE:
		alpha = 1.0;
		break;
	case BLINKING_APPEAR:
		alpha = 0.0;
		break;
	}
}

void Text::display() const {
	glColor4f(r, g, b, alpha);
	switch (style) {
	case NORMAL:
	case APPEAR:
	case BLINKING_FADE:
	case BLINKING_APPEAR:
		glPushMatrix();
		glTranslatef(startposx, startposy, 0.0);
		for (int i = 0; str[i] != '\0'; i++)
			glutStrokeCharacter(Text::font, str[i]);
		glPopMatrix();
		break;
	case APPEAR_ROTATE_Y:
		glPushMatrix();
		glTranslatef(startposx, startposy, 0.0);
		for (int i = 0; str[i] != '\0'; i++) {
			glPushMatrix();
			glTranslatef(i*100.0, 0.0, 0.0);
			glRotatef(rotationY, 0.0, 1.0, 0.0);
			glutStrokeCharacter(Text::font, str[i]);
			glPopMatrix();
		}
		glPopMatrix();
		break;
	case MOVE_RIGHT:
	case MOVE_LEFT:
		glPushMatrix();
		glTranslatef(curposx, startposy, 0.0);
		for (int i = 0; str[i] != '\0'; i++)
			glutStrokeCharacter(Text::font, str[i]);
		glPopMatrix();
		break;
	}
}

void Text::animate() {
	switch (style) {
	case APPEAR:
		if (!fade && alpha < 1.0)
			alpha += 0.0002*Timer::GetElapsedTime();
		break;
	case APPEAR_ROTATE_Y:
		rotationY += 0.1*Timer::GetElapsedTime();
		if (rotationY >= 360.0)
			rotationY = 0.0;
		if (!fade && alpha < 1.0)
			alpha += 0.0002*Timer::GetElapsedTime();
		break;
	case MOVE_RIGHT:
		if (curposx > startposx) {
			curposx -= 2.0*Timer::GetElapsedTime();
			if (curposx < startposx)
				curposx = startposx;
		}
		break;
	case MOVE_LEFT:
		if (curposx < startposx) {
			curposx += 2.0*Timer::GetElapsedTime();
			if (curposx > startposx)
				curposx = startposx;
		}
		break;
	case BLINKING_APPEAR:
		if (!fade && alpha < 1.0)
			alpha += 0.003*Timer::GetElapsedTime();
		else if (!fade && alpha >= 1.0)
			style = BLINKING_FADE;
		break;
	case BLINKING_FADE:
		if (!fade && alpha > 0.0)
			alpha -= 0.003*Timer::GetElapsedTime();
		else if (!fade && alpha <= 0.0)
			style = BLINKING_APPEAR;
		break;
	}
	if (fade && alpha > 0.0)
		alpha -= 0.0005*Timer::GetElapsedTime();
}

void Text::setStyle(int style) {
	this->style = style;
	if (style == NORMAL)
		alpha = 1.0;
	rotationY = 0.0;
}

void Text::setColor(GLfloat _r, GLfloat _g, GLfloat _b) {
	r = _r;
	g = _g;
	b = _b;
}

void Text::setFade(bool fade) {
	this->fade = fade;
}

const char* Text::getText() const {
	return str;
}
// -------------------- Class: Text --------------------

// -------------------- Namespace: Graphics --------------------
namespace Graphics {
	// -------------------- Basic functions used by glut when each specific event occurs --------------------
	void Init() {														// Initialize Graphics Library
		srand((unsigned int)time(NULL));
		glEnable(GL_DEPTH_TEST);										// Enable hidden surface removal
		glDepthFunc(GL_LEQUAL);											// The depth comparison function: Passes if incoming depth value is >= to the stored depth value
		glEnable(GL_CULL_FACE);											// Enable culling for performance
		glFrontFace(GL_CCW);											// Counter clockwise culling
		glShadeModel(GL_SMOOTH);										// Set shading model to smooth
		glEnable(GL_BLEND);												// Enable Blending (transparency)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		GLfloat light_position[] = { -1.0, -1.0, -1.0, 1.0f };			// Position of lighting source
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);				// Set position of lighting source
		GLfloat ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);					// Set ambient lighting for light0
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);					// Set diffuse lighting for light0
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);				// Set specular lighting for light0
		GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 64);
		glEnable(GL_LIGHT0);											// Enable one light source
		glHint(GL_LINE_SMOOTH, GL_NICEST);
		quadric = gluNewQuadric();
		gluQuadricTexture(quadric, true);								// Enable generation of texture coordinates for quadric object
		gluQuadricNormals(quadric, GLU_SMOOTH);
		graphics = new Intro();
	}

	void Exit() {														// Clean up Graphics Library
		gluDeleteQuadric(quadric);
		delete graphics;
	}

	void reshape(int width, int height) {								// Main reshape function - when window is resized
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);				// Establish Viewport (the whole window)
		glMatrixMode(GL_PROJECTION);									// Select the projection matrix
		glLoadIdentity();												// Initialize projection matrix to identical
		gluPerspective(60.0, (GLdouble)width / (GLdouble)height, 1.0, zFar);	// Establish desired projection
		glMatrixMode(GL_MODELVIEW);										// Select the modelview matrix for further operations
	}

	void display() {													// Main display function
		Timer::Start();													// Start Timer
		graphics->display();											// Display content
		Utilities::Update();											// Update Utilities Library
		glutSwapBuffers();												// Swap buffers
		Timer::Stop();													// Stop Timer
		glutPostRedisplay();											// Call function to redisplay the next seen
	}

	void animate() {													// Main animation function
		graphics->animate();
	}

	void keyboard(unsigned char key, int x, int y) {					// Main keyboard callback function
		graphics->keyboard(key, x, y);
	}

	void specialKeyboard(int key, int x, int y) {						// Main special keyboard callback function
		graphics->specialKeyboard(key, x, y);
	}
	// -------------------- Basic functions used by glut when each specific event occurs --------------------

	// -------------------- Class: Intro --------------------
	Intro::Intro() : elapsedTime(0.0) {
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		strs.clear();
		Audio::Load("intro");
		Audio::SetVolume("intro", 0.25);
		Audio::Play("intro");
	}

	Intro::~Intro() {
		glDisable(GL_LINE_SMOOTH);
		Audio::Stop("intro");
		Audio::Unload("intro");
		strs.clear();
	}

	void Intro::display() const {
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		std::list <Text>::const_iterator it;
		glTranslatef(0.0, 0.0, -4900.0);
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
	}

	void Intro::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++)
			it->animate();
		elapsedTime += Timer::GetElapsedTime();
		if (strs.size() == 0) {
			strs.push_back(Text("University: National & Kapodistrian University of Athens", Text::APPEAR, -5000.0, 2500.0));
		}
		else if (strs.size() == 1 && elapsedTime >= 1000) {
			strs.push_back(Text("Department: Informatics & Telecommunications", Text::APPEAR, -5000.0, 2300.0));
		}
		else if (strs.size() == 2 && elapsedTime >= 2000) {
			strs.push_back(Text("Lesson    : Graphics I", Text::APPEAR, -5000.0, 2100.0));
		}
		else if (strs.size() == 3 && elapsedTime >= 4000) {
			strs.push_back(Text("Alertas Productions Proudly Presents", Text::APPEAR, -(int)strlen("Alertas Productions Proudly Presents") / 2 * 100, 500.0));
		}
		else if (strs.size() == 4 && elapsedTime >= 5500) {
			strs.push_back(Text("Animation Project: Walking with obstacles", Text::MOVE_RIGHT, -(int)strlen("Animation Project: Walking with obstacles") / 2 * 100, 100.0));
			strs.push_back(Text("By Alertas Spyros", Text::MOVE_LEFT, -(int)strlen("By Alertas Spyros") / 2 * 100, -100.0));
		}
		else if (strs.size() == 6 && elapsedTime >= 9000) {
			strs.push_back(Text("Programming Language: C++", Text::APPEAR, -5000.0, -2300.0));
		}
		else if (strs.size() == 7 && elapsedTime >= 9250) {
			strs.push_back(Text("Graphics Powered by : OpenGL", Text::APPEAR, -5000.0, -2500.0));
		}
		else if (strs.size() == 8 && elapsedTime >= 9500) {
			strs.push_back(Text("Audio Powered by    : FMOD", Text::APPEAR, -5000.0, -2700.0));
		}
		else if (strs.size() == 9 && elapsedTime >= 10500) {
			strs.push_back(Text("Greece, Athens 2012-2013", Text::APPEAR, 2400.0, -2500.0));
		}
		else if (strs.size() == 10 && elapsedTime >= 15000) {
			for (it = strs.begin(); it != strs.end(); it++)
				it->setFade(true);
		}
		if (elapsedTime >= 18000) {
			delete graphics;
			graphics = new MainMenu();
		}
	}

	void Intro::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 27: delete graphics; graphics = new MainMenu(); break;
		}
	}
	// -------------------- Class: Intro --------------------

	// -------------------- Class: MainMenu --------------------
	bool MainMenu::changed = false;

	MainMenu::MainMenu() : elapsedTime(0.0) {
		if (!Audio::IsPlaying("main_theme")) {
			Audio::Load("main_theme");
			Audio::SetRepeat("main_theme", true);
			Audio::Play("main_theme");
		}
		else {
			Audio::SetPaused("main_theme", false);
		}
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		if (changed) {
			strs.push_back(Text("New Game", Text::NORMAL, -(int)strlen("New Game") / 2 * 100, 400.0));
			strs.push_back(Text("Continue", Text::NORMAL, -(int)strlen("Continue") / 2 * 100, 200.0));
			strs.push_back(Text("Options", Text::NORMAL, -50 - (int)strlen("Options") / 2 * 100, 0.0));
			strs.push_back(Text("Replay Intro", Text::NORMAL, -(int)strlen("Replay Intro") / 2 * 100, -200.0));
			strs.push_back(Text("Quit", Text::NORMAL, -(int)strlen("Quit") / 2 * 100, -400.0));
		}
		else {
			strs.push_back(Text("New Game", Text::MOVE_LEFT, -(int)strlen("New Game") / 2 * 100, 400.0));
			strs.push_back(Text("Continue", Text::MOVE_RIGHT, -(int)strlen("Continue") / 2 * 100, 200.0));
			strs.push_back(Text("Options", Text::MOVE_LEFT, -50 - (int)strlen("Options") / 2 * 100, 0.0));
			strs.push_back(Text("Replay Intro", Text::MOVE_RIGHT, -(int)strlen("Replay Intro") / 2 * 100, -200.0));
			strs.push_back(Text("Quit", Text::MOVE_LEFT, -(int)strlen("Quit") / 2 * 100, -400.0));
		}
		std::list <Text>::iterator it = strs.begin();;
		if (changed) {
			selected = NEW_GAME;
			it->setStyle(Text::BLINKING_FADE);
			it->setColor(1.0, 1.0, 0.0);
		}
		else
			selected = NONE;
		if (lastRace == NULL) {
			int c = CONTINUE;
			advance(it, c);
			it->setColor(0.5, 0.5, 0.5);
		}
	}

	MainMenu::~MainMenu() {
		glDisable(GL_LINE_SMOOTH);
		strs.clear();
	}

	void MainMenu::display() const {
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		std::list <Text>::const_iterator it;
		glTranslatef(0.0, 0.0, -4900.0);
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
	}

	void MainMenu::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++) {
			it->animate();
			if (!changed) {
				it->animate();
				it->animate();
			}
		}
		elapsedTime += Timer::GetElapsedTime();
		if (!changed && elapsedTime >= 1250) {
			selected = NEW_GAME;
			changed = true;
			for (; it != strs.end(); it++)
				it->setStyle(Text::NORMAL);
			it = strs.begin();
			advance(it, selected);
			it->setStyle(Text::BLINKING_FADE);
			it->setColor(1.0, 1.0, 0.0);
		}
	}

	void MainMenu::keyboard(unsigned char key, int x, int y) {
		if (!changed)
			return;
		switch (key) {
		case 13:
			if (selected == NEW_GAME) {
				delete graphics;
				graphics = new NewGameMenu();
			}
			else if (selected == CONTINUE && lastRace != NULL) {
				lastRace->continueWalk();
			}
			else if (selected == OPTIONS) {
				delete graphics;
				graphics = new OptionsMenu();
			}
			else if (selected == REPLAY_INTRO) {
				Audio::SetPaused("main_theme", true);
				delete graphics;
				graphics = new Intro();
			}
			else if (selected == QUIT) {
				delete graphics;
				graphics = new Quit();
			}
			break;
		}
	}

	void MainMenu::specialKeyboard(int key, int x, int y) {
		if (!changed)
			return;
		switch (key) {
		case GLUT_KEY_UP:
			if (selected > NEW_GAME) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				--selected;
				if (selected == CONTINUE && lastRace == NULL) {
					--selected;
					--it;
				}
				--it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		case GLUT_KEY_DOWN:
			if (selected < QUIT) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				++selected;
				if (selected == CONTINUE && lastRace == NULL) {
					++selected;
					++it;
				}
				++it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		}
	}
	// -------------------- Class: MainMenu --------------------

	// -------------------- Class: NewGameMenu --------------------
	NewGameMenu::NewGameMenu() {
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		strs.push_back(Text("Main Menu", Text::NORMAL, -(int)strlen("Main Menu") / 2 * 100 - 3500, 2500.0));
		strs.push_back(Text("Select Character", Text::NORMAL, -(int)strlen("Select Character") / 2 * 100, 2500.0));
		Text text("Start Game", Text::BLINKING_FADE, -(int)strlen("Start Game") / 2 * 100 + 3500, 2500.0);
		text.setColor(1.0, 1.0, 0.0);
		strs.push_back(text);
		selected = START_GAME;
		characters.push_back(new HumanRunner());
		characters.push_back(new BananaRunner());
		characters.push_back(new SkeletonRunner());
		Audio::SetVolume("step", 0.0);
	}

	NewGameMenu::~NewGameMenu() {
		glDisable(GL_LINE_SMOOTH);
		strs.clear();
		std::list <Runner*>::iterator itb;
		for (itb = characters.begin(); itb != characters.end(); itb++)
			delete *itb;
		characters.clear();
	}

	void NewGameMenu::display() const {
		int i;
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		glTranslatef(0.0, 0.0, -4900.0);
		std::list <Text>::const_iterator ita;
		for (ita = strs.begin(); ita != strs.end(); ita++)
			ita->display();
		std::list <Runner*>::const_iterator itb;
		glEnable(GL_LIGHTING);
		Texture::Enable();
		glLoadIdentity();
		if (selected == CHOOSING_CHARACTER)
			glTranslatef(-50.0, -50.0, -150.0);
		else
			glTranslatef(-50.0, -50.0, -250.0);
		for (itb = characters.begin(), i = -1; itb != characters.end(); itb++, i++) {
			glPushMatrix();
			glTranslatef(i*100.0, 0.0, 0.0);
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			(*itb)->display();
			glPopMatrix();
		}
		Texture::Disable();
		glDisable(GL_LIGHTING);
	}

	void NewGameMenu::animate() {
		std::list <Text>::iterator ita;
		for (ita = strs.begin(); ita != strs.end(); ita++)
			ita->animate();
		std::list <Runner*>::iterator itb;
		for (itb = characters.begin(); itb != characters.end(); itb++)
			(*itb)->animate();
	}

	void NewGameMenu::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 13:
			if (selected == MAIN_MENU) {
				delete graphics;
				graphics = new MainMenu();
			}
			else if (selected == START_GAME) {
				Audio::SetPaused("main_theme", true);
				std::list <Runner*>::iterator it = characters.begin();
				advance(it, characters.size() / 2);
				int character = (*it)->getKindOf();
				delete graphics;
				graphics = new World(character);
			}
			else if (selected == SELECT_CHARACTER) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				selected = CHOOSING_CHARACTER;
			}
			else if (selected == CHOOSING_CHARACTER) {
				std::list <Text>::iterator it = strs.begin();
				selected = SELECT_CHARACTER;
				advance(it, selected);
				it->setStyle(Text::BLINKING_FADE);
			}
			break;
		}
	}

	void NewGameMenu::specialKeyboard(int key, int x, int y) {
		switch (key) {
		case GLUT_KEY_LEFT:
			if (selected != CHOOSING_CHARACTER && selected > MAIN_MENU) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				--selected;
				it--;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			else if (selected == CHOOSING_CHARACTER) {
				for (unsigned int i = 0; i < characters.size() - 1; i++) {
					characters.push_front(characters.back());
					characters.pop_back();
				}
			}
			break;
		case GLUT_KEY_RIGHT:
			if (selected != CHOOSING_CHARACTER && selected < START_GAME) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				++selected;
				++it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			else if (selected == CHOOSING_CHARACTER) {
				for (unsigned int i = 0; i < characters.size() - 1; i++) {
					characters.push_back(characters.front());
					characters.pop_front();
				}
			}
			break;
		}
	}
	// -------------------- Class: NewGameMenu --------------------

	// -------------------- Class: OptionsMenu --------------------
	OptionsMenu::OptionsMenu() {
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		strs.push_back(Text("Sound", Text::NORMAL, -(int)strlen("Sound") / 2 * 100, 200.0));
		strs.push_back(Text("Keyboard Layout", Text::NORMAL, -(int)strlen("Keyboard Layout") / 2 * 100, 0.0));
		strs.push_back(Text("Main Menu", Text::NORMAL, -(int)strlen("Main Menu") / 2 * 100, -200.0));
		selected = MAIN_MENU;
		int s = MAIN_MENU;
		std::list <Text>::iterator it = strs.begin();
		advance(it, s);
		it->setColor(1.0, 1.0, 0.0);
		it->setStyle(Text::BLINKING_FADE);
	}

	OptionsMenu::~OptionsMenu() {
		glDisable(GL_LINE_SMOOTH);
		strs.clear();
	}

	void OptionsMenu::display() const {
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		std::list <Text>::const_iterator it;
		glTranslatef(0.0, 0.0, -4900.0);
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
	}

	void OptionsMenu::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++) {
			it->animate();
		}
	}

	void OptionsMenu::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 13:
			if (selected == MAIN_MENU) {
				delete graphics;
				graphics = new MainMenu();
			}
			else if (selected == SOUND) {
				delete graphics;
				graphics = new SoundMenu();
			}
			else if (selected == KEYBOARD) {
				delete graphics;
				graphics = new KeyboardMenu();
			}
			break;
		}
	}

	void OptionsMenu::specialKeyboard(int key, int x, int y) {
		switch (key) {
		case GLUT_KEY_UP:
			if (selected > SOUND) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				--selected;
				--it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		case GLUT_KEY_DOWN:
			if (selected < MAIN_MENU) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				++selected;
				++it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		}
	}
	// -------------------- Class: OptionsMenu --------------------

	// -------------------- Class: SoundMenu --------------------
	SoundMenu::SoundMenu() {
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		selected = OPTIONS;
		strs.push_back(Text("+", Text::NORMAL, 4400.0 - strlen("Back to Options") / 2 * 100, 400.0));
		strs.push_back(Text("-", Text::NORMAL, 4400.0 - strlen("Back to Options") / 2 * 100, 200.0));
		{ Text text("Back to Options", Text::BLINKING_FADE, 4500.0 - strlen("Back to Options") * 100, 0.0);
		text.setColor(1.0, 1.0, 0.0);
		strs.push_back(text); }
	}

	SoundMenu::~SoundMenu() {
		glDisable(GL_LINE_SMOOTH);
		strs.clear();
	}

	void SoundMenu::display() const {
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		std::list <Text>::const_iterator it;
		glTranslatef(0.0, 0.0, -4900.0);
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
		glColor3f(0.0, 1.0, 0.0);
		for (int i = 0; i < Audio::GetMasterVolume() / 5; i++) {
			glPushMatrix();
			glTranslatef(-3500.0 + i*150.0, 0.0, 0.0);
			glRectf(-50.0, -50.0, 50.0, 50.0);
			glPopMatrix();
		}
		glColor3f(1.0, 1.0, 1.0);
		for (int i = Audio::GetMasterVolume() / 5; i < 20; i++) {
			glPushMatrix();
			glTranslatef(-3500.0 + i*150.0, 0.0, 0.0);
			glRectf(-50.0, -50.0, 50.0, 50.0);
			glPopMatrix();
		}
		std::string volume;
		std::ostringstream convert;										// Stream used for the conversion
		convert << Audio::GetMasterVolume();
		volume = convert.str() + "/100";
		Text text(volume.c_str(), Text::NORMAL, 500.0, 0.0);
		text.display();
	}

	void SoundMenu::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++) {
			it->animate();
		}
	}

	void SoundMenu::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 13:
			if (selected == INCREASE_VOLUME) {
				Audio::SetMasterVolume(Audio::GetMasterVolume() + 5);
			}
			else if (selected == DECREASE_VOLUME) {
				Audio::SetMasterVolume(Audio::GetMasterVolume() - 5);
			}
			else if (selected == OPTIONS) {
				delete graphics;
				graphics = new OptionsMenu();
			}
			break;
		}
	}

	void SoundMenu::specialKeyboard(int key, int x, int y) {
		switch (key) {
		case GLUT_KEY_UP:
			if (selected > INCREASE_VOLUME) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				--selected;
				--it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		case GLUT_KEY_DOWN:
			if (selected < OPTIONS) {
				std::list <Text>::iterator it = strs.begin();
				advance(it, selected);
				it->setStyle(Text::NORMAL);
				it->setColor(0.0, 1.0, 0.0);
				++selected;
				++it;
				it->setStyle(Text::BLINKING_FADE);
				it->setColor(1.0, 1.0, 0.0);
			}
			break;
		}
	}
	// -------------------- Class: SoundMenu --------------------

	// -------------------- Class: KeyboardMenu --------------------
	KeyboardMenu::KeyboardMenu() {
		glEnable(GL_LINE_SMOOTH);										// Enable Line Smoothing
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		strs.push_back(Text("> General <", Text::NORMAL, -4500.0, 2200.0));
		strs.push_back(Text("Back to Main Menu:", Text::NORMAL, -4500.0, 1900.0));
		strs.push_back(Text("Esc", Text::NORMAL, -2300.0 - 150, 1900.0));
		strs.push_back(Text("> Camera <", Text::NORMAL, -4500.0, 1200.0));
		strs.push_back(Text("Rotate Left:", Text::NORMAL, -4500.0, 900.0));
		strs.push_back(Text("4", Text::NORMAL, -2300.0, 900.0));
		strs.push_back(Text("Rotate Right:", Text::NORMAL, -4500.0, 700.0));
		strs.push_back(Text("6", Text::NORMAL, -2300.0, 700.0));
		strs.push_back(Text("Rotate Up:", Text::NORMAL, -4500.0, 500.0));
		strs.push_back(Text("8", Text::NORMAL, -2300.0, 500.0));
		strs.push_back(Text("Rotate Down:", Text::NORMAL, -4500.0, 300.0));
		strs.push_back(Text("5", Text::NORMAL, -2300.0, 300.0));
		strs.push_back(Text("Zoom In:", Text::NORMAL, -4500.0, 100.0));
		strs.push_back(Text("+", Text::NORMAL, -2300.0, 100.0));
		strs.push_back(Text("Zoom Out:", Text::NORMAL, -4500.0, -100.0));
		strs.push_back(Text("-", Text::NORMAL, -2300.0, -100.0));
		strs.push_back(Text("> Character <", Text::NORMAL, -4500.0, -800.0));
		strs.push_back(Text("Speed Up:", Text::NORMAL, -4500.0, -1100.0));
		strs.push_back(Text("Up Arrow", Text::NORMAL, -2300.0 - 400.0, -1100.0));
		strs.push_back(Text("Slow Down:", Text::NORMAL, -4500.0, -1300.0));
		strs.push_back(Text("Down Arrow", Text::NORMAL, -2300.0 - 500.0, -1300.0));
		strs.push_back(Text("Jump:", Text::NORMAL, -4500.0, -1500.0));
		strs.push_back(Text("Space", Text::NORMAL, -2300.0 - 250.0, -1500.0));
		{ Text text("Back to Options", Text::BLINKING_FADE, 4500.0 - strlen("Back to Options") * 100, 0.0);
		text.setColor(1.0, 1.0, 0.0);
		strs.push_back(text); }
	}

	KeyboardMenu::~KeyboardMenu() {
		glDisable(GL_LINE_SMOOTH);
		strs.clear();
	}

	void KeyboardMenu::display() const {
		glClearColor(0.07, 0.07, 0.07, 1.0);							// Set clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Color, depth & stencil buffers
		glLoadIdentity();												// Initialize modelview matrix to identical
		std::list <Text>::const_iterator it;
		glTranslatef(0.0, 0.0, -4900.0);
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
	}

	void KeyboardMenu::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++)
			it->animate();
	}

	void KeyboardMenu::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 13: delete graphics; graphics = new OptionsMenu(); break;
		}
	}
	// -------------------- Class: KeyboardMenu --------------------

	// -------------------- Class: Quit --------------------
	Quit::Quit() {
		selected = NO;
		strs.push_back(Text("YES", Text::NORMAL, -500.0, 0.0));
		strs.push_back(Text("NO", Text::NORMAL, 500.0, 0.0));
		strs.push_back(Text("Are you sure?", Text::NORMAL, -550.0, 200.0));
		std::list <Text>::iterator it;
		it = strs.begin();
		it->setColor(0.0, 1.0, 0.0);
		++it;
		it->setColor(1.0, 1.0, 0.0);
		it->setStyle(Text::BLINKING_FADE);
	}

	void Quit::display() const {
		glColor4f(0.1, 0.1, 0.1, 0.1);
		glPushMatrix();
		glTranslatef(-100.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(100.0, 100.0, 0.0);
		glRectf(-1000.0, -300.0, 1000.0, 300.0);
		glPopMatrix();
		std::list <Text>::const_iterator it;
		for (it = strs.begin(); it != strs.end(); it++)
			it->display();
		glPopMatrix();
	}

	void Quit::animate() {
		std::list <Text>::iterator it;
		for (it = strs.begin(); it != strs.end(); it++)
			it->animate();
	}

	void Quit::keyboard(unsigned char key, int, int y) {
		switch (key) {
		case 13:
			if (selected == NO) {
				delete graphics;
				graphics = new MainMenu();
			}
			else
				Application::Exit();
			break;
		}
	}

	void Quit::specialKeyboard(int key, int x, int y) {
		std::list <Text>::iterator it;
		switch (key) {
		case GLUT_KEY_LEFT:
			if (selected == YES)
				return;
			selected = YES;
			it = strs.begin();
			it->setColor(1.0, 1.0, 0.0);
			it->setStyle(Text::BLINKING_FADE);
			++it;
			it->setColor(0.0, 1.0, 0.0);
			it->setStyle(Text::NORMAL);
			break;
		case GLUT_KEY_RIGHT:
			if (selected == NO)
				return;
			selected = NO;
			it = strs.begin();
			it->setColor(0.0, 1.0, 0.0);
			it->setStyle(Text::NORMAL);
			++it;
			it->setColor(1.0, 1.0, 0.0);
			it->setStyle(Text::BLINKING_FADE);
			break;
		}
	}
	// -------------------- Class: Quit --------------------

	// -------------------- Class: World --------------------
	World::World(int character) : paused(false), hit(false), noHit(true), elapsedTime(0) {	// Create the World
		glEnable(GL_LIGHTING);
		Texture::Enable();
		Audio::Load("background");
		Audio::SetRepeat("background", true);
		Audio::Play("background");
		Texture::Load("cave");
		Texture::Load("sand");
		Texture::Load("sea");
		Texture::Load("lilipad");
		if (character == Character::BANANA_RUNNER)
			this->character = new BananaRunner();
		else if (character == Character::HUMAN_RUNNER)
			this->character = new HumanRunner();
		else if (character == Character::SKELETON_RUNNER)
			this->character = new SkeletonRunner();
		Tree::Load();
		for (int i = -10; i < 10; i++) {
			contentG.push_back(new Tree(i*500.0, -10.0, -300.0));
		}
		for (int i = -10; i < 10; i++) {
			contentG.push_back(new Tree(i*500.0, -10.0, 100.0));
		}
		for (int i = -10; i < 0; i++) {
			contentG.push_back(new Container(i*500.0, 25.0, 100.0));
		}
		for (int i = 0; i < 10; i++) {
			contentG.push_back(new Container(i*500.0, 25.0, 100.0));
		}
		for (int i = -10; i < 10; i++) {
			contentO.push_back(new Obstacle(i*750.0, 25.0, -50.0));
		}
		path = new Path();
		text = new Text(Text("Paused", Text::BLINKING_FADE, 0.0, 0.0));
		textB = new Text(Text("HIT", Text::BLINKING_FADE, 0.0, 0.0));
		textB->setColor(1.0, 0.0, 0.0);
	}

	World::~World() {													// Destroy the Wolrd
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		Audio::Unload("background");
		Texture::Unload("cave");
		Texture::Unload("sand");
		Texture::Unload("sea");
		Texture::Unload("wood");
		Texture::Unload("lilipad");
		delete path;
		std::list <Object*>::iterator it;
		for (it = contentG.begin(); it != contentG.end(); it++)
			delete *it;
		contentG.clear();
		for (it = contentO.begin(); it != contentO.end(); it++)
			delete *it;
		contentO.clear();
		Tree::Unload();
		delete text;
		delete textB;
	}

	void World::display() const {										// Display World
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.place();
		// Display Cave
		Texture::Use("cave");
		specref[0] = 0.05375; specref[1] = 0.05; specref[2] = 0.06625; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.18275; specref[1] = 0.17; specref[2] = 0.22525; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.332741; specref[1] = 0.328634; specref[2] = 0.346435; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 38.4);
		glPushMatrix();
		glRotatef(90.0, 1.0, 0.0, 0.0);
		gluQuadricOrientation(quadric, GLU_INSIDE);
		gluSphere(quadric, zFar / 2.0, 25.0, 25.0);
		gluQuadricOrientation(quadric, GLU_OUTSIDE);
		glPopMatrix();
		// Display Terrain - Sand
		Texture::Use("sand");
		specref[0] = 0.2125; specref[1] = 0.1275; specref[2] = 0.054; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.714; specref[1] = 0.4284; specref[2] = 0.18144; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.393548; specref[1] = 0.271906; specref[2] = 0.166721; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.6);
		glPushMatrix();
		glTranslatef(0.0, -150.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, zFar / 2.0, 25.0, 25.0);
		glPopMatrix();
		// Display the two golden statues - Base
		Texture::Use("rock");
		specref[0] = 0.05375; specref[1] = 0.05; specref[2] = 0.06625; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.18275; specref[1] = 0.17; specref[2] = 0.22525; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.332741; specref[1] = 0.328634; specref[2] = 0.346435; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 38.4);
		glPushMatrix();
		glTranslatef(zFar / 2.0 - 500.0, 0.0, -550.0);
		glRotatef(30.0, 0.0, 1.0, 0.0);
		Cube(250.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(zFar / 2.0 - 500.0, 0.0, 550.0);
		glRotatef(-30, 0.0, 1.0, 0.0);
		Cube(250.0);
		glPopMatrix();
		// Display the two golden statues - Statue
		Texture::Use("gold");
		specref[0] = 0.24725; specref[1] = 0.2245; specref[2] = 0.0645; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.34615; specref[1] = 0.3143; specref[2] = 0.0903; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.797357; specref[1] = 0.723991; specref[2] = 0.208006; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 83.2);
		glPushMatrix();
		glTranslatef(zFar / 2.0 - 500.0, -140.0, -550.0);
		glRotatef(-180.0, 0.0, 1.0, 0.0);
		glScalef(5.0, 5.0, 5.0);
		HumanRunner::display(HumanRunner::STATUE_LEFT_HAND);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(zFar / 2.0 - 500.0, -140.0, 550.0);
		glRotatef(-180.0, 0.0, 1.0, 0.0);
		glScalef(5.0, 5.0, 5.0);
		HumanRunner::display(HumanRunner::STATUE_RIGHT_HAND);
		glPopMatrix();
		// Display Entrance
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 1.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
		glPushMatrix();
		glTranslatef(zFar / 2.0 - 100.0, 0.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glScalef(1.0, 2.0, 0.1);
		gluSphere(quadric, 500.0, 25.0, 25.0);
		glPopMatrix();
		Texture::Use("lilipad");
		specref[0] = 0.135; specref[1] = 0.2225; specref[2] = 0.1575; specref[3] = 0.95;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.54; specref[1] = 0.89; specref[2] = 0.63; specref[3] = 0.95;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.316228; specref[1] = 0.316228; specref[2] = 0.316228; specref[3] = 0.95;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
		glPushMatrix();
		glTranslatef(800.0, -29.0, -850.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, 50.0, 25.0, 25.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1500.0, -29.0, -1000.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, 50.0, 25.0, 25.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-660.0, -29.0, -1350.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, 50.0, 25.0, 25.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-930.0, -29.0, -850.0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, 50.0, 25.0, 25.0);
		glPopMatrix();
		character->display();
		path->display();
		std::list <Object*>::const_iterator it;
		for (it = contentG.begin(); it != contentG.end(); it++)
			(*it)->display();
		for (it = contentO.begin(); it != contentO.end(); it++)
			(*it)->display();
		// Display Terrain - Sea - last because of blending
		Texture::Use("sea");
		specref[0] = 0.1; specref[1] = 0.18725; specref[2] = 0.1745; specref[3] = 0.7;
		glMaterialfv(GL_FRONT, GL_AMBIENT, specref);
		specref[0] = 0.396; specref[1] = 0.74151; specref[2] = 0.69102; specref[3] = 0.7;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specref);
		specref[0] = 0.297254; specref[1] = 0.30829; specref[2] = 0.306678; specref[3] = 0.7;
		glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
		specref[0] = 0.0; specref[1] = 0.0; specref[2] = 0.0; specref[3] = 0.0;
		glMaterialfv(GL_FRONT, GL_EMISSION, specref);
		glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
		glPushMatrix();
		glTranslatef(0.0, -30.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluDisk(quadric, 0.0, zFar / 2.0, 50.0, 50.0);
		glPopMatrix();
		if (paused) {
			Texture::Disable();
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glTranslatef(0.0, 200.0, -500.0);
			text->display();
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0.0, 200.0, 500.0);
			glRotatef(180.0, 0.0, 1.0, 0.0);
			text->display();
			glPopMatrix();
			Texture::Enable();
			glEnable(GL_LIGHTING);
		}
		else if (hit) {
			Texture::Disable();
			glDisable(GL_LIGHTING);
			glTranslatef(500.0, 100.0, -150.0);
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			textB->display();
			Texture::Enable();
			glEnable(GL_LIGHTING);
		}
	}

	void World::animate() {												// Animate World
		if (paused) {													// If user paused it
			text->animate();
			return;														// do nothing
		}
		else if (hit) {
			elapsedTime += Timer::GetElapsedTime();
			textB->animate();
			if (elapsedTime >= 3000) {
				hit = false;
				noHit = true;
				elapsedTime = 0;
			}
			return;
		}
		else if (noHit) {
			elapsedTime += Timer::GetElapsedTime();
			if (elapsedTime >= 2000) {
				noHit = false;
				elapsedTime = 0;
			}
		}
		character->animate();											// Animate character
		GLfloat speed = character->getSpeed();
		GLfloat posX;
		path->animate(speed);
		std::list <Object*>::iterator it, itB;
		for (it = contentO.begin(); it != contentO.end(); ) {
			(*it)->animate(speed);
			posX = (*it)->getPositionX();
			if (posX <= 5.0 && posX >= -5.0 && !noHit) {
				if (character->getHeight() < 30.0) {
					hit = true;
					//					for(itB = contentO.begin(); itB != contentO.end(); itB++) {
					//						(*itB)->setPositionX((*itB)->getPositionX()-750.0);
					//					}
				}
			}
			else if (posX < -zFar / 2.0 - 500.0) {						// If content is out of view - re-add it!
				(*it)->setPositionX(-posX);
			}
			it++;
		}
		for (it = contentG.begin(); it != contentG.end(); ) {
			(*it)->animate(speed);
			posX = (*it)->getPositionX();
			if (posX < -zFar / 2.0 - 500.0) {							// If content is out of view - re-add it!
				(*it)->setPositionX(-posX);
			}
			it++;
		}
	}

	void World::keyboard(unsigned char key, int x, int y) {
		switch (key) {
		case 'P': case 'p':												// P or p -> Pause/Continue game
			paused = !paused; Audio::SetPaused("background", paused); break;
		case '8': camera.moveUp(); break;								// 8 -> Move camera Up - Y Axis
		case '5': camera.moveDown(); break;								// 5 -> Move camera Down - Y Axis
		case '4': camera.moveLeft(); break;								// 4 -> Move Camera Left - X Axis
		case '6': camera.moveRight(); break;							// 6 -> Move Camera Right - X Axis
		case '-': camera.zoomOut(); break;								// - -> Move Camera Away - Z Axis
		case '+': camera.zoomIn(); break;								// + -> Move Camera Closer - Z Axis
		case 27:
			pauseWalk();
			break;
		case 32:														// Space -> jump
			if (!paused)
				character->jump();
			break;
		case 'e': Application::Exit(); break;							// 'e' -> Terminates program
		}
	}

	void World::specialKeyboard(int key, int x, int y) {
		if (!paused && !hit)											// If game is not paused or user has failed to pass obstacle - else these keys are "disabled"
			switch (key) {
			case GLUT_KEY_UP:											// Arrow key Up - Increase Speed
				character->speedUp();
				break;
			case GLUT_KEY_DOWN:
				character->slowDown();									// Arrow key Down - Slow Down
				break;
			}
	}

	void World::continueWalk() {										// When user continues his last walk - stops main menu & continues walk
		Audio::SetPaused("main_theme", true);
		Audio::SetPaused("background", false);
		glEnable(GL_LIGHTING);
		Texture::Enable();
		delete graphics;
		graphics = lastRace;
		if (character->getKindOf() == Character::HUMAN_RUNNER)
			Texture::Load("skin");
		else if (character->getKindOf() == Character::BANANA_RUNNER)
			Texture::Load("banana_skin");
		else if (character->getKindOf() == Character::SKELETON_RUNNER)
			Texture::Load("gold");
		Texture::Load("black");
	}

	void World::pauseWalk() {											// When user presses escape - pause walk & returns to main menu
		lastRace = this;
		Audio::SetPaused("background", true);
		Audio::SetPaused("main_theme", false);
		glDisable(GL_LIGHTING);
		Texture::Disable();
		graphics = new MainMenu();
	}
	// -------------------- Class: World --------------------
}
// -------------------- Namespace: Graphics --------------------