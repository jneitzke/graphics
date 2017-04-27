// reflection.cpp : Adds reflections.
//

// surface.cpp : Extends superellipsoid
//  example surface -- damped, centered cosine wave propagation -- like droplet in a pond
//
// superellipsoid.cpp : Defines the entry point for the console application.
//// added superellipsoid  4/17/14


// build from textures3.cpp :
//

//  main project file for raytracing, phong specular.
#include "stdafx.h"      // WINDOWS
#include <windows.h>     // WINDOWS

#include "raycam.h" // camera class

Cam cam;   // global camera object
int wid = 750;
int hig = 750;
int wideangle = 70;

//  myKeyboard
double red = 0.0; double green = 0.0; double blue = 0.0;


// in raytracing, myKeyboard works but is verrrry slow.
void myKeyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
		// camera controls
	case 'F':  cam.slide(0.0, 0.0, 0.2); break;		// slide camera forward
	case 'f':  cam.slide(0.0, 0.0, -0.2); break;	// slide camera back

	case 'P':  cam.pitch(-1.0); break;				// tilt camera down
	case 'p':  cam.pitch(1.0); break;				// tilt camera up

	case 'Y':  cam.yaw(-1.0); break;				// pan camera left (image right)
	case 'y':  cam.yaw(1.0); break;				// pan camera right (image left)

	case 'R':  cam.roll(-1.0); break;				// roll camera anticlockwise (image clk)
	case 'r':  cam.roll(1.0); break;				// roll camera clockwise (image anticlk)

	case 'U':  cam.slide(-0.25, 0.0, 0.0); break;		// slide camera left (image right)
	case 'u':  cam.slide(0.25, 0.0, 0.0); break;		// slide camera right (image left)

	case 'V':  cam.slide(0.0, -0.25, 0.0); break;		// slide camera down (image up)
	case 'v':  cam.slide(0.0, 0.25, 0.0); break;		// slide camera up (image down)

		// following redundant with F and f; note sense change
	case 'N':  cam.slide(0.0, 0.0, -0.25); break;		// slide camera forward (image larger)
	case 'n':  cam.slide(0.0, 0.0, 0.25); break;		// slide camera back (image smaller)

	case 'X':
		exit(-1);  // end program

	default:
		break;
	}

	//glutPostRedisplay();	// redraw
}




