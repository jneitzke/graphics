//  raycam.h
#ifndef raycam_h
#define raycam_h
#include "stdafx.h"      // WINDOWS
#include <windows.h>     // WINDOWS

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <c:/GL/glut.h>
#include <GL/glu.h>
#include <gl/gl.h>
#endif

#include <math.h>

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include "ThreeDim.h"

const int MAXREFLECTIONS = 40;

// @@@@@@@@@@@@@@@@@@@@@ Coefficients3 class @@@@@@@@@@@@@@@@
class Coefficients3 { // holds a  3-tuple.  Intended for coefficients that are not colors: ambient, diffuse, specular.
public:
	double red, green, blue;
	Coefficients3(){ red = green = blue = 0; }
	Coefficients3(double r, double g, double b){ red = r; green = g; blue = b; }
	Coefficients3(Coefficients3& c){ red = c.red; green = c.green; blue = c.blue; }
	void set(double r, double g, double b){ red = r; green = g; blue = b;  }
	void set(Coefficients3& c){ red = c.red; green = c.green; blue = c.blue; }
	void add(double r, double g, double b)
	{
		red += r; green += g; blue += b;
	}
	void add(Coefficients3&  src, Coefficients3& refl);
	//void Coefficients3(Coefficients3  colr);
};


// @@@@@@@@@@@@@@@@@@@@@ Color3 class @@@@@@@@@@@@@@@@
class Color3 { // holds a red,green,blue 3-tuple
public:
	double red, green, blue;
	Color3(){ red = green = blue = 0; }
	Color3(double r, double g, double b){ red = r; green = g; blue = b; }
	Color3(Color3& c){ red = c.red; green = c.green; blue = c.blue; }
	void set(double r, double g, double b){ red = r; green = g; blue = b; }
	void set(Color3& c){ red = c.red; green = c.green; blue = c.blue; }
	void add(double r, double g, double b)
	{
		red += r; green += g; blue += b;
	}
	void add(Color3&  src, Color3& refl);
	void add(Color3  colr);
	void AddDiffuse(Color3 lighting, Coefficients3 diffuse);
	void AddAmbient(Color3 amb, Coefficients3 ambcoeff);
	void AddSpecular(Color3 lighting, Coefficients3 spec);
	void build4tuple(double v[]);
};


//  jjn  22 Apr 2001
//  lighting:  each light has a location and an intensity
//  location is a point; intensity is a color

class light {  //  holds a color and a location
public:
	Color3 light_color;
	Point3 position;
	
	void set(double r, double g, double b, double x, double y, double z)
	{
		Point3 p;
		light_color.red = r; 
	//	printf("%f %f",light_color.red, r);
		light_color.green = g; 
		light_color.blue = b;
		p.set(x,y,z);
		position = p;
	}
};

//this was Shapes.h 
//Shapes class and Supporting classes
//@@@@@@@@@@@@@@@@@ Material class @@@@@@@@@@@@@@

// alias for textures
const int SOLID = 0;
const int CHECKERBOARD = 1;
const int GRID = 2;
const int DIAG = 3;
const int RINGXY = 4;
const int RINGXZ = 5;
const int RINGYZ = 6;

//  properties of an object
class Material{
public:
	Color3 emissive;
	Coefficients3 ambient, diffuse, specular, reflective ;  // coefficients
	//	coefficients in each of three colors
	int numParams; // for textures  ??     // not used
	double params[10]; // for textures  ??     // not used
	int textureType;  // 0 for none, neg for solids, pos for images  ??     // not used
	double specularExponent ;     
	double specularFraction, surfaceRoughness;	     //    not used
	void setDefault();
	void set(Material& m);
}; // end of Material

//  2x2 matrices
class mat2x2
{
public:
	double m[2][2];// a 2x2 matrix
	double det();
	void set(mat2x2 a);
 	void set (double a[4]);
	void setone(int row, int col, double val)
	{
		 m[row][col] = val;
//			printf(" %d  %d %f %f \n", row,col, m[row,col], val);
	};
	void set2x2(double a, double b,double c,double d)
	{ 
		m[0][0] = a;		m[0][1] = b;
		m[1][0] = c;		m[1][1] = d;
	};

};

