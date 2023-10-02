/* Include the library header files and constants */
#include <freeglut.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#define PI 3.1415926
#define DEG_TO_RAD PI/180.0
#define BOID_COUNT 40
#define CLOSEST_COUNT 6

/* Defining function signatures */
void printKeyboardControls(void);
void drawButton(void);
void drawBoids(void);
void initializeBoids(void);
bool inClosestN(int);
void findClosestN(int, int[]);
float findDistance(int, int);
int compareDistanceIndexPair(const void* elem1, const void* elem2);
void handleBoidWallInteraction(int);
void handleFlockingInteraction(int);

/* Struct for boid for easy management of position and direction */
typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat direction;
	GLfloat speed;
} Boid;

/* Struct with a boid's direction along with their index in the boid array for qsort*/
typedef struct {
	float distance;
	int index;
} DistanceIndexPair;

// Global mouse variables
GLint   mousePressed = 0;
GLfloat mouseX, mouseY;

// Window size parameters
GLint windowHeight = 500;
GLint windowWidth = 500;
GLfloat buttonAreaHeight = 0.25;
GLfloat boundaryPercent = 0.05;

// Button variables
GLboolean paused = GL_FALSE;

// Control variables
GLint highlightedBoid = 0;
GLfloat boidSpeed = 0.0030;
GLfloat maxBoidSpeed = 0.040;
GLfloat minBoidSpeed = 0.0001;

// Boid variables
Boid currentFlock[BOID_COUNT];
Boid previousFlock[BOID_COUNT];
int closestN[] = { -1, -1, -1, -1, -1, -1 };
float boidSideLength = 0.02;
float boidAngle = PI / 16;

// Tweak parameters
float initialTurnFactor = 0.005;
float flockingFactor = 0.5;
float minBoidDistApart = 0.02;
float boidAvoidanceFactor = 0.000075;
float a = 0.002;



/************************************************************************

	Function:		initializeGL

	Description:	Initializes the OpenGL rendering context for display.

*************************************************************************/
void initializeGL(void)
{
	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// set it to draw a big red dot
	glColor3f(1.0, 1.0, 1.0);


	// set window mode to 2D orthographic and set the window size to be 0.0 to 1.0
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
}


/************************************************************************

	Function:		myDisplay

	Description:	Displays a black square on a black background.

*************************************************************************/
void myDisplay()
{
	// clear the screen 
	glClear(GL_COLOR_BUFFER_BIT);


	//draw boids
	drawBoids();



	//draw button area
	drawButton();


	//use swap buffers instead of flush() b/c of double buffering
	glutSwapBuffers();
}


/************************************************************************

	Function:		drawButton

	Description:	Used in tandem with myDisplay() to take care of displaying
					the button area and button

*************************************************************************/
void drawButton(void) {
	
	//Button Area
	glBegin(GL_POLYGON);

		glColor3f(178.0 / 255, 178.0 / 255, 128.0 / 255);
		glVertex2f(0, 0);
		glVertex2f(0, buttonAreaHeight);
		glVertex2f(1, buttonAreaHeight);
		glVertex2f(1, 0);

	glEnd();

	//button area line accent
	glBegin(GL_LINES);
	glColor3f(204.0 / 255, 204.0 / 255, 154.0 / 255);
		glVertex2f(0, 0.2);
		glVertex2f(1, 0.2);
	glEnd();

	//boid button shadow
	glBegin(GL_POLYGON);

		glColor3f(122.0 / 255, 122.0 / 255, 72.0 / 255);
		glVertex2f(0.34, 0.04);
		glVertex2f(0.34, 0.14);
		glVertex2f(0.68, 0.14);
		glVertex2f(0.68, 0.04);
	glEnd();

	//boid button
	glBegin(GL_POLYGON);
		if (!paused) {
			glColor3f(204.0 / 255, 204.0 / 255, 154.0 / 255);
		}
		else {
			glColor3f(165.0 / 255, 165.0 / 255, 99.0 / 255);
		}

		glVertex2f(0.33, 0.05);
		glVertex2f(0.33, 0.15);
		glVertex2f(0.67, 0.15);
		glVertex2f(0.67, 0.05);

	glEnd();

	//Text Color to black
	glColor3f(0.0, 0.0, 0.0);  

	//Position of text
	glRasterPos2f(0.37, 0.08); 

	//Setting text according to current status
	char* buttonText;
	if (paused == GLU_FALSE) {
		buttonText = "Pause Boids";
	}
	else {
		buttonText = "Resume Boids";
	}

	//Printing the text onto the projection
	for (int i = 0; i < strlen(buttonText); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buttonText[i]);
	}


}


