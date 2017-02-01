// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//   

#include <windows.h>
#include <math.h>
#include <gl/Gl.h>
//#include <gl/Glu.h> 
#include <c:/gl/glut-3.7/include/gl/Glut.h>  // your location may vary
const int screenWidth = 640;  // screen width in pixels
const int screenHeight = 480;  // screen height in pixels


void myInit(void)
{
	glClearColor(0.1, 0.1, 0.1, 0.0);	//background color: 
	glColor3f(0.0f, 0.0f, 1.0f);		//drawing color green
	glPointSize(2);					    //'dots' are 2x2 pixels
	glMatrixMode(GL_PROJECTION);		// set camera shape
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)screenWidth, 0.0, (GLdouble)screenHeight);
}

void drawDot(GLint x, GLint y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

struct GLPoint{
	GLfloat x, y;
};

struct GLintPoint{
	GLint x, y;
};
void sierpinski()
{
	GLintPoint T[3] = { { 10, 10 }, { 600, 10 }, { 300, 400 } };
	int index = rand() % 3;
	GLintPoint point = T[index];
	drawDot(point.x, point.y);
	glPointSize(1);
	for (int i = 0; i < 5000; i++)
	{
		index = rand() % 3;
		point.x = (point.x + T[index].x )/ 2;
		point.y = (point.y + T[index].y )/ 2;
		drawDot(point.x, point.y);
	}
	glFlush();
}
void drawLineInt(GLint x1, GLint y1, GLint x2, GLint y2)
{
	glBegin(GL_LINES);

	glVertex2i(x1, y1);
	glVertex2i(x2, y2);

	glEnd();
}

void square(GLint x, GLint y, GLint side)
{
	glBegin(GL_LINE_LOOP);
	glVertex2i(x, y);              // draw perimeter of square
	glVertex2i(x + side, y);
	glVertex2i(x + side, y + side);
	glVertex2i(x, y + side);
	//	glVertex2i(x,  y); 
	glEnd();
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
/*	glBegin(GL_POLYGON);
	glVertex2i(100, 50);
	glVertex2i(100, 250);
	glVertex2i(200, 50);
	glVertex2i(200, 250);
	glEnd();
	*/
	//glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.1, 0.1);
	//sierpinski();
	
	//square(100, 100, 40);

	//square(200, 300, 55);

	glColor3f(1.0f, 1.0f, 0.0f);		// new drawing color
	float c;
	for (int i = 1; i < 100; i++)
	{
		c = (float)bi / 100.0;
		glColor3f(c, c, 0.0f);
		square(100, 100, 4 * i);
	}
/*
	glColor3f(0.4f, 0.7f, 0.5f);		// new drawing color
	for (int i = 1; i < 20; i++)
	{
		square(400 - 2 * i, 100 - 2 * i, 4 * i);
	}
	*/
	glFlush();
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);								//  initialize the tooklit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);			//  set the display mode
	glutInitWindowSize(screenWidth, screenHeight);		//  set window size
	glutInitWindowPosition(20, 50);					//  set window position on screen
	glutCreateWindow(" Example 1  ");			//  open the window
	//  register the callback functions

	glutDisplayFunc(myDisplay);    // only callback for this program
	myInit();			// additional initializations as needed
	glutMainLoop();		//  go into a perpetual loop

}