class mat3x3
{
public:
	double m[3][3];// a 3x3 matrix
	double det();
	void setone(int row, int col, double val)
	{
		m[row][col] = val;
//			printf("bbbb  %d  %d %f %f \n", row,col, m[row,col], val);

	};
	double  cofactor(int nr, int nc);
	void set(mat3x3 a);
 	void set (double a[9]);
	void set3x3(double a, double b,double c,double d,double e, double f,double g,double h,double i)
	{ 
		m[0][0] = a;		m[0][1] = b;		m[0][2] = c;
		m[1][0] = d;		m[1][1] = e;		m[1][2] = f;
		m[2][0] = g;		m[2][1] = h;		m[2][2] = i;
	};

};



// @@@@@@@@@@@@@@@@@@@@@ Affine4 class @@@@@@@@@@@@@@@@
class Affine4 {// manages homogeneous affine transformations
	// including inverse transformations
	// and a stack to put them on
	// used by Scene class to read SDL files
// jjn:  enlarged to handle inverses

public:
	double m[16]; // hold a 4 by 4 matrix
	Affine4();
	void setIdentityMatrix();
	void set(Affine4 a);
	
	void preMult(Affine4 n);
	void postMult(Affine4 n);
	void postTranXfrmVect(vector3 res, Affine4  aff,vector3  v);

	void putAffine();
	double cofactor();
}; // end of Affine4 class

class mat4x4{
public:
	double mm[4][4];
	void set(Affine4 a);
	void set(mat4x4 a);
	
	double   cofactor(int nr, int nc);
	double det();
	void setone(int row, int col, double val)
	{
		mm[row][col] = val;
	};
	void set4x4(double a, double b,double c,double d,
		double e, double f,double g,double h,
		double i, double j,double k,double l,
		double m, double n,double o,double p )
	{ 
		mm[0][0] = a;		mm[0][1] = b;	mm[0][2] = c;     mm[0][3] = d;
		mm[1][0] = e;		mm[1][1] = f;	mm[1][2] = g;     mm[1][3] = h;
		mm[2][0] = i;		mm[2][1] = j;	mm[2][2] = k;     mm[2][3] = l;
		mm[3][0] = m;		mm[3][1] = n;	mm[3][2] = o;     mm[3][3] = p;
	};
	
};
	mat4x4 copy ( mat4x4 incom);
	mat4x4   inverse(mat4x4 mmm);  //  cofactor method
	mat4x4   transpose(mat4x4 mmm);  //     
	Affine4 toAffine4(mat4x4 mmm);  // back to Affine4
///////////////////////////////////////////
//  3-dimensional objects
//  For use in ray tracing
//  each object will supply the following functions:
//  timeHit -- returns a double for the time hit
//  whereHit -- returns a point, if hit; undefined if not
//  normalHit -- returns a normal vector at the place where hit; undefined if not.

class sceneobject  {
	public:
		Material mat1;
		Material mat2;
		Affine4 aff;
		int textureNumber;
		double superexponent; //  for superellipse
		int step;  // used in two-material textures

	virtual double timeHit(Ray  r) {   
		printf("must be overridden \n")  ;
		return -120.0;  
	}  
	virtual Point3 whereHit (Ray  r){   
		printf("must be overridden \n")  ;
		Point3 p (0,0,0); 
		return p;
	};
	virtual vector3 normalHit(Ray  r){
		printf("must be overridden \n") ;
		vector3 v (0,0,0); 
		return v;
	};
	 virtual void iama(){printf("i am an object \n");};
	};

////////////////

class sphere: public sceneobject{
	//  generic sphere:  centered at origin, radius 1
	public:

	sphere:: sphere ( ){
	}
	