void display(void)
{

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen	
	//	glEnable(GL_NORMALIZE);

	scene sc;

	Color3 back;
	back.set(0.1, 0.09, 0.12); //  background color
	Color3 amb;
	amb.set(.23, .35, .415);  //ambient light
	sc.setambient(amb);
	sc.setbackground(back);
	sc.steps = 10;

	//sphere sp;
	light li;

	//  design the scene 
	li.set(0.7, 0.7, 0.7, 14.0, 4.0, 0.0);
	sc.setActivelight(1);
	sc.setnthlight(1, li);

	li.set(1.0, 0.0, 0.0, 0.0, 2.0, 14.0);
	sc.setActivelight(2);
	sc.setnthlight(2, li);


	li.set(.6, .6, 1.6, 14.0, 11.4, 4.0);
	sc.setActivelight(3);
	sc.setnthlight(3, li);

	li.set(0.49, 0.0, 0, 0, 8, 0);
	sc.setActivelight(4);
	sc.setnthlight(4, li);

	li.set(0.5, 0.4, 0.6, 4.5, 0, 4.2);
	sc.setActivelight(5);
	sc.setnthlight(5, li);

	li.set(1.5, 0.4, 0.6, 4.5, 5, 4.2);
	sc.setActivelight(6);
	sc.setnthlight(6, li);

	li.set(1, 1, 1, 6, 3, 16);
	sc.setActivelight(7);
	sc.setnthlight(7, li);

	plane * pl;
	sphere * sp;
	square * sq;
	superellipsoid *se;
	circularWave *cw;

	mat4x4 mat;

	Material material1;
	Material material2;


	mat.set4x4(1.8, 0.0, 0.0, -1.0,  //central sphere
		0.0, 1.8, 0.0, 2.8,
		0.0, 0.0, 1.8, -1.0,
		0.0, 0.0, 0.0, 1.0);
	sp = new sphere();
	sp->textureNumber = SOLID;
	sp->aff.set(toAffine4(inverse(mat)));
	material1.reflective.set(1.0, 1.0, 1.0);
	material1.ambient.set(0.1, 0.1, 0.1);
	material1.emissive.set(0.0, 0.0, 0.0);
	material1.diffuse.set(0.1, 0.1, 0.1);
	material1.specular.set(1.0, 1.0, 1.0);
	material1.specularExponent = 100;

	material2.reflective.set(.18, .181, .18);
	material2.ambient.set(0.28, 0.27, 0.16);
	material2.emissive.set(0.17, 0.17, 0.2);
	material2.diffuse.set(0.394, 0.35, 0.25);
	material2.specular.set(0.8, 0.8, 0.2);
	material2.specularExponent = 11;

	sp->mat1 = material1;
	sp->mat2 = material1;

	sc.setActiveobjects(1);
	sc.setnthobject(1, sp);

	mat.set4x4(4, 0, 0, 0,  // board top
		0, 0, -4, 0,
		0, 4, 0, 0,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = CHECKERBOARD;
	sq->step = 2;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1.reflective.set(.323, .123, .523);
	sq->mat1.ambient.set(159.0 / 255.0 / 3, 0.00, 197.0 / 255.0 / 3);// purple
	sq->mat1.emissive.set(0.0, 0.00, 0.00);
	sq->mat1.diffuse.set(159.0 / 255.0 / 3, 0.00, 197.0 / 255.0 / 3);
	sq->mat1.specular.set(159.0 / 255.0 / 6, 0.00, 197.0 / 255.0 / 6);
	sq->mat1.specularExponent = 12.8;

	sq->mat2.reflective.set(.146, .146, .146);
	sq->mat2.ambient.set(0.03, 0.03, 0.03);
	sq->mat2.emissive.set(0.0, 0.0, 0.0);
	sq->mat2.diffuse.set(0.15, 0.15, 0.15);
	sq->mat2.specular.set(0.038, 0.038, 00.2);
	sq->mat2.specularExponent = 11;

	sc.setActiveobjects(2);
	sc.setnthobject(2, sq);

	mat.set4x4(0, 0, 4, 4,// FRONT RIGHT
		0, 0.5, 0, -.4999,
		4, 0, 0, 0,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = CHECKERBOARD;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	material1.ambient.set(0, 0, 0);
	material1.emissive.set(0.4, 0.1, 0.1);
	material1.diffuse.set(0.0, 0.00, 0.0);
	material1.specular.set(0, 0, 0);
	material1.specularExponent = 12.8;
	material1.reflective.set(0.8, 0.8, 0.8);

	sq->mat1 = material1;

	sq->mat2.ambient.set(.2, .3, .4);
	sq->mat2.emissive.set(0, 0, 0);
	sq->mat2.diffuse.set(.4, .2, .1);
	sq->mat2.specular.set(.2, .2, .2);
	sq->mat2.specularExponent = 4;
	sq->mat2.reflective.set(.3, .3, .3);

	sc.setActiveobjects(3);
	sc.setnthobject(3, sq);

	mat.set4x4(4, 0, 0, 0,   /// FRONT LEFT 
		0, 0.5, 0, -.5,
		0, 0, 4, 4,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = CHECKERBOARD;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1 = material1;
	sq->mat2 = material2;

	sc.setActiveobjects(4);
	sc.setnthobject(4, sq);

	mat.set4x4(3.4, 0, 0, 0,  // RIGHT MIRROR
		0, 3, 0, 3.1,
		0, 0, 3.4, -6,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = SOLID;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1.ambient.set(0.01, 0.01, 0.01);
	sq->mat1.emissive.set(0.0, 0.0, 0.0);
	sq->mat1.diffuse.set(0.01, 0.01, 0.01);
	sq->mat1.specular.set(0, 0, 0);
	sq->mat1.specularExponent = 12.8;
	sq->mat1.reflective.set(1, 1, 1);

	sc.setActiveobjects(5);
	sc.setnthobject(5, sq);

	mat.set4x4(3.6, 0, 0, 0,  //RIGHT MIRROR FRAME
		0, 3.2, 0, 3.1,
		0, 0, 3.6, -6.01,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = SOLID;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1.ambient.set(223.0 / 255.0 / 2, 115.0 / 255.0 / 2, 0.5);   //heliotrope, split ambient/diffuse
	sq->mat1.emissive.set(0, 0, 0);
	sq->mat1.diffuse.set(223.0 / 2.0 / 255, 115.0 / 255.0 / 2, 0.5);
	sq->mat1.specular.set(0, 0, 0);
	sq->mat1.specularExponent = 12.8;
	sq->mat1.reflective.set(0, 0, 0);

	sc.setActiveobjects(6);
	sc.setnthobject(6, sq);

	mat.set4x4(0, 0, 3.4, -6,  // left MIRROR
		0, 3, 0, 3.1,
		-3.4, 0, 0, 0,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = SOLID;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1.ambient.set(0.01, 0.01, 0.01);
	sq->mat1.emissive.set(0.0, 0.0, 0.0);
	sq->mat1.diffuse.set(0.01, 0.01, 0.01);
	sq->mat1.specular.set(0, 0, 0);
	sq->mat1.specularExponent = 12.8;
	sq->mat1.reflective.set(1, 1, 1);

	sc.setActiveobjects(7);
	sc.setnthobject(7, sq);

	mat.set4x4(0, 0, -3.6, -6.01,  // left MIRROR frame
		0, 3.2, 0, 3.1,
		3.6, 0, 0, 0,
		0, 0, 0, 1);
	sq = new square();
	sq->textureNumber = SOLID;
	sq->step = 4;
	sq->aff.set(toAffine4(inverse(mat)));
	sq->mat1.ambient.set(255.0 / 255.0 / 2, 69.0 / 255.0 / 2, 0);   // orange red, split ambient/diffuse
	sq->mat1.emissive.set(0, 0, 0);
	sq->mat1.diffuse.set(255.0 / 255.0 / 2, 69.0 / 255.0 / 2, 0);
	sq->mat1.specular.set(0, 0, 0);
	sq->mat1.specularExponent = 12.8;
	sq->mat1.reflective.set(0, 0, 0);

	sc.setActiveobjects(8);
	sc.setnthobject(8, sq);

	mat.set4x4(0.8, 0.0, 0.0, 3.0,  //pure reflective sphere
		0.0, 0.8, 0.0, 2.8,
		0.0, 0.0, 0.8, 1.0,
		0.0, 0.0, 0.0, 1.0);
	sp = new sphere();
	sp->textureNumber = SOLID;
	sp->aff.set(toAffine4(inverse(mat)));
	material1.reflective.set(1.0, 1.0, 1.0);
	material1.ambient.set(0.0, 0.0, 0.0);
	material1.emissive.set(0.0, 0.0, 0.0);
	material1.diffuse.set(0.0, 0.0, 0.0);
	material1.specular.set(0.0, 0.0, 0.0);
	material1.specularExponent = 100;

	material2.reflective.set(.18, .181, .18);
	material2.ambient.set(0.28, 0.27, 0.16);
	material2.emissive.set(0.17, 0.17, 0.2);
	material2.diffuse.set(0.394, 0.35, 0.25);
	material2.specular.set(0.8, 0.8, 0.2);
	material2.specularExponent = 11;

	sp->mat1 = material1;
	sp->mat2 = material1;

	sc.setActiveobjects(9);
	sc.setnthobject(9, sp);

		mat.set4x4(0.8, 0.0, 0.0, 6.0,  //off-camera sphere
		0.0, 0.8, 0.0, 2.8,
		0.0, 0.0, 0.8, 1.0,
		0.0, 0.0, 0.0, 1.0);
	sp = new sphere();
	sp->textureNumber = SOLID;
	sp->aff.set(toAffine4(inverse(mat)));
	material1.reflective.set(1.0, 1.0, 1.0);
	material1.ambient.set(0.1, 0.1, 0.1);
	material1.emissive.set(0.0, 0.0, 0.0);
	material1.diffuse.set(0.4, 0.5, 0.6);
	material1.specular.set(0.7, 0.8, 0.9);
	material1.specularExponent = 100;

	material2.reflective.set(.18, .181, .18);
	material2.ambient.set(0.28, 0.27, 0.16);
	material2.emissive.set(0.17, 0.17, 0.2);
	material2.diffuse.set(0.394, 0.35, 0.25);
	material2.specular.set(0.8, 0.8, 0.2);
	material2.specularExponent = 11;

	sp->mat1 = material1;
	sp->mat2 = material1;

	sc.setActiveobjects(10);
	sc.setnthobject(10, sp);

	int doRayTrace = 1;
	if (doRayTrace)
	{

		cam.raytrace(sc, 1);

		doRayTrace = 0;
	}
	//glFlush();  //  remove for single buffering
	//glutSwapBuffers();  //  remove for single buffering
} // end of display

void main(int argc, char** argv)
{
	glutInit(&argc, argv);				// initialize the tooklit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  //  set the display mode
	//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  //  set the display mode
	glutInitWindowSize(wid, hig);		//  set window size  
	glutInitWindowPosition(150, 0);	// set window position on screen
	glutCreateWindow("  raytracing test  ");	// open the window	//  register the callback functions

	//  glutReshapeFunc(myReshape);
	//	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(display);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 1.0);
	glViewport(0, 0, wid, hig);  // sets screen window.
	cam.set(6, 2, 6, 0, 0, 0, 0, 1, 0);  // camera position and orientation
	cam.setShape(75.0, wid / hig, 0.5, 50.0);   //  angle, aspect, near, far

	cam.setDisplay(wid, hig);  //  sets resolution -- up to viewport size

	//	myInit();			// additional initializations as needed
	glutMainLoop();		// go into a perpetual loop

}


