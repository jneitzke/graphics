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
const float pi = 3.1415972;

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

float sinc(float x)
{
	if (x == 0.0)
	{
		return 0.0;
	}
	else
	{
		return sin(pi * x) / (pi * x);
	}
}

void graphSinc(GLdouble xMin, GLdouble xMax)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	GLdouble step = (xMax - xMin) / screenWidth;
	glBegin(GL_LINE_STRIP);

	for (float x = xMin; x < xMax; x += step)
	{
		if (x == 0.0)
		{
			glVertex2f(0.0, 1.0);
		}
		else
		{
			glVertex2f(x, sinc(x));
		}
	}
	glEnd();
	glFlush();
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

struct GLPoint{
	GLfloat x, y;
};

struct GLintPoint{
	GLint x, y;
};

void tileSinc(void)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j <7; j++)

		{
			setViewport(i * (screenWidth / 5), (i + 1) * (screenWidth / 5), j * screenHeight / 7, (j + 1) * screenHeight / 7);
			if ((i + j) % 2 == 0)
			{
				setWindow(5.0, -5.0, 1.0, -0.3);
			}
			else
			{

				setWindow(-5.0, 5.0, -0.3, 1.0);
			}
			//glViewport(i * (screenWidth / 5), j * screenHeight / 7, screenWidth / 5, screenHeight / 7);
			graphSinc(-4.0, 3.0);
		}
	}
}
void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 0.1, 0.1);

	//graphSinc(-4.0, 5.0);
	tileSinc();
	glFlush();
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);								//  initialize the tooklit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);			//  set the display mode
	glutInitWindowSize(screenWidth, screenHeight);		//  set window size
	glutInitWindowPosition(20, 50);					//  set window position on screen
	glutCreateWindow(" Graph of sinc  ");			//  open the window
	//  register the callback functions

	glutDisplayFunc(myDisplay);    // only callback for this program
	myInit();			// additional initializations as needed
	setWindow(-5.0, 5.0, -0.3, 1.0);
	setViewport(0, screenWidth, 0, screenHeight);
	glutMainLoop();		//  go into a perpetual loop

}


