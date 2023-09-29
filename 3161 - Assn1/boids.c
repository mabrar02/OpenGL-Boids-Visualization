/* include the library header files */
#include <freeglut.h>
#include <stdint.h>
#include <stdio.h>


void printKeyboardControls(void);
void drawButton(void);



// global mouse variables
GLint   mousePressed = 0;
GLfloat mouseX, mouseY;

// Window size parameters
GLint windowHieght = 500;
GLint windowWidth = 500;

// button variables
GLboolean paused = GL_FALSE;

// control variables
GLint highlightedBoid = 0;
GLfloat boidSpeed = 0.001;

// boid variables



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

	//draw button area
	drawButton();


	glFlush();
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
		glVertex2f(0, 0.25);
		glVertex2f(1, 0.25);
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
		mouseY = (GLfloat)windowHieght - (GLfloat)y;  // first invert mouse Y position
		mouseY = mouseY / (GLfloat)windowHieght;

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
		if (key == '0') {
			highlightedBoid = 0;
		}
		else {
			highlightedBoid = key - '0';
		}
		printf("\nhighlighted: %d", highlightedBoid);
	}

	glutPostRedisplay();
}


/************************************************************************

	Function:		specialKeyboard

	Description:	Handles the functionality of special key keyboard 
					presses by the user

*************************************************************************/
void specialKeyboard(int key, int x, int y) {

	//Increasing or decreasing speed based on page up or page down press respectively
	switch (key) {
	case GLUT_KEY_PAGE_UP:
		boidSpeed += 0.001;
		if (boidSpeed >= 0.010) {
			boidSpeed = 0.010;
		}
		printf("\nfaster, %.3f", boidSpeed);
		break;
	case GLUT_KEY_PAGE_DOWN:
		boidSpeed -= 0.001;
		if (boidSpeed <= 0.001) {
			boidSpeed = 0.001;
		}
		printf("\nslower, %.3f", boidSpeed);
		break;
	}
	glutPostRedisplay();
}

/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);

	// set display mode
	glutInitDisplayMode(GLUT_RGB);

	// set window size
	glutInitWindowSize(windowWidth, windowHieght);

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
	glutSpecialFunc(specialKeyboard);


	//initialize the rendering context
	initializeGL();

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

	printf("Note: May need to use FN key to use Page Up and Page Down on Laptops.");
}