	 double sphere:: timeHit(Ray  r);
	 Point3 sphere:: whereHit (Ray  r);
	 vector3 sphere:: normalHit(Ray  r);
	 void sphere:: iama();
};

	
	class plane: public sceneobject{
	//  generic plane:  
public:

	plane:: plane ( ){
	}
	
	 double plane::timeHit(Ray  r);
	 Point3 plane::whereHit (Ray  r);
	 vector3 plane::normalHit(Ray  r);
	void plane:: iama();//{printf("i am a plane \n");};

};

	class square: public sceneobject{
	//  generic square:  
public:
	square:: square ( ){
	}
	
	 double square::timeHit(Ray  r);
	 Point3 square::whereHit (Ray  r);
	 vector3 square::normalHit(Ray  r);
	void square:: iama();//{printf("i am a square \n");};

};

	class superellipsoid : public sceneobject{
		//  generic superellipsoid:  
	public:
		superellipsoid::superellipsoid(){
		}

		double superellipsoid::timeHit(Ray  r);
		Point3 superellipsoid::whereHit(Ray  r);
		vector3 superellipsoid::normalHit(Ray  r);
		void superellipsoid::iama();//{printf("i am a superellipsoid \n");};

	};


	class circularWave : public sceneobject{
		//  generic wave, center at origin:  
	public:
		circularWave::circularWave(){
		}

		double circularWave::timeHit(Ray  r);
		Point3 circularWave::whereHit(Ray  r);
		vector3 circularWave::normalHit(Ray  r);
		void circularWave::iama();//{printf("i am a circularWave \n");};

	};

const int sceneSize = 100;

class scene 
{
public:
	scene:: scene ( )
	{
		activeobjects = 0;
		activesphere = 0;
		activecone = 0;
		activeplane = 0;
		activelight = 0;
	}
	void setActiveobjects (int n) 
	{
		activeobjects = n;
	}
	void setActivelight (int n) 
	{
		activelight = n;
	}
	int  getActiveobjects () { return activeobjects; }	
	int  getActivelight() { return activelight;}
	Color3 getBackground() {return background;};
	Color3 getAmbient() {return ambient;};
	sceneobject *  getnthobject (int n) { return sceneobjects[n - 1]; }
	light  getnthlight(int n) { return sceneobjectlight[n - 1];}
	void setnthobject(int n, sceneobject  * c);
	void setnthlight(int n, light l);
	void setbackground(Color3 c) {background = c;};
	void setambient(Color3 c) {ambient = c;};
	Color3 diffuse(Ray r, int what,Material whichmaterial, Point3 where, vector3 norm);
	Color3 specular(Ray r, int what,Point3 where, double PhongExp, vector3 norm);
	double timeHit2(Ray r);
	Point3 whereHit2  (Ray r);
	vector3 tanHit2(Ray r);
	int   whatHit2(Ray r);
	int steps; //  divisions in checkerboard
	int checkerboard3 (Point3 p);

//private:
	Color3 ambient;
	Color3 background;
	int activeobjects;
	int activesphere;
	int activecone;
	int activeplane;
	int activelight;
	sceneobject * sceneobjects [sceneSize];

	light sceneobjectlight[sceneSize];  //  way more lights than reasonable
};



//  raytracing camera
class Cam{
public:
	int nRows, nCols;
	Cam:: Cam();
	
	//	void printCamera(void);
	void roll(double angle);
	void pitch(double angle);
	void yaw(double angle);
	void slide(double du, double dv, double dn);
	void set(Point3 Eye,  Point3 look, vector3 up);  // like gluLookAt
	void set(double ex, double ey, double ez, double lx, double ly, double lz, double upx, double upy, double upz);
	void setDisplay(int numRows, int numCols);
	void setProjectionMatrix();
	void setShape(double viewAngle, double aspect, double Near, double Far);
	void setAspect(double asp);
	Color3 computeColor(scene& scn, Ray theRay, int which, Material whichmaterial,
		Point3 where, vector3 norm);
	Color3 Cam::mirrorColor(scene& scn, Ray theRay, int which,
		Point3 where, vector3 norm, int reflectionsRemaining);
	
	void rotAxes(vector3& a, vector3& b, double angle);
	void setDefaultCamera();
	
	void raytrace(scene& scn, int blockSize);
	
private:
	Point3 eye;
	vector3 u, v, n;
	double aspect, nearDist, farDist, viewAngle, W, H;  // view volume shape
	void setModelViewMatrix();  //  tell openGL where the camera is
	void  setDefaultCam(void);
};

const int maxTexture = 4;
//int (*textureArray[maxTexture])(Point3 p);

void initTextures(void);

#endif