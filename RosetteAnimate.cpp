// RosetteAnimate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// ConsoleApplication5.cpp : Defines the entry point for the console application.
//

// ConsoleApplication4.cpp : Defines the entry point for the console application.
//
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
void rosette(int n, float radius, Point2 center)
{
	Point2 * pointList = new Point2[n];
	GLfloat theta = 2.0f * PI / n;
	for (int c = 0; c < n; c++)
	{
		pointList[c].set(radius * sin(theta * c) + center.x, radius * cos(theta * c) + center.y);
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			moveTo(pointList[i]);
			lineTo(pointList[j]);
		}
	}
}


void myDisplay()
{
	//float i = 12.0;
	for (float i = 12.0; i >= 0; i -= 0.1)
	{


		glClear(GL_COLOR_BUFFER_BIT);
		setWindow(-i, i, -i  * 0.75, i * 0.75);
		setViewport(0, screenWidth, 0, screenHeight);

		glColor3f(1.0, 0.1, 0.1);

		Point2 q(0, 0);
		rosette(100, 2, q);
		glFlush();
		glutSwapBuffers();
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);								//  initialize the tooklit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);			//  set the display mode
	glutInitWindowSize(screenWidth, screenHeight);		//  set window size
	glutInitWindowPosition(20, 50);					//  set window position on screen
	glutCreateWindow(" Graph of sinc  ");			//  open the window
	//  register the callback functions

	glutDisplayFunc(myDisplay);    // only callback for this program
	myInit();			// additional initializations as needed
	setWindow(-7.0, 7.0, -5.0, 5.0);
	setViewport(0, screenWidth, 0, screenHeight);
	glutMainLoop();		//  go into a perpetual loop

}






