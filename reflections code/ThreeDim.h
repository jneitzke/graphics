//#include <gl/GL.h>
//#include <gl/glu.h>
//#include <C:\gl/glut.h>

#define _USE_MATH_DEFINES // for C++
#include <cmath>

//  three-dimensional  classes

#ifndef ThreeDim_h
#define ThreeDim_h

// utility function



double radian2degree(double rad) ;  // radian to degree  conversion


class Point3
{
public:
	Point3() {x = y = z = 0.0f;}		//constructor 1
	Point3(double xx, double yy, double zz)  //constructor 2
	{
		x = xx; 
		y = yy;
		z = zz;
	}	
	void set (double xx, double yy, double zz)  
	{
		x = xx; 
		y = yy;
		z = zz;
	}
	void set (Point3 p)  
	{
		x = p.getX(); 
		y = p.getY();
		z = p.getZ();
	}
	double getX()  {return x;}
	double getY()  {return y;}
	double getZ()  {return z;}


private:
	double x,y,z;
};

// utility function

double radian2degree(double rad) ;  // radian to degree  conversion


//  now for 3-vectors:

class vector3
{
public:
	vector3() {dx = 0.0; dy = 0.0; dz = 0.0;}//  constructors
	vector3(double x, double y, double z)
	{
		dx = x;
		dy = y;
		dz = z;
	}
	void set (double x, double y, double z)
	{
		dx = x;
		dy = y;
		dz = z;
	
	}
	void set (Point3 p)
	{
		dx = p.getX();
		dy = p.getY();
		dz = p.getZ();
		}
//	void draw(void); // draw this vector

	double length(void);
	double deltaX(void);	// return sides
	double deltaY(void);
	double deltaZ(void);
private:
	double dx;
	double dy;
	double dz;
};

const vector3 i3(1.0, 0.0, 0.0);
const vector3 j3(0.0, 1.0, 0.0);
const vector3 k3(0.0, 0.0, 1.0);

double inner(vector3 u, vector3 v);

void putVector(vector3 u);

vector3 operator+ (vector3 u, vector3 v); //  overloaded +

Point3 operator+ (Point3 u, vector3 v);  //  overloaded +

vector3 operator- (vector3 u, vector3 v);  //  overloaded -

vector3 operator- (vector3 u);			  //  overloaded  unary -
//  vector3 perp(vector3 u );  not sensible

double operator* (vector3 u, vector3 v);   // inner product

vector3 operator* (double s, vector3 u);  // scalar product

vector3 cross (vector3 u, vector3 v);   // vector product -- 3D only


vector3 normalize (vector3 u);      //  scale to unit length; if zero, stays zero

double cosAngleBetween(vector3 u, vector3 v);  //return cosine of angle between vectors

double angleBetween(vector3 u, vector3 v);  //return angle between vectors

vector3 project(vector3 u, vector3 v);  // projection of u onto v

vector3 operator- (Point3 q, Point3 p);   // vector from p to q

vector3 reflect(vector3 a, vector3 m);  // reflection of a in m

//  double directionOf(vector3 v);  // angle in which vector points(radians) // not in 3D

class Ray{     // from Hill's SDL.h
public:
	Point3 start;
	vector3 dir;
//	int recurseLevel;
//	int row, col; // for screen extents
//	int numInside; // number of objects on list
//	GeomObj* inside[10]; // array of object pointers

	Ray(){start.set(0,0,0); dir.set(0,0,0);}
	Ray(Point3 origin); //constructor: set start point of ray
//	Ray(Point3& origin, vector3& direction)
//		{ start.set(origin); dir.set(direction); numInside = 0;} //constructor
	void setStart(Point3  p){start.set(p);}
	void setDir(double x, double y, double z)
	{dir.set(x,y,z);} 
	void setDir(vector3 vect) { dir = vect;} 


//	void setRayDirection(Light *L); //for shadow feelers
//	void setRayDirection(vector3& dir); //for reflected and transmitted rays
//	int isInShadow();
//	void getHitParticulars(Intersection& inter,Point3& hitPoint,Vector3& normal); 
//	void makeGenericRay(GeomObj* p, Ray& gr);
};

#endif