/************************************************************************

	Function:		myMouse

	Description:	Grabs the position of a mouse click.

*************************************************************************/
void myMouseButton(int button, int state, int x, int y)
{
	// if the right button is pressed then quit

	// if the left button is pressed then note the position and force a re-draw
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		// convert x from Mouse coordinates to OpenGL coordinates
		mouseX = (GLfloat)x / (GLfloat)windowWidth;

		// convert y from Mouse coordinates to OpenGL coordinates
		mouseY = (GLfloat)windowHeight - (GLfloat)y;  // first invert mouse Y position
		mouseY = mouseY / (GLfloat)windowHeight;

		//checks to see if mouse is clicking inside the pause button
		if (mouseX <= 0.67 && mouseX >= 0.33 && mouseY >= 0.05 && mouseY <= 0.15) {
			paused = !paused;
		}

		// now force OpenGL to redraw the change
		glutPostRedisplay();
	}

}


/************************************************************************

	Function:		myKeyboard

	Description:	Handles the functionality of normal key keyboard 
					presses by the user

*************************************************************************/
void myKeyboard(unsigned char key, int x, int y) {

	//Quits if the user hit's q
	if (key == 'q' || key == 'Q') {
		exit(0);
	}

	//If the user hits a number key, the ASCII value of the char is converted to the int number
	else if (key >= '0' && key <= '9') {
		for (int i = 0; i < 6; i++) {
			closestN[i] = -1;
		}
		if (key == '0') {
			highlightedBoid = 0;
		}
		else {
			highlightedBoid = key - '0';

		}
	}

	// now force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		specialKeyboard

	Description:	Handles the functionality of special key keyboard 
					presses by the user

*************************************************************************/
void mySpecialKeyboard(int key, int x, int y) {

	//Increasing or decreasing speed based on page up or page down press respectively
	switch (key) {
	case GLUT_KEY_PAGE_UP:
		boidSpeed += 0.001;
		if (boidSpeed >= maxBoidSpeed) {
			boidSpeed = maxBoidSpeed;
		}
		break;
	case GLUT_KEY_PAGE_DOWN:
		boidSpeed -= 0.001;
		if (boidSpeed <= minBoidSpeed) {
			boidSpeed = minBoidSpeed;
		}
		break;
	}

	// now force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		initializeBoids

	Description:	initializes positions, speeds, and directions of all
					the boids

*************************************************************************/
void initializeBoids(void) {
	
	//Iterate through boid array to set the initial parameters for each boid
	for (int i = 0; i < BOID_COUNT; i++) {
		
		//Getting a random float between 0.0 and 1.0, then pushing it in our boid boundaries
		currentFlock[i].x = (GLfloat)rand() / RAND_MAX;
		currentFlock[i].y = (GLfloat)rand() / RAND_MAX;
		if (currentFlock[i].y < buttonAreaHeight + boundaryPercent) {
			currentFlock[i].y += buttonAreaHeight + boundaryPercent;
		}
		if (currentFlock[i].y >= 1.0 - boundaryPercent) {
			currentFlock[i].y -= boundaryPercent;
		}
		if (currentFlock[i].x <= boundaryPercent) {
			currentFlock[i].x += boundaryPercent;
		}
		if (currentFlock[i].x >= 1.0 - boundaryPercent) {
			currentFlock[i].x -= boundaryPercent;
		}

		//Setting a random direction for every boid in radians
		currentFlock[i].direction = ((GLfloat)rand() / RAND_MAX) * 1.999 * PI;

		//Setting initial speed in every boid
		currentFlock[i].speed = boidSpeed;

		//Copy all parameters for the ith boid in currentFlock into the ith boid in previousFlock
		previousFlock[i].x = currentFlock[i].x;
		previousFlock[i].y = currentFlock[i].y;
		previousFlock[i].direction = currentFlock[i].direction;
		previousFlock[i].speed = currentFlock[i].speed;

	}
}


/************************************************************************

	Function:		drawBoids

	Description:	draws all the boids in the boids array with their 
					corresponding directions and points

*************************************************************************/
void drawBoids(void) {
	// iterate through the array and draw the boid as a triangle based on its position and direction
	for (int i = 0; i < BOID_COUNT; i++) {
		glBegin(GL_TRIANGLES);

		//Need to draw the boids as blue, green, or red, depending on if it is highlighted, in closest N, or unhighlighted
		if (i == highlightedBoid - 1) {
			glColor3f(1.0, 0.0, 0.0);
		}
		else if (inClosestN(i)) {
			glColor3f(0.0, 1.0, 0.0);
		}
		else {
			glColor3f(0.0, 0.0, 1.0);
		}
		/*
			Every boid needs to be a triangle whose point at (x,y), which we call p1, is also the point in which our direction vector
			dictates how the rest of our triangle is drawn. Because we also need to draw the points in clockwise, we make sure to draw
			p2 before p3. 

			Essentially, we are finding the angle/direction to p2 relative to p1, which we can do using a couple of known variables, 
			currenFlock[i].direction and boidAngle, then we draw the point in that direction at the distance away that we've defined,
			boid side length. We get the x component from the cos and the y component with the sign

			I was extremely proud of myself for figuring out this math but I know I will forget which is why this explanation is
			lengthy
		*/
		glVertex2f(currentFlock[i].x, currentFlock[i].y);
		glVertex2f(currentFlock[i].x + (boidSideLength * cos(PI + currentFlock[i].direction + boidAngle)), currentFlock[i].y + (boidSideLength * sin(PI + currentFlock[i].direction + boidAngle)));
		glVertex2f(currentFlock[i].x + (boidSideLength * cos(PI + currentFlock[i].direction - boidAngle)), currentFlock[i].y + (boidSideLength * sin(PI + currentFlock[i].direction - boidAngle)));
		glEnd();
	}
}


/************************************************************************

	Function:		myIdle

	Description:	idle function used for nonevents for GLUT, handles
					movement of boids to be animated

*************************************************************************/
void myIdle(void) {

	//Always find the closest N to our highlighted boid so it is updated in real time
	if (highlightedBoid != 0) {
		findClosestN(highlightedBoid - 1, closestN);
	}

	//If the scene is not paused, our nonevent animation can occur
	if (!paused) {
		for (int i = 0; i < BOID_COUNT; i++) {

			//Following pseudo code, if too close to the wall, the wall repeling takes precedence over flocking
			if (currentFlock[i].x < 0.05 || currentFlock[i].x > 0.95 || currentFlock[i].y < buttonAreaHeight + 0.05 || currentFlock[i].y > 0.95) {
				handleBoidWallInteraction(i);
			}
			else {
				handleFlockingInteraction(i);
			}


			//Move boids forward based on their direction and speed
			currentFlock[i].speed = boidSpeed;
			currentFlock[i].x += currentFlock[i].speed * cos(currentFlock[i].direction);
			currentFlock[i].y += currentFlock[i].speed * sin(currentFlock[i].direction);


		}

		//After each step, copy currentFlock values into previousFlock
		for (int i = 0; i < BOID_COUNT; i++) {
			previousFlock[i] = currentFlock[i];
		}
	}

	// now force OpenGL to redraw the change
	glutPostRedisplay();
}


/************************************************************************

	Function:		handleFlockingInteraction

	Description:	function used to handle what happens when boids are not
					near walls. Ensures boids move in a direction avg to 
					their neighbours but also stay away from eachother

*************************************************************************/
void handleFlockingInteraction(int i) {


	/* BOID FLOCKING SECTION */


	//Find the indices of the N closest boids to this specific one
	int N[CLOSEST_COUNT];
	findClosestN(i, N);

	//Variables to hold sum of each vector component
	float sumX = cos(currentFlock[i].direction);  
	float sumY = sin(currentFlock[i].direction); 

	//Add each of the closest boids vector components to our sums to be averaged
	for (int j = 0; j < CLOSEST_COUNT; j++) {
		float direction = previousFlock[N[j]].direction;
		sumX += cos(direction);
		sumY += sin(direction);
	}

	//Find the average direction based off our summed components using arctan
	float avgDirection = atan2(sumY, sumX);

	//Find the different in direction
	float deltaAngle = avgDirection - currentFlock[i].direction;

	//Normalize directions to be within our [0,2PI) range
	if (deltaAngle > PI) {
		deltaAngle -= 2 * PI;
	}
	else if (deltaAngle < -PI) {
		deltaAngle += 2 * PI;
	}

	//Adjust our current direction by a small amount to head towards the desired angle
	currentFlock[i].direction += flockingFactor * deltaAngle;

	//Normalize directions to be within our [0,2PI) range
	currentFlock[i].direction = fmod(currentFlock[i].direction, 2 * PI);


	/* BOID AVOIDANCE SECTION */

	//Check if any of the closest N boids are TOO close to this specific boid
	for (int j = 0; j < CLOSEST_COUNT; j++) {
		float dist = findDistance(i, N[j]);
		if (dist < 0.03) {

			//Push close boids away inversely by distance and calculate where they want to push to
			float inverseDist = 1 / dist;
			float deltaX = previousFlock[N[j]].x - previousFlock[i].x;
			float deltaY = previousFlock[N[j]].y - previousFlock[i].y;
			float desiredDir = atan2f(deltaY, deltaX) + PI;
			desiredDir = fmod(desiredDir, 2 * PI);

			//Push boids away by the calculated amount times a small constant parameter
			currentFlock[i].x += boidAvoidanceFactor * inverseDist * cos(desiredDir);
			currentFlock[i].y += boidAvoidanceFactor * inverseDist * sin(desiredDir);
		}
	}
}


/************************************************************************

	Function:		handleBoidWallInteraction

	Description:	function used to ensure boids don't pass through walls
					and are pushed away by the inverse distance

*************************************************************************/
void handleBoidWallInteraction(int i) {

	//Normalize directions to be within our [0,2PI) range
	float modFlock = fmod(currentFlock[i].direction, 2 * PI);

	//Higher speeds require the wall boundaries to be stronger, so scale it as such
	float turnFactor = initialTurnFactor + boidSpeed;

	//Normalize directions to be within our [0,2PI) range
	if (modFlock < 0) {
		modFlock += 2 * PI;
	}

	//Each boundary will have slightly different interactions, series of if's to see which boundary we are hitting
	if (currentFlock[i].x < 0.05) {

		float inverseDist = 1 / currentFlock[i].x;

		//If we hit the left wall coming down, we add to our direction to push away
		//If we hit the left wall coming up, we subtract to our direction to push away
		if (modFlock >= PI) {
			currentFlock[i].direction += inverseDist * turnFactor;
		}
		else if (modFlock < PI) {
			currentFlock[i].direction -= inverseDist * turnFactor;
		}
	}
	if (currentFlock[i].x > 0.95) {
		float inverseDist = 1 / (1 - currentFlock[i].x);

		//If we hit the right wall coming up, we add to our direction to push away
		//If we hit the right wall coming down, we subtract to our direction to push away
		if (modFlock >= 0 && modFlock < PI) {
			currentFlock[i].direction += inverseDist * turnFactor;
		}
		else if (modFlock >= PI / 2 && modFlock < 2 * PI) {
			currentFlock[i].direction -= inverseDist * turnFactor;
		}

	}

	if (currentFlock[i].y < buttonAreaHeight + 0.05) {
		float inverseDist = 1 / (currentFlock[i].y - buttonAreaHeight);

		//If we hit the lower wall coming right, we add to our direction to push away
		//If we hit the lower wall coming left, we subtract to our direction to push away
		if ((modFlock >= 0 && modFlock < PI / 2) || (modFlock < 2 * PI && modFlock >= 3 * PI / 2)) {
			currentFlock[i].direction += inverseDist * turnFactor;
		}
		else if (modFlock >= PI / 2 && modFlock < 3 * PI / 2) {
			currentFlock[i].direction -= inverseDist * turnFactor;
		}
	}
	if (currentFlock[i].y > 0.95) {
		float inverseDist = 1 / (1 - currentFlock[i].y);

		//If we hit the upper wall coming left, we add to our direction to push away
		//If we hit the upper wall coming right, we subtract to our direction to push away
		if (modFlock >= PI / 2 && modFlock < 3 * PI / 2) {
			currentFlock[i].direction += inverseDist * turnFactor;
		}
		else if ((modFlock >= 0 && modFlock < PI / 2) || (modFlock < 2 * PI && modFlock >= 3 * PI / 2)) {
			currentFlock[i].direction -= inverseDist * turnFactor;
		}

	}
}


/************************************************************************

	Function:		inClosestN

	Description:	given an index, checks to see if the parameter boid 
					should be highlighted as one of the closest N (6)

*************************************************************************/
bool inClosestN(int index) {
	for (int i = 0; i < CLOSEST_COUNT; i++) {
		//If a boid is in the global closest array, we know to highlight it
		if (index == closestN[i]) return true;
	}
	return false;
}


/************************************************************************

	Function:		compareDistanceIndexPair

	Description:	compare function used for qsort, compares distances
					in distance index pairs

	reference: 
	https://stackoverflow.com/questions/1787996/c-library-function-to-perform-sort

*************************************************************************/
int compareDistanceIndexPair(const void* elem1 , const void* elem2) {
	const DistanceIndexPair* pair1 = (const DistanceIndexPair*)elem1;
	const DistanceIndexPair* pair2 = (const DistanceIndexPair*)elem2;
	
	//Checking to see which distance in the distance index pair elements is shorter
	if (pair1->distance > pair2->distance) return 1;
	if (pair1->distance < pair2->distance) return -1;
	return 0;
}


/************************************************************************

	Function:		findClosestSix

	Description:	finds the closest six boids relative to the boid who's
					index is passed as a parameter, places the six in an array
					for easy indexing

*************************************************************************/
void findClosestN(int index, int closestNArray[]) {

	//Initialize our distance index pairs
	DistanceIndexPair distanceIndexPairs[BOID_COUNT];

	for (int i = 0; i < BOID_COUNT; i++) {
		
		//If we have the current boid's index, ensure that it won't be highlighted as it's own neighbour
		//This is done by setting it's distance to infinity
		if (i != index) {
			distanceIndexPairs[i].distance = findDistance(index, i);
			distanceIndexPairs[i].index = i;
		}
		else {
			distanceIndexPairs[i].distance = INFINITY;
			distanceIndexPairs[i].index = -1; 
		}
	}

	//Use qsort to sort our distance index pair array so we can retrieve the first N closest neighbours
	qsort(distanceIndexPairs, BOID_COUNT, sizeof(DistanceIndexPair), compareDistanceIndexPair);

	for (int i = 0; i < CLOSEST_COUNT; i++) {
		closestNArray[i] = distanceIndexPairs[i].index;
	}
}


/************************************************************************

	Function:		findDistance

	Description:	Finds the distance between two boids based off their 
					corresponding index from the boids array

*************************************************************************/
float findDistance(int index1, int index2) {
	
	//basic distance calculation formula between two points, square root of the square of delta x and delta y
	float x1 = previousFlock[index1].x;
	float y1 = previousFlock[index1].y;

	float x2 = previousFlock[index2].x;
	float y2 = previousFlock[index2].y;

	float dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

	//distance is based off of two indices instead of two points, allowing for easy boid retrieval in other functions
	return dist;
}


/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{

	//seeding the random number generator
	srand(time(NULL));

	// initialize the toolkit
	glutInit(&argc, argv);

	// set display mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// set window size
	glutInitWindowSize(windowWidth, windowHeight);

	// set window position on screen
	glutInitWindowPosition(100, 150);

	// open the screen window
	glutCreateWindow("Assignment 1");

	// register redraw function
	glutDisplayFunc(myDisplay);

	// register mouse function
	glutMouseFunc(myMouseButton);

	// register keyboard function
	glutKeyboardFunc(myKeyboard);

	// register special keys function
	glutSpecialFunc(mySpecialKeyboard);

	//initialize the rendering context
	initializeGL();

	//initialize the boids
	initializeBoids();

	// regiser idle function for animation
	glutIdleFunc(myIdle);

	//display keyboard controls on console
	printKeyboardControls();

	// go into a perpetual loop
	glutMainLoop();
}


/************************************************************************

	Function:		printKeyboardControls

	Description:	prints the necessary user key controls to the console
					for ease of understanding the program

*************************************************************************/
void printKeyboardControls(void)
{
	printf("Scene Controls\n-------------------\n");
	printf("Page Up \t: Faster\n");
	printf("Page Down \t: Slower\n");
	printf("[1-9] \t\t: Highlight boid and its neighbors\n");
	printf("0 \t\t: Turn off highlighting\n");
	printf("q \t\t: Quit\n\n\n");

	printf("Note: May need to use FN key to use Page Up and Page Down on Laptops.\n");
}

