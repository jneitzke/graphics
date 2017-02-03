// ConsoleApplication5.cpp : Defines the entry point for the console application.

#include "stdafx.h"

//   parametric curves

#include <windows.h>
#include <math.h>
#include <gl/Gl.h>
//#include <gl/Glu.h> 
#include <c:/gl/glut-3.7/include/gl/Glut.h>  // your location may vary
const int screenWidth = 640;  // screen width in pixels
const int screenHeight = 480;  // screen height in pixels
const float PI = 3.1415972;

void setWindow(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top)
{
	glViewport(left, bottom, right - left, top - bottom);
}

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

class Point2
{
public:
	float x, y;
	void set(float xCoord, float yCoord){ x = xCoord; y = yCoord; }
	void set(Point2 p){ x = p.x; y = p.y; }
	Point2(float xx, float yy){ x = xx; y = yy; }
	Point2() { x = y = 0.0; }
};

class GLintPoint
{
public:
	GLint x, y;
};

//  set up for moveTo, lineTo:  current position CP
Point2 CP;

void moveTo(Point2 p)
{
	CP.set(p);
}

void lineTo(Point2 p)
{
	glBegin(GL_LINES);
	glVertex2f(CP.x, CP.y);
	glVertex2f(p.x, p.y);
	glEnd();
	glFlush();
	CP.set(p);
}
// Hill page 122
void cardoid(float K, Point2 center)
{
	glColor3f(1.0f, 0.0f, 0.0f);		//drawing color red
	Point2 p = Point2(2 * K+ center.x, 0.0+ center.y);
	moveTo(p);
	for (float theta = 0.0; theta < 2 * PI; theta += 0.01)
	{
		float card = K * (1 + cos(theta));
		p.set(card*cos(theta)+ center.x, card * sin(theta)+ center.y);
		lineTo(p);
	}
}

void rose(float K, int n, Point2 center){
	Point2 p = Point2( K + center.x, 0.0 + center.y);
	moveTo(p);
	for (float theta = 0.0; theta < 2 * PI; theta += 0.01)
	{
		float dist = K *  cos(n *theta);
		p.set(dist*cos(theta) + center.x, dist * sin(theta) + center.y);
		lineTo(p);
	}
}

void archimedes(float growth, float bound, Point2 center)
{
	Point2 p;
	moveTo(center);
	for (float theta = 0.0; theta < 1.0/growth * bound ; theta += 0.01)
	{
		float dist = growth * theta;
		p.set(dist*cos(theta) + center.x, dist * sin(theta) + center.y);
		lineTo(p);
	}

}
void myDisplay()
{
	Point2 q = Point2(1.0, 1.0);

	cardoid(1.0, q);

	q.set(-2.0, 1.0);

	rose(1, 5, q);

	q.set(-2.0, -1.2);

	rose(1, 8, q);

	q.set(2.0, -1.2);

	archimedes(.02, 1.0, q);

		glFlush();
		glutSwapBuffers();
	
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);								//  initialize the tooklit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);			//  set the display mode
	glutInitWindowSize(screenWidth, screenHeight);		//  set window size
	glutInitWindowPosition(20, 50);					//  set window position on screen
	glutCreateWindow(" Graph of polar  ");			//  open the window
	//  register the callback functions

	glutDisplayFunc(myDisplay);    // only callback for this program
	myInit();			// additional initializations as needed
	setWindow(-7.0, 7.0, -5.0, 5.0);
	setViewport(0, screenWidth, 0, screenHeight);
	glutMainLoop();		//  go into a perpetual loop

}







